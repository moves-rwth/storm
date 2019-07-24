//
// Created by Florent Delgrange on 2019-05-28.
//

#ifndef STORM_SPARSEMODELNONDETERMINISTICTRANSITIONSBASEDMEMORYPRODUCT_H
#define STORM_SPARSEMODELNONDETERMINISTICTRANSITIONSBASEDMEMORYPRODUCT_H

#include <storm/models/sparse/StateLabeling.h>
#include <storm/storage/SparseMatrix.h>
#include <storm/models/sparse/StandardRewardModel.h>
#include <storm/storage/memorystructure/NondeterministicMemoryStructure.h>
#include <storm/storage/sparse/ModelComponents.h>
#include <storm/utility/graph.h>
#include <storm/exceptions/NotSupportedException.h>
#include <storm/models/sparse/Mdp.h>

namespace storm {
    namespace storage {


        template<typename SparseModelType>
        class SparseModelNondeterministicTransitionsBasedMemoryProduct {
        public:

            typedef typename SparseModelType::ValueType ValueType;

            SparseModelNondeterministicTransitionsBasedMemoryProduct(SparseModelType const& model, storm::storage::NondeterministicMemoryStructure const& memory, bool forceLabeling = false);

            std::shared_ptr<SparseModelType> build();

            uint64_t getProductState(uint64_t modelState, uint64_t memoryState) const;
            uint64_t getModelState(uint64_t productState) const;
            uint64_t getMemoryState(uint64_t productState) const;
            bool isProductStateReachable(uint64_t modelState, uint64_t memoryState) const;

        private:
            storm::storage::SparseMatrix<ValueType> buildTransitions();
            storm::models::sparse::StateLabeling buildStateLabeling() const;
            storm::models::sparse::ChoiceLabeling buildChoiceLabeling(storm::storage::SparseMatrix<ValueType> const& transitions) const;
            storm::models::sparse::StandardRewardModel<ValueType> buildRewardModel(storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel, storm::storage::BitVector const& reachableStates, storm::storage::SparseMatrix<ValueType> const& resultTransitionMatrix) const;

            std::vector<uint64_t> generateOffsetVector(storm::storage::BitVector const& reachableStates);

            SparseModelType const& model;
            storm::storage::NondeterministicMemoryStructure const& memory;
            std::vector<uint64_t> productStates; // has a size equals to the number of states of the original model
            std::vector<uint64_t> fullProductStatesOffset; // has a size equal to the number of states of the full product
            storm::storage::BitVector reachableStates;
            bool forceLabeling;
        };

    }
}


#endif //STORM_SPARSEMODELNONDETERMINISTICTRANSITIONSBASEDMEMORYPRODUCT_H
