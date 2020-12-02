#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class OrderExtenderDtmc : public OrderExtender<ValueType, ConstantType> {
            public:
                OrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region);

                OrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix);

            private:
            std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState) override;
        };
    }
}