#ifndef STORM_ORDEREXTENDER_H
#define STORM_ORDEREXTENDER_H

#include "storm/api/storm.h"
#include "storm/logic/Formula.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/VariableExpression.h"

#include "storm-pars/analysis/Order.h"
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "AssumptionMaker.h"


namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        class OrderExtender {

        public:
            typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
            typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
            typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;

           /*!
            * Constructs a new OrderExtender.
            *
            * @param model The model for which the order should be extended.
            * @param formula The considered formula.
            * @param region The Region of the model's parameters.
            */
           OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula, storage::ParameterRegion<ValueType> region);

            /*!
             * Constructs a new OrderExtender.
             *
             * @param topStates The top states of the order.
             * @param bottomStates The bottom states of the order.
             * @param matrix The matrix of the considered model.
             */
           OrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix);

            /*!
             * Creates an order based on the given formula.
             *
             * @param monRes The monotonicity result so far.
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> toOrder(std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr);

            /*!
             * Extends the order based on the given assumption.
             *
             * @param order The order.
             * @param assumption The assumption on states.
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr);

            /*!
             * Extends the order for the given region.
             *
             * @param order pointer to the order.
             * @param region The region on which the order needs to be extended.
             * @return Two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced or needed.
             */
            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region);

            void setMinMaxValues(std::vector<ConstantType> &minValues, std::vector<ConstantType> &maxValues);

            void setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2);

            std::pair<uint_fast64_t, uint_fast64_t> getUnknownStates(std::shared_ptr<Order> order);

            void setUnknownStates(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy);

        // TODO changed private to protected for inheritance for now. Change later.
        protected:
            std::shared_ptr<Order> getBottomTopOrder();

            Order::NodeComparison addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) const;

            std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors) const;

            std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors);

            void handleAssumption(std::shared_ptr<Order> order, std::shared_ptr<expressions::BinaryRelationExpression> assumption) const;

            void getMinMaxValues();

            std::shared_ptr<Order> bottomTopOrder = nullptr;

            std::vector<ConstantType> minValues;

            std::vector<ConstantType> maxValues;

            storage::SparseMatrix<ValueType> matrix;

            std::shared_ptr<models::sparse::Model<ValueType>> model;

            std::map<uint_fast64_t, std::vector<uint_fast64_t>> stateMap;
            std::map<std::shared_ptr<Order>, std::pair<uint_fast64_t, uint_fast64_t>> unknownStatesMap;
            std::map<std::shared_ptr<Order>, std::pair<uint_fast64_t, uint_fast64_t>> lastUnknownStatesMap;


            bool usePLA;

            bool cyclic;

            std::shared_ptr<logic::Formula const> formula;

            storage::ParameterRegion<ValueType> region;

            uint_fast64_t numberOfStates;

            Monotonicity checkTransitionMonRes(ValueType function, typename OrderExtender<ValueType, ConstantType>::VariableType param);

            void checkParOnStateMonRes(uint_fast64_t s, const std::vector<uint_fast64_t>& succ, typename OrderExtender<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult);

            std::set<VariableType> params;

            ValueType getDerivative(ValueType function, VariableType param);

            std::unordered_map<ValueType, std::unordered_map<VariableType, ValueType>> derivatives;

            analysis::AssumptionMaker<ValueType, ConstantType>* assumptionMaker;
        };
    }
}

#endif //STORM_ORDEREXTENDER_H
