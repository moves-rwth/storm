#pragma once

#include <ostream>

#include "storm-dft/storage/DFT.h"

namespace storm::dft {
namespace utility {

template<typename ValueType>
class DftValidator {
   public:
    /*!
     * Check whether the DFT is well-formed.
     * A DFT is well-formed if it satisfies several constraints such as
     * - graph is acyclic
     * - valid threshold for VOT
     * - SEQ and PDEP are at least binary and hove no parents
     * - etc.
     * @param dft DFT
     * @param stream Output stream which contains additional information if the DFT is well-formed.
     * @return True iff the DFT is well-formed.
     */
    static bool isDftWellFormed(storm::dft::storage::DFT<ValueType> const& dft, std::ostream& stream);

    /*!
     * Check whether the DFT can be analysed by translation to a Markov model.
     * Such as DFT is sometimes called conventional.
     * A DFT can be analysed if it satisfies several constraints:
     * - it is well-formed
     * - all BE failure distributions are exponential distributions
     * - spare modules are independent
     * @param dft DFT
     * @param stream Output stream which contains additional information if the DFT is invalid for Markovian analysis.
     * @return True iff the DFT is valid for Markovian analysis.
     */
    static bool isDftValidForMarkovianAnalysis(storm::dft::storage::DFT<ValueType> const& dft, std::ostream& stream);
};

}  // namespace utility
}  // namespace storm::dft