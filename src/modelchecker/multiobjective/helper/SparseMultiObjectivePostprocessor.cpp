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
                STORM_LOG_ASSERT(preprocessorData.originalModel.getInitialStates().getNumberOfSetBits() == 1, "Multi Objective Model checking on model with multiple initial states is not supported.");
                STORM_LOG_WARN_COND(!resultData.getMaxStepsPerformed(), "Multi-objective model checking has been aborted since the maximum number of refinement steps has been performed. The results are most likely incorrect.");
                switch(preprocessorData.queryType) {
                    case PreprocessorData::QueryType::Achievability:
                        return postprocessAchievabilityQuery(preprocessorData, resultData);
                    case PreprocessorData::QueryType::Numerical:
                        return postprocessNumericalQuery(preprocessorData, resultData);
                    case PreprocessorData::QueryType::Pareto:
                        return postprocessParetoQuery(preprocessorData, resultData);
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown Query Type");
                }
                
                  return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult());;
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessAchievabilityQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                STORM_LOG_ERROR_COND(resultData.isThresholdsAreAchievableSet(), "Could not find out whether the thresholds are achievable.");
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(initState, resultData.getThresholdsAreAchievable()));
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessNumericalQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
                uint_fast64_t initState = preprocessorData.originalModel.getInitialStates().getNextSetIndex(0);
                if(resultData.isThresholdsAreAchievableSet() && resultData.getThresholdsAreAchievable()) {
                    STORM_LOG_ASSERT(resultData.isNumericalResultSet(), "Postprocessing for numerical query invoked, but no numerical result is given");
                    STORM_LOG_ASSERT(preprocessorData.indexOfOptimizingObjective, "Postprocessing for numerical query invoked, but no index of optimizing objective is specified");
                    ObjectiveInformation optimizingObjective = preprocessorData.objectives[*preprocessorData.indexOfOptimizingObjective];
                    ValueType resultForOriginalModel = resultData.template getNumericalResult<ValueType>() * optimizingObjective.toOriginalValueTransformationFactor + optimizingObjective.toOriginalValueTransformationOffset;
                    STORM_LOG_WARN_COND(resultData.getTargetPrecisionReached(), "The target precision for numerical queries has not been reached.");
                    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initState, resultForOriginalModel));
                } else {
                    STORM_LOG_ERROR_COND(resultData.isThresholdsAreAchievableSet(), "Could not find out whether the thresholds are achievable.");
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
