#pragma once
#include "storm-pars/analysis/OrderExtender.h"

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class ReachabilityOrderExtender : public OrderExtender<ValueType, ConstantType> {

           public:
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;

            /*!
                    * Constructs a new ReachabilityOrderExtender.
                    *
                    * @param model The model for which the reachability order should be extended.
                    * @param formula The considered formula.
             */
            ReachabilityOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

            /*!
                     * Constructs a new ReachabilityOrderExtender.
                     *
                     * @param topStates The top states of the reachability order.
                     * @param bottomStates The bottom states of the reachability order.
                     * @param matrix The matrix of the considered model.
             */
            ReachabilityOrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix);


           private:

            // Override functions

            /*!
                     * Attempts to add a state to a reachability order via Backward Reasoning
                     *
                     * @param order The considered reachability order
                     * @param currentState The current state to be added to the reachability order
                     * @param successors The state's successors TODO do we assume they are ordered here or do we order them?
                     * @param allowMerge TODO ??
                     * @return A pair of unorderable states. If no such states exist, the pair is <numberOfStates, numberOfStates>
             */
            std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) override;

            /*!
                     * Attempts to add a state to a reachability order via Forward Reasoning
                     *
                     * @param order The considered reachability order
                     * @param currentState The current state to be added to the reachability order
                     * @param successors The state's successors TODO do we assume they are ordered here or do we order them?
                     * @param allowMerge TODO ??
                     * @return A pair of unorderable states. If no such states exist, the pair is <numberOfStates, numberOfStates>
             */
            std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) override;

            /*!
                     * Handles states that only have one successor and integrates them into the reachability order
                     *
                     * @param order The reachability order the state is to be added to
                     * @param currentState The considered state
                     * @param successor The sole successor of the state
             */
            void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) override;

            /*!
                     * (If necessary, creates and) returns the initial order
                     *
                     * @return A pointer to the initial order
             */
            std::shared_ptr<Order> getInitialOrder() override;

        };
    }
}