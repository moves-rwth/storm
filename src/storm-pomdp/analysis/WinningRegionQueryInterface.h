#pragma once
#include "storm-pomdp/analysis/WinningRegion.h"
#include "storm/models/sparse/Pomdp.h"

namespace storm {
namespace pomdp {
template<typename ValueType>
class WinningRegionQueryInterface {
   public:
    WinningRegionQueryInterface(storm::models::sparse::Pomdp<ValueType> const& pomdp, WinningRegion const& winningRegion);

    bool isInWinningRegion(storm::storage::BitVector const& beliefSupport) const;

    bool staysInWinningRegion(storm::storage::BitVector const& beliefSupport, uint64_t actionIndex) const;

    void validate(storm::storage::BitVector const& badStates) const;

    void validateIsMaximal(storm::storage::BitVector const& badStates) const;

   private:
    storm::models::sparse::Pomdp<ValueType> const& pomdp;
    WinningRegion const& winningRegion;
    // TODO consider sharing this.
    std::vector<std::vector<uint64_t>> statesPerObservation;
};
}  // namespace pomdp
}  // namespace storm