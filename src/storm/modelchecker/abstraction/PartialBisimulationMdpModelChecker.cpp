#include "storm/modelchecker/abstraction/PartialBisimulationMdpModelChecker.h"

#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/modelchecker/results/CheckResult.h"

#include "storm/logic/FragmentSpecification.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ModelType>
        PartialBisimulationMdpModelChecker<ModelType>::PartialBisimulationMdpModelChecker(ModelType const& model) : SymbolicPropositionalModelChecker<ModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename ModelType>
        bool PartialBisimulationMdpModelChecker<ModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::reachability();
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            return nullptr;
        }
        
        template<typename ModelType>
        std::unique_ptr<CheckResult> PartialBisimulationMdpModelChecker<ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            return nullptr;
        }
        
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class PartialBisimulationMdpModelChecker<storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
