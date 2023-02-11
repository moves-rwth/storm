#pragma once

#include "storm-dft/modelchecker/DFTASFChecker.h"
#include "storm-dft/storage/DFT.h"

namespace storm::dft {
namespace utility {

class FailureBoundFinder {
   public:
    /**
     * Get the minimal number of BEs necessary for the TLE to fail (lower bound for number of failures to check)
     *
     * @param dft the DFT to check
     * @param useSMT if set, an SMT solver is used to improve the bounds
     * @param timeout timeout for each query in seconds, defaults to 10 seconds
     * @return the minimal number
     */
    static uint64_t getLeastFailureBound(storm::dft::storage::DFT<double> const &dft, bool useSMT = false, uint_fast64_t timeout = 10);

    static uint64_t getLeastFailureBound(storm::dft::storage::DFT<storm::RationalFunction> const &dft, bool useSMT = false, uint_fast64_t timeout = 10);

    /**
     * Get the number of BE failures for which the TLE always fails (upper bound for number of failures to check).
     *
     * @param dft the DFT to check
     * @param useSMT if set, an SMT solver is used to improve the bounds
     * @param timeout timeout for each query in seconds, defaults to 10 seconds
     * @return the number
     */
    static uint64_t getAlwaysFailedBound(storm::dft::storage::DFT<double> const &dft, bool useSMT = false, uint_fast64_t timeout = 10);

    static uint64_t getAlwaysFailedBound(storm::dft::storage::DFT<storm::RationalFunction> const &dft, bool useSMT = false, uint_fast64_t timeout = 10);

   private:
    /**
     * Helper function for correction of least failure bound when dependencies are present.
     * The main idea is to check if a later point of failure for the TLE than the pre-computed bound exists, but
     * up until that point the number of non-Markovian states visited is so large, that less than the pre-computed bound BEs fail by themselves.
     * The corrected bound is then (newTLEFailureTimepoint)-(nrNonMarkovianStatesVisited). This term is minimized.
     *
     * @param smtchecker the SMT checker to use
     * @param bound known lower bound to be corrected
     * @param timeout timeout timeout for each query in seconds
     * @return the corrected bound
     */
    static uint64_t correctLowerBound(std::shared_ptr<storm::dft::modelchecker::DFTASFChecker> smtchecker, uint64_t bound, uint_fast64_t timeout);

    /**
     * Helper function for correction of bound for number of BEs such that the DFT always fails when dependencies are present
     *
     * @param smtchecker the SMT checker to use
     * @param bound known bound to be corrected
     * @param timeout timeout timeout for each query in seconds
     * @return the corrected bound
     */
    static uint64_t correctUpperBound(std::shared_ptr<storm::dft::modelchecker::DFTASFChecker> smtchecker, uint64_t bound, uint_fast64_t timeout);
};

}  // namespace utility
}  // namespace storm::dft
