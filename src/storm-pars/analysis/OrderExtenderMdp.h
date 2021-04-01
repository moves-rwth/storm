#pragma once
#include "storm-pars/analysis/ReachabilityOrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class OrderExtenderMdp : public ReachabilityOrderExtender<ValueType, ConstantType> {
            public:
                typedef typename utility::parametric::VariableType<ValueType>::type VariableType;

                enum ActionComparison {
                    GEQ,
                    LEQ,
                    UNKNOWN,
                };

                OrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, bool prMax = true);

                OrderExtenderMdp(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax = true);

            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

            private:

                storm::storage::BitVector gatherPotentialSuccs(uint64_t state);

                std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

                std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState) override;

                /*!
                 * Compares two rational functions
                 * @param f1 The first rational function
                 * @param f2 The second reational function
                 * @param region The region for parameters
                 * @return true iff The first function is greater or equal to the second one
                 */
                bool isFunctionGreaterEqual(storm::RationalFunction f1, storm::RationalFunction f2, storage::ParameterRegion<ValueType> region);

                std::pair<uint64_t, uint64_t> rangeOfSuccsForAction(typename storage::SparseMatrix<ValueType>::rows* action, std::vector<uint64_t> orderedSuccs);

                storage::BitVector getHitSuccs(uint64_t state, uint64_t action, std::vector<uint64_t> orderedSuccs);

                std::pair<bool, uint64_t> simpleCaseCheck(uint64_t state, std::vector<uint64_t> orderedSuccs);

                OrderExtenderMdp::ActionComparison actionSmtCompare(typename storage::SparseMatrix<ValueType>::rows* action1, typename storage::SparseMatrix<ValueType>::rows* action2, std::vector<uint64_t> orderedSuccs, std::shared_ptr<Order> order);

                bool prMax{};


        };
    }
}