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

                storm::storage::BitVector gatherPotentialSuccs(uint64_t state);


                // TODO can't really do override with different parameter signature (also this version doesnt need the successor param). How to do this?
                std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState);

                /*!
                 * Compares two rational functions
                 * @param f1 The first rational function
                 * @param f2 The second reational function
                 * @param region The region for parameters
                 * @return true iff The first function is greater or equal to the second one
                 */
                bool isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2, storage::ParameterRegion<ValueType> region);

                bool prMax;
        };
    }
}