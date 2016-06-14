 #include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePostprocessor.h"

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
                STORM_LOG_ASSERT(preprocessor.
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
            
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessNumericalQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
            
            }
            
            template<typename SparseModelType, typename RationalNumberType>
            typename std::unique_ptr<CheckResult> SparseMultiObjectivePostprocessor<SparseModelType, RationalNumberType>::postprocessParetoQuery(PreprocessorData const& preprocessorData, ResultData const& resultData) {
            
                std::cout << "Postprocessing pareto queries not yet implemented";
            }
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectivePostprocessor<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
            
        }
    }
}
