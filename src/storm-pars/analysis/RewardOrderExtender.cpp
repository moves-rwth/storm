#include "storm-pars/analysis/RewardOrderExtender.h"

namespace storm {
namespace analysis {

template<typename ValueType, typename ConstantType>
RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model,
                                                                  std::shared_ptr<logic::Formula const> formula)
    : OrderExtender<ValueType, ConstantType>(model, formula) {
    this->rewardModel = this->model->getUniqueRewardModel();
    this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(
        this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel()));
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (rewardModel.hasStateActionRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateActionReward(i).isConstant(), "Expecting rewards to be constant");
        } else if (rewardModel.hasStateRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateReward(i).isConstant(), "Expecting rewards to be constant");
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
    }
}

template<typename ValueType, typename ConstantType>
RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates,
                                                                  storm::storage::SparseMatrix<ValueType> matrix,
                                                                  storm::models::sparse::StandardRewardModel<ValueType> rewardModel, bool prMax)
    : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix, prMax) {
    this->rewardModel = rewardModel;
    this->assumptionMaker =
        new AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(rewardModel));
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (rewardModel.hasStateActionRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateActionReward(i).isConstant(), "Expecting rewards to be constant");
        } else if (rewardModel.hasStateRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateReward(i).isConstant(), "Expecting rewards to be constant");
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
    }
}
template<typename ValueType, typename ConstantType>
RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(storm::storage::BitVector& topStates, storm::storage::BitVector& bottomStates,
                                                                  storm::storage::SparseMatrix<ValueType> matrix,
                                                                  storm::models::sparse::StandardRewardModel<ValueType> rewardModel)
    : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix, false) {
    STORM_LOG_ASSERT(this->deterministic, "Expecting model to be deterministic if prMax is not set");
    this->rewardModel = rewardModel;
    this->assumptionMaker =
        new AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(rewardModel));
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (rewardModel.hasStateActionRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateActionReward(i).isConstant(), "Expecting rewards to be constant");
        } else if (rewardModel.hasStateRewards()) {
            STORM_LOG_ASSERT(rewardModel.getStateReward(i).isConstant(), "Expecting rewards to be constant");
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
    }
}
//
// template<typename ValueType, typename ConstantType>
// std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> RewardOrderExtender<ValueType, ConstantType>::extendOrder(
//    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes,
//    std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
//    STORM_LOG_ASSERT(order != nullptr, "Order should be provided");
//    STORM_LOG_INFO_COND(assumption == nullptr, "Extending order with assumption: " << *assumption);
//    if (assumption != nullptr) {
//        this->handleAssumption(order, assumption);
//    }
//
//    auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
//    while (currentStateMode.first != this->numberOfStates) {
//        STORM_LOG_ASSERT(currentStateMode.first < this->numberOfStates, "Unexpected state number");
//        auto& currentState = currentStateMode.first;
//
//        if (currentStateMode.second) {
//            STORM_LOG_INFO("Currently considering state: " << currentState << " based on topological sorting");
//            //                    assumptionsCreated.set(currentState, false);
//        } else {
//            STORM_LOG_INFO("Currently considering state: " << currentState << " based on statesToHandle (not topological approach)");
//        }
//
//        auto const& successors = this->getSuccessors(currentState, order);
//        std::pair<uint_fast64_t, uint_fast64_t> result = {this->numberOfStates, this->numberOfStates};
//
//        if (successors.size() == 1) {
//            auto succ0 = *(successors.begin());
//            if (!currentStateMode.second && !order->contains(succ0)) {
//                order->add(succ0);
//            }
//            STORM_LOG_ASSERT(order->contains(succ0), "Expecting order to contain successor of state " << currentState);
//            this->handleOneSuccessor(order, currentState, succ0);
//        } else if (!successors.empty()) {
//            if (order->isOnlyInitialOrder()) {
//                order->add(currentState);
//                if (!order->isTrivial(currentState)) {
//                    // This state is part of an scc, therefore, we could do forward reasoning here
//                    result = this->extendByForwardReasoning(order, region, currentState);
//                } else {
//                    result = {this->numberOfStates, this->numberOfStates};
//                }
//            } else {
//                result = this->extendNormal(order, region, currentState);
//            }
//        }
//
//        if (result.first == this->numberOfStates) {
//            // We did extend the order
//            STORM_LOG_ASSERT(result.second == this->numberOfStates, "Expecting both parts of result to contain the number of states");
//            STORM_LOG_ASSERT(order->sortStates(successors).size() == successors.size(), "Something went wrong while sorting states, number of states
//            differs"); STORM_LOG_ASSERT(order->contains(currentState) && order->getNode(currentState) != nullptr, "Expecting order to contain the current
//            State");
//
//            if (monRes != nullptr) {
//                for (auto& param : this->occuringVariablesAtState[currentState]) {
//                    this->checkParOnStateMonRes(currentState, order, param, region, monRes);
//                }
//            }
//            for (auto& entry : this->transposeMatrix.getRow(currentState)) {
//                order->addStateToHandle(entry.getColumn());
//            }
//            // Get the next state
//            currentStateMode = this->getNextState(order, currentState, true);
//        } else {
//            STORM_LOG_ASSERT(result.first < this->numberOfStates && result.second < this->numberOfStates,
//                             "Expecting both result numbers to correspond to states");
//            STORM_LOG_ASSERT(order->compare(result.first, result.second) == Order::UNKNOWN, "Expecting relation between the two states to be unknown");
//            // Try to add states based on min/max and assumptions, only if we are not in statesToHandle mode
//            if (currentStateMode.second && this->extendWithAssumption(order, region, result.first, result.second)) {
//                continue;
//            }
//            // We couldn't extend the order
//            if (this->nonParametricStates.find(currentState) != this->nonParametricStates.end()) {
//                if (!order->contains(currentState)) {
//                    // State is not parametric, so we hope that just adding it between =) and =( will help us
//                    order->add(currentState);
//                }
//                for (auto& entry : this->transposeMatrix.getRow(currentState)) {
//                    order->addStateToHandle(entry.getColumn());
//                }
//                currentStateMode = this->getNextState(order, currentState, true);
//                continue;
//            } else {
//                if (!currentStateMode.second) {
//                    // The state was based on statesToHandle, so it is not bad if we cannot continue with this.
//                    currentStateMode = this->getNextState(order, currentState, false);
//                    continue;
//                } else {
//                    // The state was based on the topological sorting, so we need to return, but first add this state to the states Sorted as we are not done
//                    // with it
//                    order->addStateSorted(currentState);
//                    this->continueExtending[order] = false;
//                    return {order, result.first, result.second};
//                }
//            }
//        }
//
//        STORM_LOG_ASSERT(order->sortStates(successors).size() == successors.size(), "Expecting all successor states to be sorted");
//    }
//
//    STORM_LOG_ASSERT(order->getDoneBuilding(), "Expecting to have a final order");
//    if (monRes != nullptr) {
//        // monotonicity result for the in-build checking of monotonicity
//        monRes->setDone();
//    }
//    return std::make_tuple(order, this->numberOfStates, this->numberOfStates);
//}

template<typename ValueType, typename ConstantType>
void RewardOrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
    ValueType reward = ValueType(0);
    if (rewardModel.hasStateActionRewards()) {
        reward = rewardModel.getStateActionReward(currentState);
    } else if (rewardModel.hasStateRewards()) {
        reward = rewardModel.getStateReward(currentState);
    } else {
        STORM_LOG_ASSERT(false, "Expecting reward");
    }
    if (reward == ValueType(0)) {
        order->addToNode(currentState, order->getNode(successor));
    } else {
        STORM_LOG_ASSERT(!(reward < ValueType(0)), "Expecting reward to be positive");
        order->addAbove(currentState, order->getNode(successor));
    }
}

template<typename ValueType, typename ConstantType>
void RewardOrderExtender<ValueType, ConstantType>::setBottomTopStates() {
    if (this->bottomStates == boost::none || this->topStates == boost::none) {
        STORM_LOG_ASSERT(this->model != nullptr, "Can't get initial order if model is not specified");
        modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*(this->model));

        // TODO check if eventually formula is true for all initial states?
        // TODO Add UNTIL formulas
        STORM_LOG_THROW(this->formula->asRewardOperatorFormula().getSubformula().isEventuallyFormula(), storm::exceptions::NotImplementedException,
                        "Expected reward only implemented for eventually formulla");
        this->topStates = storage::BitVector(this->numberOfStates, false);
        this->bottomStates = propositionalChecker.check(this->formula->asRewardOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())
                                 ->asExplicitQualitativeCheckResult()
                                 .getTruthValuesVector();
    }
}

template<typename ValueType, typename ConstantType>
void RewardOrderExtender<ValueType, ConstantType>::checkRewardsForOrder(std::shared_ptr<Order> order) {
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        auto& successors = this->getSuccessors(i, order);
        if (successors.size() == 2) {
            rewardHack(order, i, *(successors.begin()), *(successors.begin() + 1));
        }
    }

    for (uint_fast64_t i : order->getBottom()->states) {
        for (auto& entry : this->transposeMatrix.getRow(i)) {
            order->addStateToHandle(entry.getColumn());
        }
    }

    // TODO aanpassen voor mdps
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        auto& successorsI = this->getSuccessors(i, order);
        if (successorsI.size() == 2) {
            for (uint_fast64_t j = i + 1; j < this->numberOfStates; ++j) {
                auto& successorsJ = this->getSuccessors(j, order);
                bool checkReward = false;

                if (*(successorsI.begin()) == *(successorsJ.begin()) && *(successorsI.begin() + 1) == *(successorsJ.begin() + 1)) {
                    checkReward = this->matrix.getRow(i).begin()->getValue() == this->matrix.getRow(j).begin()->getValue();
                }

                if (checkReward) {
                    // check welke reward t grootste is

                    ValueType rewardI = ValueType(0);
                    ValueType rewardJ = ValueType(0);
                    if (rewardModel.hasStateActionRewards()) {
                        rewardI = rewardModel.getStateActionReward(i);
                        rewardJ = rewardModel.getStateActionReward(j);
                    } else if (rewardModel.hasStateRewards()) {
                        rewardI = rewardModel.getStateReward(i);
                        rewardJ = rewardModel.getStateReward(j);
                    } else {
                        STORM_LOG_ASSERT(false, "Expecting reward");
                    }

                    if (!order->contains(i)) {
                        order->add(i);
                        order->addStateToHandle(i);
                    }
                    if (!order->contains(j)) {
                        order->add(j);
                        order->addStateToHandle(j);
                    }
                    if (rewardI.constantPart() > rewardJ.constantPart()) {
                        order->addAbove(i, order->getNode(j));
                    } else if (rewardJ.constantPart() > rewardI.constantPart()) {
                        order->addAbove(j, order->getNode(i));
                    } else {
                        order->addRelation(i, j, true);
                    }
                }
            }
        }
    }
}

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtender<ValueType, ConstantType>::extendByForwardReasoning(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
    STORM_LOG_INFO("Doing Forward reasoning");
    STORM_LOG_ASSERT(order->contains(currentState), "Expecting order to contain the current state for forward reasoning");

    auto const& successors = this->getSuccessors(currentState, order);

    if (successors.size() == 2 && this->extendByForwardReasoningOneSucc(order, region, currentState)) {
        return {this->numberOfStates, this->numberOfStates};
    }
    std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sorted = this->sortForFowardReasoning(currentState, order);
    uint_fast64_t s1 = sorted.first.first;
    uint_fast64_t s2 = sorted.first.second;
    std::vector<uint_fast64_t>& statesSorted = sorted.second;

    if (s1 != this->numberOfStates && s2 != this->numberOfStates) {
        // Several states could not be ordered
        if (s1 == currentState || s2 == currentState) {
            // We could not order a successor because the relation to our currentSTate is unknown
            // It could be that we can actually order all successors, therefore we try backward reasoning
            return extendByBackwardReasoning(order, region, currentState);
        } else {
            return {s1, s2};
        }
    } else if (statesSorted.size() == (this->getSuccessors(currentState, order).size() + 1)) {
        // Everything is sorted, so no need to sort stuff
        assert(s1 == this->numberOfStates && s2 == this->numberOfStates);
        return {s1, s2};
    } else {
        // s1 is the state we could not sort, this should be one of the successors.
        STORM_LOG_ASSERT(s2 == this->numberOfStates, "Expecting only one state not to be sorted");
        STORM_LOG_ASSERT(s1 != this->numberOfStates, "Expecting only one state not to be sorted");
        STORM_LOG_ASSERT(s1 != currentState, "Expecting the unsorted state to not be the current state");
        // TODO: change all usePLA[order] to find version.
        if (statesSorted.at(statesSorted.size() - 1) == currentState) {
            // the current state is lower than all other successors, so s1 should be smaller then all other successors
            order->addBelow(s1, order->getNode(currentState));
            order->addStateToHandle(s1);
            return {s2, s2};
        } else if (this->usePLA[order]) {
            // TODO: make use of forward reasoning and do backward as last solution
            return extendByBackwardReasoning(order, region, currentState);
        } else {
            for (uint_fast64_t state : this->getSuccessors(currentState, order)) {
                // Find a state to which we cannot order s1, it should be one of the successors
                if (state != s1 && order->compare(s1, state) == Order::UNKNOWN) {
                    // Problem is with comparing
                    return {s1, state};
                }
            }
            // In case this did not help, all successor states can be sorted.
            if (!order->contains(s1)) {
                // It could be that s1 was not yet added, and could only be sorted because the other state was top/bottom, so we add it.
                order->add(s1);
                order->addStateToHandle(s1);
            }
            // Relation between s1 and currState is still unknown, maybe we can find out

            // We check if we can also sort something based on assumptions.
            //                    if (!assumptionsCreated[currentState]) {
            if (this->usePLA[order]) {
                auto assumptions =
                    this->assumptionMaker->createAndCheckAssumptions(currentState, s1, order, region, this->minValues[order], this->maxValues[order]);
                if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                    this->handleAssumption(order, assumptions.begin()->first);
                    //                            } else {
                    //                                assumptionsCreated.set(currentState);
                }
            } else {
                auto assumptions = this->assumptionMaker->createAndCheckAssumptions(currentState, s1, order, region);
                if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                    this->handleAssumption(order, assumptions.begin()->first);
                    //                            } else {
                    //                                assumptionsCreated.set(currentState);
                }
            }
            //                    }
            STORM_LOG_ASSERT(order->sortStates(this->getSuccessors(currentState, order)).size() == this->getSuccessors(currentState, order).size(),
                             "Expecting all successor states to be ordered");
            return {s2, s2};
        }
    }
    return {this->numberOfStates, this->numberOfStates};
}

template<typename ValueType, typename ConstantType>
bool RewardOrderExtender<ValueType, ConstantType>::extendByForwardReasoningOneSucc(std::shared_ptr<Order> order,
                                                                                   storm::storage::ParameterRegion<ValueType> region,
                                                                                   uint_fast64_t currentState) {
    STORM_LOG_ASSERT(order->contains(currentState), "Expecting order to contain the current state for forward reasoning");

    auto const& successors = this->getSuccessors(currentState, order);
    STORM_LOG_ASSERT(successors.size() == 2, "Expecting only two successors");

    auto succ0 = *(successors.begin());
    auto succ1 = *(successors.begin() + 1);
    if (succ0 == currentState || succ1 == currentState) {
        // current state actually only has one real successor
        auto realSucc = succ0 == currentState ? succ1 : succ0;
        ValueType reward = ValueType(0);
        if (rewardModel.hasStateActionRewards()) {
            reward = rewardModel.getStateActionReward(currentState);
        } else if (rewardModel.hasStateRewards()) {
            reward = rewardModel.getStateReward(currentState);
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }

        if (reward.isZero()) {
            if (!order->contains(realSucc)) {
                order->add(realSucc);
                order->addStateToHandle(realSucc);
            }
            order->addToNode(currentState, order->getNode(realSucc));
        } else {
            if (!order->contains(realSucc)) {
                order->add(realSucc);
                order->addStateToHandle(realSucc);
            }
            order->addAbove(currentState, order->getNode(realSucc));
        }
    } else if (order->isBottomState(succ0) || order->isBottomState(succ1)) {
        auto bottomState = order->isBottomState(succ0) ? succ0 : succ1;
        auto otherState = bottomState == succ0 ? succ1 : succ0;
        // If there is only one transition going into bottomState (+ selfloop) we can do some normal forward reasoning
        if (this->transposeMatrix.getRow(bottomState).getNumberOfEntries() == 2) {
            if (!order->contains(otherState)) {
                order->add(otherState);
                order->addStateToHandle(otherState);
            }
            order->addBetween(currentState, otherState, bottomState);
        }
    } else {
        return false;
    }
    return true;
}

template<typename ValueType, typename ConstantType>
std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(
    std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
    STORM_LOG_INFO("Doing backward reasoning");
    bool addedSomething = false;
    auto const& successors = this->getSuccessors(currentState, order);
    // We sort the states, and then apply min/max comparison.
    // This also adds states to the order if they are not yet sorted, but can be sorted based on min/max values

    auto sortedSuccStates = this->sortStatesOrderAndMinMax(successors, order);
    if (sortedSuccStates.first.first != this->numberOfStates) {
        if (successors.size() == 2 && (*(successors.begin()) == currentState || *(successors.begin() + 1) == currentState)) {
            // current state actually only has one real successor
            auto realSucc = *(successors.begin()) == currentState ? *(successors.begin() + 1) : *(successors.begin());
            ValueType reward = ValueType(0);
            if (rewardModel.hasStateActionRewards()) {
                reward = rewardModel.getStateActionReward(currentState);
            } else if (rewardModel.hasStateRewards()) {
                reward = rewardModel.getStateReward(currentState);
            } else {
                STORM_LOG_ASSERT(false, "Expecting reward");
            }

            if (!order->contains(realSucc)) {
                order->add(realSucc);
                order->addStateToHandle(realSucc);
            }
            if (reward.isZero()) {
                order->addToNode(currentState, order->getNode(realSucc));
            } else {
                order->addAbove(currentState, order->getNode(realSucc));
            }

        } else {
            return sortedSuccStates.first;
        }
    } else {
        // We could order all successor states
        ValueType reward = ValueType(0);
        if (rewardModel.hasStateActionRewards()) {
            reward = rewardModel.getStateActionReward(currentState);
        } else if (rewardModel.hasStateRewards()) {
            reward = rewardModel.getStateReward(currentState);
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }

        STORM_LOG_INFO("Reward at this state (" << currentState << "): " << reward);

        if (reward.isZero()) {
            if (order->compare(*(sortedSuccStates.second.begin()), sortedSuccStates.second.back()) == Order::NodeComparison::SAME) {
                order->addToNode(currentState, order->getNode(sortedSuccStates.second.back()));
            } else {
                order->addBetween(currentState, *(sortedSuccStates.second.begin()), sortedSuccStates.second.back());
            }
        } else {
            // We are considering rewards, so our current state is always above the lowest one of all our successor states
            order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));
            // We check if we can also sort something based on assumptions.
            if (this->usePLA[order]) {
                for (uint_fast64_t succ : successors) {
                    if (order->compare(currentState, succ) == Order::NodeComparison::UNKNOWN) {
                        auto compare = this->addStatesBasedOnMinMax(order, currentState, succ);
                        if (compare == Order::NodeComparison::UNKNOWN) {
                            // check if r(s) > (1-P(s,sj)) * (max(sj) - min(s1))
                            // for |successors| == 2 check if r(s) > P (s,s1)*(min(s2)-max(s1))
                            // s1 is lowest in order
                            // succ == sj == s2
                            auto s1 = *(successors.begin());
                            ValueType function;
                            for (auto& entry : this->matrix.getRow(currentState)) {
                                if (entry.getColumn() == succ) {
                                    function = entry.getValue();
                                    break;
                                }
                            }
                            if (function.gatherVariables().size() == 1) {
                                std::map<VariableType, CoefficientType> val1, val2;
                                auto& var = *(function.gatherVariables().begin());
                                val1.insert({var, region.getLowerBoundary(var)});
                                val2.insert({var, region.getUpperBoundary(var)});
                                CoefficientType res1 = function.evaluate(val1);
                                CoefficientType res2 = function.evaluate(val2);
                                if (res2 < res1) {
                                    std::swap(res1, res2);
                                }
                                if (storm::utility::convertNumber<ConstantType>(reward.constantPart()) >
                                    storm::utility::convertNumber<ConstantType>(res2) * (this->maxValues[order][succ] - this->minValues[order][s1])) {
                                    if (!order->contains(succ)) {
                                        order->add(succ);
                                    }
                                    order->addAbove(currentState, order->getNode(succ));
                                    compare = Order::NodeComparison::ABOVE;
                                } else if (successors.size() == 2 && succ == *(successors.begin())) {
                                    if (storm::utility::convertNumber<ConstantType>(reward.constantPart()) <
                                        storm::utility::convertNumber<ConstantType>(res1) * (this->minValues[order][succ] - this->maxValues[order][s1])) {
                                        if (!order->contains(currentState)) {
                                            order->add(currentState);
                                        }
                                        order->addAbove(succ, order->getNode(currentState));
                                        compare = Order::NodeComparison::BELOW;
                                    }
                                }
                            }
                        }

                        if (compare == Order::NodeComparison::UNKNOWN) {
                            auto assumptions = this->assumptionMaker->createAndCheckAssumptions(currentState, succ, order, region, this->minValues[order],
                                                                                                this->maxValues[order]);
                            if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                                this->handleAssumption(order, assumptions.begin()->first);
                            }
                        } else if (!order->contains(currentState)) {
                            order->add(currentState);
                        }
                    } else if (!order->contains(currentState)) {
                        order->add(currentState);
                    }
                }
            } else {
                for (uint_fast64_t succ : successors) {
                    if (order->compare(currentState, succ) == Order::NodeComparison::UNKNOWN) {
                        auto assumptions = this->assumptionMaker->createAndCheckAssumptions(currentState, succ, order, region);
                        if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                            this->handleAssumption(order, assumptions.begin()->first);
                        }
                    } else if (!order->contains(currentState)) {
                        order->add(currentState);
                    }
                }
            }
        }
    }

    STORM_LOG_ASSERT(order->contains(currentState), "Expecting order to contain state " << currentState);
    STORM_LOG_ASSERT(order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE,
                     "Expecting " << currentState << " to be above " << *order->getBottom()->states.begin());
    return std::make_pair(this->numberOfStates, this->numberOfStates);
}

template<typename ValueType, typename ConstantType>
bool RewardOrderExtender<ValueType, ConstantType>::rewardHack(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t succ0,
                                                              uint_fast64_t succ1) {
    auto& successors0 = this->getSuccessors(succ0, order);
    if (successors0.size() == 1 && *(successors0.begin()) == currentState) {
        // We have curr --> succ0 and curr --> succ1 and succ0 --> curr
        if (!order->contains(currentState)) {
            order->add(currentState);
            order->addStateToHandle(currentState);
        }
        if (!order->contains(succ1)) {
            order->add(succ1);
            order->addStateToHandle(succ1);
        }
        if (!order->contains(succ0)) {
            order->addStateToHandle(succ1);
        }
        this->handleOneSuccessor(order, succ0, currentState);
        order->addAbove(currentState, order->getNode(succ1));
        return false;
    }
    auto& successors1 = this->getSuccessors(succ1, order);

    if (successors1.size() == 1 && *(successors1.begin()) == currentState) {
        // We have curr --> succ0 and curr --> succ1 and succ1 --> curr
        if (!order->contains(currentState)) {
            order->add(currentState);
            order->addStateToHandle(currentState);
        }
        if (!order->contains(succ0)) {
            order->add(succ0);
            order->addStateToHandle(succ0);
        }
        if (!order->contains(succ1)) {
            order->addStateToHandle(succ1);
        }
        this->handleOneSuccessor(order, succ1, currentState);
        order->addAbove(currentState, order->getNode(succ0));
        return true;
    }
    return false;
}

template class RewardOrderExtender<RationalFunction, double>;
template class RewardOrderExtender<RationalFunction, RationalNumber>;
}  // namespace analysis
}  // namespace storm