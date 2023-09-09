#pragma once

#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/ModelFeatures.h"

#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/Smg.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StochasticTwoPlayerGame.h"
#include "storm/storage/sparse/ModelComponents.h"

#include "storm/builder/BuilderType.h"
#include "storm/builder/DdJaniModelBuilder.h"
#include "storm/builder/DdPrismModelBuilder.h"

#include "storm/generator/JaniNextStateGenerator.h"
#include "storm/generator/PrismNextStateGenerator.h"

#include "storm/builder/ExplicitModelBuilder.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace api {

inline storm::jani::ModelFeatures getSupportedJaniFeatures(storm::builder::BuilderType const& builderType) {
    return storm::builder::getSupportedJaniFeatures(builderType);
}

template<storm::dd::DdType LibraryType, typename ValueType>
std::shared_ptr<storm::models::symbolic::Model<LibraryType, ValueType>> buildSymbolicModel(
    storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas,
    bool buildFullModel = false, bool applyMaximumProgress = true) {
    if (model.isPrismProgram()) {
        typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options options;
        options = typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options(formulas);
        if (buildFullModel) {
            options.buildAllLabels = true;
            options.buildAllRewardModels = true;
            options.terminalStates.clear();
        }

        storm::builder::DdPrismModelBuilder<LibraryType, ValueType> builder;
        return builder.build(model.asPrismProgram(), options);
    } else {
        STORM_LOG_THROW(model.isJaniModel(), storm::exceptions::NotSupportedException, "Building symbolic model from this model description is unsupported.");
        typename storm::builder::DdJaniModelBuilder<LibraryType, ValueType>::Options options(formulas);

        if (buildFullModel) {
            options.buildAllLabels = true;
            options.buildAllRewardModels = true;
            options.applyMaximumProgressAssumption = false;
            options.terminalStates.clear();
        } else {
            options.applyMaximumProgressAssumption = (model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::MA && applyMaximumProgress);
        }

        storm::builder::DdJaniModelBuilder<LibraryType, ValueType> builder;
        return builder.build(model.asJaniModel(), options);
    }
}

template<>
inline std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, storm::RationalNumber>> buildSymbolicModel(
    storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool, bool) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support rational numbers.");
}

template<>
inline std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, storm::RationalFunction>> buildSymbolicModel(
    storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool, bool) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support rational functions.");
}

/**
 * Initializes an explict model builder; an object/algorithm that is used to build sparse models
 * @tparam ValueType Type of the probabilities in the sparse model
 * @param model SymbolicModelDescription of the model
 * @param options Builder options
 * @param actionMask An object to restrict which actions are expanded in the builder
 * @return A builder
 */
template<typename ValueType>
storm::builder::ExplicitModelBuilder<ValueType> makeExplicitModelBuilder(storm::storage::SymbolicModelDescription const& model,
                                                                         storm::builder::BuilderOptions const& options,
                                                                         std::shared_ptr<storm::generator::ActionMask<ValueType>> actionMask = nullptr) {
    std::shared_ptr<storm::generator::NextStateGenerator<ValueType, uint32_t>> generator;
    if (model.isPrismProgram()) {
        generator = std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(model.asPrismProgram(), options, actionMask);
    } else if (model.isJaniModel()) {
        STORM_LOG_THROW(actionMask == nullptr, storm::exceptions::NotSupportedException, "Action masks for JANI are not yet supported");
        generator = std::make_shared<storm::generator::JaniNextStateGenerator<ValueType, uint32_t>>(model.asJaniModel(), options);
    } else {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot build sparse model from this symbolic model description.");
    }
    return storm::builder::ExplicitModelBuilder<ValueType>(generator);
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::storage::SymbolicModelDescription const& model,
                                                                          storm::builder::BuilderOptions const& options) {
    storm::builder::ExplicitModelBuilder<ValueType> builder = makeExplicitModelBuilder<ValueType>(model, options);
    return builder.build();
}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::storage::SymbolicModelDescription const& model,
                                                                          std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
    storm::builder::BuilderOptions options(formulas, model);
    return buildSparseModel<ValueType>(model, options);
}

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> buildSparseModel(
    storm::models::ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components) {
    switch (modelType) {
        case storm::models::ModelType::Dtmc:
            return std::make_shared<storm::models::sparse::Dtmc<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::Ctmc:
            return std::make_shared<storm::models::sparse::Ctmc<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::Mdp:
            return std::make_shared<storm::models::sparse::Mdp<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::MarkovAutomaton:
            return std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::Pomdp:
            return std::make_shared<storm::models::sparse::Pomdp<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::S2pg:
            return std::make_shared<storm::models::sparse::StochasticTwoPlayerGame<ValueType, RewardModelType>>(std::move(components));
        case storm::models::ModelType::Smg:
            return std::make_shared<storm::models::sparse::Smg<ValueType, RewardModelType>>(std::move(components));
    }
}

}  // namespace api
}  // namespace storm
