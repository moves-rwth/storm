#include "src/modelchecker/multiobjective/SparseMdpMultiObjectiveModelChecker.h"

#include "src/utility/macros.h"
#include "src/logic/Formulas.h"
#include "src/logic/FragmentSpecification.h"

#include "src/models/sparse/StandardRewardModel.h"

#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/modelchecker/multiobjective/pcaa.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::SparseMdpMultiObjectiveModelChecker(SparseMdpModelType const& model) : SparseMdpPrctlModelChecker<SparseMdpModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
            // A formula without multi objective (sub)formulas can be handled by the base class
            if(SparseMdpPrctlModelChecker<SparseMdpModelType>::canHandle(checkTask)) return true;
            //In general, each initial state requires an individual scheduler (in contrast to single objective model checking). Let's exclude this.
            if(this->getModel().getInitialStates().getNumberOfSetBits() > 1) return false;
            if(!checkTask.isOnlyInitialStatesRelevantSet()) return false;
            return checkTask.getFormula().isInFragment(storm::logic::multiObjective().setCumulativeRewardFormulasAllowed(true));
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpMultiObjectiveModelChecker<SparseMdpModelType>::checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula, ValueType> const& checkTask) {
            STORM_LOG_ASSERT(this->getModel().getInitialStates().getNumberOfSetBits() == 1, "Multi Objective Model checking on model with multiple initial states is not supported.");
            return multiobjective::performPcaa(this->getModel(), checkTask.getFormula());
        }
        
                
        template class SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
        template class SparseMdpMultiObjectiveModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
    }
}
