#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/prism.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpLearningModelChecker<ValueType>::SparseMdpLearningModelChecker(storm::prism::Program const& program) : program(storm::utility::prism::preprocessProgram<ValueType>(program)), variableInformation(this->program), generator(program, variableInformation, false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpLearningModelChecker<ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::propositional().setProbabilityOperatorsAllowed(true).setReachabilityProbabilityFormulasAllowed(true);
            return formula.isInFragment(fragment);
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpLearningModelChecker<ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            
            // Create a callback for the next-state generator to enable it to request the index of states.
            std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback = std::bind(&SparseMdpLearningModelChecker<ValueType>::getOrAddStateIndex, this, std::placeholders::_1);

            return nullptr;
        }
        
        template<typename ValueType>
        typename SparseMdpLearningModelChecker<ValueType>::StateType SparseMdpLearningModelChecker<ValueType>::getOrAddStateIndex(storm::generator::CompressedState const& state) {
            
        }
        
        template class SparseMdpLearningModelChecker<double>;
    }
}