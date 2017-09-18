#include "storm/modelchecker/multiobjective/rewardbounded/MemoryStateManager.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/IllegalArgumentException.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            MemoryStateManager::MemoryStateManager(uint64_t dimensionCount) : dimensionCount(dimensionCount), dimensionBitMask(1ull), relevantBitsMask((1ull << dimensionCount) - 1), stateCount(dimensionBitMask << dimensionCount) {
                STORM_LOG_THROW(dimensionCount > 0, storm::exceptions::IllegalArgumentException, "Invoked MemoryStateManager with zero dimension count.");
                STORM_LOG_THROW(dimensionCount <= 64, storm::exceptions::IllegalArgumentException, "Invoked MemoryStateManager with too many dimensions.");
            }
            
            uint64_t const& MemoryStateManager::getDimensionCount() const {
                return dimensionCount;
            }
            
            uint64_t const& MemoryStateManager::getMemoryStateCount() const {
                return stateCount;
            }
            
            MemoryStateManager::MemoryState MemoryStateManager::getInitialMemoryState() const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked MemoryStateManager with zero dimension count.");
                return relevantBitsMask;
            }
            
            bool MemoryStateManager::isRelevantDimension(MemoryState const& state, uint64_t dimension) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked MemoryStateManager with zero dimension count.");
                return (state & (dimensionBitMask << dimension)) != 0;
            }
            
            void MemoryStateManager::setRelevantDimension(MemoryState& state, uint64_t dimension, bool value) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked MemoryStateManager with zero dimension count.");
                STORM_LOG_ASSERT(dimension < dimensionCount, "Tried to set a dimension that is larger then the number of considered dimensions");
                if (value) {
                    state |= (dimensionBitMask << dimension);
                } else {
                    state &= ~(dimensionBitMask << dimension);
                }
                assert(state < getMemoryStateCount());
            }
            
            void MemoryStateManager::setRelevantDimensions(MemoryState& state, storm::storage::BitVector const& dimensions, bool value) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked MemoryStateManager with zero dimension count.");
                STORM_LOG_ASSERT(dimensions.size() == dimensionCount, "Invalid size of given bitset.");
                if (value) {
                    for (auto const& d : dimensions) {
                        state |= (dimensionBitMask << d);
                    }
                } else {
                    for (auto const& d : dimensions) {
                        state &= ~(dimensionBitMask << d);
                    }
                }
                assert(state < getMemoryStateCount());
            }
            
            std::string MemoryStateManager::toString(MemoryState const& state) const {
                STORM_LOG_ASSERT(dimensionCount > 0, "Invoked EpochManager with zero dimension count.");
                std::string res = "[";
                res += (isRelevantDimension(state, 0) ? "1" : "0");
                for (uint64_t d = 1; d < dimensionCount; ++d) {
                    res += ", ";
                    res += (isRelevantDimension(state, d) ? "1" : "0");
                }
                res += "]";
                return res;
            }
                
        }
    }
}