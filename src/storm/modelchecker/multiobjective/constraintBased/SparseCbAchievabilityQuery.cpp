#include "storm/modelchecker/multiobjective/constraintbased/SparseCbAchievabilityQuery.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseModelType>
            SparseCbAchievabilityQuery<SparseModelType>::SparseCbAchievabilityQuery(SparseMultiObjectivePreprocessorReturnType<SparseModelType>& preprocessorResult) : SparseCbQuery<SparseModelType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparseMultiObjectivePreprocessorReturnType<SparseModelType>::QueryType::Achievability, "Invalid query Type");
                
            }

            template <class SparseModelType>
            std::unique_ptr<CheckResult> SparseCbAchievabilityQuery<SparseModelType>::check() {
               
                bool result = this->checkAchievability();
                
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), result));
                
            }
            
            template <class SparseModelType>
            bool SparseCbAchievabilityQuery<SparseModelType>::checkAchievability() {
                return false;
            }

#ifdef STORM_HAVE_CARL
            template class SparseCbAchievabilityQuery<storm::models::sparse::Mdp<double>>;
            template class SparseCbAchievabilityQuery<storm::models::sparse::MarkovAutomaton<double>>;
            
            template class SparseCbAchievabilityQuery<storm::models::sparse::Mdp<storm::RationalNumber>>;
         //   template class SparseCbAchievabilityQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>>;
#endif
        }
    }
}
