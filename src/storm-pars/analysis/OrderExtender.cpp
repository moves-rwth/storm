#include "OrderExtender.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"

#include "storm-pars/api/export.h"
#include "storm-pars/api/region.h"
#include "storm/api/verification.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
namespace analysis {

//------------------------------------------------------------------------------
// Constructors
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
OrderExtender<ValueType, ConstantType>::OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula)
    : monotonicityChecker(model->getTransitionMatrix()) {
    this->model = model;
    init(model->getTransitionMatrix());
    STORM_LOG_THROW(formula->isProbabilityOperatorFormula() || formula->isRewardOperatorFormula(), storm::exceptions::NotSupportedException,
                    "Only supporting reward and probability operator formula's");
    if (formula->hasQualitativeResult()) {
        storm::logic::OperatorInformation opInfo;
        if (model->isOfType(storm::models::ModelType::Mdp)) {
            opInfo = storm::logic::OperatorInformation(formula->asOperatorFormula().getOptimalityType(), boost::none);
        } else {
            opInfo = storm::logic::OperatorInformation(boost::none, boost::none);
        }

        if (formula->isProbabilityOperatorFormula()) {
            this->formula =
                std::make_shared<storm::logic::ProbabilityOperatorFormula>(formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo);
        } else {
            STORM_LOG_ASSERT(formula->isRewardOperatorFormula(), "Expecting formula to be reward formula");
            this->formula = std::make_shared<storm::logic::RewardOperatorFormula>(formula->asRewardOperatorFormula().getSubformula().asSharedPointer(),
                                                                                  model->getUniqueRewardModelName(), opInfo);
        }
    } else {
        this->formula = formula;
    }
    assert(this->formula->hasQuantitativeResult());

    this->bottomStates = boost::none;
    this->topStates = boost::none;
    if (!deterministic && formula->isRewardOperatorFormula()) {
        this->prMax = formula->asRewardOperatorFormula().getOptimalityType() == OptimizationDirection::Maximize;
    } else if (!deterministic) {
        STORM_LOG_ASSERT(formula->isProbabilityOperatorFormula(), "Expecting reward or probability formula");
        this->prMax = formula->asProbabilityOperatorFormula().getOptimalityType() == OptimizationDirection::Maximize;
    } else {
        // Don't care as it is non-deterministic
        this->prMax = true;
    }
}

template<typename ValueType, typename ConstantType>
OrderExtender<ValueType, ConstantType>::OrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates,
                                                      storm::storage::SparseMatrix<ValueType> matrix, bool prMax)
    : monotonicityChecker(matrix) {
    STORM_LOG_ASSERT(topStates.size() == bottomStates.size(), "Expecting the bitvectors for the top- and bottom states to have the same size");
    init(matrix);
    this->model = nullptr;
    this->formula = nullptr;
    this->topStates = std::move(topStates);
    this->bottomStates = std::move(bottomStates);
    this->prMax = prMax;
}

//------------------------------------------------------------------------------
// Private methods
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::init(storm::storage::SparseMatrix<ValueType> matrix) {
    this->matrix = matrix;
    this->numberOfStates = this->matrix.getColumnCount();
    this->deterministic = matrix.getRowGroupCount() == matrix.getRowCount();
    this->actionComparator = ActionComparator<ValueType>();
    this->transposeMatrix = this->matrix.getSquareMatrix().transpose();
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::buildStateMap() {
    this->dependentStates = std::vector<boost::container::flat_set<uint_fast64_t>>(numberOfStates, boost::container::flat_set<uint_fast64_t>());
    // Build stateMap
    uint_fast64_t rowCount = 0;
    uint_fast64_t currentOption = 0;
    uint_fast64_t numberOfOptionsForState = 0;
    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
        stateMap[state] = std::vector<boost::container::flat_set<uint_fast64_t>>();
        std::set<VariableType> occurringVariables;
        numberOfOptionsForState = matrix.getRowGroupSize(state);
        while (currentOption < numberOfOptionsForState) {
            auto row = matrix.getRow(rowCount);
            stateMap[state].push_back(boost::container::flat_set<uint64_t>());
            bool selfloop = false;
            for (auto& entry : row) {
                if (!(storm::utility::isZero<ValueType>(entry.getValue()))) {
                    selfloop |= state == entry.getColumn();
                    stateMap[state][currentOption].insert(entry.getColumn());
                    storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);
                }
            }
            if (selfloop && stateMap[state][currentOption].size() == 2) {
                // We have a selfloop and one additional successor, so we add the state to states to handle
                statesToHandleInitially.push_back(state);
            }
            currentOption++;
            rowCount++;
        }

        if (occurringVariables.empty()) {
            nonParametricStates.insert(state);
        }

        for (auto& var : occurringVariables) {
            occuringStatesAtVariable[var].push_back(state);
        }
        occuringVariablesAtState.push_back(std::move(occurringVariables));

        currentOption = 0;
    }
    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
        bool first = true;
        boost::container::flat_set<uint_fast64_t> successors;
        for (auto& succs : this->stateMap[state]) {
            if (first) {
                for (auto succ : succs) {
                    successors.insert(succ);
                }
                first = false;
            } else {
                for (auto succ : succs) {
                    if (std::find(successors.begin(), successors.end(), succ) == successors.end()) {
                        successors.insert(succ);
                    }
                }
            }
        }
        stateMapAllSucc[state] = std::move(successors);
    }
}

template<typename ValueType, typename ConstantType>
std::pair<std::vector<uint_fast64_t>, storage::StronglyConnectedComponentDecomposition<ValueType>>
OrderExtender<ValueType, ConstantType>::sortStatesAndDecomposeForOrder() {
    // Sorting the states
    storm::storage::StronglyConnectedComponentDecompositionOptions options;
    options.forceTopologicalSort();

    this->numberOfStates = this->matrix.getColumnCount();
    std::vector<uint64_t> firstStates;

    storm::storage::BitVector subStates(this->topStates->size(), true);
    for (auto state : (this->topStates.get())) {
        firstStates.push_back(state);
        subStates.set(state, false);
    }
    for (auto state : (this->bottomStates.get())) {
        firstStates.push_back(state);
        subStates.set(state, false);
    }
    this->cyclic = storm::utility::graph::hasCycle(this->matrix, subStates);
    storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition;
    if (this->cyclic) {
        decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(this->matrix, options);
    }
    if (this->matrix.getColumnCount() == this->matrix.getRowCount()) {
        return {storm::utility::graph::getTopologicalSort(transposeMatrix, firstStates), decomposition};
    } else {
        //        return {storm::utility::graph::getTopologicalSort(transposeMatrix, firstStates), decomposition};
        return {storm::utility::graph::getBFSTopologicalSort(transposeMatrix, this->matrix, firstStates), decomposition};
    }
}

//------------------------------------------------------------------------------
// Order creation/extension
//------------------------------------------------------------------------------
// public
template<typename ValueType, typename ConstantType>
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::toOrder(
    storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
    auto order = getInitialOrder();
    if (order == nullptr) {
        return {nullptr, numberOfStates, numberOfStates};
    }
    return extendOrder(order, region, monRes);
}

template<typename ValueType, typename ConstantType>
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendOrder(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes,
    std::optional<Assumption> assumption) {
    STORM_LOG_ASSERT(order != nullptr, "Order should be provided");
    if (assumption.has_value()) {
        this->handleAssumption(order, assumption.value());
    }

    auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
    while (currentStateMode.first != this->numberOfStates) {
        if (order->isInvalid()) {
            return {order, numberOfStates, numberOfStates};
        }
        STORM_LOG_ASSERT(currentStateMode.first < this->numberOfStates, "Unexpected state number");
        auto& currentState = currentStateMode.first;
        STORM_LOG_INFO("Currently considering state " << currentState << " from " << (currentStateMode.second ? "sortedStates" : "statesToHandle"));
        while (order->isTopState(currentState) || order->isBottomState(currentState)) {
            currentStateMode = this->getNextState(order, currentState, true);
            currentState = currentStateMode.first;
        }
        bool actionFound = this->findBestAction(order, region, currentState);

        std::pair<uint_fast64_t, uint_fast64_t> result = {this->numberOfStates, this->numberOfStates};
        auto const& successors = this->getSuccessors(currentState, order);
        if (actionFound) {
            uint_fast64_t succ0 = *(successors.begin());
            if (successors.size() == 1) {
                if (order->contains(succ0)) {
                    handleOneSuccessor(order, currentState, succ0);
                } else {
                    result = {succ0, succ0};
                }
            } else if (!successors.empty()) {
                if (order->isOnlyInitialOrder()) {
                    order->add(currentState);
                    if (!order->isTrivial(currentState)) {
                        // This state is part of an scc, therefore, we could do forward reasoning here
                        result = this->extendByForwardReasoning(order, region, currentState);
                    } else {
                        result = {this->numberOfStates, this->numberOfStates};
                    }
                } else {
                    result = this->extendNormal(order, region, currentState);
                }
            }
        } else if (currentStateMode.second) {
            // We have a non-deterministic state, and cannot (yet) fix the action
            // We only go into here if we need to handle the state because it was its turn by the ordering
            result = this->extendNormal(order, region, currentState);
            if (result.first != this->numberOfStates) {
                // We could not extend it, so we first make assumptions for the actions (therefore we return order, currentState, currentState)
                order->addStateSorted(currentState);
                this->continueExtending[order] = false;
                return {order, currentState, currentState};
            }
        } else {
            // We couldn't deal with the state, its statemode is from statesToHandle
            // we reset result
            result = {currentState, currentState};
        }

        // Now that we tried to order it, see what we should do next
        if (result.first == this->numberOfStates) {
            // We did extend the order
            STORM_LOG_ASSERT(result.second == this->numberOfStates, "Expecting both parts of result to contain the number of states");
            STORM_LOG_ASSERT(order->sortStates(successors).size() == successors.size(), "Something went wrong while sorting states, number of states differs");
            STORM_LOG_ASSERT(order->contains(currentState) && order->getNode(currentState) != nullptr, "Expecting order to contain the current State");

            if (monRes != nullptr) {
                for (auto& param : this->occuringVariablesAtState[currentState]) {
                    this->checkParOnStateMonRes(currentState, order, param, region, monRes);
                }
            }
            // Get the next state
            currentStateMode = this->getNextState(order, currentState, true);
        } else {
            STORM_LOG_ASSERT(result.first < this->numberOfStates && result.second < this->numberOfStates,
                             "Expecting both result numbers to correspond to states");
            STORM_LOG_ASSERT(
                (!currentStateMode.second && result.first == currentState && result.first == result.second && !order->isActionSetAtState(currentState)) ||
                    (order->compare(result.first, result.second) == Order::UNKNOWN && order->compare(result.second, result.first) == Order::UNKNOWN),
                "Expecting relation between the two states to be unknown");

            // We couldn't extend the order
            if (this->nonParametricStates.find(currentState) != this->nonParametricStates.end()) {
                if (!order->contains(currentState)) {
                    // State is not parametric, so we hope that just adding it between =) and =( will help us
                    order->add(currentState);
                }
                // We set is as sufficient as it is non-parametric and non-parametric states are by definition sufficient
                order->setSufficientForState(currentState);
                if (this->matrix.getRowGroupSize(currentState) > 1) {
                    // State is non-deterministic
                    // If one of the successors is not yet in the order, we add it to a waitinglist and see if we can handle it as soon as we are done for the
                    // successor
                    for (auto& succ : this->getSuccessors(currentState, order)) {
                        if (!order->isSufficientForState(succ)) {
                            this->dependentStates[succ].insert(currentState);
                        }
                    }
                }
                currentStateMode = this->getNextState(order, currentState, false);

                continue;
            } else {
                // Try to add states based on min/max and assumptions, only if we are not in statesToHandle mode
                if (currentStateMode.second && this->extendWithAssumption(order, region, result.first, result.second)) {
                    continue;
                }
                if (!currentStateMode.second) {
                    // The state was based on statesToHandle, so it is not bad if we cannot continue with this.
                    currentStateMode = this->getNextState(order, currentState, false);
                    continue;
                } else {
                    // The state was based on the topological sorting, so we need to return, but first add this state to the states Sorted as we are not done
                    // with it
                    order->addStateSorted(currentState);
                    this->continueExtending[order] = false;
                    return {order, result.first, result.second};
                }
            }
        }
        STORM_LOG_ASSERT(order->sortStates(successors).size() == successors.size(), "Expecting all successor states to be sorted");
    }

    STORM_LOG_ASSERT(order->getNumberOfSufficientStates() == this->numberOfStates, "Expecting to be sufficient for all states");
    if (monRes != nullptr) {
        // monotonicity result for the in-build checking of monotonicity
        monRes->setDone();
    }
    STORM_LOG_INFO("Done for order");
    return std::make_tuple(order, this->numberOfStates, this->numberOfStates);
}

template<typename ValueType, typename ConstantType>
bool OrderExtender<ValueType, ConstantType>::isHope(std::shared_ptr<Order> order) const {
    if (order->getDoneBuilding()) {
        return false;
    }
    return continueExtending.find(order) != continueExtending.end() && continueExtending.at(order);
}

// protected
template<typename ValueType, typename ConstantType>
std::shared_ptr<Order> OrderExtender<ValueType, ConstantType>::getInitialOrder() {
    setBottomTopStates();
    STORM_LOG_THROW(this->bottomStates.is_initialized(), storm::exceptions::UnexpectedException, "Expecting the formula to yield bottom states for the order");
    if (this->bottomStates->getNumberOfSetBits() == 0 && (this->topStates == boost::none || this->topStates->getNumberOfSetBits() == 0)) {
        return nullptr;
    }

    auto statesSortedAndDecomposition = this->sortStatesAndDecomposeForOrder();

    // Create Order
    std::shared_ptr<Order> order =
        std::shared_ptr<Order>(new Order(&(this->topStates.get()), &(this->bottomStates.get()), this->numberOfStates,
                                         std::move(statesSortedAndDecomposition.second), std::move(statesSortedAndDecomposition.first)));
    this->buildStateMap();
    for (auto& state : this->statesToHandleInitially) {
        order->addStateToHandle(state);
    }

    if (this->minValuesInit) {
        this->minValues[order] = this->minValuesInit.get();
    }

    if (this->maxValuesInit) {
        this->maxValues[order] = this->maxValuesInit.get();
    }

    if (this->minValuesInit && this->maxValuesInit) {
        this->continueExtending[order] = true;
        this->usePLA[order] = true;
        this->addStatesMinMax(order);
    } else {
        this->usePLA[order] = false;
    }
    this->continueExtending[order] = true;

    checkRewardsForOrder(order);
    return order;
}

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendNormal(std::shared_ptr<Order> order,
                                                                                             storm::storage::ParameterRegion<ValueType> region,
                                                                                             uint_fast64_t currentState) {
    // when it is cyclic and the current state is part of an SCC we do forwardreasoning
    if (this->cyclic && !order->isTrivial(currentState) && !order->contains(currentState)) {
        boost::container::flat_set<uint_fast64_t> const& successors = this->getSuccessors(currentState, order);
        if (successors.size() == 2 && (*(successors.begin()) == currentState || *(successors.begin() + 1) == currentState)) {
            order->add(currentState);
        }
    }
    if (this->cyclic && !order->isSufficientForState(currentState) && !order->isTrivial(currentState) && order->contains(currentState)) {
        // Try to extend the order for this scc
        auto result = extendByForwardReasoning(order, region, currentState);
        if (result.first == this->numberOfStates) {
            return result;
        } else {
            auto result2 = extendByBackwardReasoning(order, region, currentState);
            if (result2.first == this->numberOfStates) {
                return result2;
            } else {
                return result;
            }
        }
    } else {
        assert(order->isTrivial(currentState) || !order->contains(currentState) || order->isSufficientForState(currentState));
        // Do backward reasoning, all successor states must be in the order
        return extendByBackwardReasoning(order, region, currentState);
    }
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::handleAssumption(std::shared_ptr<Order> order, Assumption assumption) const {
    STORM_LOG_INFO("Handling " << assumption);
    if (assumption.isStateAssumption()) {
        STORM_LOG_ASSERT(assumption.getAssumption()->getFirstOperand()->isVariable() && assumption.getAssumption()->getSecondOperand()->isVariable(),
                         "Expecting the assumption operands to be variables");

        expressions::Variable var1 = assumption.getAssumption()->getFirstOperand()->asVariableExpression().getVariable();
        expressions::Variable var2 = assumption.getAssumption()->getSecondOperand()->asVariableExpression().getVariable();
        auto const& val1 = std::stoul(var1.getName(), nullptr, 0);
        auto const& val2 = std::stoul(var2.getName(), nullptr, 0);

        STORM_LOG_ASSERT(order->compare(val1, val2) == Order::UNKNOWN, "Assumption for states that are already ordered, expecting them to be unordered");

        Order::Node* n1 = order->getNode(val1);
        Order::Node* n2 = order->getNode(val2);

        if (assumption.getAssumption()->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal) {
            if (n1 != nullptr && n2 != nullptr) {
                order->mergeNodes(n1, n2);
            } else if (n1 != nullptr) {
                order->addToNode(val2, n1);
            } else if (n2 != nullptr) {
                order->addToNode(val1, n2);
            } else {
                order->add(val1);
                order->addStateToHandle(val1);

                order->addToNode(val2, order->getNode(val1));
            }
        } else {
            STORM_LOG_ASSERT(assumption.getAssumption()->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater,
                             "Unknown comparision type found, it is neither equal nor greater");
            if (n1 != nullptr && n2 != nullptr) {
                order->addRelationNodes(n1, n2);  //, true);
            } else if (n1 != nullptr) {
                order->addBetween(val2, n1, order->getBottom());
            } else if (n2 != nullptr) {
                // TODO: This should be moved to reward/reachorderextender, as top is only nullptr for rewards
                if (order->getTop() == nullptr) {
                    order->addAbove(val1, n2);
                } else {
                    order->addBetween(val1, order->getTop(), n2);
                }
            } else {
                order->add(val1);
                order->addStateToHandle(val1);

                order->addBetween(val2, order->getNode(val1), order->getBottom());
            }
        }
    } else {
        expressions::Variable var1 = assumption.getAssumption()->getFirstOperand()->asVariableExpression().getVariable();
        expressions::Variable var2 = assumption.getAssumption()->getSecondOperand()->asVariableExpression().getVariable();
        auto const& val1 = std::stoul(var1.getName(), nullptr, 0);
        auto const& val2 = std::stoul(var2.getName(), nullptr, 0);
        order->addToMdpScheduler(val1, val2);
        STORM_LOG_ASSERT(val2 < this->matrix.getRowGroupSize(val1), "Expecting the action to be a valid action for state " << val1);
    }
}

// private
template<typename ValueType, typename ConstantType>
bool OrderExtender<ValueType, ConstantType>::extendWithAssumption(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region,
                                                                  uint_fast64_t stateSucc1, uint_fast64_t stateSucc2) {
    bool usePLANow = this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order];
    assert(order->compare(stateSucc1, stateSucc2) == Order::UNKNOWN);
    auto assumptions =
        usePLANow ? this->assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2, order, region, this->minValues[order], this->maxValues[order])
                  : this->assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2, order, region);
    if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
        this->handleAssumption(order, assumptions.begin()->first);
        // Assumptions worked, we continue
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// Min/Max values for bounds on the reward/probability
//------------------------------------------------------------------------------
// public
template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::initializeMinMaxValues(storage::ParameterRegion<ValueType> region, std::shared_ptr<Order> order) {
    assert(this->formula != nullptr);
    assert(this->formula->hasQuantitativeResult());

    if (model != nullptr) {
        std::unique_ptr<modelchecker::CheckResult> checkResult;
        auto env = Environment();
        boost::optional<modelchecker::CheckTask<logic::Formula, ValueType>> checkTask = storm::api::createTask<ValueType>(formula, false);
        if (model->isOfType(storm::models::ModelType::Dtmc)) {
            // Use parameter lifting modelchecker to get initial min/max values for order creation
            modelchecker::SparseDtmcParameterLiftingModelChecker<models::sparse::Dtmc<ValueType>, ConstantType> plaModelChecker;
            STORM_LOG_THROW(plaModelChecker.canHandle(model, checkTask.get()), exceptions::NotSupportedException, "Cannot handle this formula");
            plaModelChecker.specify(env, model, checkTask.get(), false, false);

            modelchecker::ExplicitQuantitativeCheckResult<ConstantType> minCheck =
                plaModelChecker.check(env, region, solver::OptimizationDirection::Minimize)->template asExplicitQuantitativeCheckResult<ConstantType>();
            modelchecker::ExplicitQuantitativeCheckResult<ConstantType> maxCheck =
                plaModelChecker.check(env, region, solver::OptimizationDirection::Maximize)->template asExplicitQuantitativeCheckResult<ConstantType>();

            if (order != nullptr) {
                minValues[order] = minCheck.getValueVector();
                maxValues[order] = maxCheck.getValueVector();
                usePLA[order] = true;
            } else {
                minValuesInit = minCheck.getValueVector();
                maxValuesInit = maxCheck.getValueVector();
            }
        } else if (model->isOfType(storm::models::ModelType::Mdp)) {
            // Use parameter lifting modelchecker to get initial min/max values for order creation
            modelchecker::SparseMdpParameterLiftingModelChecker<models::sparse::Mdp<ValueType>, ConstantType> plaModelChecker;

            STORM_LOG_THROW(plaModelChecker.canHandle(model, checkTask.get()), exceptions::NotSupportedException, "Cannot handle this formula");
            plaModelChecker.specify(env, model, checkTask.get(), false, false);

            assert(checkTask->getFormula().hasQuantitativeResult());
            modelchecker::ExplicitQuantitativeCheckResult<ConstantType> minCheck =
                plaModelChecker.check(env, region, solver::OptimizationDirection::Minimize)->template asExplicitQuantitativeCheckResult<ConstantType>();
            modelchecker::ExplicitQuantitativeCheckResult<ConstantType> maxCheck =
                plaModelChecker.check(env, region, solver::OptimizationDirection::Maximize)->template asExplicitQuantitativeCheckResult<ConstantType>();

            if (order != nullptr) {
                minValues[order] = minCheck.getValueVector();
                maxValues[order] = maxCheck.getValueVector();
                usePLA[order] = true;
            } else {
                minValuesInit = minCheck.getValueVector();
                maxValuesInit = maxCheck.getValueVector();
            }
        }
    }
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::setMinMaxValues(boost::optional<std::vector<ConstantType>&> minValues,
                                                             boost::optional<std::vector<ConstantType>&> maxValues, std::shared_ptr<Order> order) {
    STORM_LOG_ASSERT(!minValues || minValues.get().size() == numberOfStates,
                     "Expecting min values to not be initialized, or to have the size of the number of states");
    STORM_LOG_ASSERT(!maxValues || maxValues.get().size() == numberOfStates,
                     "Expecting max values to not be initialized, or to have the size of the number of states");
    STORM_LOG_ASSERT(minValues || maxValues, "Expecting either min or max values to be initialized");

    if (order == nullptr) {
        if (minValues) {
            this->minValuesInit = std::move(minValues.get());
        }
        if (maxValues) {
            this->maxValuesInit = std::move(maxValues.get());
        }
    } else {
        usePLA[order] = true;
        if (minValues && maxValues && unknownStatesMap.find(order) != unknownStatesMap.end()) {
            auto& unknownStates = unknownStatesMap[order];
            if (unknownStates.first != numberOfStates) {
                // only continue if the difference is large enough to be correct
                continueExtending[order] = (minValues.get()[unknownStates.first] > maxValues.get()[unknownStates.second] &&
                                            (minValues.get()[unknownStates.first] - maxValues.get()[unknownStates.second]) > error) ||
                                           (minValues.get()[unknownStates.second] > maxValues.get()[unknownStates.first] &&
                                            ((minValues.get()[unknownStates.second] - maxValues.get()[unknownStates.first]) > error));
            } else {
                continueExtending[order] = true;
            }
        } else {
            continueExtending[order] = true;
        }
        if (minValues) {
            this->minValues[order] = std::move(minValues.get());
        }
        if (maxValues) {
            this->maxValues[order] = std::move(maxValues.get());
        }
    }
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::copyMinMax(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy) {
    STORM_LOG_ASSERT(maxValues.find(orderOriginal) != maxValues.end(), "Max values can't be copied, order not found");
    STORM_LOG_ASSERT(minValues.find(orderOriginal) != minValues.end(), "Min values can't be copied, order not found");

    usePLA[orderCopy] = usePLA[orderOriginal];
    if (usePLA[orderCopy]) {
        minValues[orderCopy] = minValues[orderOriginal];
        maxValues[orderCopy] = maxValues[orderOriginal];
    }
    continueExtending[orderCopy] = continueExtending[orderOriginal];
}

// protected
template<typename ValueType, typename ConstantType>
Order::NodeComparison OrderExtender<ValueType, ConstantType>::addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1,
                                                                                     uint_fast64_t state2) const {
    if (order->compareFast(state1, state2) != Order::UNKNOWN) {
        if (!order->contains(state1)) {
            order->add(state1);
        }
        if (!order->contains(state2)) {
            order->add(state2);
        }
        assert(order->compareFast(state1, state2) != Order::UNKNOWN);
        return order->compareFast(state1, state2);
    }
    STORM_LOG_ASSERT(minValues.find(order) != minValues.end() && maxValues.find(order) != maxValues.end(),
                     "Cannot add states based on min max values if the minmax values are not initialized for this order");
    std::vector<ConstantType> const& mins = minValues.at(order);
    std::vector<ConstantType> const& maxs = maxValues.at(order);
    if (mins[state1] > error && mins[state1] == maxs[state1] && mins[state2] == maxs[state2] && mins[state1] == mins[state2]) {
        if (order->contains(state1)) {
            if (order->contains(state2)) {
                STORM_LOG_INFO("Merging state " << state1 << " and " << state2 << " based on min max values");
                order->merge(state1, state2);
            } else {
                STORM_LOG_INFO("Adding state " << state2 << " to " << state1 << " based on min max values");

                order->addToNode(state2, order->getNode(state1));
            }
        } else if (order->contains(state2)) {
            STORM_LOG_INFO("Adding state " << state1 << " to " << state2 << " based on min max values");

            order->addToNode(state1, order->getNode(state2));
        }
        return Order::SAME;
    }
    if (mins[state1] > error && mins[state1] > maxs[state2]) {
        // state 1 will always be larger than state2
        if (!order->contains(state1)) {
            order->add(state1);
            order->addStateToHandle(state1);
        }
        if (!order->contains(state2)) {
            order->add(state2);
            order->addStateToHandle(state2);
        }
        STORM_LOG_ASSERT(order->compare(state1, state2) != Order::BELOW, "Expecting " << state1 << " to NOT be BELOW " << state2 << ".");
        STORM_LOG_ASSERT(order->compare(state1, state2) != Order::SAME, "Expecting " << state1 << " to NOT be SAME " << state2 << ".");
        STORM_LOG_INFO("Adding state " << state1 << " above " << state2 << " based on min max values");

        order->addRelation(state1, state2);

        return Order::ABOVE;
    } else if (mins[state2] > error && mins[state2] > maxs[state1]) {
        // state2 will always be larger than state1
        if (!order->contains(state1)) {
            order->add(state1);
            order->addStateToHandle(state1);
        }
        if (!order->contains(state2)) {
            order->add(state2);
            order->addStateToHandle(state2);
        }
        STORM_LOG_ASSERT(order->compare(state2, state1) != Order::BELOW, "Expecting " << state2 << " to NOT be BELOW " << state1 << ".");
        STORM_LOG_ASSERT(order->compare(state2, state1) != Order::SAME, "Expecting " << state2 << " to NOT be SAME " << state1 << ".");
        STORM_LOG_INFO("Adding state " << state2 << " above " << state1 << " based on min max values");
        order->addRelation(state2, state1);
        return Order::BELOW;
    } else {
        // Couldn't add relation between state1 and state 2 based on min/max values;
        return Order::UNKNOWN;
    }
}
template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::addStatesMinMax(std::shared_ptr<Order> order) {
    auto& min = this->minValues[order];
    auto& max = this->maxValues[order];

    // Add the states that can be ordered based on min/max values
    assert(this->usePLA[order]);
    std::set<typename storm::storage::ParameterRegion<ValueType>::VariableType> vars;
    for (auto& entry : this->matrix) {
        for (auto& var : entry.getValue().gatherVariables()) {
            vars.insert(var);
        }
    }
    auto& states = order->getStatesSorted();
    auto fakeRegion = storm::api::createRegion<ValueType>("0", vars);
    for (uint_fast64_t i = 0; i < this->numberOfStates; i++) {
        auto state = states[this->numberOfStates - i - 1];
        auto& successors = this->getSuccessors(state, order);
        bool allSorted = true;
        for (uint_fast64_t i1 = 0; i1 < successors.size(); ++i1) {
            for (uint_fast64_t i2 = i1 + 1; i2 < successors.size(); ++i2) {
                auto succ1 = *(successors.begin() + i1);
                auto succ2 = *(successors.begin() + i2);
                allSorted &= this->addStatesBasedOnMinMax(order, succ1, succ2) != Order::NodeComparison::UNKNOWN;
            }
        }
        if (!this->cyclic && (allSorted && successors.size() > 1 || (successors.size() == 1 && state != *(successors.begin())))) {
            STORM_LOG_INFO("All successors of state " << state << " sorted based on min max values");
            order->setSufficientForState(state);
            for (auto& entry : transposeMatrix.getRow(state)) {
                if (!order->isSufficientForState(entry.getColumn())) {
                    order->addStateToHandle(entry.getColumn());
                }
            }
        } else if (allSorted && this->cyclic) {
            // All successors are sorted, but the model is cyclic, so it might be that we are missing the forward reasoning for this state. Therefore we add it
            // to the stateToHandle queue.
            order->addStateToHandle(state);
        }
    }
}

//------------------------------------------------------------------------------
// Two states that couldn't be ordered
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) {
    assert(state1 != numberOfStates && state2 != numberOfStates);
    unknownStatesMap[order] = {state1, state2};
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::copyUnknownStates(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy) {
    assert(unknownStatesMap.find(orderCopy) == unknownStatesMap.end());
    unknownStatesMap.insert({orderCopy, {unknownStatesMap[orderOriginal].first, unknownStatesMap[orderOriginal].second}});
}

//------------------------------------------------------------------------------
// Getters
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, bool> OrderExtender<ValueType, ConstantType>::getNextState(std::shared_ptr<Order> order, uint_fast64_t currentState, bool done) {
    if (done) {
        assert(currentState != numberOfStates);
        order->setSufficientForState(currentState);
        order->setDoneForState(currentState);
        for (auto& state : this->dependentStates[currentState]) {
            // only non-deterministic, non-parametric states can occur in here
            order->addSpecialStateToHandle(state);
            order->setSufficientForState(state);
            if (order->contains(state)) {
                order->setDoneForState(state);
            }
        }
    }
    if (order->existsStateToHandle()) {
        return order->getStateToHandle();
    }
    if (currentState == numberOfStates) {
        return order->getNextStateNumber();
    }
    if (currentState != numberOfStates) {
        return order->getNextStateNumber();
    }
    return {numberOfStates, true};
}

template<typename ValueType, typename ConstantType>
boost::container::flat_set<uint_fast64_t>& OrderExtender<ValueType, ConstantType>::getSuccessors(uint_fast64_t state, std::shared_ptr<Order> order) {
    if (this->deterministic || this->stateMap[state].size() == 1) {
        return getSuccessors(state, 0);
    }
    if (order->isActionSetAtState(state)) {
        STORM_LOG_ASSERT(stateMap[state][order->getActionAtState(state)].size() > 0, "Expecting state " << state << " to have successors.");
        return getSuccessors(state, order->getActionAtState(state));
    }
    return stateMapAllSucc[state];
}

template<typename ValueType, typename ConstantType>
boost::container::flat_set<uint_fast64_t>& OrderExtender<ValueType, ConstantType>::getSuccessors(uint_fast64_t state, uint_fast64_t action) {
    STORM_LOG_ASSERT(state < stateMap.size(), "State number too large");
    STORM_LOG_ASSERT(action < stateMap[state].size(), "Action number too large");
    return stateMap[state][action];
}

//------------------------------------------------------------------------------
// Checking for monotonicity
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t state, std::shared_ptr<Order> order,
                                                                   typename OrderExtender<ValueType, ConstantType>::VariableType param,
                                                                   storm::storage::ParameterRegion<ValueType> region,
                                                                   std::shared_ptr<MonotonicityResult<VariableType>> monResult) {
    monResult->updateMonotonicityResult(param, monotonicityChecker.checkLocalMonotonicity(order, state, param, region));
}

//------------------------------------------------------------------------------
// Sorting states
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> OrderExtender<ValueType, ConstantType>::sortStatesOrderAndMinMax(
    boost::container::flat_set<uint_fast64_t> const& states, std::shared_ptr<Order> order) {
    uint_fast64_t numberOfStatesToSort = states.size();
    std::vector<uint_fast64_t> result;
    // Go over all states
    for (auto state : states) {
        if (result.size() == 0) {
            result.push_back(state);
            if (!order->contains(state)) {
                order->add(state);
                order->addStateToHandle(state);
            }
        } else {
            bool added = false;
            for (auto itr = result.begin(); itr != result.end(); ++itr) {
                auto nextState = *itr;
                auto compareRes = order->compareFast(state, nextState);
                // If fast sorting didn't work, we try using PLA
                if (compareRes == Order::NodeComparison::UNKNOWN && usePLA[order]) {
                    compareRes = addStatesBasedOnMinMax(order, state, nextState);
                }
                // If fast sorting and PLA didn't work, we try using normal sorting
                if (compareRes == Order::NodeComparison::UNKNOWN) {
                    compareRes = order->compare(state, nextState);
                }
                if (compareRes == Order::NodeComparison::ABOVE || compareRes == Order::NodeComparison::SAME) {
                    if (!order->contains(state)) {
                        // This can only happen if index refers to top/bottom state
                        order->add(state);
                        order->addStateToHandle(state);
                    }
                    // insert at current pointer (while keeping other values)
                    result.insert(itr, state);
                    added = true;
                    break;
                } else if (compareRes == Order::NodeComparison::UNKNOWN) {
                    return {{(nextState), state}, std::move(result)};
                }
            }
            if (!added) {
                // In this case our state is below all currently added states
                result.push_back(state);
            }
        }
    }
    STORM_LOG_ASSERT(result.size() == numberOfStatesToSort, "Expecting all states to be sorted");
    return {{numberOfStates, numberOfStates}, std::move(result)};
}

template<typename ValueType, typename ConstantType>
std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> OrderExtender<ValueType, ConstantType>::sortForForwardReasoning(
    uint_fast64_t currentState, std::shared_ptr<Order> order) {
    std::vector<uint_fast64_t> statesSorted;
    statesSorted.push_back(currentState);
    bool pla = (this->usePLA.find(order) != this->usePLA.end() && this->usePLA.at(order));
    // Go over all states
    bool oneUnknown = false;
    bool unknown = false;
    uint_fast64_t s1 = this->numberOfStates;
    uint_fast64_t s2 = this->numberOfStates;
    boost::container::flat_set<uint_fast64_t> const& successors = this->getSuccessors(currentState, order);

    // Quick hack for 2 successors
    if (successors.size() == 2) {
        auto succ0 = *(successors.begin());
        auto succ1 = *(successors.begin() + 1);
        auto res1 = order->compareFast(currentState, succ0);
        auto res2 = order->compareFast(currentState, succ1);
        auto res3 = order->compareFast(succ0, succ1);

        bool unknownOne = res1 == Order::NodeComparison::UNKNOWN;
        bool unknownTwo = res2 == Order::NodeComparison::UNKNOWN;
        bool unknownThree = res3 == Order::NodeComparison::UNKNOWN;

        oneUnknown =
            (unknownOne && !unknownTwo && !unknownThree) || (!unknownOne && unknownTwo && !unknownThree) || (!unknownOne && !unknownTwo && unknownThree);
        if (oneUnknown && res1) {
            if (res1 == Order::NodeComparison::ABOVE) {
                // succ0 is below current state
                statesSorted.push_back(succ0);
            } else {
                // succ0 is above current state
                statesSorted.insert(statesSorted.begin(), succ0);
            }
            return {{succ1, s2}, std::move(statesSorted)};
        } else if (oneUnknown && res2) {
            if (res2 == Order::NodeComparison::ABOVE) {
                statesSorted.push_back(succ1);
            } else {
                statesSorted.insert(statesSorted.begin(), succ1);
            }
            return {{succ0, s2}, std::move(statesSorted)};
        } else if (oneUnknown && res3) {
            return {{currentState, succ1}, std::move(statesSorted)};
        } else if (!res1 && !res2 && !res3) {
            // everything is already ordered
        }
        // Otherwise we do the normal sorting
    }
    oneUnknown = false;

    for (auto& state : successors) {
        unknown = false;
        bool added = false;
        for (auto itr = statesSorted.begin(); itr != statesSorted.end(); ++itr) {
            auto compareRes = order->compareFast(state, (*itr));
            if (pla && compareRes == Order::NodeComparison::UNKNOWN) {
                compareRes = this->addStatesBasedOnMinMax(order, state, (*itr));
            }
            if (compareRes == Order::NodeComparison::UNKNOWN) {
                compareRes = order->compare(state, *itr);
            }
            if (compareRes == Order::NodeComparison::ABOVE || compareRes == Order::NodeComparison::SAME) {
                if (!order->contains(state) && compareRes == Order::NodeComparison::ABOVE) {
                    order->add(state);
                    order->addStateToHandle(state);
                }
                added = true;
                // insert at current pointer (while keeping other values)
                statesSorted.insert(itr, state);
                break;
            } else if (compareRes == Order::NodeComparison::UNKNOWN && !oneUnknown) {
                // We miss state in the result.
                s1 = state;
                oneUnknown = true;
                added = true;
                break;
            } else if (compareRes == Order::NodeComparison::UNKNOWN && oneUnknown) {
                s1 = state;
                s2 = *itr;
                unknown = true;
                added = true;
                break;
            }
        }
        if (!(unknown && oneUnknown) && !added) {
            // State will be last in the list
            statesSorted.push_back(state);
        }
        if (unknown && oneUnknown) {
            break;
        }
    }
    if (!unknown && oneUnknown) {
        STORM_LOG_ASSERT(statesSorted.size() == successors.size(), "Expecting all states to be sorted except for one");
        s2 = this->numberOfStates;
    }

    STORM_LOG_ASSERT(s1 != s2 || s1 != this->numberOfStates || (statesSorted.size() == successors.size() + 1),
                     "Expecting all states to be sorted, or s1 to at least contain a valid state number");
    return {{s1, s2}, std::move(statesSorted)};
}

//------------------------------------------------------------------------------
// MDP
//------------------------------------------------------------------------------
template<typename ValueType, typename ConstantType>
bool OrderExtender<ValueType, ConstantType>::findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state) {
    if (deterministic) {
        return true;
    }
    // Finding the best action for the current state
    STORM_LOG_INFO("Looking for best action for state " << state);
    if (order->isActionSetAtState(state)) {
        STORM_LOG_INFO("Best action for state " << state << " is already set.");
        return true;
    }
    if (this->stateMap[state].size() == 1) {
        // if we only have one possible action, we already know which one we take.
        STORM_LOG_INFO("Only one Action available, take it.");
        order->addToMdpScheduler(state, 0);
        return true;
    }
    if (order->isTopState(state)) {
        // in this case the state should be absorbing so we just take action 0
        STORM_LOG_INFO("State is top state, thus absorbing. Take action 0.");
        order->addToMdpScheduler(state, 0);
        return true;
    }
    if (order->isBottomState(state)) {
        // in this case the state should be absorbing so we just take action 0
        STORM_LOG_INFO("State is bottom state, thus absorbing. Take action 0.");
        order->addToMdpScheduler(state, 0);
        return true;
    }

    // note that succs in this function mean potential succs
    auto successors = this->getSuccessors(state, order);
    auto orderedSuccs = order->sortStates(successors);
    if (orderedSuccs.back() == this->numberOfStates) {
        if (successors.size() == 2) {
            bool usePLANow = this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order];
            auto assumptions = usePLANow ? this->assumptionMaker->createAndCheckAssumptions(*(successors.begin()), *(successors.begin() + 1), order, region,
                                                                                            this->minValues[order], this->maxValues[order])
                                         : this->assumptionMaker->createAndCheckAssumptions(*(successors.begin()), *(successors.begin() + 1), order, region);
            if (assumptions.size() == 1) {
                this->handleAssumption(order, (assumptions.begin()->first));
                auto comp = order->compare(*(successors.begin()), *(successors.begin() + 1));
                if (comp == Order::NodeComparison::SAME) {
                    order->addToMdpScheduler(state, 0);
                    STORM_LOG_INFO("Best action for state " << state << " set to 0.");
                    return true;
                } else if (comp == Order::NodeComparison::ABOVE && prMax || comp == Order::NodeComparison::BELOW && !prMax) {
                    order->addToMdpScheduler(state, 0);
                    STORM_LOG_INFO("Best action for state " << state << " set to 0.");
                    return true;
                } else if (comp == Order::NodeComparison::BELOW && prMax || comp == Order::NodeComparison::ABOVE && !prMax) {
                    order->addToMdpScheduler(state, 1);
                    STORM_LOG_INFO("Best action for state " << state << " set to 1.");
                    return true;
                }
            }
        } else if (successors.size() == 3) {
            bool usePLANow = this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order];
            auto succ0 = *(successors.begin());
            auto succ1 = *(successors.begin() + 1);
            auto succ2 = *(successors.begin() + 2);
            auto comp01 = order->compareFast(succ0, succ1);
            auto comp02 = order->compareFast(succ0, succ2);
            auto comp12 = order->compareFast(succ1, succ2);
            if (comp01 == Order::NodeComparison::UNKNOWN) {
                auto assumptions =
                    usePLANow ? this->assumptionMaker->createAndCheckAssumptions(succ0, succ1, order, region, this->minValues[order], this->maxValues[order])
                              : this->assumptionMaker->createAndCheckAssumptions(succ0, succ1, order, region);
                if (assumptions.size() == 1) {
                    this->handleAssumption(order, (assumptions.begin()->first));
                    auto comp01 = order->compare(succ0, succ1);
                }
            }
            if (comp01 != Order::NodeComparison::UNKNOWN && comp02 == Order::NodeComparison::UNKNOWN) {
                auto assumptions =
                    usePLANow ? this->assumptionMaker->createAndCheckAssumptions(succ0, succ2, order, region, this->minValues[order], this->maxValues[order])
                              : this->assumptionMaker->createAndCheckAssumptions(succ0, succ2, order, region);
                if (assumptions.size() == 1) {
                    this->handleAssumption(order, (assumptions.begin()->first));
                    auto comp01 = order->compare(succ0, succ2);
                }
            }
            if (comp01 != Order::NodeComparison::UNKNOWN && comp02 != Order::NodeComparison::UNKNOWN && comp12 == Order::NodeComparison::UNKNOWN) {
                auto assumptions =
                    usePLANow ? this->assumptionMaker->createAndCheckAssumptions(succ1, succ2, order, region, this->minValues[order], this->maxValues[order])
                              : this->assumptionMaker->createAndCheckAssumptions(succ1, succ2, order, region);
                if (assumptions.size() == 1) {
                    this->handleAssumption(order, (assumptions.begin()->first));
                    auto comp01 = order->compare(succ1, succ2);
                }
            }
            if (comp01 != Order::NodeComparison::UNKNOWN && comp02 != Order::NodeComparison::UNKNOWN && comp12 != Order::NodeComparison::UNKNOWN) {
                return findBestAction(order, region, state);
            }
        }
        STORM_LOG_INFO("No best action found for state " << state << ".");
        return false;
    }
    if (prMax) {
        STORM_LOG_INFO("Interested in PrMax.");
        uint_fast64_t action = 0;
        auto numberOfOptionsForState = this->matrix.getRowGroupSize(state);
        std::set<uint_fast64_t> actionsToIgnore;
        while (action < numberOfOptionsForState) {
            if (actionsToIgnore.find(action) == actionsToIgnore.end()) {
                auto rowA = this->matrix.getRow(state, action);
                bool changed = false;
                for (uint_fast64_t i = action + 1; i < numberOfOptionsForState; ++i) {
                    if (actionsToIgnore.find(i) == actionsToIgnore.end()) {
                        auto rowB = this->matrix.getRow(state, i);
                        auto compRes = actionComparator.actionSMTCompare(order, orderedSuccs, region, &rowA, &rowB);
                        if (compRes == CompareResult::GEQ) {
                            // rowA is smaller or equal than rowB, so action is smaller than i, so we continue with action
                            // We will ignore i as we know action is better
                            actionsToIgnore.insert(i);
                        } else if (compRes == CompareResult::LEQ) {
                            changed = true;
                        } else {
                            // we ignore both action and i as i is sometimes better than action and vice versa
                            actionsToIgnore.insert(i);
                            actionsToIgnore.insert(action);
                        }
                    }
                }
                if (!changed) {
                    // this action is better than all other actions
                    order->addToMdpScheduler(state, action);
                    STORM_LOG_INFO("Best action for state " << state << " is " << action << ".");
                    return true;
                }
            }
            action++;
        }
        STORM_LOG_INFO("No best action found for state " << state << ".");
        return false;
    } else {
        STORM_LOG_INFO("Interested in PrMin.");
        auto action = 0;
        auto numberOfOptionsForState = this->matrix.getRowGroupSize(state);
        std::set<uint_fast64_t> actionsToIgnore;
        while (action < numberOfOptionsForState) {
            if (actionsToIgnore.find(action) == actionsToIgnore.end()) {
                auto rowA = this->matrix.getRow(state, action);
                bool changed = false;
                for (uint_fast64_t i = action + 1; i < numberOfOptionsForState; ++i) {
                    if (actionsToIgnore.find(i) == actionsToIgnore.end()) {
                        auto rowB = this->matrix.getRow(state, i);
                        auto compRes = actionComparator.actionSMTCompare(order, orderedSuccs, region, &rowA, &rowB);
                        if (compRes == CompareResult::LEQ) {
                            // rowA is smaller or equal than rowB, so action is smaller than i, so we continue with action
                            // We will ignore i as we know action is better
                            actionsToIgnore.insert(i);
                        } else if (compRes == CompareResult::GEQ) {
                            changed = true;
                        } else {
                            // we ignore both action and i as i is sometimes better than action and vice versa
                            actionsToIgnore.insert(i);
                            actionsToIgnore.insert(action);
                        }
                    }
                }
                if (!changed) {
                    // this action is better than all other actions
                    order->addToMdpScheduler(state, action);
                    STORM_LOG_INFO("Best action for state " << state << " is " << action << ".");
                    return true;
                }
            }
            action++;
        }
        STORM_LOG_INFO("No best action found for state " << state << ".");
        return false;
    }
}

template class OrderExtender<RationalFunction, double>;
template class OrderExtender<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm
