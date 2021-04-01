#include "storm-pars/analysis/OrderExtenderDtmc.h"


namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        OrderExtenderDtmc<ValueType, ConstantType>::OrderExtenderDtmc(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : ReachabilityOrderExtender<ValueType, ConstantType>(model, formula) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        OrderExtenderDtmc<ValueType, ConstantType>::OrderExtenderDtmc(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : ReachabilityOrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            // intentionally left empty
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtenderDtmc<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState) {
            std::vector<uint64_t> successors = this->stateMap[currentState][0]; // Get succs
            return ReachabilityOrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(order, currentState, successors, false); // Call Base Class function.

        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtenderDtmc<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            this->region = region;
            if (order == nullptr) {
                order = this->getBottomTopOrder();
                if (this->usePLA[order]) {
                    auto &min = this->minValues[order];
                    auto &max = this->maxValues[order];
                    // Try to make the order as complete as possible based on pla results
                    auto &statesSorted = order->getStatesSorted();
                    auto itr = statesSorted.begin();
                    while (itr != statesSorted.end()) {
                        auto state = *itr;
                        auto &successors = this->stateMap[state][0];
                        bool all = true;
                        for (auto i = 0; i < successors.size(); ++i) {
                            auto state1 = successors[i];
                            for (auto j = i + 1; j < successors.size(); ++j) {
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
                            order->setDoneState(state);
                        }
                        ++itr;
                    }
                }
                this->continueExtending[order] = true;
            }
            if (this->continueExtending[order] || assumption != nullptr) {
                return extendOrder(order, monRes, assumption);
            } else {
                auto& res = this->unknownStatesMap[order];
                this->continueExtending[order] = false;
                return {order, res.first, res.second};
            }
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtenderDtmc<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            if (assumption != nullptr) {
                STORM_LOG_INFO("Handling assumption " << *assumption << std::endl);
                this->handleAssumption(order, assumption);
            }

            auto currentStateMode = this->getNextState(order, this->numberOfStates, false);
            while (currentStateMode.first != this->numberOfStates) {
                assert (currentStateMode.first < this->numberOfStates);
                auto& currentState = currentStateMode.first;
                auto& successors = this->stateMap[currentState][0];
                std::pair<uint_fast64_t, uint_fast64_t> result =  {this->numberOfStates, this->numberOfStates};

                if (successors.size() == 1) {
                    assert (order->contains(successors[0]));
                    this->handleOneSuccessor(order, currentState, successors[0]);
                } else if (!successors.empty()) {
                    if (order->isOnlyBottomTopOrder()) {
                        order->add(currentState);
                        if (!order->isTrivial(currentState)) {
                            // This state is part of an scc, therefore, we could do forward reasoning here
                            result = this->extendByForwardReasoning(order, currentState, successors, assumption!=nullptr);
                        } else {
                            result = {this->numberOfStates, this->numberOfStates};
                        }
                    } else {
                        result = this->extendNormal(order, currentState, successors, assumption != nullptr);
                    }
                }

                if (result.first == this->numberOfStates) {
                    // We did extend the order
                    assert (result.second == this->numberOfStates);
                    assert (order->sortStates(&successors).size() == successors.size());
                    assert (order->contains(currentState) && order->getNode(currentState) != nullptr);

                    if (monRes != nullptr && currentStateMode.second != -1) {
                        for (auto& param : this->occuringVariablesAtState[currentState]) {
                            this ->checkParOnStateMonRes(currentState, order, param, monRes);
                        }
                    }
                    // Get the next state
                    currentStateMode = this->getNextState(order, currentState, true);
                } else {
                    assert (result.first < this->numberOfStates);
                    assert (result.second < this->numberOfStates);
                    assert (order->compare(result.first, result.second) == Order::UNKNOWN);
                    assert (order->compare(result.second, result.first) == Order::UNKNOWN);
                    // Try to add states based on min/max and assumptions, only if we are not in statesToHandle mode
                    if (currentStateMode.second && this->extendByAssumption(order, currentState, result.first, result.second)) {
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
                assert (order->sortStates(&successors).size() == successors.size());
            }

            assert (order->getDoneBuilding());
            if (monRes != nullptr) {
                // monotonicity result for the in-build checking of monotonicity
                monRes->setDone();
            }
            return std::make_tuple(order, this->numberOfStates, this->numberOfStates);
        }

        template class OrderExtenderDtmc<RationalFunction, double>;
        template class OrderExtenderDtmc<RationalFunction, RationalNumber>;

    }
}