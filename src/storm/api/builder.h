#pragma once

#include "storm/parser/AutoParser.h"
#include "storm/parser/DirectEncodingParser.h"
#include "storm/parser/ImcaMarkovAutomatonParser.h"

#include "storm/storage/SymbolicModelDescription.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StochasticTwoPlayerGame.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/builder/DdJaniModelBuilder.h"

#include "storm/generator/PrismNextStateGenerator.h"
#include "storm/generator/JaniNextStateGenerator.h"

#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/builder/jit/ExplicitJitJaniModelBuilder.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace api {
        
        template<storm::dd::DdType LibraryType, typename ValueType>
        std::shared_ptr<storm::models::symbolic::Model<LibraryType, ValueType>> buildSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool buildFullModel = false) {
            if (model.isPrismProgram()) {
                typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options options;
                options = typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options(formulas);
                
                if (buildFullModel) {
                    options.buildAllLabels = true;
                    options.buildAllRewardModels = true;
                }
                
                storm::builder::DdPrismModelBuilder<LibraryType, ValueType> builder;
                return builder.build(model.asPrismProgram(), options);
            } else {
                STORM_LOG_THROW(model.isJaniModel(), storm::exceptions::NotSupportedException, "Building symbolic model from this model description is unsupported.");
                typename storm::builder::DdJaniModelBuilder<LibraryType, ValueType>::Options options;
                options = typename storm::builder::DdJaniModelBuilder<LibraryType, ValueType>::Options(formulas);
                
                if (buildFullModel) {
                    options.buildAllLabels = true;
                    options.buildAllRewardModels = true;
                }
                
                storm::builder::DdJaniModelBuilder<LibraryType, ValueType> builder;
                return builder.build(model.asJaniModel(), options);
            }
        }
        
        template<>
        inline std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, storm::RationalNumber>> buildSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool buildFullModel) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support rational numbers.");
        }

        template<>
        inline std::shared_ptr<storm::models::symbolic::Model<storm::dd::DdType::CUDD, storm::RationalFunction>> buildSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool buildFullModel) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "CUDD does not support rational functions.");
        }

        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::storage::SymbolicModelDescription const& model, storm::builder::BuilderOptions const& options, bool jit = false, bool doctor = false) {
            if (jit) {
                STORM_LOG_THROW(model.isJaniModel(), storm::exceptions::NotSupportedException, "Cannot use JIT-based model builder for non-JANI model.");

                storm::builder::jit::ExplicitJitJaniModelBuilder<ValueType> builder(model.asJaniModel(), options);

                if (doctor) {
                    bool result = builder.doctor();
                    STORM_LOG_THROW(result, storm::exceptions::NotSupportedException, "The JIT-based model builder cannot be used on your system.");
                    STORM_LOG_INFO("The JIT-based model builder seems to be working.");
                }

                return builder.build();
            } else {
                std::shared_ptr<storm::generator::NextStateGenerator<ValueType, uint32_t>> generator;
                if (model.isPrismProgram()) {
                    generator = std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(model.asPrismProgram(), options);
                } else if (model.isJaniModel()) {
                    generator = std::make_shared<storm::generator::JaniNextStateGenerator<ValueType, uint32_t>>(model.asJaniModel(), options);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot build sparse model from this symbolic model description.");
                }
                storm::builder::ExplicitModelBuilder<ValueType> builder(generator);
                return builder.build();
            }
        }

        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool jit = false, bool doctor = false) {
            storm::builder::BuilderOptions options(formulas);
            return buildSparseModel<ValueType>(model, options, jit, doctor);
        }
        
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> buildSparseModel(storm::models::ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components) {
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
            }

        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::none, boost::optional<std::string> const& transitionRewardsFile = boost::none, boost::optional<std::string> const& choiceLabelingFile = boost::none) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exact or parametric models with explicit input are not supported.");
        }
        
        template<>
        inline std::shared_ptr<storm::models::sparse::Model<double>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile, boost::optional<std::string> const& transitionRewardsFile, boost::optional<std::string> const& choiceLabelingFile) {
            return storm::parser::AutoParser<double, double>::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "", choiceLabelingFile ? choiceLabelingFile.get() : "" );
        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitDRNModel(std::string const& drnFile) {
            return storm::parser::DirectEncodingParser<ValueType>::parseModel(drnFile);
        }
        
        template<>
        inline std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> buildExplicitDRNModel(std::string const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exact models with direct encoding are not supported.");
        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitIMCAModel(std::string const&) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Exact models with direct encoding are not supported.");
        }
        
        template<>
        inline std::shared_ptr<storm::models::sparse::Model<double>> buildExplicitIMCAModel(std::string const& imcaFile) {
            return storm::parser::ImcaMarkovAutomatonParser<double>::parseImcaFile(imcaFile);
        }

    }
}
