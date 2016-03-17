#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateStorage.h"

#include "src/generator/PrismNextStateGenerator.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/prism.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpLearningModelChecker<ValueType>::SparseMdpLearningModelChecker(storm::prism::Program const& program) : program(storm::utility::prism::preprocessProgram<ValueType>(program)), variableInformation(this->program) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpLearningModelChecker<ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::propositional().setProbabilityOperatorsAllowed(true).setReachabilityProbabilityFormulasAllowed(true);
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpLearningModelChecker<ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            // Create a callback for the next-state generator to enable it to request the index of states.
            std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback = std::bind(&SparseMdpLearningModelChecker<ValueType>::getOrAddStateIndex, this, std::placeholders::_1);
            
            // A container for the encountered states.
            storm::storage::sparse::StateStorage<StateType> stateStorage(variableInformation.getTotalBitOffset(true));
            
            // A generator used to explore the model.
            storm::generator::PrismNextStateGenerator<ValueType, StateType> generator(program, variableInformation, false);
            
            // A container that stores the transitions found so far.
            std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;

            // A vector storing where the row group of each state starts.
            std::vector<StateType> rowGroupIndices;
            
            // A vector storing the mapping from state ids to row groups.
            std::vector<StateType> stateToRowGroupMapping;
            
            // Vectors to store the lower/upper bounds for each action (in each state).
            std::vector<ValueType> lowerBounds;
            std::vector<ValueType> upperBounds;
            
            // Now perform the actual exploration loop.

            return nullptr;
        }
        
        template<typename ValueType>
        typename SparseMdpLearningModelChecker<ValueType>::StateType SparseMdpLearningModelChecker<ValueType>::getOrAddStateIndex(storm::generator::CompressedState const& state) {
            
        }
        
        template class SparseMdpLearningModelChecker<double>;
    }
}