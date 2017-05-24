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
         * Note: we already do one memory-structure-step for the initial state, i.e., if s is an initial state of
         * the given model and s satisfies memoryStructure.getTransitionMatrix[0][n], then (s,n) will be the corresponding
         * initial state of the resulting model.
         *
         * The states of the resulting sparse model will have the original state labels plus the labels of this
         * memory structure.
         * An exception is thrown if the state labelings are not disjoint.
         */
        template <typename ValueType>
        class SparseModelMemoryProduct {
        public:
            
            SparseModelMemoryProduct(storm::models::sparse::Model<ValueType> const& sparseModel, storm::storage::MemoryStructure const& memoryStructure);
            
            // Invokes the building of the product
            std::shared_ptr<storm::models::sparse::Model<ValueType>> build();
            
            // Retrieves the state of the resulting model that represents the given memory and model state.
            // An invalid index is returned, if the specifyied state does not exist (i.e., if it is not reachable).
            uint_fast64_t const& getResultState(uint_fast64_t const& modelState, uint_fast64_t const& memoryState) const;
            
        private:
            
            // Computes for each pair of memory and model state the successor memory state
            // The resulting vector maps (modelState * memoryStateCount) + memoryState to the corresponding successor state of the memory structure
            std::vector<uint_fast64_t> computeMemorySuccessors() const;
            
            // Computes the reachable states of the resulting model
            storm::storage::BitVector computeReachableStates(std::vector<uint_fast64_t> const& memorySuccessors, storm::storage::BitVector const& initialStates) const;
            
            // Methods that build the model components
            // Matrix for deterministic models
            storm::storage::SparseMatrix<ValueType> buildDeterministicTransitionMatrix(storm::storage::BitVector const& reachableStates, std::vector<uint_fast64_t> const& memorySuccessors) const;
            // Matrix for nondeterministic models
            storm::storage::SparseMatrix<ValueType> buildNondeterministicTransitionMatrix(storm::storage::BitVector const& reachableStates, std::vector<uint_fast64_t> const& memorySuccessors) const;
            // State labeling. Note: DOES NOT ADD A LABEL FOR THE INITIAL STATES
            storm::models::sparse::StateLabeling buildStateLabeling(storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix) const;
            // Reward models
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> buildRewardModels(storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix, std::vector<uint_fast64_t> const& memorySuccessors) const;
            
            // Builds the resulting model
            std::shared_ptr<storm::models::sparse::Model<ValueType>> buildResult(storm::storage::SparseMatrix<ValueType>&& matrix, storm::models::sparse::StateLabeling&& labeling, std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>>&& rewardModels) const;
            
            
            // Maps (modelState * memoryStateCount) + memoryState to the state in the result that represents (memoryState,modelState)
            std::vector<uint_fast64_t> toResultStateMapping;
            

            storm::models::sparse::Model<ValueType> const& model;
            storm::storage::MemoryStructure const& memory;
            
        };
    }
}


