#pragma once

#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"

namespace storm::pars {

template<typename ValueType>
void verifyProperties(
    std::vector<storm::jani::Property> const& properties,
    std::function<std::unique_ptr<storm::modelchecker::CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> const& verificationCallback,
    std::function<void(std::unique_ptr<storm::modelchecker::CheckResult> const&)> const& postprocessingCallback) {
    for (auto const& property : properties) {
        storm::cli::printModelCheckingProperty(property);
        STORM_LOG_THROW(property.getRawFormula()->isOperatorFormula(), storm::exceptions::NotSupportedException,
                        "We only support operator formulas (P=?, R=?, etc).");
        STORM_LOG_THROW(!property.getRawFormula()->asOperatorFormula().hasBound(), storm::exceptions::NotSupportedException,
                        "We only support unbounded operator formulas (P=?, R=?, etc).");
        storm::utility::Stopwatch watch(true);
        std::unique_ptr<storm::modelchecker::CheckResult> result = verificationCallback(property.getRawFormula());
        watch.stop();
        printInitialStatesResult<ValueType>(result, &watch);
        postprocessingCallback(result);
    }
}

template<typename ValueType>
void computeSolutionFunctionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, cli::SymbolicInput const& input) {
    verifyProperties<ValueType>(
        input.properties,
        [&model](std::shared_ptr<storm::logic::Formula const> const& formula) {
            std::unique_ptr<storm::modelchecker::CheckResult> result =
                storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(formula, true));
            if (result) {
                result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
            }
            return result;
        },
        [&model](std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Dtmc)) {
                auto dtmc = model->template as<storm::models::sparse::Dtmc<ValueType>>();
                std::optional<ValueType> rationalFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
                auto constraintCollector = storm::analysis::ConstraintCollector<ValueType>(*dtmc);
                storm::api::exportParametricResultToFile<ValueType>(rationalFunction, constraintCollector, parametricSettings.exportResultPath());
            } else if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Ctmc)) {
                auto ctmc = model->template as<storm::models::sparse::Ctmc<ValueType>>();
                std::optional<ValueType> rationalFunction = result->asExplicitQuantitativeCheckResult<ValueType>()[*model->getInitialStates().begin()];
                auto constraintCollector = storm::analysis::ConstraintCollector<ValueType>(*ctmc);
                storm::api::exportParametricResultToFile<ValueType>(rationalFunction, constraintCollector, parametricSettings.exportResultPath());
            }
        });
}

template<storm::dd::DdType DdType, typename ValueType>
void computeSolutionFunctionsWithSymbolicEngine(std::shared_ptr<storm::models::symbolic::Model<DdType, ValueType>> const& model,
                                                cli::SymbolicInput const& input) {
    verifyProperties<ValueType>(
        input.properties,
        [&model](std::shared_ptr<storm::logic::Formula const> const& formula) {
            std::unique_ptr<storm::modelchecker::CheckResult> result =
                storm::api::verifyWithDdEngine<DdType, ValueType>(model, storm::api::createTask<ValueType>(formula, true));
            if (result) {
                result->filter(storm::modelchecker::SymbolicQualitativeCheckResult<DdType>(model->getReachableStates(), model->getInitialStates()));
            }
            return result;
        },
        [&model](std::unique_ptr<storm::modelchecker::CheckResult> const& result) {
            auto parametricSettings = storm::settings::getModule<storm::settings::modules::ParametricSettings>();
            if (parametricSettings.exportResultToFile() && model->isOfType(storm::models::ModelType::Dtmc)) {
                STORM_LOG_WARN("For symbolic engines, we currently do not support collecting graph-preserving constraints.");
                std::optional<ValueType> rationalFunction = result->asSymbolicQuantitativeCheckResult<DdType, ValueType>().sum();
                storm::api::exportParametricResultToFile<ValueType>(rationalFunction, storm::NullRef, parametricSettings.exportResultPath());
            }
        });
}
}  // namespace storm::pars