#pragma once
#include "storm-pars/analysis/ReachabilityOrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class ReachabilityOrderExtenderDtmc : public ReachabilityOrderExtender<ValueType, ConstantType> {
            public:
                typedef typename utility::parametric::VariableType<ValueType>::type VariableType;

                ReachabilityOrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, bool useAssumptions = true);

                ReachabilityOrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool useAssumptions = true);

                // Override methods from OrderExtender
                std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

            protected:
                // Override methods from OrderExtender
                void addInitialStatesMinMax(std::shared_ptr<Order> order) override;

        };
    }
}