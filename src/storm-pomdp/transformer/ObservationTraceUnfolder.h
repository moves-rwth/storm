#include "storm/models/sparse/Pomdp.h"

namespace storm {
    namespace pomdp {
        template<typename ValueType>
        class ObservationTraceUnfolder {

        public:
            ObservationTraceUnfolder(storm::models::sparse::Pomdp<ValueType> const& model);
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> transform(std::vector<uint32_t> const& observations, std::vector<ValueType> const& risk);
        private:
            storm::models::sparse::Pomdp<ValueType> const& model;
            std::vector<storm::storage::BitVector> statesPerObservation;

        };

    }
}