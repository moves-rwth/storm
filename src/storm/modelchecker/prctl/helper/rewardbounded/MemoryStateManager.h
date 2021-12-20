#pragma once

#include <set>
#include <string>
#include <vector>

#include "storm/storage/BitVector.h"

namespace storm {
namespace modelchecker {
namespace helper {
namespace rewardbounded {

class MemoryStateManager {
   public:
    typedef uint64_t MemoryState;

    MemoryStateManager(uint64_t dimensionCount);

    void setDimensionWithoutMemory(uint64_t dimension);

    uint64_t const& getDimensionCount() const;
    uint64_t const& getMemoryStateCount() const;
    MemoryState const& getUpperMemoryStateBound() const;  // is larger then every valid memory state m, i.e., m < getUpperMemoryStateBound() holds for all m

    MemoryState getInitialMemoryState() const;
    bool isRelevantDimension(MemoryState const& state, uint64_t dimension) const;
    void setRelevantDimension(MemoryState& state, uint64_t dimension, bool value = true) const;
    void setRelevantDimensions(MemoryState& state, storm::storage::BitVector const& dimensions, bool value = true) const;

    std::string toString(MemoryState const& state) const;

   private:
    uint64_t const dimensionCount;
    uint64_t const dimensionBitMask;
    uint64_t const relevantBitsMask;
    uint64_t stateCount;
    uint64_t dimensionsWithoutMemoryMask;
    MemoryState const upperMemoryStateBound;
};
}  // namespace rewardbounded
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm