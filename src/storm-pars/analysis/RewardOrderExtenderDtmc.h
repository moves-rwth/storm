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
            RewardOrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix);


            /*!
             * Determines the relation of a state to one of its successor in an order
             *
             * @param state The considered state
             * @param succ The considered successor
             * @param order The considered reward order
             * @param hypothesis Optional hypothesis to be checked first. If none or UNKNOWN is given, we check GEQ first
             * @return Enum representing the relation between the two states
             */
            StateComparison compareStateWithSuccSmt(uint_fast64_t state, uint_fast64_t succ, std::shared_ptr<Order> order, StateComparison hypothesis = UNKNOWN);


            // Moved to public for testing

            /*!
             * Attempts to add a state to a reward order via Backward Reasoning
             *
             * @param order The considered reward order
             * @param currentState The current state to be added to the reward order
             * @param successors The state's successors (not necessarily ordered)
             * @param allowMerge Boolean to allow merging of states when assumptions are used
             * @return A pair of unorderable states. If no such states exist, the pair is <numberOfStates, numberOfStates>
             */
            std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order,storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) override;

            /*!
             * (If necessary, creates and) returns the initial order
             *
             * @return A pointer to the initial order
             */
            std::shared_ptr<Order> getInitialOrder() override;

           private:
            // Not implemented yet but here so that this class is not abstract. Document when implemented!
            std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) override;

            /*!
             * Inserts a state into an order which has only one successor
             * @param order The considered order
             * @param currentState The state to be added to the order
             * @param successor The sole successor of the state
             */
            void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) override;


            /*** Model Info ***/
            // Reward model of our model
            storm::models::sparse::StandardRewardModel<ValueType> rewardModel; // TODO is valueType correct?
        };
    }
}