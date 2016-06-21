 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePostprocessor.h"

#include <memory>

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/storage/geometry/Polytope.h"
#include "src/utility/macros.h"
#include "src/utility/vector.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocess(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                STORM_LOG_WARN_COND(!resultData.getMaxStepsPerformed(), "Multi-objective model checking has been aborted since the maximum number of refinement steps has been performed. The results are most likely incorrect.");
                if(preprocessorData.originalFormula.hasQualitativeResult()) {
                    return postprocessAchievabilityQuery(preprocessorData, resultData);
                } else if(preprocessorData.originalFormula.hasNumericalResult()){
                    return postprocessNumericalQuery(preprocessorData, resultData);
                } else if (preprocessorData.originalFormula.hasParetoCurveResult()) {
                    return postprocessParetoQuery(preprocessorData, resultData);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown Query Type");
                }
                  return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult());;
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
                STORM_LOG_ERROR("Postprocessing pareto queries not yet implemented");
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, false));
            }
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectivePostprocessor<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
            
        }
    }
}
