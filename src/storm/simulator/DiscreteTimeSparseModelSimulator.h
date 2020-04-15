#include <cstdint>
#include "storm/models/sparse/Model.h"
#include "storm/utility/random.h"

namespace storm {
    namespace simulator {

        /**
         * This class is a low-level interface to quickly sample from Discrete-Time Models
         * stored explicitly as a SparseModel.
         * Additional information about state, actions, should be obtained via the model itself.
         *
         * TODO: It may be nice to write a CPP wrapper that does not require to actually obtain such informations yourself.
         * @tparam ModelType
         */
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class DiscreteTimeSparseModelSimulator {
        public:
            DiscreteTimeSparseModelSimulator(storm::models::sparse::Model<ValueType, RewardModelType> const& model);
            void setSeed(uint64_t);
            bool step(uint64_t action);
            uint64_t getCurrentState() const;
            bool resetToInitial();
        protected:
            uint64_t currentState;
            storm::models::sparse::Model<ValueType, RewardModelType> const& model;
            storm::utility::RandomProbabilityGenerator<ValueType> generator;
        };
    }
}