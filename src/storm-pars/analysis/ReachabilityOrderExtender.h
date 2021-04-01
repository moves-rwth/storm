#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class ReachabilityOrderExtender : public OrderExtender<ValueType, ConstantType> {

        public:
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;

            ReachabilityOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

            ReachabilityOrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix);

            void checkParOnStateMonRes(uint_fast64_t s, std::shared_ptr<Order> order, typename OrderExtender<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult);

            virtual std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> toOrder(storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr) override;

            virtual std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

        protected:
            virtual std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

            virtual std::pair<uint_fast64_t, uint_fast64_t> extendNormal(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) override;

            virtual std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) override;

            virtual std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) override;

            virtual bool extendByAssumption(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t succState2, uint_fast64_t succState1) override;

            virtual void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) override;

            virtual std::shared_ptr<Order> getBottomTopOrder() override;


            MonotonicityChecker<ValueType> monotonicityChecker;
        };
    }
}