#include "storm-pars/analysis/RewardOrderExtenderDtmc.h"
#include <storage/StronglyConnectedComponentDecomposition.h>
#include <queue>

namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
    RewardOrderExtenderDtmc<ValueType, ConstantType>::RewardOrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : OrderExtender<ValueType, ConstantType>(model, formula) {
            this->rewardModel = this->model->getUniqueRewardModel();
            this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel()));
        }

        template<typename ValueType, typename ConstantType>
        RewardOrderExtenderDtmc<ValueType, ConstantType>::RewardOrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            this->rewardModel = this->model->getUniqueRewardModel();
            this->assumptionMaker = new AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel()));
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
            STORM_LOG_INFO("Doing backward reasoning");
            bool addedSomething = false;
            auto& successors = this->getSuccessors(currentState);

            // We sort the states, and then apply min/max comparison.
            // This also adds states to the order if they are not yet sorted, but can be sorted based on min/max values

            auto sortedSuccStates = this->sortStatesOrderAndMinMax(successors, order);
            if (sortedSuccStates.first.first != this->numberOfStates) {
                return sortedSuccStates.first;
            } else {
                // We could order all successor states
                // We are considering rewards, so our current state is always above the lowest one of all our successor states
                order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));

                if (this->usePLA[order]) {
                    for (uint_fast64_t succ : successors) {
                        if (order->compare(currentState, succ) == Order::NodeComparison::UNKNOWN) {
                            this->addStatesBasedOnMinMax(order, currentState, succ);
                        }
                    }
                }
            }

            STORM_LOG_ASSERT (order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE, "Expecting order to contain state");
            return std::make_pair(this->numberOfStates, this->numberOfStates);
        }

        template <typename ValueType, typename ConstantType>
        std::shared_ptr<Order> RewardOrderExtenderDtmc<ValueType, ConstantType>::getInitialOrder() {
            if (this->initialOrder == nullptr) {
                assert (this->model != nullptr);
                STORM_LOG_THROW(this->matrix.getRowCount() == this->matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for non-square matrix");
                modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*(this->model));

                // TODO check if eventually formula is true for all initial states?
                // TODO Add UNTIL formulas
                assert (this->formula->asRewardOperatorFormula().getSubformula().isEventuallyFormula());
                storage::BitVector topStates = storage::BitVector(this->numberOfStates, false);
                storage::BitVector bottomStates = propositionalChecker.check(this->formula->asRewardOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();

                auto& matrix = this->model->getTransitionMatrix();
                std::vector<uint64_t> firstStates;
                storm::storage::BitVector subStates (topStates.size(), true);

                for (auto state : bottomStates) {
                    firstStates.push_back(state);
                    subStates.set(state, false);
                }

                this->cyclic = storm::utility::graph::hasCycle(matrix, subStates);
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition;
                if (this->cyclic) {
                    storm::storage::StronglyConnectedComponentDecompositionOptions options;
                    options.forceTopologicalSort();
                    decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);
                }

                auto statesSorted = storm::utility::graph::getTopologicalSort(matrix.transpose(), firstStates);

                // Create Order
                this->initialOrder = std::shared_ptr<Order>(new Order(&topStates, &bottomStates, this->numberOfStates, std::move(decomposition), std::move(statesSorted)));
                this->buildStateMap();

            }

            if (this->minValues.empty() && this->minValuesInit) {
                this->minValues[this->initialOrder] = this->minValuesInit.get();
            }

            if (this->maxValues.empty() && this->maxValuesInit) {
                this->maxValues[this->initialOrder] = this->maxValuesInit.get();
            }

            if (!this->minValues.empty() && !this->maxValues.empty()) {
                this->usePLA[this->initialOrder] = true;
            } else {
                this->usePLA[this->initialOrder] = false;
            }
            this->continueExtending[this->initialOrder] = true;
            return this->initialOrder;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
            STORM_LOG_INFO("Forwardsreasoning");

            std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> sorted = this->sortForFowardReasoning(currentState, order);
            uint_fast64_t s1= sorted.first.first;
            uint_fast64_t s2 = sorted.first.second;
            std::vector<uint_fast64_t>& statesSorted = sorted.second;
            STORM_LOG_ASSERT(order->contains(currentState), "Expecting order to contain the current state for forward reasoning");

            if (s1 != this->numberOfStates && s2 != this->numberOfStates) {
                // Several states could not be ordered, it can be however that we can order all successor states, so we continue with backward reasoning
                if (s1 == currentState || s2 == currentState) {
                    for (auto& state : this->getSuccessors(currentState)) {
                        if (!order->contains(state)) {
                            order->add(state);
                        }
                    }
                    return extendByBackwardReasoning(order, region, currentState);
                } else {
                    return {s1, s2};
                }
            } else if (statesSorted.size() == (this->getSuccessors(currentState).size() + 1)) {
                // Everything is sorted
                assert (s1 == this->numberOfStates && s2 == this->numberOfStates);
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
                    return {s2, s2};
                } else if (this->usePLA[order]) {
                    // TODO: make use of forward reasoning and do backward as last solution
                    return extendByBackwardReasoning(order, region, currentState);
                } else {
                    for (auto& state :this->getSuccessors(currentState)) {
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
                    }
                    STORM_LOG_ASSERT(order->sortStates(this->getSuccessors(currentState)).size() == this->getSuccessors(currentState).size(), "Expecting all successor states to be ordered");
                    return {s2, s2};
                }

            }
//
//
//
//
//            auto& successors = this->getSuccessors(currentState);
//            auto succ0 = successors[0];
//            auto succ1 = successors[1];
//            Order::NodeComparison orderFirstSucc = order->compare(currentState, succ0);
//            Order::NodeComparison orderSecondSucc = order->compare(currentState, succ1);
//            if (this->usePLA[order] && orderFirstSucc == Order::NodeComparison::UNKNOWN) {
//                orderFirstSucc = this->addStatesBasedOnMinMax(order, currentState, succ0);
//            }
//            if (this->usePLA[order] && orderSecondSucc == Order::NodeComparison::UNKNOWN) {
//                orderSecondSucc = this->addStatesBasedOnMinMax(order, currentState, succ1);
//            }
//            STORM_LOG_ASSERT (orderFirstSucc == Order::NodeComparison::UNKNOWN || order->contains(succ0), "Expecting order to contain succ0 or to not know how to order succ0");
//            STORM_LOG_ASSERT (orderSecondSucc == Order::NodeComparison::UNKNOWN || order->contains(succ1), "Expecting order to contain succ0 or to not know how to order succ1");
//            if (orderFirstSucc == Order::NodeComparison::UNKNOWN && orderSecondSucc == Order::NodeComparison::UNKNOWN) {
//                if (order->compare(succ0, succ1) != Order::NodeComparison::UNKNOWN) {
//                    return extendByBackwardReasoning(order, region, currentState);
//                } else {
//                    return {succ0, succ1};
//                }
//            }
//            bool success = false;
//            if (orderFirstSucc == Order::NodeComparison::UNKNOWN) {
//                std::swap(orderFirstSucc, orderSecondSucc);
//                std::swap(succ0, succ1);
//            }
//            if (orderSecondSucc == Order::NodeComparison::UNKNOWN) {
//                // we can order currentState and succ0, try if we can add succ1 as well
//                if (orderFirstSucc == Order::NodeComparison::SAME) {
//                    success = true;
//                    // curr is same as succ0, ER(curr) = rew(curr) + f * ER(succ0) + (1-f) * ER(succ1)
//                    // ER(curr) = ER(succ0)
//                    // ER(curr) = rew(curr) + f * ER(curr) + (1-f) * ER(succ1)
//                    // (1-f)*ER(curr) = rew(curr) + (1-f) * ER(succ1)
//                    if (order->contains(succ1)) {
//                        order->addRelation(succ0, succ1);
//                    } else {
//                        order->addBelow(succ1, order->getNode(succ0));
//                    }
//                }
//                if (orderFirstSucc == Order::NodeComparison::BELOW) {
//                    success = true;
//                    // curr is below succ0, ER(curr) = rew(curr) + f * ER(succ0) + (1-f) * ER(succ1)
//                    // ER(curr) < ER(succ0)
//
//                    // Order wrt succ0
//                    // ER(succ0) > rew(curr) + f * ER(succ0) + (1-f) * ER(succ1)
//                    // non-negative rewards, so ER(succ1) < ER(succ0)
//                    if (order->contains(succ1)) {
//                        order->addRelation(succ0, succ1);
//                    } else {
//                        order->addBelow(succ1, order->getNode(succ0));
//                    }
//                    // Order wrt curr
//                    // ER(curr) > rew(curr) + f * ER(curr) + (1-f) * ER(succ1)
//                    // (1-f) * ER(curr) > rew(curr) + (1-f) * ER(succ1)
//                    if (order->contains(succ1)) {
//                        order->addRelation(currentState, succ1);
//                    } else {
//                        order->addBelow(succ1, order->getNode(currentState));
//                    }
//                }
//                if (orderFirstSucc == Order::NodeComparison::ABOVE) {
//                    // curr is above succ0, ER(curr) = rew(curr) + f * ER(succ0) + (1-f) * ER(succ1)
//                    // ER(curr) > ER(succ0)
//
//                    // Order wrt succ0
//                    // ER(succ0) < rew(curr) + f * ER(succ0) + (1-f) * ER(succ1)
//                    // (1-f) * ER(succ0) < rew(curr)  + (1-f) * ER(succ1)
//                    // ER(succ0) < rew(curr)/(1-f) + ER(succ1)
//                    // We can only do something when we know something from min/max
//                    // We do backward reasoning afterwards
//                    if (this->usePLA[order] && this->addStatesBasedOnMinMax(order, succ0, succ1) != Order::NodeComparison::UNKNOWN) {
//                        return extendByBackwardReasoning(order, region, currentState);
//                    }
//                }
//            } else {
//
//            }
//            if (success) {
//                return {this->numberOfStates, this->numberOfStates};
//            } else {
//                return extendByBackwardReasoning(order, region, currentState);
//            }
            return {this->numberOfStates, this->numberOfStates};
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            STORM_LOG_ASSERT(!(assumption != nullptr && order == nullptr), "Can't deal with assumptions for non-existing order");
            STORM_LOG_INFO_COND(assumption == nullptr, "Extending order with assumption: " << *assumption);
            if (assumption != nullptr) {
                this->handleAssumption(order, assumption);
            }
            if (order == nullptr) {
                order = this->getInitialOrder();
            }

            auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
            while (!order->isInvalid() && currentStateMode.first != this->numberOfStates) {
                STORM_LOG_ASSERT (currentStateMode.first < this->numberOfStates, "Unexpected state number");
                auto& currentState = currentStateMode.first;

                if (currentStateMode.second) {
                    STORM_LOG_INFO("Currently considering state: " << currentState << " based on topological sorting");
                } else {
                    STORM_LOG_INFO("Currently considering state: " << currentState << " based on statesToHandle (not topological approach)");
                }
                auto const & successors = this->getSuccessors(currentState);
                std::pair<uint_fast64_t, uint_fast64_t> result =  {this->numberOfStates, this->numberOfStates};

                if (successors.size() == 1) {
                    STORM_LOG_ASSERT (order->contains(successors[0]), "Expecting order to contain successor of state " << currentState);
                    this->handleOneSuccessor(order, currentState, successors[0]);
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

                if (order->isInvalid()) {
                    return {order, 0,0};
                }

                if (result.first == this->numberOfStates) {
                    // We did extend the order
                    STORM_LOG_ASSERT (result.second == this->numberOfStates, "Expecting both parts of result to contain the number of states");
                    STORM_LOG_ASSERT (order->sortStates(successors).size() == successors.size(), "Something went wrong while sorting states, number of states differs");
                    STORM_LOG_ASSERT (order->contains(currentState) && order->getNode(currentState) != nullptr, "Expecting order to contain the current State");

                    if (monRes != nullptr) {
                        for (auto& param : this->occuringVariablesAtState[currentState]) {
                            this->checkParOnStateMonRes(currentState, order, param, region, monRes);
                        }
                    }
                    // Get the next state
                    currentStateMode = this->getNextState(order, currentState, true);
                } else {
                    STORM_LOG_ASSERT (result.first < this->numberOfStates && result.second < this->numberOfStates, "Expecting both result numbers to correspond to states");
                    STORM_LOG_ASSERT (order->compare(result.first, result.second) == Order::UNKNOWN, "Expecting relation between the two states to be unknown");
                    // Try to add states based on min/max and assumptions, only if we are not in statesToHandle mode
                    if (currentStateMode.second && this->extendWithAssumption(order, region, result.first, result.second)) {
                        continue;
                    }
                    // We couldn't extend the order
                    if (this->nonParametricStates.find(currentState) != this->nonParametricStates.end()) {
                        if (!order->contains(currentState)) {
                            // State is not parametric, so we hope that just adding it between =) and =( will help us
                            order->add(currentState);
                        }
                        currentStateMode = this->getNextState(order, currentState, true);
                        continue;
                    } else {
                        if (!currentStateMode.second) {
                            // The state was based on statesToHandle, so it is not bad if we cannot continue with this.
                            currentStateMode = this->getNextState(order, currentState, false);
                            continue;
                        } else {
                            // The state was based on the topological sorting, so we need to return, but first add this state to the states Sorted as we are not done with it
                            order->addStateSorted(currentState);
                            this->continueExtending[order] = false;
                            return {order, result.first, result.second};
                        }
                    }
                }
                STORM_LOG_ASSERT (order->sortStates(successors).size() == successors.size(), "Expecting all successor states to be sorted");
            }

            STORM_LOG_ASSERT (order->getDoneBuilding(), "Expecting to have a final order");
            if (monRes != nullptr) {
                // monotonicity result for the in-build checking of monotonicity
                monRes->setDone();
            }
            return std::make_tuple(order, this->numberOfStates, this->numberOfStates);
        }



        template<typename ValueType, typename ConstantType>
        void RewardOrderExtenderDtmc<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
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
                if (order->compareFast(currentState, successor) != Order::NodeComparison::ABOVE) {
                    order->addAbove(currentState, order->getNode(successor));
                }
            }
        }


        template class RewardOrderExtenderDtmc<RationalFunction, double>;
        template class RewardOrderExtenderDtmc<RationalFunction, RationalNumber>;
    }
}