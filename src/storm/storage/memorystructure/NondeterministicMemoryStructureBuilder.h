#pragma once

#include "storm/storage/memorystructure/NondeterministicMemoryStructure.h"

namespace storm {
namespace storage {

enum class NondeterministicMemoryStructurePattern { Trivial, FixedCounter, SelectiveCounter, FixedRing, SelectiveRing, SettableBits, Full };

std::string toString(NondeterministicMemoryStructurePattern const& pattern);

class NondeterministicMemoryStructureBuilder {
   public:
    // Builds a memory structure with the given pattern and the given number of states.
    NondeterministicMemoryStructure build(NondeterministicMemoryStructurePattern pattern, uint64_t numStates) const;

    // Builds a memory structure that consists of just a single memory state
    NondeterministicMemoryStructure buildTrivialMemory() const;

    // Builds a memory structure that consists of a chain of the given number of states.
    // Every state has exactly one transition to the next state. The last state has just a selfloop.
    NondeterministicMemoryStructure buildFixedCountingMemory(uint64_t numStates) const;

    // Builds a memory structure that consists of a chain of the given number of states.
    // Every state has a selfloop and a transition to the next state. The last state just has a selfloop.
    NondeterministicMemoryStructure buildSelectiveCountingMemory(uint64_t numStates) const;

    // Builds a memory structure that consists of a ring of the given number of states.
    // Every state has a transition to the successor state
    NondeterministicMemoryStructure buildFixedRingMemory(uint64_t numStates) const;

    // Builds a memory structure that consists of a ring of the given number of states.
    // Every state has a transition to the successor state and a selfloop
    NondeterministicMemoryStructure buildSelectiveRingMemory(uint64_t numStates) const;

    // Builds a memory structure that represents floor(log(numStates)) bits that can only be set from zero to one or from zero to zero.
    NondeterministicMemoryStructure buildSettableBitsMemory(uint64_t numStates) const;

    // Builds a memory structure that consists of the given number of states which are fully connected.
    NondeterministicMemoryStructure buildFullyConnectedMemory(uint64_t numStates) const;
};

}  // namespace storage
}  // namespace storm