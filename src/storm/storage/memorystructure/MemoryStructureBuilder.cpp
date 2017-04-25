#include "storm/storage/memorystructure/MemoryStructureBuilder.h"

#include "storm/logic/FragmentSpecification.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace storage {
        
        MemoryStructureBuilder::MemoryStructureBuilder(uint_fast64_t const& numberOfStates) : transitions(numberOfStates, std::vector<std::shared_ptr<storm::logic::Formula const>>(numberOfStates)), stateLabeling(numberOfStates) {
            // Intentionally left empty
        }
            
        void MemoryStructureBuilder::setTransition(uint_fast64_t const& startState, uint_fast64_t const& goalState, std::shared_ptr<storm::logic::Formula const> formula) {
            STORM_LOG_THROW(startState < transitions.size(), storm::exceptions::InvalidOperationException, "Invalid index of start state: " << startState << ". There are only " << transitions.size() << " states in this memory structure.");
            STORM_LOG_THROW(goalState < transitions.size(), storm::exceptions::InvalidOperationException, "Invalid index of goal state: " << startState << ". There are only " << transitions.size() << " states in this memory structure.");
            STORM_LOG_THROW(formula->isInFragment(storm::logic::propositional()), storm::exceptions::InvalidOperationException, "The formula '" << *formula << "' is not propositional");
            
            transitions[startState][goalState] = formula;
        }
                        
        void MemoryStructureBuilder::setLabel(uint_fast64_t const& state, std::string const& label) {
            STORM_LOG_THROW(state < transitions.size(), storm::exceptions::InvalidOperationException, "Can not add label to state with index " << state << ". There are only " << transitions.size() << " states in this memory structure.");
            if (!stateLabeling.containsLabel(label)) {
                stateLabeling.addLabel(label);
            }
            stateLabeling.addLabelToState(label, state);
        }
            
        MemoryStructure MemoryStructureBuilder::build() {
            return MemoryStructure(std::move(transitions), std::move(stateLabeling));
        }
        
    }
}
