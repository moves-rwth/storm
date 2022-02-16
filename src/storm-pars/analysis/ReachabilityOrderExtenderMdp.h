#pragma once
#include "storm-pars/analysis/ReachabilityOrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class ReachabilityOrderExtenderMdp : public ReachabilityOrderExtender<ValueType, ConstantType> {
            public:
                typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
                typedef typename storage::SparseMatrix<ValueType>::rows* Rows;

                enum ActionComparison {
                    GEQ,
                    LEQ,
                    UNKNOWN,
                };

                ReachabilityOrderExtenderMdp(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, bool prMax);

                ReachabilityOrderExtenderMdp(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax);

                std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

            protected:
                void addInitialStatesMinMax(std::shared_ptr<Order> order) override;

            private:

                storm::storage::BitVector gatherPotentialSuccs(uint64_t state);

                // Determines which choice to take, and then calls its parents extendByBackwardReasoning
                std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) override;

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

                ReachabilityOrderExtenderMdp::ActionComparison actionSMTCompare(std::shared_ptr<Order> order, std::vector<uint64_t> const& orderedSuccs, storage::ParameterRegion<ValueType>& region, Rows action1, Rows action2);

                bool prMax{};


        };
    }
}