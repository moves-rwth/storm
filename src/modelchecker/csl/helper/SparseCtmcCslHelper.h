#ifndef STORM_MODELCHECKER_SPARSE_CTMC_CSL_MODELCHECKER_HELPER_H_
#define STORM_MODELCHECKER_SPARSE_CTMC_CSL_MODELCHECKER_HELPER_H_

#include <vector>

#include "src/models/sparse/StandardRewardModel.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            template <typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
            class SparseCtmcCslHelper {
            public:

            }
        }
    }
}

#endif /* STORM_MODELCHECKER_SPARSE_CTMC_CSL_MODELCHECKER_HELPER_H_ */