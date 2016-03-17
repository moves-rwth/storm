#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/logic/FragmentSpecification.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        template<typename SparseMdpModelType>
        SparseMdpLearningModelChecker<SparseMdpModelType>::SparseMdpLearningModelChecker(storm::prism::Program const& program) : program(program) {
            // Intentionally left empty.
        }
        
        template<typename SparseMdpModelType>
        bool SparseMdpLearningModelChecker<SparseMdpModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::propositional().setProbabilityOperatorsAllowed(true).setReachabilityProbabilityFormulasAllowed(true);
            return formula.isInFragment(fragment);
        }
        
        template<typename SparseMdpModelType>
        std::unique_ptr<CheckResult> SparseMdpLearningModelChecker<SparseMdpModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            return nullptr;
        }
        
        template class SparseMdpLearningModelChecker<double>;
    }
}