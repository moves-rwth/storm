#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <vector>
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {
namespace storage {
// Data to restore memory incorporation applied with SparseModelMemoryProduct
// Only stores the least necessary to reverse data from SparseModelMemoryProduct
struct SparseModelMemoryProductReverseData {
    storm::storage::MemoryStructure memory;
    // Maps (modelState * memoryStateCount) + memoryState to the state in the result that represents (memoryState,modelState)
    std::vector<uint64_t> toResultStateMapping;

    SparseModelMemoryProductReverseData(storm::storage::MemoryStructure const& memory, std::vector<uint64_t> const& toResultStateMapping)
        : memory(memory), toResultStateMapping(toResultStateMapping) {
        // Intentionally left empty
    }
};

}  // namespace storage
}  // namespace storm
