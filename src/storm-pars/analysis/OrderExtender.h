#pragma once
#include <boost/container/flat_set.hpp>
#include "storm/api/storm.h"
#include "storm/logic/Formula.h"
#include "storm/models/sparse/Model.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/VariableExpression.h"

#include "Assumption.h"
#include "AssumptionMaker.h"
#include "storage/StronglyConnectedComponentDecomposition.h"
#include "storm-pars/analysis/ActionComparator.h"
#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/analysis/MonotonicityChecker.h"
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm-pars/analysis/Order.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm {
namespace analysis {
template<typename ValueType, typename ConstantType>
class OrderExtender {
   public:
    typedef typename utility::parametric::CoefficientType<ValueType>::type CoefficientType;
    typedef typename utility::parametric::VariableType<ValueType>::type VariableType;
    typedef typename MonotonicityResult<VariableType>::Monotonicity Monotonicity;
    typedef typename ActionComparator<ValueType, ConstantType>::ComparisonResult CompareResult;
    typedef typename storage::SparseMatrix<ValueType>::rows* Rows;
    //------------------------------------------------------------------------------
    // Constructors
    //------------------------------------------------------------------------------

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
    OrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates, storm::storage::SparseMatrix<ValueType> matrix, bool prMax);

    //------------------------------------------------------------------------------
    // Order creation/extension
    //------------------------------------------------------------------------------
    /*!
     * Creates an order based on the given formula.
     *
     * @param region The region for the order.
     * @param monRes The monotonicity result so far.
     * @return A triple with a pointer to the order and two states of which the current place in the order
     *         is unknown but needed. When the states have as number the number of states, no states are
     *         unplaced but needed.
     */
    std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> toOrder(storage::ParameterRegion<ValueType> region,
                                                                             std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr);

    /*!
     * Extends the order for the given region.
     *
     * @param order Pointer to the order.
     * @param region The region on which the order needs to be extended.
     * @return Two states of which the current place in the order
     *         is unknown but needed. When the states have as number the number of states, no states are
     *         unplaced or needed.
     */
    std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> extendOrder(std::shared_ptr<Order> order,
                                                                                 storm::storage::ParameterRegion<ValueType> region,
                                                                                 std::shared_ptr<MonotonicityResult<VariableType>> monRes = nullptr,
                                                                                 std::optional<Assumption> assumption = {});

    /**
     * Checks if there is hope to continue extending the current order.
     * @param order Order for which we want to check.
     * @return true if the unknown states for this order can be sorted based on the min max values.
     */
    bool isHope(std::shared_ptr<Order> order) const;

    //------------------------------------------------------------------------------
    // Min/Max values for bounds on the reward/probability
    //------------------------------------------------------------------------------
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
    void setMinMaxValues(boost::optional<std::vector<ConstantType>&> minValues, boost::optional<std::vector<ConstantType>&> maxValues,
                         std::shared_ptr<Order> order = nullptr);

    /**
     * Copies the min max values for one order to the other one (deep-copy).
     *
     * @param orderOriginal original order.
     * @param orderCopy order for which we want the min max values to be copied to.
     */
    void copyMinMax(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy);

    //------------------------------------------------------------------------------
    // Two states that couldn't be ordered
    //------------------------------------------------------------------------------
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

    //------------------------------------------------------------------------------
    // Getters
    //------------------------------------------------------------------------------
    /**
     * Returns a set with the successors for this state.
     * If an action is set in the order, the successors for this action are returned, otherwise all possible successors are returned.
     *
     * @param state Considered state
     * @param order Considered order
     * @return Set with successors
     */
    boost::container::flat_set<uint_fast64_t>& getSuccessors(uint_fast64_t state, std::shared_ptr<Order> order);
    /**
     * Returns a set with the successors for this state.
     *
     * @param state Considered state
     * @param action Considered action
     * @return Set with successors
     */
    boost::container::flat_set<uint_fast64_t>& getSuccessors(uint_fast64_t state, uint_fast64_t action);

    //------------------------------------------------------------------------------
    // Checking for monotonicity
    //------------------------------------------------------------------------------
    /**
     * Checks for local monotonicity at state for variable param in the given order. It updates the given monotonicity result.
     * @param state State for which monotonicity has to be checked.
     * @param order The ordering of states.
     * @param param Variable for which monotonicity has to be checked.
     * @param region Region at which the monotonicity has to be checked.
     * @param monResult The monotonicity result to update the monotonicity in.
     */
    void checkParOnStateMonRes(uint_fast64_t state, std::shared_ptr<Order> order, typename OrderExtender<ValueType, ConstantType>::VariableType param,
                               storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monResult);

   protected:
    //------------------------------------------------------------------------------
    // Order creation/extension
    //------------------------------------------------------------------------------
    std::shared_ptr<Order> getInitialOrder(storage::ParameterRegion<ValueType> region);
    std::pair<uint_fast64_t, uint_fast64_t> extendNormal(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                         uint_fast64_t currentState);
    void handleAssumption(std::shared_ptr<Order> order, Assumption assumption) const;

    //------------------------------------------------------------------------------
    // Sorting states
    //------------------------------------------------------------------------------
    std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sortForForwardReasoning(uint_fast64_t currentState,
                                                                                                           std::shared_ptr<Order> order);
    std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sortStatesOrderAndMinMax(
        boost::container::flat_set<uint_fast64_t> const& states, std::shared_ptr<Order> order);

    //------------------------------------------------------------------------------
    // Min/Max values for bounds on the reward/probability
    //------------------------------------------------------------------------------
    void addStatesMinMax(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region);
    Order::NodeComparison addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) const;

    //------------------------------------------------------------------------------
    // Getters
    //------------------------------------------------------------------------------
    std::pair<uint_fast64_t, bool> getNextState(std::shared_ptr<Order> order, uint_fast64_t stateNumber, bool done);

    //------------------------------------------------------------------------------
    // MDP
    //------------------------------------------------------------------------------
    bool findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state);

    // Virtual protected methods
    virtual std::pair<uint_fast64_t, uint_fast64_t> extendByBackwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                                              uint_fast64_t currentState) = 0;
    virtual std::pair<uint_fast64_t, uint_fast64_t> extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                                             uint_fast64_t currentState) = 0;

    bool extendNonDeterministic(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState);
    virtual void handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) = 0;
    virtual void setBottomTopStates() = 0;

    virtual void checkRewardsForOrder(std::shared_ptr<Order> order) = 0;

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
    bool rewards;
    std::shared_ptr<storm::models::sparse::StandardRewardModel<ValueType>> rewardModel;

    // States, transitions and variables occuring
    std::map<VariableType, std::vector<uint_fast64_t>> occuringStatesAtVariable;
    std::vector<std::set<VariableType>> occuringVariablesAtState;
    std::map<uint_fast64_t, std::vector<boost::container::flat_set<uint_fast64_t>>> stateMap;

    std::map<uint_fast64_t, boost::container::flat_set<uint_fast64_t>> stateMapAllSucc;

    boost::container::flat_set<uint_fast64_t> nonParametricStates;

    // To make assumptions
    AssumptionMaker<ValueType, ConstantType>* assumptionMaker;
    ActionComparator<ValueType, ConstantType> actionComparator;
    std::vector<uint_fast64_t> statesToHandleInitially;

    std::vector<boost::container::flat_set<uint_fast64_t>> dependentStates;
    bool deterministic;
    storage::SparseMatrix<ValueType> transposeMatrix;

   private:
    void init(storm::storage::SparseMatrix<ValueType> matrix);
    void buildStateMap();
    bool isStateReachable(uint_fast64_t state, std::shared_ptr<Order> order);
    std::optional<storm::storage::BitVector> reachableStates;
    std::pair<std::vector<uint_fast64_t>, storage::StronglyConnectedComponentDecomposition<ValueType>> sortStatesAndDecomposeForOrder();

    //------------------------------------------------------------------------------
    // Order creation/extension
    //------------------------------------------------------------------------------
    bool extendWithAssumption(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t succState2,
                              uint_fast64_t succState1);

    MonotonicityChecker<ValueType> monotonicityChecker;
    bool useAssumptions;
    ConstantType precision;

    bool prMax;
};
}  // namespace analysis
}  // namespace storm
