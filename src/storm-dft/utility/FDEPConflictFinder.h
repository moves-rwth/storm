#include <vector>
#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/modelchecker/dft/DFTASFChecker.h"

namespace storm {
    namespace dft {
        namespace utility {
            class FDEPConflictFinder {
            public:
                /**
                 * Get a vector of index pairs of FDEPs in the DFT which are conflicting. Two FDEPs are conflicting if
                 * their simultaneous triggering may cause unresolvable non-deterministic behavior.
                 *
                 * @param dft the DFT
                 * @param useSMT if set, an SMT solver is used to refine the conflict set
                 * @param timeout timeout for each query in seconds, defaults to 10 seconds
                 * @return a vector of pairs of indices. The indices in a pair refer to FDEPs which are conflicting
                 */
                static std::vector<std::pair<uint64_t, uint64_t>>
                getDependencyConflicts(storm::storage::DFT<double> const &dft,
                                       bool useSMT = false, uint_fast64_t timeout = 10);

                static std::vector<std::pair<uint64_t, uint64_t>>
                getDependencyConflicts(storm::storage::DFT<storm::RationalFunction> const &dft,
                                       bool useSMT = false, uint_fast64_t timeout = 10);
            };
        }
    }
}
