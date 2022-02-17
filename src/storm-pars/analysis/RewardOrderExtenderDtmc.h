#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class RewardOrderExtenderDtmc : public OrderExtender<ValueType, ConstantType> {


            /*!
             * Constants for the comparison of states via SMT checker
             */
            enum StateComparison {
                GEQ,
                LEQ,
                UNKNOWN,
            };

           public:
            typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
            typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;

            /*!
             * Constructs a new RewardOrderExtender.
             *
             * @param model The model for which the order should be extended.
             * @param formula The considered formula.
             */
            RewardOrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

            /*!
             * Constructs a new RewardOrderExtender.
             *
             * @param topStates The top states of the reward order.
             * @param bottomStates The bottom states of the reward order.
             * @param matrix The matrix of the considered model.
             */
            RewardOrderExtenderDtmc(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix, storm::models::sparse::StandardRewardModel<ValueType> rewardModel);



            // Override methods from OrderExtender
            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) override;

           protected:
            std::shared_ptr<Order> getInitialOrder(bool isOptimistic) override;
            std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) override;
            std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order,storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) override;

           private:

            /*!
             * Inserts a state into an order which has only one successor
             * @param order The considered order
             * @param currentState The state to be added to the order
             * @param successor The sole successor of the state
             */
            void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) override;


            /*** Model Info ***/
            // Reward model of our model
            storm::models::sparse::StandardRewardModel<ValueType> rewardModel;
            storage::SparseMatrix<ValueType> transposeMatrix;

        };
    }
}