#include "storm/models/sparse/Pomdp.h"
#include "storm/storage/BitVector.h"

namespace storm {
    namespace generator {
        template<typename ValueType>
        class BeliefSupportTracker {
            /**
             * Tracks the current belief support.
             * TODO refactor into tracker and generator.
             * @param pomdp
             */
        public:
            BeliefSupportTracker(storm::models::sparse::Pomdp<ValueType> const& pomdp);

            storm::storage::BitVector const& getCurrentBeliefSupport() const;

            void track(uint64_t action, uint64_t observation);

        private:
            storm::models::sparse::Pomdp<ValueType> const& pomdp;
            storm::storage::BitVector currentBeliefSupport;

        };
    }
}