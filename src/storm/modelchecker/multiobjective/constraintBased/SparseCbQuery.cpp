#include "storm/modelchecker/multiobjective/constraintbased/SparseCbQuery.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseModelType>
            SparseCbQuery<SparseModelType>::SparseCbQuery(SparseMultiObjectivePreprocessorReturnType<SparseModelType>& preprocessorResult) :
                originalModel(preprocessorResult.originalModel), originalFormula(preprocessorResult.originalFormula),
                preprocessedModel(std::move(*preprocessorResult.preprocessedModel)), objectives(std::move(preprocessorResult.objectives)) {
                
            }
            
            
#ifdef STORM_HAVE_CARL
            template class SparseCbQuery<storm::models::sparse::Mdp<double>>;
            template class SparseCbQuery<storm::models::sparse::MarkovAutomaton<double>>;
            
            template class SparseCbQuery<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
        }
    }
}
