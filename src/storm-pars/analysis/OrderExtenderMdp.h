#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class OrderExtenderMdp : public OrderExtender<ValueType, ConstantType> {
            public:
                OrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region, bool prMax = true);

                OrderExtenderMdp(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax = true);

            private:
                std::map<uint_fast64_t, std::vector<std::vector<uint_fast64_t>>> mdpStateMap;

                storm::storage::BitVector gatherPotentialSuccs(uint64_t state);

                void initMdpStateMap();

                // TODO can't really do override with different parameter signature (bc of prMax bool) (also this version doesnt need the successor param). How to do this?
                std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState);

                bool prMax;
        };
    }
}