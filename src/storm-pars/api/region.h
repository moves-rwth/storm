#pragma once

#include <string>
#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm-pars/storage/ParameterRegion.h"

#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseMdpParameterLiftingModelChecker.h"
#include "storm-pars/parser/ParameterRegionParser.h"

#include "storm/api/transformation.h"
#include "storm/models/sparse/Model.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    
    namespace api {
        
        
        enum class RegionModelCheckerType {
            ParameterLifting,
            ExactParameterLifting,
            ValidatingParameterLifting
        };

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables) {
            // If the given input string looks like a file (containing a dot and there exists a file with that name),
            // we try to parse it as a file, otherwise we assume it's a region string.
            if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
                return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegionsFromFile(inputString, consideredVariables);
            } else {
                return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegions(inputString, consideredVariables);
            }
        }
        
        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::string const& inputString, storm::models::sparse::Model<ValueType> const& model) {
            auto modelParameters = storm::models::sparse::getProbabilityParameters(*model);
            auto rewParameters = storm::models::sparse::getRewardParameters(*model);
            modelParameters.insert(rewParameters.begin(), rewParameters.end());
            return parseRegions(inputString, modelParameters);
        }
        
        template <typename ParametricType, typename ConstantType>
        std::unique_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> initializeParameterLiftingRegionModelChecker(std::shared_ptr<storm::models::sparse::Model<ParametricType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task) {
            // Treat continuous time models
            if (model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    STORM_LOG_WARN("Parameter lifting not supported for continuous time models. Transforming continuous model to discrete model...");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> taskFormulaAsVector { task.getFormula().asSharedPointer() };
                    auto discreteTimeModel = storm::api::transformContinuousToDiscreteTimeSparseModel(model, taskFormulaAsVector);
                    STORM_LOG_THROW(discreteTimeModel->isOfType(storm::models::ModelType::Dtmc) || discreteTimeModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::UnexpectedException, "Transformation to discrete time model has failed.");
                    return initializeParameterLiftingRegionModelChecker(discreteTimeModel, task);
            }
            
            // Obtain the region model checker
            std::unique_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> result;
            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                auto const& dtmc = *model->template as<storm::models::sparse::Dtmc<ParametricType>>();
                result = std::make_unique<storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ValueType>, ConstantType>>(dtmc);
            } else if (model->isOfType(storm::models::ModelType::Mdp)) {
                auto const& mdp = *model->template as<storm::models::sparse::Mdp<ParametricType>>();
                result = std::make_unique<storm::modelchecker::SparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<ValueType>, ConstantType>>(mdp);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform parameterLifting on the provided model type.");
            }
                
            result->specifyFormula(task);
        
            return result;
        }
        
        template <typename ParametricType, typename ImpreciseType, typename PreciseType>
        std::unique_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeValidatingRegionModelChecker(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task) {
            // todo
            return nullptr;
        }
        
        template <typename ValueType>
        std::unique_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeRegionModelChecker(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, RegionModelCheckerType checkerType) {
            switch (checkerType) {
                    case RegionModelCheckerType::ParameterLifting:
                            return initializeParameterLiftingRegionModelChecker<ValueType, double>(model, task);
                    case RegionModelCheckerType::ExactParameterLifting:
                            return initializeParameterLiftingRegionModelChecker<ValueType, storm::RationalNumber>(model, task);
                    case RegionModelCheckerType::ValidatingParameterLifting:
                            return initializeValidatingRegionModelChecker<ValueType, double, storm::RationalNumber>(model, task);
                    default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected region model checker type.");
            }
            return nullptr;
        }
        
        template <typename ValueType>
        std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> checkRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, RegionModelCheckerType checkerType) {
            auto regionChecker = initializeRegionModelChecker(model, task, checkerType);
            return regionChecker->analyzeRegions(regions, true);
        }
    
        
        template <typename ValueType>
        td::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> checkAndRefineRegionWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::storage::ParameterRegion<ValueType> const& region, ValueType const& refinementThreshold) {
            auto regionChecker = initializeRegionModelChecker(model, task, checkerType);
            return regionChecker->performRegionRefinement(region, refinementThreshold);
        }
        

        template <typename ValueType>
        void exportRegionCheckResultToFile(std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> const& checkResult, std::string const& filename) {

            std::ofstream filestream;
            storm::utility::openFile(path, filestream);
            for (auto const& res : checkResult->getRegionResults()) {
                    filestream << res.second << ": " << res.first << std::endl;
            }
        }
    
    }
}
