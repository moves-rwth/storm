#pragma once

#include <vector>
#include <memory>

#include "storm/logic/Formula.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {
    namespace storage {
        
        
        class MemoryStructureBuilder {
        public:
            
            /*!
             * Initializes a new builder for a memory structure
             * @param numberOfStates The number of states the resulting memory structure should have
             */
            MemoryStructureBuilder(uint_fast64_t const& numberOfStates);
            
            /*!
             * Specifies a transition. The formula should be a propositional formula
             */
            void setTransition(uint_fast64_t const& startState, uint_fast64_t const& goalState, std::shared_ptr<storm::logic::Formula const> formula);
                        
            /*!
             * Sets a label to the given state.
             */
            void setLabel(uint_fast64_t const& state, std::string const& label);
            
            /*!
             * Builds the memory structure.
             * @note Calling this invalidates this builder.
             * @note When calling this method, the specified transitions should be deterministic and complete. This is not checked.
             */
            MemoryStructure build();
            

        private:
            MemoryStructure::TransitionMatrix transitions;
            storm::models::sparse::StateLabeling stateLabeling;
        };
        
    }
}


