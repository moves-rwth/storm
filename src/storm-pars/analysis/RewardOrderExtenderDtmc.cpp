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
            auto sortedSuccStates = order->sortStatesUnorderedPair(successors);
            for (uint_fast64_t succ: successors) {
                if (order->compare(currentState, succ) == Order::NodeComparison::UNKNOWN) {
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
            } else if (!addedSomething) {
                // We are considering rewards, so our current state is always above the lowest one of all our successor states
                order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));
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

            if (!this->minValues.empty() && !this->maxValues.empty()) {
                this->usePLA[this->initialOrder] = true;
            } else {
                this->usePLA[this->initialOrder] = false;
            }
            return this->initialOrder;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendByForwardReasoning(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, uint_fast64_t currentState) {
            STORM_LOG_THROW(false, exceptions::NotImplementedException, "The function is not (yet) implemented in this class");
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> RewardOrderExtenderDtmc<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            if (order == nullptr) {
                order = getInitialOrder();
                if (this->usePLA[order]) {
                    auto &min = this->minValues[order];
                    auto &max = this->maxValues[order];
                    // Try to make the order as complete as possible based on pla results
                    auto &statesSorted = order->getStatesSorted();
                    auto itr = statesSorted.begin();
                    while (itr != statesSorted.end()) {
                        auto state = *itr;
                        auto const &successors = this->getSuccessors(state);
                        bool all = true;
                        for (uint_fast64_t i = 0; i < successors.size(); ++i) {
                            auto state1 = successors[i];
                            for (uint_fast64_t j = i + 1; j < successors.size(); ++j) {
                                auto state2 = successors[j];
                                if (min[state1] > max[state2]) {
                                    if (!order->contains(state1)) {
                                        order->add(state1);
                                    }
                                    if (!order->contains(state2)) {
                                        order->add(state2);
                                    }
                                    order->addRelation(state1, state2, false);
                                } else if (min[state2] > max[state1]) {
                                    if (!order->contains(state1)) {
                                        order->add(state1);
                                    }
                                    if (!order->contains(state2)) {
                                        order->add(state2);
                                    }
                                    order->addRelation(state2, state1, false);
                                } else if (min[state1] == max[state2] && max[state1] == min[state2]) {
                                    if (!order->contains(state1) && !order->contains(state2)) {
                                        order->add(state1);
                                        order->addToNode(state2, order->getNode(state1));
                                    } else if (!order->contains(state1)) {
                                        order->addToNode(state1, order->getNode(state2));
                                    } else if (!order->contains(state2)) {
                                        order->addToNode(state2, order->getNode(state1));
                                    } else {
                                        order->merge(state1, state2);
                                        assert (!order->isInvalid());
                                    }
                                } else {
                                    all = false;
                                }
                            }
                        }
                        if (all) {
                            STORM_LOG_INFO("All successors of state " << state << " sorted based on min max values");
                            order->setSufficientForState(state);
                        }
                        ++itr;
                    }
                }
                this->continueExtending[order] = true;
            }

            if (this->continueExtending[order] || assumption != nullptr) {
                return extendOrder(order, region, monRes, assumption);
            } else {
                auto& res = this->unknownStatesMap[order];
                this->continueExtending[order] = false;
                return {order, res.first, res.second};
            }
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