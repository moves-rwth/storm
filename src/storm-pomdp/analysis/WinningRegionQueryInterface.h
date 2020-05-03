#pragma once
#include "storm/models/sparse/Pomdp.h"
#include "storm-pomdp/analysis/WinningRegion.h"


namespace storm {
    namespace pomdp {
        template<typename ValueType>
        class WinningRegionQueryInterface {
        public:
            WinningRegionQueryInterface(storm::models::sparse::Pomdp<ValueType> const& pomdp, WinningRegion const& winningRegion);

            bool isInWinningRegion(storm::storage::BitVector const& beliefSupport) const;

            bool staysInWinningRegion(storm::storage::BitVector const& beliefSupport, uint64_t actionIndex) const;

            void validate() const;
        private:
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            WinningRegion const& winningRegion;
            // TODO consider sharing this.
            std::vector<std::vector<uint64_t>> statesPerObservation;
        };
    }
}