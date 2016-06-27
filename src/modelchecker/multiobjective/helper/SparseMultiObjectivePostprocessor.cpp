 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePostprocessor.h"

#include <memory>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ParetoCurveCheckResult.h"
#include "src/storage/geometry/Polytope.h"
#include "src/storage/geometry/Hyperrectangle.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/MultiObjectiveSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/utility/export.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocess(PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch, boost::optional<storm::utility::Stopwatch> const& helperStopwatch, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch) {
                STORM_LOG_WARN_COND(!resultData.getMaxStepsPerformed(), "Multi-objective model checking has been aborted since the maximum number of refinement steps has been performed. The results are most likely incorrect.");
                
                std::unique_ptr<CheckResult> result(new ExplicitQualitativeCheckResult());;
                
                if(preprocessorData.originalFormula.hasQualitativeResult()) {
                    result = postprocessAchievabilityQuery(preprocessorData, resultData);
                } else if(preprocessorData.originalFormula.hasNumericalResult()){
                    result = postprocessNumericalQuery(preprocessorData, resultData);
                } else if (preprocessorData.originalFormula.hasParetoCurveResult()) {
                    result = postprocessParetoQuery(preprocessorData, resultData);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown Query Type");
                }
                
                STORM_LOG_INFO(getInfo(result, preprocessorData, resultData, preprocessorStopwatch, helperStopwatch, postprocessorStopwatch));
                
                exportPlot(result, preprocessorData, resultData, preprocessorStopwatch, helperStopwatch, postprocessorStopwatch);
                
                return result;
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessAchievabilityQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                STORM_LOG_ASSERT(preprocessorData.queryType == PreprocessorData::QueryType::Achievability, "Expected an achievability query.");
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                
                //Incorporate the results from prerpocessing
                for(uint_fast64_t subformulaIndex = 0; subformulaIndex < preprocessorData.originalFormula.getNumberOfSubformulas(); ++subformulaIndex) {
                    switch(preprocessorData.solutionsFromPreprocessing[subformulaIndex]) {
                        case PreprocessorData::PreprocessorObjectiveSolution::None:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::False:
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        case PreprocessorData::PreprocessorObjectiveSolution::True:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Undefined:
                            STORM_LOG_ERROR("The result for the objective " << preprocessorData.originalFormula.getSubformula(subformulaIndex) << " is not defined.");
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of solution obtained in preprocessing.");
                    }
                }
                
                if(preprocessorData.objectives.empty()) {
                    return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, true));
                }
                
                // This might be due to reaching the max. number of refinement steps (so no exception is thrown)
                STORM_LOG_ERROR_COND(resultData.isThresholdsAreAchievableSet(), "Could not find out whether the thresholds are achievable.");
                
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, resultData.isThresholdsAreAchievableSet() &&resultData.getThresholdsAreAchievable()));
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessNumericalQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                
                //The queryType might be achievability (when the numerical result was obtained in preprocessing)
                STORM_LOG_ASSERT(preprocessorData.queryType == PreprocessorData::QueryType::Numerical ||
                                 preprocessorData.queryType == PreprocessorData::QueryType::Achievability, "Expected a numerical or an achievability query.");
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                
                //Incorporate the results from prerpocessing
                boost::optional<ValueType> preprocessorNumericalResult;
                for(uint_fast64_t subformulaIndex = 0; subformulaIndex < preprocessorData.originalFormula.getNumberOfSubformulas(); ++subformulaIndex) {
                    switch(preprocessorData.solutionsFromPreprocessing[subformulaIndex]) {
                        case PreprocessorData::PreprocessorObjectiveSolution::None:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::False:
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        case PreprocessorData::PreprocessorObjectiveSolution::True:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Zero:
                            STORM_LOG_ASSERT(!preprocessorNumericalResult, "There are multiple numerical results obtained in preprocessing");
                            preprocessorNumericalResult = storm::utility::zero<ValueType>();
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Unbounded:
                            STORM_LOG_ASSERT(!preprocessorNumericalResult, "There are multiple numerical results obtained in preprocessing");
                            preprocessorNumericalResult = storm::utility::infinity<ValueType>();
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Undefined:
                            STORM_LOG_ERROR("The result for the objective " << preprocessorData.originalFormula.getSubformula(subformulaIndex) << " is not defined.");
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected solution obtained in preprocessing.");
                    }
                }
                
                // Check whether the given thresholds are achievable
                if(preprocessorData.objectives.empty() || (resultData.isThresholdsAreAchievableSet() && resultData.getThresholdsAreAchievable())) {
                    // Get the numerical result
                    if(preprocessorNumericalResult) {
                        STORM_LOG_ASSERT(!resultData.isNumericalResultSet(), "Result was found in preprocessing but there is also one in the resultData");
                        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initState, *preprocessorNumericalResult));
                    } else {
                        STORM_LOG_ASSERT(resultData.isNumericalResultSet(), "Postprocessing for numerical query invoked, but no numerical result is given");
                        STORM_LOG_ASSERT(preprocessorData.indexOfOptimizingObjective, "Postprocessing for numerical query invoked, but no index of optimizing objective is specified");
                        ObjectiveInformation const& optimizingObjective = preprocessorData.objectives[*preprocessorData.indexOfOptimizingObjective];
                        ValueType resultForOriginalModel = resultData.template getNumericalResult<ValueType>() * optimizingObjective.toOriginalValueTransformationFactor + optimizingObjective.toOriginalValueTransformationOffset;
                        STORM_LOG_WARN_COND(resultData.getTargetPrecisionReached(), "The target precision for numerical queries has not been reached.");
                        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initState, resultForOriginalModel));
                    }
                } else {
                    STORM_LOG_ERROR_COND(resultData.isThresholdsAreAchievableSet(), "Could not find out whether the thresholds are achievable.");  // This might be due to reaching the max. number of refinement steps (so no exception is thrown)
                    
                    return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                }
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessParetoQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                
                //Issue a warning for objectives that have been solved in preprocessing
                for(uint_fast64_t subformulaIndex = 0; subformulaIndex < preprocessorData.originalFormula.getNumberOfSubformulas(); ++subformulaIndex) {
                    switch(preprocessorData.solutionsFromPreprocessing[subformulaIndex]) {
                        case PreprocessorData::PreprocessorObjectiveSolution::None:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::False:
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        case PreprocessorData::PreprocessorObjectiveSolution::True:
                            // Nothing to be done
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Zero:
                            STORM_LOG_WARN("The result of the objective " << preprocessorData.originalFormula.getSubformula(subformulaIndex) << " was obtained in preprocessing and will not be incorporated in the check result. Objective Result is zero.");
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Unbounded:
                            STORM_LOG_WARN("The result of the objective " << preprocessorData.originalFormula.getSubformula(subformulaIndex) << " was obtained in preprocessing and will not be incorporated in the check result. Objective Result is infinity.");
                            break;
                        case PreprocessorData::PreprocessorObjectiveSolution::Undefined:
                            STORM_LOG_ERROR("The result for the objective " << preprocessorData.originalFormula.getSubformula(subformulaIndex) << " is not defined.");
                            return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected solution obtained in preprocessing.");
                    }
                }
                
                std::vector<std::vector<ValueType>> paretoOptimalPoints;
                paretoOptimalPoints.reserve(resultData.refinementSteps().size());
                for(auto const& step : resultData.refinementSteps()) {
                    paretoOptimalPoints.push_back(storm::utility::vector::convertNumericVector<ValueType>(transformToOriginalValues(step.getPoint(), preprocessorData)));
                }
                return std::unique_ptr<CheckResult>(new ParetoCurveCheckResult<ValueType>(
                          initState,
                          std::move(paretoOptimalPoints),
                          transformToOriginalValues(resultData.underApproximation(), preprocessorData)->template convertNumberRepresentation<ValueType>(),
                          transformToOriginalValues(resultData.overApproximation(), preprocessorData)->template convertNumberRepresentation<ValueType>()));
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            std::vector<RationalNumberType> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::transformToOriginalValues(std::vector<RationalNumberType> const& vector, PreprocessorData const& preprocessorData) {
                std::vector<RationalNumberType> result;
                result.reserve(vector.size());
                for(uint_fast64_t objIndex = 0; objIndex < preprocessorData.objectives.size(); ++objIndex) {
                    result.push_back(vector[objIndex] * storm::utility::convertNumber<RationalNumberType>(preprocessorData.objectives[objIndex].toOriginalValueTransformationFactor) + storm::utility::convertNumber<RationalNumberType>(preprocessorData.objectives[objIndex].toOriginalValueTransformationOffset));
                }
                return result;
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::transformToOriginalValues(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, PreprocessorData const& preprocessorData) {
                
                uint_fast64_t numObjectives = preprocessorData.objectives.size();
                std::vector<std::vector<RationalNumberType>> transformationMatrix(numObjectives, std::vector<RationalNumberType>(numObjectives, storm::utility::zero<RationalNumberType>()));
                std::vector<RationalNumberType> transformationVector;
                transformationVector.reserve(numObjectives);
                for(uint_fast64_t objIndex = 0; objIndex < numObjectives; ++objIndex) {
                    transformationMatrix[objIndex][objIndex] = storm::utility::convertNumber<RationalNumberType>(preprocessorData.objectives[objIndex].toOriginalValueTransformationFactor);
                    transformationVector.push_back(storm::utility::convertNumber<RationalNumberType>(preprocessorData.objectives[objIndex].toOriginalValueTransformationOffset));
                }
                return polytope->linearTransformation(transformationMatrix, transformationVector);
            }
            
            
            template<typename SparseModelType, typename RationalNumberType>
            void SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::exportPlot(std::unique_ptr<CheckResult> const& checkResult, PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch, boost::optional<storm::utility::Stopwatch> const& helperStopwatch, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch) {
                
                if(!settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isExportPlotSet()) {
                    return;
                }
                STORM_LOG_ERROR_COND(preprocessorData.objectives.size()==2, "Exporting plot requested but this is only implemented for the two-dimensional case.");
                
                auto transformedUnderApprox = transformToOriginalValues(resultData.underApproximation(), preprocessorData);
                auto transformedOverApprox = transformToOriginalValues(resultData.overApproximation(), preprocessorData);
                
                // Get pareto points as well as a hyperrectangle that is used to guarantee that the resulting polytopes are bounded.
                storm::storage::geometry::Hyperrectangle<RationalNumberType> boundaries(std::vector<RationalNumber>(preprocessorData.objectives.size(), storm::utility::zero<RationalNumberType>()), std::vector<RationalNumber>(preprocessorData.objectives.size(), storm::utility::zero<RationalNumberType>()));
                std::vector<std::vector<RationalNumberType>> paretoPoints;
                paretoPoints.reserve(resultData.refinementSteps().size());
                for(auto const& step : resultData.refinementSteps()) {
                    paretoPoints.push_back(transformToOriginalValues(step.getPoint(), preprocessorData));
                    boundaries.enlarge(paretoPoints.back());
                }
                auto underApproxVertices = transformedUnderApprox->getVertices();
                for(auto const& v : underApproxVertices) {
                    boundaries.enlarge(v);
                }
                auto overApproxVertices = transformedOverApprox->getVertices();
                for(auto const& v : overApproxVertices) {
                    boundaries.enlarge(v);
                }
                
                //Further enlarge the boundaries a little
                storm::utility::vector::scaleVectorInPlace(boundaries.lowerBounds(), RationalNumberType(11) / RationalNumberType(10));
                storm::utility::vector::scaleVectorInPlace(boundaries.upperBounds(), RationalNumberType(11) / RationalNumberType(10));
                
                auto boundariesAsPolytope = boundaries.asPolytope();
                std::vector<std::string> columnHeaders = {"x", "y"};
                
                std::vector<std::vector<double>> pointsForPlotting;
                underApproxVertices = transformedUnderApprox->intersection(boundariesAsPolytope)->getVerticesInClockwiseOrder();
                pointsForPlotting.reserve(underApproxVertices.size());
                for(auto const& v : underApproxVertices) {
                    pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                }
                storm::utility::exportDataToCSVFile(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getExportPlotUnderApproximationFileName(), pointsForPlotting, columnHeaders);
                                                    
                pointsForPlotting.clear();
                overApproxVertices = transformedOverApprox->intersection(boundariesAsPolytope)->getVerticesInClockwiseOrder();
                pointsForPlotting.reserve(overApproxVertices.size());
                for(auto const& v : overApproxVertices) {
                    pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                }
                storm::utility::exportDataToCSVFile(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getExportPlotOverApproximationFileName(), pointsForPlotting, columnHeaders);
                                                    
                pointsForPlotting.clear();
                pointsForPlotting.reserve(paretoPoints.size());
                for(auto const& v : paretoPoints) {
                    pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                }
                storm::utility::exportDataToCSVFile(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getExportPlotParetoPointsFileName(), pointsForPlotting, columnHeaders);
                                                    
                pointsForPlotting.clear();
                auto boundVertices = boundariesAsPolytope->getVerticesInClockwiseOrder();
                pointsForPlotting.reserve(4);
                for(auto const& v : boundVertices) {
                    pointsForPlotting.push_back(storm::utility::vector::convertNumericVector<double>(v));
                }
                storm::utility::exportDataToCSVFile(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getExportPlotBoundariesFileName(), pointsForPlotting, columnHeaders);
                                                    
            }
            
            
            template<typename SparseModelType, typename RationalNumberType>
            std::string SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::getInfo(std::unique_ptr<CheckResult> const& checkResult, PreprocessorData const& preprocessorData, ResultData const& resultData, boost::optional<storm::utility::Stopwatch> const& preprocessorStopwatch, boost::optional<storm::utility::Stopwatch> const& helperStopwatch, boost::optional<storm::utility::Stopwatch> const& postprocessorStopwatch) {
                
                std::stringstream statistics;
                statistics << preprocessorData;
                statistics << std::endl;
                statistics << std::endl;
                statistics << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                statistics << "                                                 Multi-objective Model Checking Result                                            " << std::endl;
                statistics << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                statistics << std::endl;
                statistics << *checkResult;
                statistics << std::endl;
                statistics << std::endl;
                statistics << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                statistics << "                                                 Multi-objective Model Checking Statistics                                            " << std::endl;
                statistics << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                statistics << std::endl;
                statistics << "Recorded Runtimes (in seconds):" << std::endl;
                storm::utility::Stopwatch combinedTime;
                combinedTime.pause();
                if(preprocessorStopwatch) {
                    statistics << "\t Preprocessing:    " << std::setw(8) << *preprocessorStopwatch << std::endl;
                    combinedTime.addToAccumulatedSeconds(preprocessorStopwatch->getAccumulatedSeconds());
                }
                if(helperStopwatch) {
                    statistics << "\t Value Iterations: " << std::setw(8) << *helperStopwatch << std::endl;
                    combinedTime.addToAccumulatedSeconds(helperStopwatch->getAccumulatedSeconds());
                }
                if(postprocessorStopwatch) {
                    statistics << "\t Postprocessing:   " << std::setw(8) << *postprocessorStopwatch << std::endl;
                    combinedTime.addToAccumulatedSeconds(postprocessorStopwatch->getAccumulatedSeconds());
                }
                statistics << "\t Combined:         " << std::setw(8) << combinedTime << std::endl;
                statistics << std::endl;
                statistics << "Performed Refinement Steps: " << resultData.refinementSteps().size() << (resultData.getMaxStepsPerformed() ? " (computation aborted) " : "" ) << std::endl;
                statistics << "Precision (Approximation): " << "Goal precision: " << settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision();
                if(resultData.isPrecisionOfResultSet()) {
                    statistics << " Achieved precision: " << storm::utility::convertNumber<double>(resultData.getPrecisionOfResult()) << ( resultData.getTargetPrecisionReached() ? "" : " (goal not achieved)");
                }
                statistics << std::endl;
                statistics << "Convergence precision for iterative solvers: " << settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision() << std::endl;
                statistics << std::endl;
                statistics << "---------------------------------------------------------------------------------------------------------------------------------------" << std::endl;
                
                return statistics.str();
            }
            
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectivePostprocessor<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
            
        }
    }
}
