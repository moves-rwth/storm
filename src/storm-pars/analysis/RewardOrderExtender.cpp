#include "storm-pars/analysis/RewardOrderExtender.h"

namespace storm {
namespace analysis {

template<typename ValueType, typename ConstantType>
RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model,
                                                                  std::shared_ptr<logic::Formula const> formula)
    : OrderExtender<ValueType, ConstantType>(model, formula) {
    this->rewardModel = std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel());
    this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(this->matrix, this->rewardModel);
    this->actionComparator = ActionComparator<ValueType, ConstantType>(this->rewardModel);
    this->rewards = true;
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (this->rewardModel->hasStateActionRewards()) {
            for (auto j = 0; j < this->matrix.getRowGroupSize(i); ++j) {
                //                std::cout << i << " " << j << ": " << this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + j) <<
                //                std::endl;
                STORM_LOG_ASSERT(this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + j).isConstant(),
                                 "Expecting rewards to be constant");
            }
        } else if (this->rewardModel->hasStateRewards()) {
            STORM_LOG_ASSERT(this->rewardModel->getStateReward(i).isConstant(), "Expecting rewards to be constant");
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
    this->rewardModel = std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(rewardModel);
    this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(this->matrix, this->rewardModel);
    this->actionComparator = ActionComparator<ValueType, ConstantType>(this->rewardModel);
    this->rewards = true;
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (this->rewardModel->hasStateActionRewards()) {
            for (auto j = 0; j < this->matrix.getRowGroupSize(i); ++j) {
                //                std::cout << i << " " << j << ": " << this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + j) <<
                //                std::endl;
                STORM_LOG_ASSERT(this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + j).isConstant(),
                                 "Expecting rewards to be constant");
            }
        } else if (this->rewardModel->hasStateRewards()) {
            STORM_LOG_ASSERT(this->rewardModel->getStateReward(i).isConstant(), "Expecting rewards to be constant");
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
    this->rewardModel = std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(rewardModel);
    this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(this->matrix, this->rewardModel);
    this->actionComparator = ActionComparator<ValueType, ConstantType>(this->rewardModel);

    this->rewards = true;
    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (this->rewardModel->hasStateActionRewards()) {
            for (auto j = 0; j < this->matrix.getRowGroupSize(i); ++j) {
                STORM_LOG_ASSERT(this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + j).isConstant(),
                                 "Expecting rewards to be constant");
            }
        } else if (this->rewardModel->hasStateRewards()) {
            STORM_LOG_ASSERT(this->rewardModel->getStateReward(i).isConstant(), "Expecting rewards to be constant");
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
    }
}

template<typename ValueType, typename ConstantType>
void RewardOrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
    auto compRes = order->compareFast(currentState, successor);
    if (compRes == Order::NodeComparison::UNKNOWN) {
        bool allZero = true;
        bool allGreaterZero = true;
        if (this->rewardModel->hasStateActionRewards()) {
            if (order->isActionSetAtState(currentState)) {
                allZero =
                    this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState)).isZero();
                allGreaterZero = !allZero;
            } else {
                for (auto i = 0; i < this->matrix.getRowGroupSize(currentState); ++i) {
                    bool zero = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + i).isZero();
                    allZero &= zero;
                    allGreaterZero &= !zero;
                }
            }
        } else if (this->rewardModel->hasStateRewards()) {
            allZero = this->rewardModel->getStateReward(currentState).isZero();
            allGreaterZero = !allZero;
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
        if (allZero) {
            order->addToNode(currentState, order->getNode(successor));
        } else if (allGreaterZero) {
            order->addAbove(currentState, order->getNode(successor));
        } else {
            STORM_LOG_ASSERT(false, "Cannot handle one successor with different rewards");
        }
    } else {
        if (!order->contains(currentState)) {
            if (compRes == Order::SAME) {
                order->addToNode(currentState, order->getNode(successor));
            } else if (compRes == Order::ABOVE) {
                order->addAbove(currentState, order->getNode(successor));
            } else if (compRes == Order::BELOW) {
                order->addBelow(currentState, order->getNode(successor));
            }
        }
        assert(order->contains(currentState));
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
    for (uint_fast64_t i : order->getBottom()->states) {
        for (auto& entry : this->transposeMatrix.getRow(i)) {
            order->addStateToHandle(entry.getColumn());
        }
    }

    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (!order->isActionSetAtState(i)) {
            if (this->deterministic || this->stateMap[i].size() == 1) {
                order->addToMdpScheduler(i, 0);
            } else {
                continue;
            }
        }
        auto& successors = this->getSuccessors(i, order);
        if (successors.size() == 2 && order->isActionSetAtState(i)) {
            rewardHack(order, i, *(successors.begin()), *(successors.begin() + 1));
        }
    }

    for (uint_fast64_t i = 0; i < this->numberOfStates; ++i) {
        if (!order->isActionSetAtState(i)) {
            continue;
        }
        auto& successorsI = this->getSuccessors(i, order);
        if (successorsI.size() == 2) {
            for (uint_fast64_t j = i + 1; j < this->numberOfStates; ++j) {
                if (!order->isActionSetAtState(j)) {
                    continue;
                }
                auto& successorsJ = this->getSuccessors(j, order);

                if (successorsJ.size() != 2 ||
                    !(*(successorsI.begin()) == *(successorsJ.begin()) && *(successorsI.begin() + 1) == *(successorsJ.begin() + 1)) ||
                    this->matrix.getRow(i, order->getActionAtState(i)).begin()->getValue() !=
                        this->matrix.getRow(j, order->getActionAtState(j)).begin()->getValue()) {
                    continue;
                }

                // check welke reward t grootste is
                if (order->isActionSetAtState(i) && order->isActionSetAtState(j)) {
                    ValueType rewardI = ValueType(0);
                    ValueType rewardJ = ValueType(0);
                    if (this->rewardModel->hasStateActionRewards()) {
                        STORM_LOG_ASSERT(order->getActionAtState(i) != std::numeric_limits<uint64_t>::max(), "Expecting action to be set");
                        STORM_LOG_ASSERT(order->getActionAtState(j) != std::numeric_limits<uint64_t>::max(), "Expecting action to be set");
                        rewardI = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + order->getActionAtState(i));
                        rewardJ = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[j] + order->getActionAtState(j));
                    } else if (this->rewardModel->hasStateRewards()) {
                        rewardI = this->rewardModel->getStateReward(i);
                        rewardJ = this->rewardModel->getStateReward(j);
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
                } else if (order->isActionSetAtState(i)) {
                    ValueType rewardI = ValueType(0);
                    ValueType rewardJ = ValueType(0);
                    bool iAboveJ = true;
                    bool jAboveI = true;
                    for (auto actJ = 0; actJ < this->stateMap[j].size(); ++actJ) {
                        if (this->rewardModel->hasStateActionRewards()) {
                            STORM_LOG_ASSERT(order->getActionAtState(i) != std::numeric_limits<uint64_t>::max(), "Expecting action to be set");
                            rewardI = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + order->getActionAtState(i));
                            rewardJ = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[j] + actJ);
                        } else if (this->rewardModel->hasStateRewards()) {
                            rewardI = this->rewardModel->getStateReward(i);
                            rewardJ = this->rewardModel->getStateReward(j);
                        } else {
                            STORM_LOG_ASSERT(false, "Expecting reward");
                        }

                        if (rewardI.constantPart() > rewardJ.constantPart()) {
                            jAboveI = false;
                        } else if (rewardJ.constantPart() > rewardI.constantPart()) {
                            iAboveJ = false;
                        }
                    }
                    if (iAboveJ || jAboveI) {
                        if (!order->contains(i)) {
                            order->add(i);
                            order->addStateToHandle(i);
                        }
                        if (!order->contains(j)) {
                            order->add(j);
                            order->addStateToHandle(j);
                        }
                        if (iAboveJ && jAboveI) {
                            order->addRelation(i, j, true);

                        } else if (jAboveI) {
                            order->addAbove(j, order->getNode(i));
                        } else {
                            order->addAbove(i, order->getNode(j));
                        }
                    }
                } else if (order->isActionSetAtState(j)) {
                    ValueType rewardI = ValueType(0);
                    ValueType rewardJ = ValueType(0);
                    bool iAboveJ = true;
                    bool jAboveI = true;
                    for (auto actI = 0; actI < this->stateMap[i].size(); ++actI) {
                        if (this->rewardModel->hasStateActionRewards()) {
                            STORM_LOG_ASSERT(order->getActionAtState(j) != std::numeric_limits<uint64_t>::max(), "Expecting action to be set");
                            rewardI = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + actI);
                            rewardJ = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[j] + order->getActionAtState(j));
                        } else if (this->rewardModel->hasStateRewards()) {
                            rewardI = this->rewardModel->getStateReward(i);
                            rewardJ = this->rewardModel->getStateReward(j);
                        } else {
                            STORM_LOG_ASSERT(false, "Expecting reward");
                        }

                        if (rewardI.constantPart() > rewardJ.constantPart()) {
                            jAboveI = false;
                        } else if (rewardJ.constantPart() > rewardI.constantPart()) {
                            iAboveJ = false;
                        }
                    }
                    if (iAboveJ || jAboveI) {
                        if (!order->contains(i)) {
                            order->add(i);
                            order->addStateToHandle(i);
                        }
                        if (!order->contains(j)) {
                            order->add(j);
                            order->addStateToHandle(j);
                        }
                        if (iAboveJ && jAboveI) {
                            order->addRelation(i, j, true);

                        } else if (jAboveI) {
                            order->addAbove(j, order->getNode(i));
                        } else {
                            order->addAbove(i, order->getNode(j));
                        }
                    }
                } else {
                    ValueType rewardI = ValueType(0);
                    ValueType rewardJ = ValueType(0);
                    bool iAboveJ = true;
                    bool jAboveI = true;
                    for (auto actI = 0; actI < this->stateMap[i].size(); ++actI) {
                        for (auto actJ = 0; actJ < this->stateMap[j].size(); ++actJ) {
                            if (this->rewardModel->hasStateActionRewards()) {
                                rewardI = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[i] + actI);
                                rewardJ = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[j] + actJ);
                            } else if (this->rewardModel->hasStateRewards()) {
                                rewardI = this->rewardModel->getStateReward(i);
                                rewardJ = this->rewardModel->getStateReward(j);
                            } else {
                                STORM_LOG_ASSERT(false, "Expecting reward");
                            }

                            if (rewardI.constantPart() > rewardJ.constantPart()) {
                                jAboveI = false;
                            } else if (rewardJ.constantPart() > rewardI.constantPart()) {
                                iAboveJ = false;
                            }
                        }
                    }
                    if (iAboveJ || jAboveI) {
                        if (!order->contains(i)) {
                            order->add(i);
                            order->addStateToHandle(i);
                        }
                        if (!order->contains(j)) {
                            order->add(j);
                            order->addStateToHandle(j);
                        }
                        if (iAboveJ && jAboveI) {
                            order->addRelation(i, j, true);

                        } else if (jAboveI) {
                            order->addAbove(j, order->getNode(i));
                        } else {
                            order->addAbove(i, order->getNode(j));
                        }
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
    STORM_LOG_ASSERT(order->isActionSetAtState(currentState), "Expecting action to be set at state for backward reasoning");

    auto const& successors = this->getSuccessors(currentState, order);

    if (successors.size() == 2 && this->extendByForwardReasoningOneSucc(order, region, currentState)) {
        return {this->numberOfStates, this->numberOfStates};
    }
    std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sorted = this->sortForForwardReasoning(currentState, order);
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

            if (statesSorted.size() == 2 && (order->compare(statesSorted[0], s1) == Order::BELOW || order->compare(statesSorted[1], s1) == Order::BELOW)) {
                // This can only happen if we have seen assumptions
                // We have currentState <= succ1 && currentState <= succ2
                // we don't have negative rewards, so we should merge, probably assumptions will get invalid by this
                bool allZero = true;
                bool allGreaterZero = true;
                if (this->rewardModel->hasStateActionRewards()) {
                    if (this->findBestAction(order, region, currentState)) {
                        allZero =
                            this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState))
                                .isZero();
                        allGreaterZero = !allZero;
                    } else {
                        for (auto i = 0; i < this->matrix.getRowGroupSize(currentState); ++i) {
                            bool zero = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + i).isZero();
                            allZero &= zero;
                            allGreaterZero &= !zero;
                        }
                    }
                } else if (this->rewardModel->hasStateRewards()) {
                    allZero = this->rewardModel->getStateReward(currentState).isZero();
                    allGreaterZero = !allZero;
                } else {
                    STORM_LOG_ASSERT(false, "Expecting reward");
                }
                if (allGreaterZero) {
                    order->setInvalid();
                }
                order->merge(currentState, *(successors.begin()));
                order->merge(currentState, *(successors.begin() + 1));
                STORM_LOG_ASSERT(order->compare(*(successors.begin()), currentState) == Order::SAME, "Expecting states to be at same state");
                STORM_LOG_ASSERT(order->compare(*(successors.begin() + 1), currentState) == Order::SAME, "Expecting states to be at same state");
                STORM_LOG_ASSERT(order->compare(*(successors.begin()), *(successors.begin() + 1)) == Order::SAME, "Expecting states to be at same state");
            } else {
                order->addBelow(s1, order->getNode(currentState));
                order->addStateToHandle(s1);
            }
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
            if (this->usePLA[order]) {
                auto assumptions =
                    this->assumptionMaker->createAndCheckAssumptions(currentState, s1, order, region, this->minValues[order], this->maxValues[order]);
                if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                    this->handleAssumption(order, assumptions.begin()->first);
                }
            } else {
                auto assumptions = this->assumptionMaker->createAndCheckAssumptions(currentState, s1, order, region);
                if (assumptions.size() == 1 && assumptions.begin()->second == storm::analysis::AssumptionStatus::VALID) {
                    this->handleAssumption(order, assumptions.begin()->first);
                }
            }
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
        bool allZero = true;
        bool allGreaterZero = true;
        if (this->rewardModel->hasStateActionRewards()) {
            if (this->findBestAction(order, region, currentState)) {
                allZero =
                    this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState)).isZero();
                allGreaterZero = !allZero;
            } else {
                for (auto i = 0; i < this->matrix.getRowGroupSize(currentState); ++i) {
                    bool zero = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + i).isZero();
                    allZero &= zero;
                    allGreaterZero &= !zero;
                }
            }
        } else if (this->rewardModel->hasStateRewards()) {
            allZero = this->rewardModel->getStateReward(currentState).isZero();
            allGreaterZero = !allZero;
        } else {
            STORM_LOG_ASSERT(false, "Expecting reward");
        }
        if (allZero) {
            if (!order->contains(realSucc)) {
                if (!order->contains(currentState)) {
                    order->add(currentState);
                }
                order->addToNode(realSucc, order->getNode(currentState));
            } else {
                order->addToNode(currentState, order->getNode(realSucc));
            }
        } else if (allGreaterZero) {
            if (!order->contains(realSucc)) {
                order->add(realSucc);
                order->addStateToHandle(realSucc);
            }
            order->addAbove(currentState, order->getNode(realSucc));
        } else {
            assert(false);
            // TODO Implement
        }
    } else if (order->isBottomState(succ0) || order->isBottomState(succ1)) {
        auto bottomState = order->isBottomState(succ0) ? succ0 : succ1;
        auto otherState = bottomState == succ0 ? succ1 : succ0;
        // If there is only one transition going into bottomState (+ selfloop) we can do some normal forward reasoning
        // Other state will always go through currentState to get to bottom state, as currentState is the only one with a transition to bottom
        // So other state will be above currentState
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
    STORM_LOG_ASSERT(order->isActionSetAtState(currentState), "Expecting action to be set at state for backward reasoning");

    bool addedSomething = false;
    auto const& successors = this->getSuccessors(currentState, order);
    // We sort the states, and then apply min/max comparison.
    // This also adds states to the order if they are not yet sorted, but can be sorted based on min/max values

    auto sortedSuccStates = this->sortStatesOrderAndMinMax(successors, order);
    if (sortedSuccStates.first.first != this->numberOfStates) {
        if (successors.size() == 2 && (*(successors.begin()) == currentState || *(successors.begin() + 1) == currentState)) {
            // current state actually only has one real successor
            auto realSucc = *(successors.begin()) == currentState ? *(successors.begin() + 1) : *(successors.begin());
            bool allZero = true;
            bool allGreaterZero = true;
            if (this->rewardModel->hasStateActionRewards()) {
                if (this->findBestAction(order, region, currentState)) {
                    bool zero = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState))
                                    .isZero();
                    allZero &= zero;
                    allGreaterZero &= !zero;
                } else {
                    for (auto i = 0; i < this->matrix.getRowGroupSize(currentState); ++i) {
                        bool zero = this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + i).isZero();
                        allZero &= zero;
                        allGreaterZero &= !zero;
                    }
                }
            } else if (this->rewardModel->hasStateRewards()) {
                bool zero = this->rewardModel->getStateReward(currentState).isZero();
                allZero &= zero;
                allGreaterZero &= !zero;
            } else {
                STORM_LOG_ASSERT(false, "Expecting reward");
            }

            if (!order->contains(realSucc)) {
                order->add(realSucc);
                order->addStateToHandle(realSucc);
            }
            if (allZero) {
                order->addToNode(currentState, order->getNode(realSucc));
            } else if (allGreaterZero) {
                order->addAbove(currentState, order->getNode(realSucc));
            } else {
                assert(false);
                // TODO
            }
        } else {
            return sortedSuccStates.first;
        }
    } else {
        ValueType rewardSmall = ValueType(0);
        ValueType rewardLarge = ValueType(0);
        if (order->compare(sortedSuccStates.second[0], sortedSuccStates.second[sortedSuccStates.second.size() - 1]) == Order::NodeComparison::SAME) {
            // All successors are at the same node
            this->handleOneSuccessor(order, currentState, sortedSuccStates.second[0]);
        } else {
            // We could order all successor states
            bool allZero = true;
            bool allGreaterZero = true;
            if (this->rewardModel->hasStateActionRewards()) {
                if (this->findBestAction(order, region, currentState)) {
                    rewardSmall =
                        this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState));
                    rewardLarge = rewardSmall;
                    bool zero = rewardSmall.isZero();
                    allZero &= zero;
                    allGreaterZero &= !zero;
                } else {
                    for (auto i = 0; this->matrix.getRowGroupSize(currentState); ++i) {
                        auto rew =
                            this->rewardModel->getStateActionReward(this->matrix.getRowGroupIndices()[currentState] + order->getActionAtState(currentState));
                        rewardSmall = rew.constantPart() < rewardSmall.constantPart() ? rew : rewardSmall;
                        rewardLarge = rew.constantPart() < rewardLarge.constantPart() ? rewardLarge : rew;
                        bool zero = rew.isZero();
                        allZero &= zero;
                        allGreaterZero &= !zero;
                    }
                }
            } else if (this->rewardModel->hasStateRewards()) {
                rewardSmall = this->rewardModel->getStateReward(currentState);
                rewardLarge = rewardSmall;
                bool zero = rewardSmall.isZero();
                allZero &= zero;
                allGreaterZero &= !zero;
            } else {
                STORM_LOG_ASSERT(false, "Expecting reward");
            }

            if (allZero) {
                if (order->compare(*(sortedSuccStates.second.begin()), sortedSuccStates.second.back()) == Order::NodeComparison::SAME) {
                    order->addToNode(currentState, order->getNode(sortedSuccStates.second.back()));
                } else {
                    order->addBetween(currentState, *(sortedSuccStates.second.begin()), sortedSuccStates.second.back());
                }
            } else if (allGreaterZero) {
                // We are considering rewards, so our current state is always above the lowest one of all our successor states
                order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));
                // We check if we can also sort something based on assumptions.
                if (this->usePLA[order] && order->isActionSetAtState(currentState)) {
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
                                for (auto& entry : this->matrix.getRow(currentState, order->getActionAtState(currentState))) {
                                    if (entry.getColumn() == succ) {
                                        function = entry.getValue();
                                        break;
                                    }
                                }
                                auto vars = function.gatherVariables();
                                if (vars.size() == 1) {
                                    std::map<VariableType, CoefficientType> val1, val2;
                                    auto& var = *(vars.begin());
                                    val1.insert({var, region.getLowerBoundary(var)});
                                    val2.insert({var, region.getUpperBoundary(var)});
                                    CoefficientType res1 = function.evaluate(val1);
                                    CoefficientType res2 = function.evaluate(val2);
                                    if (res2 < res1) {
                                        std::swap(res1, res2);
                                    }
                                    if (storm::utility::convertNumber<ConstantType>(rewardSmall.constantPart()) >
                                        storm::utility::convertNumber<ConstantType>(res2) * (this->maxValues[order][succ] - this->minValues[order][s1])) {
                                        if (!order->contains(succ)) {
                                            order->add(succ);
                                        }
                                        order->addAbove(currentState, order->getNode(succ));
                                        compare = Order::NodeComparison::ABOVE;
                                    } else if (successors.size() == 2 && succ == *(successors.begin())) {
                                        if (storm::utility::convertNumber<ConstantType>(rewardLarge.constantPart()) <
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
            } else {
                assert(false);
                // TODO implement
            }
        }
    }

    STORM_LOG_ASSERT(order->contains(currentState), "Expecting order to contain state " << currentState);
    STORM_LOG_ASSERT(order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE ||
                         order->compare(order->getNode(currentState), order->getBottom()) == Order::SAME,
                     "Expecting " << currentState << " to be above " << *order->getBottom()->states.begin());
    // if number of successors is 3 we do a hack to see if we can also order state wrt other state
    if (sortedSuccStates.second.size() == 3 && order->compareFast(currentState, sortedSuccStates.second[1]) == Order::UNKNOWN) {
        auto middleState = sortedSuccStates.second[1];
        auto assumptions =
            this->usePLA.find(order) != this->usePLA.end() && this->usePLA[order]
                ? this->assumptionMaker->createAndCheckAssumptions(currentState, middleState, order, region, this->minValues[order], this->maxValues[order])
                : this->assumptionMaker->createAndCheckAssumptions(currentState, middleState, order, region);
        if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
            this->handleAssumption(order, assumptions.begin()->first);
        }
    }
    return std::make_pair(this->numberOfStates, this->numberOfStates);
}

template<typename ValueType, typename ConstantType>
bool RewardOrderExtender<ValueType, ConstantType>::rewardHack(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t succ0,
                                                              uint_fast64_t succ1) {
    STORM_LOG_ASSERT(order->isActionSetAtState(currentState), "Expecting action to be set for state");
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