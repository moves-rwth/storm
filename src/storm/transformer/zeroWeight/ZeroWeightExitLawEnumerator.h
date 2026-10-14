#pragma once

#include <cstdint>
#include <vector>

#include "storm/transformer/zeroWeight/ZeroWeightComponentLp.h"

namespace storm::transformer {

/*!
 * Finds the vertices of a component entry's boundary exit laws.
 *
 * @tparam ValueType The probability type.
 */
template<typename ValueType>
class ZeroWeightExitLawEnumerator {
   public:
    /*!
     * Uses an existing component LP for support queries.
     *
     * @param componentLp The LP for the split zero-weight component.
     */
    explicit ZeroWeightExitLawEnumerator(ZeroWeightComponentLp<ValueType>& componentLp);

    /*!
     * Returns the exit-law vertices in boundary-state order.
     * Grows a hull from feasible exit laws and checks each facet with an LP support query.
     * @see https://doi.org/10.48550/arXiv.1808.01786
     *
     * @param entryState An entry state of the component.
     * @return The vertices, ordered lexicographically.
     */
    std::vector<std::vector<ValueType>> enumerate(uint64_t entryState);

   private:
    ZeroWeightComponentLp<ValueType>& componentLp;
};

}  // namespace storm::transformer
