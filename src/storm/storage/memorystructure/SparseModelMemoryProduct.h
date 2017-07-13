#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "storm/storage/BitVector.h"
#include "storm/storage/memorystructure/MemoryStructure.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace storage {
        /*!
         * This class builds the product of the given sparse model and the given memory structure.
         * This is similar to the well-known product of a model with a deterministic rabin automaton.
         * The product contains only the reachable states of the product
         *
         * The states of the resulting sparse model will have the original state labels plus the labels of this
         * memory structure.
         * An exception is thrown if the state labelings are not disjoint.
         */
        template <typename ValueType>
        class SparseModelMemoryProduct {
        public:
            
            SparseModelMemoryProduct(storm::models::sparse::Model<ValueType> const& sparseModel, storm::storage::MemoryStructure const& memoryStructure);
            
            // Enforces that the given model and memory state as well as the successor(s) are considered reachable -- even if they are not reachable from an initial state.
            void addReachableState(uint64_t const& modelState, uint64_t const& memoryState);
    
            // Enforces that every state is considered reachable. If this is set, the result has size #modelStates * #memoryStates
            void setBuildFullProduct();
            
            // Invokes the building of the product
            std::shared_ptr<storm::models::sparse::Model<ValueType>> build();
            
            // Retrieves the state of the resulting model that represents the given memory and model state.
            // Should only be called AFTER calling build();
            // An invalid index is returned, if the specifyied state does not exist (i.e., if it is not part of product).
            uint64_t const& getResultState(uint64_t const& modelState, uint64_t const& memoryState) const;
            
        private:
            
            // Computes for each pair of memory state and model transition index the successor memory state
            // The resulting vector maps (modelTransition * memoryStateCount) + memoryState to the corresponding successor state of the memory structure
            std::vector<uint64_t> computeMemorySuccessors() const;
            
            // Computes the reachable states of the resulting model
            void computeReachableStates(std::vector<uint64_t> const& memorySuccessors, storm::storage::BitVector const& initialStates);
            
            // Methods that build the model components
            // Matrix for deterministic models
            storm::storage::SparseMatrix<ValueType> buildDeterministicTransitionMatrix(std::vector<uint64_t> const& memorySuccessors) const;
            // Matrix for nondeterministic models
            storm::storage::SparseMatrix<ValueType> buildNondeterministicTransitionMatrix(std::vector<uint64_t> const& memorySuccessors) const;
            // State labeling. Note: DOES NOT ADD A LABEL FOR THE INITIAL STATES
            storm::models::sparse::StateLabeling buildStateLabeling(storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix) const;
            // Reward models
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> buildRewardModels(storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix, std::vector<uint64_t> const& memorySuccessors) const;
            
            // Builds the resulting model
            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildResult(storm::storage::SparseMatrix<ValueType>&& matrix, storm::models::sparse::StateLabeling&& labeling, std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>>&& rewardModels) const;
            
            
            // Maps (modelState * memoryStateCount) + memoryState to the state in the result that represents (memoryState,modelState)
            std::vector<uint64_t> toResultStateMapping;
            
            // Indicates which states are considered reachable. (s, m) is reachable if this BitVector is true at (s * memoryStateCount) + m
            storm::storage::BitVector reachableStates;
            
            storm::models::sparse::Model<ValueType> const& model;
            storm::storage::MemoryStructure const& memory;
            
        };
    }
}


