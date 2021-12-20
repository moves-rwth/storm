#pragma once

#include <cstdint>

#include "storm/storage/BitVectorHashMap.h"

namespace storm {
namespace storage {
namespace sparse {

// A structure holding information about the reachable state space while building it.
template<typename StateType>
struct StateStorage {
    // Creates an empty state storage structure for storing states of the given bit width.
    StateStorage(uint64_t bitsPerState);

    // This member stores all the states and maps them to their unique indices.
    storm::storage::BitVectorHashMap<StateType> stateToId;

    // A list of initial states in terms of their global indices.
    std::vector<StateType> initialStateIndices;

    // A list of deadlock states.
    std::vector<StateType> deadlockStateIndices;

    // The number of bits of each state.
    uint64_t bitsPerState;

    // Get the number of states that were found in the exploration so far.
    uint64_t getNumberOfStates() const;
};

}  // namespace sparse
}  // namespace storage
}  // namespace storm
