#pragma once

#include <vector>
#include <set>
#include <string>

#include "storm/storage/BitVector.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            class MemoryStateManager {
            public:
                
                typedef uint64_t MemoryState;
                
                MemoryStateManager(uint64_t dimensionCount);
                
                uint64_t const& getDimensionCount() const;
                uint64_t const& getMemoryStateCount() const;
                
                MemoryState getInitialMemoryState() const;
                bool isRelevantDimension(MemoryState const& state, uint64_t dimension) const;
                void setRelevantDimension(MemoryState& state, uint64_t dimension, bool value = true) const;
                void setRelevantDimensions(MemoryState& state, storm::storage::BitVector const& dimensions, bool value = true) const;
                
                std::string toString(MemoryState const& state) const;
                
            private:
                uint64_t const dimensionCount;
                uint64_t const dimensionBitMask;
                uint64_t const relevantBitsMask;
                uint64_t const stateCount;

            };
        }
    }
}