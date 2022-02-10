#pragma once

#include <cstddef>

#include "storm/storage/sparse/StateType.h"

namespace storm {
namespace solver {
namespace stateelimination {

class StatePriorityQueue {
   public:
    virtual ~StatePriorityQueue() = default;

    virtual bool hasNext() const = 0;
    virtual storm::storage::sparse::state_type pop() = 0;
    virtual void update(storm::storage::sparse::state_type state);
    virtual std::size_t size() const = 0;
};

}  // namespace stateelimination
}  // namespace solver
}  // namespace storm
