#ifndef STORM_ORDEREXTENDER_H
#define STORM_ORDEREXTENDER_H

#include <boost/container/flat_set.hpp>
#include "storm/api/storm.h"
#include "storm/logic/Formula.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/VariableExpression.h"

#include "storm-pars/analysis/Order.h"
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/analysis/MonotonicityChecker.h"
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
            * @param useAssumptions Whether assumptions can be made.
            */
           OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula);

            /*!
             * Constructs a new OrderExtender.
             *
             * @param topStates The top states of the order.
             * @param bottomStates The bottom states of the order.
             * @param matrix The matrix of the considered model.
             * @param useAssumptions Whether assumptions can be made.
             */
           OrderExtender(storm::storage::BitVector& topStates,  storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix);

            /*!
             * Creates an order based on the given formula.
             *
             * @param region The region for the order.
             * @param isOptimistic Boolean if optimistic order or normal order should be build
             * @param monRes The monotonicity result so far.
             * @return A triple with a pointer to the order and two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced but needed.
             */
            std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> toOrder(storage::ParameterRegion<ValueType> region, bool isOptimistic, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr);

            /*!
             * Extends the order for the given region.
             *
             * @param order Pointer to the order.
             * @param region The region on which the order needs to be extended.
             * @return Two states of which the current place in the order
             *         is unknown but needed. When the states have as number the number of states, no states are
             *         unplaced or needed.
             */
            virtual std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr, std::shared_ptr<expressions::BinaryRelationExpression> assumption = nullptr) = 0;


            /**
             * Initializes the min max values for a given region.
             * Stored for the given order, if the order is provided.
             *
             * @param region Region for which we compute the min max values.
             * @param order Order to which the min max values apply
             */
            void initializeMinMaxValues(storage::ParameterRegion<ValueType> region, std::shared_ptr<Order> order = nullptr);

            /**
             * Sets the min max values.
             * Stored for the given order, if the order is provided.
             * Expecting at least minValues or maxValues to be initialized.
             *
             * @param minValues Min values
             * @param maxValues Max values
             * @param order Order to which the min max values apply
             */
            void setMinMaxValues(boost::optional<std::vector<ConstantType>&> minValues, boost::optional<std::vector<ConstantType>&> maxValues, std::shared_ptr<Order> order = nullptr);

            /**
             * Sets the states which could not be ordered for the given order.
             *
             * @param order Order in which the states could not be ordered.
             * @param state1 First state, relation to state2 is unknown.
             * @param state2 Second state, relation to state1 is unknown.
             */
            void setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2);

            /**
             * Copies the unknown states for one order to the other one (deep-copy)
             *
             * @param orderOriginal original order
             * @param orderCopy order for which we want the unknown states to be copied to
             */
            void copyUnknownStates(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy);

            /**
             * Copies the min max values for one order to the other one (deep-copy).
             *
             * @param orderOriginal original order.
             * @param orderCopy order for which we want the min max values to be copied to.
             */
            void copyMinMax(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy);

            /**
             * Returns the monotonicity checker used for checking monotonicity whilst creating the order.
             *
             * @return monotonicity checker.
             */
            MonotonicityChecker<ValueType>& getMonotonicityChecker();

            /**
             * Checks if there is hope to continue extending the current order.
             * @param order Order for which we want to check.
             * @return true if the unknown states for this order can be sorted based on the min max values.
             */
            bool isHope(std::shared_ptr<Order> order) const;

            /**
             * Returns all variables occuring at the outgoing transitions of states.
             *
             * @return vector with sets of variables, vector index corresponds to state number.
             */
            std::vector<std::set<VariableType>> const& getVariablesOccuringAtState();

            /**
             * Returns a vector with the successors for this state.
             * @param state
             * @return
             */
            std::vector<uint_fast64_t> const& getSuccessors(uint_fast64_t state, uint_fast64_t choice = 0);


            /**
             * Checks for local monotonicity at state for variable param in the given order. It updates the given monotonicity result.
             * @param state State for which monotonicity has to be checked.
             * @param order The ordering of states.
             * @param param Variable for which monotonicity has to be checked.
             * @param region Region at which the monotonicity has to be checked.
             * @param monResult The monotonicity result to update the monotonicity in.
             */
            void checkParOnStateMonRes(uint_fast64_t state, std::shared_ptr<Order> order, typename OrderExtender<ValueType, ConstantType>::VariableType param, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monResult);

            /**
             * Sort the states based on the order and if available the min/max values
             * @param states
             * @param order
             * @return pair with the sorted states and a pair of states which could not be sorted
             */
            std::pair<std::pair<uint_fast64_t ,uint_fast64_t>,std::vector<uint_fast64_t>> sortStatesOrderAndMinMax(std::vector<uint_fast64_t> const& states, std::shared_ptr<Order> order);

        protected:
            void buildStateMap();
            std::pair<std::pair<uint_fast64_t ,uint_fast64_t>,std::vector<uint_fast64_t>> sortForFowardReasoning(uint_fast64_t currentState, std::shared_ptr<Order> order);
            /*!
             * Creates the initial order.
             * The order is based on either the formula and model or the provided top/bottom states.
             * These are provided when constructing the OrderExtender.
             *
             * @param isOptimistic bool ean whether the order should be optimistic or not
             * @return pointer to the created order
             */
            virtual std::shared_ptr<Order> getInitialOrder(bool isOptimistic) = 0;


            // Order extension
            Order::NodeComparison addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) const;
            bool extendWithAssumption(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t succState2, uint_fast64_t succState1);
            std::pair<uint_fast64_t, uint_fast64_t> extendNormal(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState);
            virtual std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) = 0;
            virtual std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) = 0;
            virtual void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) = 0;
            void handleAssumption(std::shared_ptr<Order> order, std::shared_ptr<expressions::BinaryRelationExpression> assumption) const;
            std::pair<uint_fast64_t, bool> getNextState(std::shared_ptr<Order> order, uint_fast64_t stateNumber, bool done);
            virtual void addStatesMinMax(std::shared_ptr<Order> order);

            // Order properties
            boost::optional<storm::storage::BitVector> topStates;
            boost::optional<storm::storage::BitVector> bottomStates;
            boost::optional<std::vector<ConstantType>> minValuesInit;
            boost::optional<std::vector<ConstantType>> maxValuesInit;
            std::map<std::shared_ptr<Order>, bool> usePLA;
            std::map<std::shared_ptr<Order>, bool> continueExtending;
            std::map<std::shared_ptr<Order>, std::vector<ConstantType>> minValues;
            std::map<std::shared_ptr<Order>, std::vector<ConstantType>> maxValues;
            // States that couldn't be ordered for a given order
            std::map<std::shared_ptr<Order>, std::pair<uint_fast64_t, uint_fast64_t>> unknownStatesMap;

            // Model properties
            std::shared_ptr<logic::Formula const> formula;
            std::shared_ptr<models::sparse::Model<ValueType>> model;
            storage::SparseMatrix<ValueType> matrix;
            bool cyclic;
            uint_fast64_t numberOfStates;

            // States, transitions and variables occuring
            std::map<VariableType, std::vector<uint_fast64_t>> occuringStatesAtVariable;
            std::vector<std::set<VariableType>> occuringVariablesAtState;
            std::map<uint_fast64_t, std::vector<std::vector<uint_fast64_t>>> stateMap;
            boost::container::flat_set<uint_fast64_t> nonParametricStates;

            // To make assumptions
            AssumptionMaker<ValueType, ConstantType>* assumptionMaker;
            std::vector<uint_fast64_t> statesToHandleInitially;



        private:
            MonotonicityChecker<ValueType> monotonicityChecker;
            bool useAssumptions;
        };
    }
}

#endif //STORM_ORDEREXTENDER_H
