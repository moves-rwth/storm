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

template<typename ValueType, typename ConstantType>
OrderExtender<ValueType, ConstantType>::OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula)
    : monotonicityChecker(model->getTransitionMatrix()) {
    this->model = model;
    init(model->getTransitionMatrix());
    this->formula = formula;
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

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::init(storm::storage::SparseMatrix<ValueType> matrix) {
    this->matrix = matrix;
    this->numberOfStates = this->matrix.getColumnCount();
    this->deterministic = matrix.getRowGroupCount() == matrix.getRowCount();
    this->actionComparator = ActionComparator<ValueType>();
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t s, std::shared_ptr<Order> order,
                                                                   typename OrderExtender<ValueType, ConstantType>::VariableType param,
                                                                   storm::storage::ParameterRegion<ValueType> region,
                                                                   std::shared_ptr<MonotonicityResult<VariableType>> monResult) {
    auto mon = monotonicityChecker.checkLocalMonotonicity(order, s, param, region);
    monResult->updateMonotonicityResult(param, mon);
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::buildStateMap() {
    this->dependentStates = std::vector<std::set<uint_fast64_t>>(numberOfStates, std::set<uint_fast64_t>());
    // Build stateMap
    auto rowCount = 0;
    auto currentOption = 0;
    auto numberOfOptionsForState = 0;
    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
        stateMap[state] = std::vector<std::vector<uint_fast64_t>>();
        std::set<VariableType> occurringVariables;
        numberOfOptionsForState = matrix.getRowGroupSize(state);
        while (currentOption < numberOfOptionsForState) {
            auto row = matrix.getRow(rowCount);
            stateMap[state].push_back(std::vector<uint64_t>());
            bool selfloop = false;
            for (auto& entry : row) {
                if (!(storm::utility::isZero<ValueType>(entry.getValue()))) {
                    selfloop |= state == entry.getColumn();
                    stateMap[state][currentOption].push_back(entry.getColumn());
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
        bool res = false;
        std::vector<uint_fast64_t> successors;
        for (auto& succs : this->stateMap[state]) {
            if (first) {
                for (auto succ : succs) {
                    successors.push_back(succ);
                }
                first = false;
            } else {
                for (auto succ : succs) {
                    if (std::find(successors.begin(), successors.end(), succ) == successors.end()) {
                        res = false;
                        successors.push_back(succ);
                    }
                }
            }
        }
        stateMapAllSucc[state] = {res, std::move(successors)};
    }
}

template<typename ValueType, typename ConstantType>
std::pair<std::vector<uint_fast64_t>,storage::StronglyConnectedComponentDecomposition<ValueType>> OrderExtender<ValueType, ConstantType>::sortStatesAndDecomposeForOrder() {
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
        return {storm::utility::graph::getTopologicalSort(this->matrix.transpose(), firstStates), decomposition};
    } else {
        auto squareMatrix = this->matrix.getSquareMatrix();
        return {storm::utility::graph::getBFSTopologicalSort(squareMatrix.transpose(), this->matrix, firstStates), decomposition};
    }
}

template<typename ValueType, typename ConstantType>
std::shared_ptr<Order> OrderExtender<ValueType, ConstantType>::getInitialOrder() {
    setBottomTopStates();
    STORM_LOG_THROW(this->bottomStates.is_initialized(), storm::exceptions::UnexpectedException, "Expecting the formula to yield bottom states for the order");
    if (this->bottomStates->getNumberOfSetBits() == 0 && (this->topStates == boost::none || this->topStates->getNumberOfSetBits() == 0)) {
        return nullptr;
    }

    auto statesSortedAndDecomposition = this->sortStatesAndDecomposeForOrder();

    // Create Order
    std::shared_ptr<Order> order = std::shared_ptr<Order>(
        new Order(&(this->topStates.get()), &(this->bottomStates.get()), this->numberOfStates, std::move(statesSortedAndDecomposition.second), std::move(statesSortedAndDecomposition.first)));
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
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::toOrder(
    storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
    auto order = getInitialOrder();
    if (order == nullptr) {
        return {nullptr, numberOfStates, numberOfStates};
    }
    return extendOrder(order, region, monRes, nullptr);
}

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

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendNormal(std::shared_ptr<Order> order,
                                                                                             storm::storage::ParameterRegion<ValueType> region,
                                                                                             uint_fast64_t currentState) {
    // when it is cyclic and the current state is part of an SCC we do forwardreasoning
    if (this->cyclic && !order->isTrivial(currentState) && !order->contains(currentState)) {
        std::vector<uint_fast64_t> const& successors = this->getSuccessors(currentState, order).second;
        if (successors.size() == 2 && (successors[0] == currentState || successors[1] == currentState)) {
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
void OrderExtender<ValueType, ConstantType>::handleAssumption(std::shared_ptr<Order> order,
                                                              std::shared_ptr<expressions::BinaryRelationExpression> assumption) const {
    STORM_LOG_ASSERT(assumption != nullptr, "Can't handle assumption when the assumption is a nullpointer");
    STORM_LOG_ASSERT(assumption->getFirstOperand()->isVariable() && assumption->getSecondOperand()->isVariable(),
                     "Expecting the assumption operands to be variables");

    expressions::Variable var1 = assumption->getFirstOperand()->asVariableExpression().getVariable();
    expressions::Variable var2 = assumption->getSecondOperand()->asVariableExpression().getVariable();
    auto const& val1 = std::stoul(var1.getName(), nullptr, 0);
    auto const& val2 = std::stoul(var2.getName(), nullptr, 0);

    STORM_LOG_ASSERT(order->compare(val1, val2) == Order::UNKNOWN, "Assumption for states that are already ordered, expecting them to be unordered");

    Order::Node* n1 = order->getNode(val1);
    Order::Node* n2 = order->getNode(val2);

    if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal) {
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
        STORM_LOG_ASSERT(assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater,
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
}

template<typename ValueType, typename ConstantType>
std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> OrderExtender<ValueType, ConstantType>::sortStatesOrderAndMinMax(
    std::vector<uint_fast64_t> const& states, std::shared_ptr<Order> order) {
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
            for (auto index = 0; index < result.size(); ++index) {
                auto compareRes = order->compare(state, result[index]);
                if (compareRes == Order::NodeComparison::ABOVE || compareRes == Order::NodeComparison::SAME) {
                    if (!order->contains(state)) {
                        // This can only happen if *itr refers to top/bottom state
                        order->add(state);
                        order->addStateToHandle(state);
                    }
                    // insert at current pointer (while keeping other values)
                    result.insert(result.begin() + index, state);
                    added = true;
                    break;
                } else if (compareRes == Order::NodeComparison::UNKNOWN) {
                    if (usePLA[order]) {
                        compareRes = addStatesBasedOnMinMax(order, state, result[index]);
                    }
                    if (compareRes == Order::NodeComparison::ABOVE || compareRes == Order::NodeComparison::SAME) {
                        // insert at current pointer (while keeping other values)
                        result.insert(result.begin() + index, state);
                        added = true;
                        STORM_LOG_ASSERT(order->contains(state), "Expecting order to contain state" << state);
                        STORM_LOG_ASSERT(order->contains(result[index]), "Expecting order to contain state" << result[index]);
                        break;
                    } else if (compareRes == Order::NodeComparison::UNKNOWN) {
                        return {{(result[index]), state}, std::move(result)};
                    }
                }
            }
            if (!added) {
                result.push_back(state);
            }
        }
    }

    assert(result.size() == numberOfStatesToSort);
    return {{numberOfStates, numberOfStates}, std::move(result)};
}

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
    if (mins[state1] > maxs[state2]) {
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
    } else if (mins[state2] > maxs[state1]) {
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
void OrderExtender<ValueType, ConstantType>::initializeMinMaxValues(storage::ParameterRegion<ValueType> region, std::shared_ptr<Order> order) {
    if (model != nullptr && model->isOfType(storm::models::ModelType::Dtmc)) {
        // Use parameter lifting modelchecker to get initial min/max values for order creation
        modelchecker::SparseDtmcParameterLiftingModelChecker<models::sparse::Dtmc<ValueType>, ConstantType> plaModelChecker;
        std::unique_ptr<modelchecker::CheckResult> checkResult;
        auto env = Environment();
        boost::optional<modelchecker::CheckTask<logic::Formula, ValueType>> checkTask;
        if (this->formula->hasQuantitativeResult()) {
            checkTask = storm::api::createTask<ValueType>(formula, false);
        } else {
            // Remove the >=/<= information from the formula
            storm::logic::OperatorInformation opInfo(boost::none, boost::none);
            if (formula->isProbabilityOperatorFormula()) {
                auto newFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                    formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo);
                checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*newFormula);
            } else {
                STORM_LOG_ASSERT(formula->isRewardOperatorFormula(), "Expecting formula to be reward formula");
                auto newFormula = std::make_shared<storm::logic::RewardOperatorFormula>(formula->asRewardOperatorFormula().getSubformula().asSharedPointer(),
                                                                                        model->getUniqueRewardModelName(), opInfo);
                checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*newFormula);
            }
        }
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
    } else if (model != nullptr && model->isOfType(storm::models::ModelType::Mdp)) {
        // Use parameter lifting modelchecker to get initial min/max values for order creation
        modelchecker::SparseMdpParameterLiftingModelChecker<models::sparse::Mdp<ValueType>, ConstantType> plaModelChecker;
        std::unique_ptr<modelchecker::CheckResult> checkResult;
        auto env = Environment();
        boost::optional<modelchecker::CheckTask<logic::Formula, ValueType>> checkTask;
        if (this->formula->hasQuantitativeResult()) {
            checkTask = storm::api::createTask<ValueType>(formula, false);
        } else {
            storm::logic::OperatorInformation opInfo(boost::none, boost::none);
            if (formula->isProbabilityOperatorFormula()) {
                auto newFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                    formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo);
                checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*newFormula);
            } else {
                STORM_LOG_ASSERT(formula->isRewardOperatorFormula(), "Expecting formula to be reward formula");
                auto newFormula = std::make_shared<storm::logic::RewardOperatorFormula>(formula->asRewardOperatorFormula().getSubformula().asSharedPointer(),
                                                                                        model->getUniqueRewardModelName(), opInfo);
                checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*newFormula);
            }
        }
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
void OrderExtender<ValueType, ConstantType>::setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) {
    assert(state1 != numberOfStates && state2 != numberOfStates);
    unknownStatesMap[order] = {state1, state2};
}

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::copyUnknownStates(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy) {
    assert(unknownStatesMap.find(orderCopy) == unknownStatesMap.end());
    unknownStatesMap.insert({orderCopy, {unknownStatesMap[orderOriginal].first, unknownStatesMap[orderOriginal].second}});
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
            order->setDoneForState(state);
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
bool OrderExtender<ValueType, ConstantType>::isHope(std::shared_ptr<Order> order) const {
    if (order->getDoneBuilding()) {
        return false;
    }
    return continueExtending.find(order) != continueExtending.end() && continueExtending.at(order);
}

template<typename ValueType, typename ConstantType>
std::pair<bool, std::vector<uint_fast64_t>&> OrderExtender<ValueType, ConstantType>::getSuccessors(uint_fast64_t state, std::shared_ptr<Order> order) {
    if (this->stateMap[state].size() == 1) {
        assert(stateMap[state][0].size() > 0);
        return {true, stateMap[state][0]};
    }
    if (order->isActionSetAtState(state)) {
        assert(stateMap[state][order->getActionAtState(state)].size() > 0);
        return {true, stateMap[state][order->getActionAtState(state)]};
    }
    return stateMapAllSucc[state];
}
template<typename ValueType, typename ConstantType>
std::vector<uint_fast64_t>& OrderExtender<ValueType, ConstantType>::getSuccessors(uint_fast64_t state, uint_fast64_t action) {
    STORM_LOG_ASSERT(state < stateMap.size(), "State number too large");
    STORM_LOG_ASSERT(action < stateMap[state].size(), "Action number too large");
    return stateMap[state][action];
}

template<typename ValueType, typename ConstantType>
std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> OrderExtender<ValueType, ConstantType>::sortForFowardReasoning(
    uint_fast64_t currentState, std::shared_ptr<Order> order) {
    std::vector<uint_fast64_t> statesSorted;
    statesSorted.push_back(currentState);
    bool pla = (this->usePLA.find(order) != this->usePLA.end() && this->usePLA.at(order));
    // Go over all states
    bool oneUnknown = false;
    bool unknown = false;
    uint_fast64_t s1 = this->numberOfStates;
    uint_fast64_t s2 = this->numberOfStates;
    std::vector<uint_fast64_t> const& successors = this->getSuccessors(currentState, order).second;
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

template<typename ValueType, typename ConstantType>
void OrderExtender<ValueType, ConstantType>::addStatesMinMax(std::shared_ptr<Order> order) {
    auto transposeMatrix = this->matrix.getSquareMatrix().transpose();
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
        auto& successors = this->getSuccessors(state, order).second;
        bool allSorted = true;
        for (uint_fast64_t i1 = 0; i1 < successors.size(); ++i1) {
            for (uint_fast64_t i2 = i1 + 1; i2 < successors.size(); ++i2) {
                auto succ1 = successors[i1];
                auto succ2 = successors[i2];
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
        }
    }
}

template<typename ValueType, typename ConstantType>
bool OrderExtender<ValueType, ConstantType>::findBestAction(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType>& region, uint_fast64_t state) {
    if (deterministic) {
        return true;
    }
    // Finding the best action for the current state
    STORM_LOG_INFO("Looking for best action for state " << state << std::endl);
    if (order->isActionSetAtState(state)) {
        STORM_LOG_INFO("Best action for state " << state << " is already set." << std::endl);
        return true;
    }
    if (this->stateMap[state].size() == 1) {
        // if we only have one possible action, we already know which one we take.
        STORM_LOG_INFO("   Only one Action available, take it." << std::endl);
        order->addToMdpScheduler(state, 0);
        return true;
    }
    if (order->isTopState(state)) {
        // in this case the state should be absorbing so we just take action 0
        STORM_LOG_INFO("   State is top state, thus absorbing. Take action 0." << std::endl);
        order->addToMdpScheduler(state, 0);
        return true;
    }
    if (order->isBottomState(state)) {
        // in this case the state should be absorbing so we just take action 0
        STORM_LOG_INFO("   State is bottom state, thus absorbing. Take action 0." << std::endl);
        order->addToMdpScheduler(state, 0);
        return true;
    }

    // note that succs in this function mean potential succs
    auto successors = this->getSuccessors(state, order);
    auto orderedSuccs = order->sortStates(successors.second);
    if (orderedSuccs.back() == this->numberOfStates) {
        STORM_LOG_WARN("    No best action found, as the successors could not be ordered.");
        return false;
    }
    if (prMax) {
        STORM_LOG_INFO("   Interested in PrMax." << std::endl);
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
                    return true;
                }
            }
            action++;
        }
        return false;
    } else {
        STORM_LOG_INFO("   Interested in PrMin." << std::endl);
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
                    return true;
                }
            }
            action++;
        }
        return false;
    }
}
template class OrderExtender<RationalFunction, double>;
template class OrderExtender<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm
