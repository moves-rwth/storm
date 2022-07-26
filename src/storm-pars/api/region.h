#pragma once

#include <string>
#include <set>
#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/modelchecker/region/RegionCheckEngine.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseMdpParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/ValidatingSparseMdpParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/ValidatingSparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/RegionResultHypothesis.h"
#include "storm-pars/parser/ParameterRegionParser.h"
#include "storm-pars/parser/MonotonicityParser.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parameterlifting.h"

#include "storm/environment/Environment.h"

#include "storm/api/transformation.h"
#include "storm/io/file.h"
#include "storm/models/sparse/Model.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    
    namespace api {
        struct MonotonicitySetting {
            bool useMonotonicity;
            bool useOnlyGlobalMonotonicity;
            bool useBoundsFromPLA;

            explicit MonotonicitySetting(bool useMonotonicity = false, bool useOnlyGlobalMonotonicity = false, bool useBoundsFromPLA = false) {
                this->useMonotonicity = useMonotonicity;
                this->useOnlyGlobalMonotonicity = useOnlyGlobalMonotonicity;
                this->useBoundsFromPLA = useBoundsFromPLA;
            }
        };

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none) {
            // If the given input string looks like a file (containing a dot and there exists a file with that name),
            // we try to parse it as a file, otherwise we assume it's a region string.
            if (inputString.find(".") != std::string::npos && std::ifstream(inputString).good()) {
                return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegionsFromFile(inputString, consideredVariables, splittingThreshold);
            } else {
                return storm::parser::ParameterRegionParser<ValueType>().parseMultipleRegions(inputString, consideredVariables, splittingThreshold);
            }
        }

        template <typename ValueType>
        storm::storage::ParameterRegion<ValueType> createRegion(std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none) {
            return storm::parser::ParameterRegionParser<ValueType>().createRegion(inputString, consideredVariables, splittingThreshold);
        }

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> parseRegions(std::string const& inputString, storm::models::ModelBase const& model, boost::optional<int> const& splittingThreshold = boost::none) {
            std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
            if (model.isSparseModel()) {
                auto const& sparseModel = dynamic_cast<storm::models::sparse::Model<ValueType> const&>(model);
                modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
                auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
                modelParameters.insert(rewParameters.begin(), rewParameters.end());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Retrieving model parameters is not supported for the given model type.");
            }
            return parseRegions<ValueType>(inputString, modelParameters, splittingThreshold);
        }

        template <typename ValueType>
        std::vector<storm::storage::ParameterRegion<ValueType>> createRegion(std::string const& inputString, storm::models::ModelBase const& model, boost::optional<int> const& splittingThreshold = boost::none) {
            std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
            if (model.isSparseModel()) {
                auto const& sparseModel = dynamic_cast<storm::models::sparse::Model<ValueType> const&>(model);
                modelParameters = storm::models::sparse::getProbabilityParameters(sparseModel);
                auto rewParameters = storm::models::sparse::getRewardParameters(sparseModel);
                modelParameters.insert(rewParameters.begin(), rewParameters.end());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Retrieving model parameters is not supported for the given model type.");
            }
            return std::vector<storm::storage::ParameterRegion<ValueType>>({createRegion<ValueType>(inputString, modelParameters, splittingThreshold)});
        }
        
        template <typename ValueType>
        storm::storage::ParameterRegion<ValueType> parseRegion(std::string const& inputString, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none) {
            // Handle the "empty region" case
            if (inputString == "" && consideredVariables.empty()) {
                return storm::storage::ParameterRegion<ValueType>();
            }
            
            auto res = parseRegions<ValueType>(inputString, consideredVariables, splittingThreshold);
            STORM_LOG_THROW(res.size() == 1, storm::exceptions::InvalidOperationException, "Parsed " << res.size() << " regions but exactly one was expected.");
            return res.front();
        }
        
        template <typename ValueType>
        storm::storage::ParameterRegion<ValueType> parseRegion(std::string const& inputString, storm::models::ModelBase const& model, boost::optional<int> const& splittingThreshold = boost::none) {
            // Handle the "empty region" case
            if (inputString == "" && !model.hasParameters()) {
                return storm::storage::ParameterRegion<ValueType>();
            }
            
            auto res = parseRegions<ValueType>(inputString, model);
            STORM_LOG_THROW(res.size() == 1, storm::exceptions::InvalidOperationException, "Parsed " << res.size() << " regions but exactly one was expected.");
            return res.front();
        }

        template <typename ValueType>
        std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>> parseMonotoneParameters(std::string const& fileName, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model) {
            std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> modelParameters;
            modelParameters = storm::models::sparse::getProbabilityParameters(*model);
            auto rewParameters = storm::models::sparse::getRewardParameters(*model);
            modelParameters.insert(rewParameters.begin(), rewParameters.end());
            return std::move(storm::parser::MonotonicityParser<typename storm::storage::ParameterRegion<ValueType>::VariableType>().parseMonotoneVariablesFromFile(fileName, modelParameters));
        }

        template <typename ParametricType, typename ConstantType>
        std::shared_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> initializeParameterLiftingRegionModelChecker(Environment const& env, std::shared_ptr<storm::models::sparse::Model<ParametricType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task, bool generateSplitEstimates = false, bool allowModelSimplification = true, bool preconditionsValidatedManually = false, MonotonicitySetting monotonicitySetting = MonotonicitySetting(), boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>>> monotoneParameters = boost::none) {

            STORM_LOG_WARN_COND(preconditionsValidatedManually || storm::utility::parameterlifting::validateParameterLiftingSound(*model, task.getFormula()), "Could not validate whether parameter lifting is applicable. Please validate manually...");
            STORM_LOG_WARN_COND(!(allowModelSimplification && monotonicitySetting.useMonotonicity), "Allowing model simplification when using monotonicity is not useful, as for monotonicity checking model simplification is done as preprocessing");
            STORM_LOG_WARN_COND(!(monotoneParameters && !monotonicitySetting.useMonotonicity), "Setting monotone parameters without setting monotonicity usage doesn't work");

            std::shared_ptr<storm::models::sparse::Model<ParametricType>> consideredModel = model;

            // Treat continuous time models
            if (consideredModel->isOfType(storm::models::ModelType::Ctmc) || consideredModel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    STORM_LOG_WARN_COND(!monotonicitySetting.useMonotonicity, "Usage of monotonicity not supported for this type of model, continuing without montonicity checking");
                    STORM_LOG_WARN("Parameter lifting not supported for continuous time models. Transforming continuous model to discrete model...");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> taskFormulaAsVector { task.getFormula().asSharedPointer() };
                    consideredModel = storm::api::transformContinuousToDiscreteTimeSparseModel(consideredModel, taskFormulaAsVector).first;
                    STORM_LOG_THROW(consideredModel->isOfType(storm::models::ModelType::Dtmc) || consideredModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::UnexpectedException, "Transformation to discrete time model has failed.");
            }

            // Obtain the region model checker
            std::shared_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> checker;
            if (consideredModel->isOfType(storm::models::ModelType::Dtmc)) {
                checker = std::make_shared<storm::modelchecker::SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>, ConstantType>>();
                checker->setUseMonotonicity(monotonicitySetting.useMonotonicity);
                checker->setUseOnlyGlobal(monotonicitySetting.useOnlyGlobalMonotonicity);
                checker->setUseBounds(monotonicitySetting.useBoundsFromPLA);
                if (monotonicitySetting.useMonotonicity && monotoneParameters) {
                    checker->setMonotoneParameters(monotoneParameters.get());
                }
            } else if (consideredModel->isOfType(storm::models::ModelType::Mdp)) {
                STORM_LOG_WARN_COND(!monotonicitySetting.useMonotonicity, "Usage of monotonicity not supported for this type of model, continuing without montonicity checking");
                checker = std::make_shared<storm::modelchecker::SparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<ParametricType>, ConstantType>>();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform parameterLifting on the provided model type.");
            }

            checker->specify(env, consideredModel, task, generateSplitEstimates, allowModelSimplification);

            return checker;
        }

        template <typename ParametricType, typename ImpreciseType, typename PreciseType>
        std::shared_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> initializeValidatingRegionModelChecker(Environment const& env, std::shared_ptr<storm::models::sparse::Model<ParametricType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ParametricType> const& task, bool generateSplitEstimates = false, bool allowModelSimplification = true) {
            
            STORM_LOG_WARN_COND(storm::utility::parameterlifting::validateParameterLiftingSound(*model, task.getFormula()), "Could not validate whether parameter lifting is applicable. Please validate manually...");

            std::shared_ptr<storm::models::sparse::Model<ParametricType>> consideredModel = model;
            
            // Treat continuous time models
            if (consideredModel->isOfType(storm::models::ModelType::Ctmc) || consideredModel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                    STORM_LOG_WARN("Parameter lifting not supported for continuous time models. Transforming continuous model to discrete model...");
                    std::vector<std::shared_ptr<storm::logic::Formula const>> taskFormulaAsVector { task.getFormula().asSharedPointer() };
                    consideredModel = storm::api::transformContinuousToDiscreteTimeSparseModel(consideredModel, taskFormulaAsVector).first;
                    STORM_LOG_THROW(consideredModel->isOfType(storm::models::ModelType::Dtmc) || consideredModel->isOfType(storm::models::ModelType::Mdp), storm::exceptions::UnexpectedException, "Transformation to discrete time model has failed.");
            }
            
            // Obtain the region model checker
            std::shared_ptr<storm::modelchecker::RegionModelChecker<ParametricType>> checker;
            if (consideredModel->isOfType(storm::models::ModelType::Dtmc)) {
                checker = std::make_shared<storm::modelchecker::ValidatingSparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<ParametricType>, ImpreciseType, PreciseType>>();
            } else if (consideredModel->isOfType(storm::models::ModelType::Mdp)) {
                checker = std::make_shared<storm::modelchecker::ValidatingSparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<ParametricType>, ImpreciseType, PreciseType>>();
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform parameterLifting on the provided model type.");
            }

            checker->specify(env, consideredModel, task, generateSplitEstimates, allowModelSimplification);
            return checker;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeRegionModelChecker(Environment const& env, std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::modelchecker::RegionCheckEngine engine, bool generateSplitEstimates = false, bool allowModelSimplification = true, bool preconditionsValidated = false, MonotonicitySetting monotonicitySetting = MonotonicitySetting(), boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>> monotoneParameters = boost::none) {
            switch (engine) {
                // TODO: now we always use regionsplitestimates
                    case storm::modelchecker::RegionCheckEngine::ParameterLifting:
                            return initializeParameterLiftingRegionModelChecker<ValueType, double>(env, model, task, generateSplitEstimates, allowModelSimplification, preconditionsValidated, monotonicitySetting, monotoneParameters);
                    case storm::modelchecker::RegionCheckEngine::ExactParameterLifting:
                            return initializeParameterLiftingRegionModelChecker<ValueType, storm::RationalNumber>(env, model, task, generateSplitEstimates, allowModelSimplification, preconditionsValidated, monotonicitySetting, monotoneParameters);
                    case storm::modelchecker::RegionCheckEngine::ValidatingParameterLifting:
                            // TODO should this also apply to monotonicity?
                            STORM_LOG_WARN_COND(preconditionsValidated, "Preconditions are checked anyway by a valicating model checker...");
                            return initializeValidatingRegionModelChecker<ValueType, double, storm::RationalNumber>(env, model, task, generateSplitEstimates, allowModelSimplification);
                    default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected region model checker type.");
            }
            return nullptr;
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::modelchecker::RegionModelChecker<ValueType>> initializeRegionModelChecker(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::modelchecker::RegionCheckEngine engine) {
            Environment env;
            initializeRegionModelChecker(env, model, task, engine);
        }
        
        template <typename ValueType>
        std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> checkRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::modelchecker::RegionCheckEngine engine, std::vector<storm::modelchecker::RegionResultHypothesis> const& hypotheses, bool sampleVerticesOfRegions) {
            Environment env;
            auto regionChecker = initializeRegionModelChecker(env, model, task, engine);
            return regionChecker->analyzeRegions(env, regions, hypotheses, sampleVerticesOfRegions);
        }
    
        template <typename ValueType>
        std::unique_ptr<storm::modelchecker::RegionCheckResult<ValueType>> checkRegionsWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, std::vector<storm::storage::ParameterRegion<ValueType>> const& regions, storm::modelchecker::RegionCheckEngine engine, storm::modelchecker::RegionResultHypothesis const& hypothesis = storm::modelchecker::RegionResultHypothesis::Unknown, bool sampleVerticesOfRegions = false) {
            std::vector<storm::modelchecker::RegionResultHypothesis> hypotheses(regions.size(), hypothesis);
            return checkRegionsWithSparseEngine(model, task, regions, engine, hypotheses, sampleVerticesOfRegions);
        }
    
        /*!
         * Checks and iteratively refines the given region with the sparse engine
         * @param engine The considered region checking engine
         * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this threshold
         * @param refinementDepthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
         * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds (and NOT the complementary result).
         * @param allowModelSimplification
         * @param useMonotonicity
         * @param monThresh if given, determines at which depth to start using monotonicity
         */
        template <typename ValueType>
        std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ValueType>> checkAndRefineRegionWithSparseEngine(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine, boost::optional<ValueType> const& coverageThreshold, boost::optional<uint64_t> const& refinementDepthThreshold = boost::none, storm::modelchecker::RegionResultHypothesis hypothesis = storm::modelchecker::RegionResultHypothesis::Unknown, bool allowModelSimplification = true, MonotonicitySetting monotonicitySetting = MonotonicitySetting(), uint64_t monThresh = 0) {
            Environment env;
            bool preconditionsValidated = false;
            auto regionChecker = initializeRegionModelChecker(env, model, task, engine, true, allowModelSimplification, preconditionsValidated, monotonicitySetting);
            return regionChecker->performRegionRefinement(env, region, coverageThreshold, refinementDepthThreshold, hypothesis, monThresh);
        }

        // TODO: update documentation
        /*!
         * Finds the extremal value in the given region
         * @param engine The considered region checking engine
         * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this threshold
         * @param refinementDepthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
         * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds (and NOT the complementary result).
         */
        template <typename ValueType>
        std::pair<ValueType, typename storm::storage::ParameterRegion<ValueType>::Valuation> computeExtremalValue(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine, storm::solver::OptimizationDirection const& dir, boost::optional<ValueType> const& precision, bool absolutePrecision, MonotonicitySetting monotonicitySetting, bool generateSplitEstimates = false, boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>& monotoneParameters = boost::none) {
            Environment env;
            bool preconditionsValidated = false;
            bool allowModelSimplification = !monotonicitySetting.useMonotonicity;
            auto regionChecker = initializeRegionModelChecker(env, model, task, engine, generateSplitEstimates, allowModelSimplification, preconditionsValidated, monotonicitySetting, monotoneParameters);
            return regionChecker->computeExtremalValue(env, region, dir, precision.is_initialized() ? precision.get() : storm::utility::zero<ValueType>(), absolutePrecision);
        }

        // TODO: update documentation
        /*!
         * Checks if a given extremal value is indeed the extremal value in the given region
         * @param engine The considered region checking engine
         * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this threshold
         * @param refinementDepthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
         * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds (and NOT the complementary result).
         */
        template <typename ValueType>
        bool checkExtremalValue(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task, storm::storage::ParameterRegion<ValueType> const& region, storm::modelchecker::RegionCheckEngine engine, storm::solver::OptimizationDirection const& dir, boost::optional<ValueType> const& precision, bool absolutePrecision, ValueType const& suggestion, MonotonicitySetting monotonicitySetting, bool generateSplitEstimates = false,  boost::optional<std::pair<std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType>>>& monotoneParameters = boost::none) {
            Environment env;
            bool preconditionsValidated = false;
            bool allowModelSimplification = !monotonicitySetting.useMonotonicity;
            auto regionChecker = initializeRegionModelChecker(env, model, task, engine, generateSplitEstimates, allowModelSimplification, preconditionsValidated, monotonicitySetting, monotoneParameters);
            return regionChecker->checkExtremalValue(env, region, dir, precision.is_initialized() ? precision.get() : storm::utility::zero<ValueType>(), absolutePrecision, suggestion);
        }
        
        template <typename ValueType>
        void exportRegionCheckResultToFile(std::unique_ptr<storm::modelchecker::CheckResult> const& checkResult, std::string const& filename, bool onlyConclusiveResults = false) {

            auto const* regionCheckResult = dynamic_cast<storm::modelchecker::RegionCheckResult<ValueType> const*>(checkResult.get());
            STORM_LOG_THROW(regionCheckResult != nullptr, storm::exceptions::UnexpectedException, "Can not export region check result: The given checkresult does not have the expected type.");
            
            std::ofstream filestream;
            storm::utility::openFile(filename, filestream);
            for (auto const& res : regionCheckResult->getRegionResults()) {

                if (!onlyConclusiveResults || res.second == storm::modelchecker::RegionResult::AllViolated || res.second == storm::modelchecker::RegionResult::AllSat) {
                    filestream << res.second << ": " << res.first << '\n';
                }
            }
        }
    
    }
}
