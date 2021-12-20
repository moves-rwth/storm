#pragma once

#include <vector>

#include "storm/solver/stateelimination/StatePriorityQueue.h"

namespace storm {
namespace solver {
namespace stateelimination {

class StaticStatePriorityQueue : public StatePriorityQueue {
   public:
    StaticStatePriorityQueue(std::vector<storm::storage::sparse::state_type> const& sortedStates);

    virtual bool hasNext() const override;
    virtual storm::storage::sparse::state_type pop() override;
    virtual std::size_t size() const override;

   private:
    std::vector<uint_fast64_t> sortedStates;
    uint_fast64_t currentPosition;
};

}  // namespace stateelimination
}  // namespace solver
}  // namespace storm
