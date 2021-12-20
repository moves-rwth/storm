#pragma once

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/storage/memorystructure/NondeterministicMemoryStructure.h"

namespace storm {
namespace storage {

template<typename SparseModelType>
class SparseModelNondeterministicMemoryProduct {
   public:
    typedef typename SparseModelType::ValueType ValueType;

    SparseModelNondeterministicMemoryProduct(SparseModelType const& model, storm::storage::NondeterministicMemoryStructure const& memory);

    std::shared_ptr<SparseModelType> build() const;

   private:
    storm::storage::SparseMatrix<ValueType> buildTransitions() const;
    storm::models::sparse::StateLabeling buildStateLabeling() const;
    storm::models::sparse::StandardRewardModel<ValueType> buildRewardModel(storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel,
                                                                           storm::storage::BitVector const& reachableStates) const;

    // Disabled because Markov automata currently not supported
    // std::vector<ValueType> buildExitRateVector(storm::models::sparse::MarkovAutomaton<ValueType> const& modelAsMA, storm::storage::BitVector const&
    // reachableStates) const; storm::storage::BitVector buildMarkovianStateLabeling(storm::models::sparse::MarkovAutomaton<ValueType> const& modelAsMA,
    // storm::storage::BitVector const& reachableStates) const;

    uint64_t getProductState(uint64_t modelState, uint64_t memoryState) const;
    uint64_t getModelState(uint64_t productState) const;
    uint64_t getMemoryState(uint64_t productState) const;

    SparseModelType const& model;
    storm::storage::NondeterministicMemoryStructure const& memory;
};
}  // namespace storage
}  // namespace storm