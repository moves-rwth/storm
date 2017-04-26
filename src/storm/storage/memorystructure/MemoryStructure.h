#pragma once

#include <vector>
#include <memory>

#include "storm/logic/Formula.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/models/sparse/Model.h"

namespace storm {
    namespace storage {
        
        template <typename ValueType>
        class SparseModelMemoryProduct;
        
        /*!
         * This class represents a (deterministic) memory structure that can be used to encode certain events
         * (such as reaching a set of goal states) into the state space of the model.
         */
        class MemoryStructure {
        public:
            
            typedef std::vector<std::vector<std::shared_ptr<storm::logic::Formula const>>> TransitionMatrix;
            
            /*!
             * Creates a memory structure with the given transition matrix and the given memory state labeling.
             * The initial state is always the state with index 0.
             * The transition matrix is assumed to contain propositional state formulas. The entry
             * transitionMatrix[m][n] specifies the set of model states which trigger a transition from memory
             * state m to memory state n.
             * Transitions are assumed to be deterministic and complete, i.e., the formulas in
             * transitionMatrix[m] form a partition of the state space of the considered model.
             *
             * @param transitionMatrix The transition matrix
             * @param memoryStateLabeling A labeling of the memory states to specify, e.g., accepting states
             */
            MemoryStructure(TransitionMatrix const& transitionMatrix, storm::models::sparse::StateLabeling const& memoryStateLabeling);
            MemoryStructure(TransitionMatrix&& transitionMatrix, storm::models::sparse::StateLabeling&& memoryStateLabeling);
            
            TransitionMatrix const& getTransitionMatrix() const;
            storm::models::sparse::StateLabeling const& getStateLabeling() const;
            uint_fast64_t getNumberOfStates() const;
            
            /*!
             * Builds the product of this memory structure and the given memory structure.
             * The resulting memory structure will have the state labels of both given structures.
             * Throws an exception if the state labelings are not disjoint.
             */
            MemoryStructure product(MemoryStructure const& rhs) const;
            
            /*!
             * Builds the product of this memory structure and the given sparse model.
             * An exception is thrown if the state labelings of this memory structure and the given model are not disjoint.
             */
            template <typename ValueType>
            SparseModelMemoryProduct<ValueType> product(storm::models::sparse::Model<ValueType> const& sparseModel) const;

        private:
            TransitionMatrix transitions;
            storm::models::sparse::StateLabeling stateLabeling;
        };
        
    }
}


