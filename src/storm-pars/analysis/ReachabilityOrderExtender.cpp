#include "storm-pars/analysis/ReachabilityOrderExtender.h"
#include <storage/StronglyConnectedComponentDecomposition.h>

namespace storm {
namespace analysis {

template<typename ValueType, typename ConstantType>
ReachabilityOrderExtender<ValueType, ConstantType>::ReachabilityOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model,
                                                                              std::shared_ptr<logic::Formula const> formula)
    : OrderExtender<ValueType, ConstantType>(model, formula) {
    this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix);
}

template<typename ValueType, typename ConstantType>
ReachabilityOrderExtender<ValueType, ConstantType>::ReachabilityOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates,
                                                                              storm::storage::SparseMatrix<ValueType> matrix, bool prMax)
    : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix, prMax) {
    this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix);
}

template<typename ValueType, typename ConstantType>
ReachabilityOrderExtender<ValueType, ConstantType>::ReachabilityOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates,
                                                                              storm::storage::SparseMatrix<ValueType> matrix)
    : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix, false) {
    this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix);
    STORM_LOG_ASSERT(this->deterministic, "Expecting model to be deterministic if prMax is not set");
}

template<typename ValueType, typename ConstantType>
std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> ReachabilityOrderExtender<ValueType, ConstantType>::extendOrder(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes,
    std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
    STORM_LOG_ASSERT(order != nullptr, "Order should be provided");
    if (assumption != nullptr) {
        this->handleAssumption(order, assumption);
    }

    auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
    while (currentStateMode.first != this->numberOfStates) {
        STORM_LOG_ASSERT(currentStateMode.first < this->numberOfStates, "Unexpected state number");
        auto& currentState = currentStateMode.first;
        STORM_LOG_INFO("Currently considering state " << currentState << " from " << (currentStateMode.second ? "sortedStates" : "statesToHandle"));
        while (order->isTopState(currentState) || order->isBottomState(currentState)) {
            currentStateMode = this->getNextState(order, currentState, true);
            currentState = currentStateMode.first;
        }
        bool actionFound = this->findBestAction(order, region, currentState);

        std::pair<uint_fast64_t, uint_fast64_t> result = {this->numberOfStates, this->numberOfStates};
        auto const& successorRes = this->getSuccessors(currentState, order);
        auto const& successors = successorRes.second;

        if (actionFound) {
            assert(successorRes.first);
            if (successors.size() == 1) {
                if (order->contains(successors[0])) {
                    this->handleOneSuccessor(order, currentState, successors[0]);
                } else {
                    result = {successors[0], successors[0]};
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
            assert(!successorRes.first);
            bool sufficientForState = true;
            for (auto itr1 = successors.begin(); itr1 < successors.end(); ++itr1) {
                for (auto itr2 = itr1 + 1; itr2 < successors.end(); ++itr2) {
                    // compare all successors with each other
                    if (order->compare(*itr1, *itr2) == Order::NodeComparison::UNKNOWN) {
                        auto assumptions =
                            this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order]
                                ? this->assumptionMaker->createAndCheckAssumptions(*itr1, *itr2, order, region, this->minValues[order], this->maxValues[order])
                                : this->assumptionMaker->createAndCheckAssumptions(*itr1, *itr2, order, region);
                        if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
                            this->handleAssumption(order, assumptions.begin()->first);
                            this->findBestAction(order, region, *itr1);
                            actionFound = this->findBestAction(order, region, currentState);
                        } else if (sufficientForState) {
                            result = {*itr1, *itr2};
                            sufficientForState = false;
                        }
                    }
                    if (actionFound) {
                        break;
                    }
                }
                if (actionFound) {
                    break;
                }
            }
            if (actionFound) {
                // We restart the loop as we now did find an action
                continue;
            }

            if (sufficientForState) {
                for (auto itr1 = successors.begin(); itr1 < successors.end(); ++itr1) {
                    // compare all successors with each other
                    if (order->compare(*itr1, currentState) == Order::NodeComparison::UNKNOWN) {
                        auto assumptions = this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order]
                                               ? this->assumptionMaker->createAndCheckAssumptions(*itr1, currentState, order, region, this->minValues[order],
                                                                                                  this->maxValues[order])
                                               : this->assumptionMaker->createAndCheckAssumptions(*itr1, currentState, order, region);
                        if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
                            this->handleAssumption(order, assumptions.begin()->first);
                        }
                    }
                }
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
            STORM_LOG_ASSERT(order->compare(result.first, result.second) == Order::UNKNOWN && order->compare(result.second, result.first) == Order::UNKNOWN,
                             "Expecting relation between the two states to be unknown");
            // Try to add states based on min/max and assumptions, only if we are not in statesToHandle mode
            if (currentStateMode.second && this->extendWithAssumption(order, region, result.first, result.second)) {
                continue;
            }
            // We couldn't extend the order
            if (this->nonParametricStates.find(currentState) != this->nonParametricStates.end()) {
                if (!order->contains(currentState)) {
                    // State is not parametric, so we hope that just adding it between =) and =( will help us
                    // We set is as sufficient as it is non-parametric and non-parametric states are by definition sufficient
                    order->add(currentState);
                    order->setSufficientForState(currentState);
                }
                if (this->matrix.getRowGroupSize(currentState) > 1) {
                    // State is non-deterministic
                    // If one of the successors is not yet in the order, we add it to a waitinglist and see if we can handle it as soon as we are done for the
                    // successor
                    for (auto& succ : this->getSuccessors(currentState, order).second) {
                        if (!order->isSufficientForState(succ)) {
                            this->dependentStates[succ].insert(currentState);
                        }
                    }
                }
                currentStateMode = this->getNextState(order, currentState, false);

                continue;
            } else {
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

    STORM_LOG_ASSERT(order->getDoneBuilding(), "Expecting to have a final order");
    if (monRes != nullptr) {
        // monotonicity result for the in-build checking of monotonicity
        monRes->setDone();
    }
    return std::make_tuple(order, this->numberOfStates, this->numberOfStates);
}

template<typename ValueType, typename ConstantType>
void ReachabilityOrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
    STORM_LOG_ASSERT(order->contains(successor), "Can't handle state with one successor if successor is not contained in order");
    if (currentState != successor) {
        if (order->contains(currentState)) {
            order->merge(currentState, successor);
        } else {
            order->addToNode(currentState, order->getNode(successor));
        }
    }
}

template<typename ValueType, typename ConstantType>
std::shared_ptr<Order> ReachabilityOrderExtender<ValueType, ConstantType>::getInitialOrder(bool isOptimistic) {
    std::shared_ptr<Order> order;
    if (this->bottomStates == boost::none || this->topStates == boost::none) {
        STORM_LOG_ASSERT(this->model != nullptr, "Can't get initial order if model is not specified");
        // STORM_LOG_THROW(this->matrix.getRowCount() == this->matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for
        // non-square matrix");
        modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*(this->model));
        storage::BitVector phiStates;
        storage::BitVector psiStates;
        STORM_LOG_ASSERT(this->formula->isProbabilityOperatorFormula(), "Can't get initial order if formula is not a probability operator formula");
        if (this->formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
            phiStates = propositionalChecker.check(this->formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())
                            ->asExplicitQualitativeCheckResult()
                            .getTruthValuesVector();
            psiStates = propositionalChecker.check(this->formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())
                            ->asExplicitQualitativeCheckResult()
                            .getTruthValuesVector();
        } else {
            STORM_LOG_ASSERT(this->formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula(),
                             "Expecting formula to be until or eventually formula");
            phiStates = storage::BitVector(this->numberOfStates, true);
            psiStates = propositionalChecker.check(this->formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())
                            ->asExplicitQualitativeCheckResult()
                            .getTruthValuesVector();
        }
        // Get the maybeStates
        std::pair<storage::BitVector, storage::BitVector> statesWithProbability01 =
            utility::graph::performProb01(this->model->getBackwardTransitions(), phiStates, psiStates);
        this->topStates = std::move(statesWithProbability01.second);
        this->bottomStates = std::move(statesWithProbability01.first);

        STORM_LOG_THROW(this->topStates->begin() != this->topStates->end(), exceptions::NotSupportedException, "Formula yields to no 1 states");
        STORM_LOG_THROW(this->bottomStates->begin() != this->bottomStates->end(), exceptions::NotSupportedException, "Formula yields to no zero states");
    }

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
        auto statesSorted = storm::utility::graph::getTopologicalSort(this->matrix.transpose(), firstStates);
        order = std::shared_ptr<Order>(new Order(&(this->topStates.get()), &(this->bottomStates.get()), this->numberOfStates, std::move(decomposition),
                                                 std::move(statesSorted), isOptimistic));
    } else {
        auto squareMatrix = this->matrix.getSquareMatrix();
        auto statesSorted = storm::utility::graph::getBFSTopologicalSort(squareMatrix.transpose(), this->matrix, firstStates);
        order = std::shared_ptr<Order>(new Order(&(this->topStates.get()), &(this->bottomStates.get()), this->numberOfStates, std::move(decomposition),
                                                 std::move(statesSorted), isOptimistic));
    }
    this->buildStateMap();

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

    return order;
}

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> ReachabilityOrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
    std::vector<uint_fast64_t> sortedSuccs;
    auto const& successors = this->getSuccessors(currentState, order).second;

    auto temp = order->sortStatesUnorderedPair(successors);
    if (temp.first.first != this->numberOfStates) {
        return temp.first;
    }
    sortedSuccs = std::move(temp.second);

    if (order->compare(sortedSuccs[0], sortedSuccs[sortedSuccs.size() - 1]) == Order::SAME) {
        if (!order->contains(currentState)) {
            order->addToNode(currentState, order->getNode(sortedSuccs[0]));
        } else {
            order->merge(currentState, sortedSuccs[0]);
        }
    } else {
        if (!order->contains(sortedSuccs[0])) {
            assert(order->isBottomState(sortedSuccs[sortedSuccs.size() - 1]));
            assert(sortedSuccs.size() == 2);
            order->addAbove(sortedSuccs[0], order->getBottom());
        }
        if (!order->contains(sortedSuccs[sortedSuccs.size() - 1])) {
            assert(order->isTopState(sortedSuccs[0]));
            assert(sortedSuccs.size() == 2);
            order->addBelow(sortedSuccs[sortedSuccs.size() - 1], order->getTop());
        }
        // sortedSuccs[0] is highest
        if (!order->contains(currentState)) {
            order->addBetween(currentState, sortedSuccs[0], sortedSuccs[sortedSuccs.size() - 1]);
        } else {
            order->addRelation(sortedSuccs[0], currentState);
            order->addRelation(currentState, sortedSuccs[sortedSuccs.size() - 1]);
        }
    }
    assert(order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE &&
           order->compare(order->getNode(currentState), order->getTop()) == Order::BELOW);
    return {this->numberOfStates, this->numberOfStates};
}

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> ReachabilityOrderExtender<ValueType, ConstantType>::extendByForwardReasoning(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
    STORM_LOG_ASSERT(order->contains(currentState), "Can't apply forward reasoning if order doesn't contain current state");
    STORM_LOG_ASSERT(this->cyclic, "Needs cyclic model for forward reasoning");

    std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sorted = this->sortForFowardReasoning(currentState, order);
    uint_fast64_t s1 = sorted.first.first;
    uint_fast64_t s2 = sorted.first.second;
    std::vector<uint_fast64_t>& statesSorted = sorted.second;
    if (s1 == this->numberOfStates) {
        STORM_LOG_ASSERT(statesSorted.size() == this->getSuccessors(currentState, order).second.size() + 1, "Expecting all states to be sorted, done for now");
        // all could be sorted, no need to do anything
    } else if (s2 == this->numberOfStates) {
        if (!order->contains(s1)) {
            order->add(s1);
        }
        if (statesSorted[0] == currentState) {
            order->addRelation(s1, statesSorted[0]);
            order->addRelation(s1, statesSorted[statesSorted.size() - 1]);  //, allowMerge);
            order->addStateToHandle(s1);
        } else if (statesSorted[statesSorted.size() - 1] == currentState) {
            order->addRelation(statesSorted[0], s1);                        //, allowMerge);
            order->addRelation(statesSorted[statesSorted.size() - 1], s1);  //, allowMerge);
            order->addStateToHandle(s1);
        } else {
            bool continueSearch = true;
            for (auto& entry : this->matrix.getRow(currentState)) {
                if (entry.getColumn() == s1) {
                    if (entry.getValue().isConstant()) {
                        continueSearch = false;
                    }
                }
            }
            if (continueSearch) {
                for (auto& i : statesSorted) {
                    if (order->compare(i, s1) == Order::UNKNOWN) {
                        return {i, s1};
                    }
                }
            }
        }
    } else {
        return {s1, s2};
    }
    assert(order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE &&
           order->compare(order->getNode(currentState), order->getTop()) == Order::BELOW);
    return {this->numberOfStates, this->numberOfStates};
}

template class ReachabilityOrderExtender<RationalFunction, double>;
template class ReachabilityOrderExtender<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm