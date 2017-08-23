#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace analysis {
        template<typename ValueType>
        class UniqueObservationStates {
            UniqueObservationStates(storm::models::sparse::Pomdp<ValueType> const& pomdp);
            storm::storage::BitVector analyse() const;

            storm::models::sparse::Pomdp<ValueType> const& pomdp;
        };
    }
}

