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
            bool addedSomething = false;
            auto& successors = this->getSuccessors(currentState);

            // We sort the states, and then apply min/max comparison.
            // This also adds states to the order if they are not yet sorted, but can be sorted based on min/max values
            auto sortedSuccStates = this->sortStatesOrderAndMinMax(successors, order);
            for (uint_fast64_t succ: successors) {
                if (this->usePLA[order] && order->compare(currentState, succ) == Order::NodeComparison::UNKNOWN) {
                    auto addRes = this->addStatesBasedOnMinMax(order, currentState, succ);
                    if (addRes == Order::NodeComparison::ABOVE) {
                        addedSomething = true;
                    } else if (addRes == Order::NodeComparison::SAME) {
                        addedSomething = true;
                        STORM_LOG_ASSERT(sortedSuccStates.first.first == this->numberOfStates,
                                         "Expecting all successor states to be sorted and to be at the same node");
                        break;
                    }
                }
            }

            if (!addedSomething && sortedSuccStates.first.first != this->numberOfStates) {
                return sortedSuccStates.first;
            }
            // We are considering rewards, so our current state is always above the lowest one of all our successor states
            order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));

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
            STORM_LOG_THROW(false, exceptions::NotImplementedException, "Forward reasoning implies cycles, not implemented for expected rewards");
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            STORM_LOG_ASSERT(!(assumption != nullptr && order == nullptr), "Can't deal with assumptions for non-existing order");
            if (assumption != nullptr) {
                this->handleAssumption(order, assumption);
            }
            if (order == nullptr) {
                order = this->getInitialOrder();
            }

            auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
            while (currentStateMode.first != this->numberOfStates) {
                STORM_LOG_ASSERT (currentStateMode.first < this->numberOfStates, "Unexpected state number");
                auto& currentState = currentStateMode.first;
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
                    STORM_LOG_ASSERT (order->compare(result.first, result.second) == Order::UNKNOWN && order->compare(result.second, result.first) == Order::UNKNOWN, "Expecting relation between the two states to be unknown");
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
            if (rewardModel.getStateActionReward(currentState) == ValueType(0)) {
                order->addToNode(currentState, order->getNode(successor));
            } else {
                assert(!(rewardModel.getStateActionReward(currentState) < ValueType(0)));
                order->addAbove(currentState, order->getNode(successor));
            }
        }


        template class RewardOrderExtenderDtmc<RationalFunction, double>;
        template class RewardOrderExtenderDtmc<RationalFunction, RationalNumber>;
    }
}