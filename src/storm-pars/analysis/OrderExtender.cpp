#include "OrderExtender.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"
#include "storm/utility/graph.h"

#include "storm-pars/api/region.h"
#include "storm-pars/api/export.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/analysis/MonotonicityChecker.h"


namespace storm {
    namespace analysis {

        template <typename ValueType, typename ConstantType>
        OrderExtender<ValueType, ConstantType>::OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula,  storage::ParameterRegion<ValueType> region) {
            init(model);
            this->region = region;
            this->formula = formula;
            usePLA = false;
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::init(std::shared_ptr<models::sparse::Model<ValueType>> model) {
            this->model = model;
            this->matrix = model->getTransitionMatrix();
            this->numberOfStates = this->model->getNumberOfStates();
            this->params = storm::models::sparse::getProbabilityParameters(*model);

            // Build stateMap
            for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
                auto row = matrix.getRow(i);
                stateMap[i] = std::vector<uint_fast64_t>(row.getNumberOfEntries());
                for (auto rowItr = row.begin(); rowItr != row.end(); ++rowItr) {
                    // ignore self-loops when there are more transitions
                    if (i != rowItr->getColumn() || row.getNumberOfEntries() == 1) {
                        stateMap[i].push_back(rowItr->getColumn());
                    }
                }
            }
            cyclic = storm::utility::graph::hasCycle(matrix);
        }

        template <typename ValueType, typename ConstantType>
        Order* OrderExtender<ValueType, ConstantType>::getBottomTopOrder() {
            if (bottomTopOrder == nullptr) {
                STORM_LOG_THROW(matrix.getRowCount() == matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for non-square matrix");
                modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*model);
                storage::BitVector phiStates;
                storage::BitVector psiStates;
                assert (formula->isProbabilityOperatorFormula());
                if (formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    phiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    psiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                } else {
                    assert (formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula());
                    phiStates = storage::BitVector(numberOfStates, true);
                    psiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                }
                // Get the maybeStates
                std::pair<storage::BitVector, storage::BitVector> statesWithProbability01 = utility::graph::performProb01(this->model->getBackwardTransitions(), phiStates, psiStates);
                storage::BitVector topStates = statesWithProbability01.second;
                storage::BitVector bottomStates = statesWithProbability01.first;

                STORM_LOG_THROW(topStates.begin() != topStates.end(), exceptions::NotSupportedException,"Formula yields to no 1 states");
                STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), exceptions::NotSupportedException,"Formula yields to no zero states");
                auto matrix = this->model->getTransitionMatrix();
                std::vector<uint_fast64_t> statesSorted = utility::graph::getTopologicalSort(matrix);
                std::reverse(statesSorted.begin(),statesSorted.end());
                bottomTopOrder = new Order(&topStates, &bottomStates, numberOfStates, &statesSorted);
            }
            return bottomTopOrder;
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::toOrder(std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            return this->extendOrder(getBottomTopOrder(), monRes);
        }

        template <typename ValueType, typename ConstantType>
        Order* OrderExtender<ValueType, ConstantType>::toOrder(std::vector<ConstantType> minValues, std::vector<ConstantType> maxValues, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            this->minValues = minValues;
            this->maxValues = maxValues;
            usePLA = true;
            return std::get<0>(this->extendOrder(getBottomTopOrder(), false, monRes));
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::handleAssumption(Order* order, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            assert (assumption != nullptr);
            assert (assumption->getFirstOperand()->isVariable() && assumption->getSecondOperand()->isVariable());

            expressions::Variable var1 = assumption->getFirstOperand()->asVariableExpression().getVariable();
            expressions::Variable var2 = assumption->getSecondOperand()->asVariableExpression().getVariable();
            auto val1 = std::stoul(var1.getName(), nullptr, 0);
            auto val2 = std::stoul(var2.getName(), nullptr, 0);

            assert(order->compare(val1, val2) == Order::UNKNOWN);
            Order::Node* n1 = order->getNode(val1);
            Order::Node* n2 = order->getNode(val2);

            if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal) {
                if (n1 != nullptr && n2 != nullptr) {
                    order->mergeNodes(n1,n2);
                } else if (n1 != nullptr) {
                    order->addToNode(val2, n1);
                } else if (n2 != nullptr) {
                    order->addToNode(val1, n2);
                } else {
                    order->add(val1);
                    order->addToNode(val2, order->getNode(val1));
                }
            } else {
                assert (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater);
                if (n1 != nullptr && n2 != nullptr) {
                    order->addRelationNodes(n1, n2);
                } else if (n1 != nullptr) {
                    order->addBetween(val2, n1, order->getBottom());
                } else if (n2 != nullptr) {
                    order->addBetween(val1, order->getTop(), n2);
                } else {
                    order->add(val1);
                    order->addBetween(val2, order->getNode(val1), order->getBottom());
                }
            }
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendOrder(Order* order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            if (assumption != nullptr) {
                handleAssumption(order, assumption);
            }
            return extendOrder(order, true, monRes);
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendOrder(Order *order, bool useAssumption, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            auto currentState = order->getNextSortedState();
            while ((*order->getAddedStates())[currentState]) {
                currentState = order->getNextSortedState();
            }
            if (cyclic && order->isOnlyBottomTopOrder()) {
                order->add(currentState);
            }
            while (currentState != numberOfStates ) {

                // Check if position of all successor states is known
                auto successors = stateMap[currentState];
                // If it is cyclic, first do forward reasoning
                if (cyclic && order->contains(currentState) && successors.size() == 2) {
                    extendByForwardReasoning(order, currentState, successors);
                }
                // Also do normal backward reasoning if the state is not yet in the order
                auto stateSucc1 = numberOfStates;
                auto stateSucc2 = numberOfStates;
                if (!order->contains(currentState)) {
                    auto backwardResult = extendByBackwardReasoning(order, currentState, successors);
                    stateSucc1 = backwardResult.first;
                    stateSucc2 = backwardResult.second;
                }

                // We only do this if it is cyclic, because than forward reasoning makes sense
                if (stateSucc1 != numberOfStates) {
                    assert (stateSucc2 != numberOfStates);
                    // create tuple for assumptions
                    if (useAssumption) {
                        // Remember that we did not yet add the currentState to the Order
                        order->addStateToHandle(currentState);
                        return std::make_tuple(order, stateSucc1, stateSucc2);
                    } else {
                        // If we don't allow for assumptions, we stop creating the order and return what we got so far
                        return std::make_tuple(order, numberOfStates, numberOfStates);
                    }
                }

                assert (order->contains(currentState) && order->getNode(currentState) != nullptr);

                auto succsOrdered = order->sortStates(&stateMap[currentState]);
                for(auto param : params) {
                    checkParOnStateMonRes(currentState, succsOrdered, param, monRes);
                }

                // Remove current state number from the list and get new one
                currentState = order->getNextSortedState();
            }

            if (useAssumption) {
                order->setDoneBuilding();
                monRes.get()->setDone(true);
            }
            return std::make_tuple(order, numberOfStates, numberOfStates);
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(Order *order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors) {
            if (!cyclic && order->isOnlyBottomTopOrder()) {
                order->add(currentState);

                return std::pair<uint_fast64_t, uint_fast64_t>(numberOfStates, numberOfStates);
            } else if (successors.size() == 1) {
                assert (order->contains(successors[0]));
                // As there is only one successor the current state and its successor must be at the same nodes.
                order->addToNode(currentState, order->getNode(successors[0]));
            } else if (successors.size() == 2) {
                // Otherwise, check how the two states compare, and add if the comparison is possible.
                uint_fast64_t succ1 = successors[0];
                uint_fast64_t succ2 = successors[1];

                int compareResult = order->compare(succ1, succ2);
                if (!cyclic && !usePLA && compareResult == Order::UNKNOWN) {
                    // Only use pla for acyclic models
                    getMinMaxValues();
                }
                if (usePLA && compareResult == Order::UNKNOWN) {
                    compareResult = addStatesBasedOnMinMax(order, succ1, succ2);
                }
                if (compareResult == Order::ABOVE) {
                    // successor 1 is closer to top than successor 2
                    order->addBetween(currentState, succ1, succ2);
                } else if (compareResult == Order::BELOW) {
                    // successor 2 is closer to top than successor 1
                    order->addBetween(currentState, order->getNode(succ2),
                                      order->getNode(succ1));
                } else if (compareResult == Order::SAME) {
                    // the successors are at the same level
                    order->addToNode(currentState, order->getNode(succ1));
                }
                if (compareResult == Order::UNKNOWN) {
                    return std::pair<uint_fast64_t, uint_fast64_t>(succ1, succ2);
                }
            } else {
                assert (successors.size() >= 2);
                auto highest = successors[0];
                auto lowest = highest;
                for (auto i = 1 ; i < successors.size(); ++i) {
                    auto compareWithHighest = order->compare(i, highest);
                    if (!cyclic && !usePLA && compareWithHighest == Order::UNKNOWN) {
                        // Only use pla for acyclic models
                        getMinMaxValues();
                    }
                    if (usePLA && compareWithHighest == Order::UNKNOWN) {
                        compareWithHighest = addStatesBasedOnMinMax(order, i, highest);
                    }
                    if (compareWithHighest == Order::ABOVE) {
                        highest = i;
                    } else if (compareWithHighest == Order::UNKNOWN) {
                        // Return current successors as the "problematic ones"
                        return std::pair<uint_fast64_t, uint_fast64_t>(i, highest);
                    }

                    auto compareWithLowest = order->compare(lowest, i);

                    if (usePLA && compareWithLowest == Order::UNKNOWN) {
                        compareWithLowest = addStatesBasedOnMinMax(order, lowest, i);
                    }
                    if (compareWithLowest == Order::ABOVE) {
                        lowest = i;
                    } else if (compareWithLowest == Order::UNKNOWN) {
                        // Return current successors as the "problematic ones"
                        return std::pair<uint_fast64_t, uint_fast64_t>(i, lowest);
                    }
                }

                if (lowest == highest) {
                    order->addToNode(currentState, order->getNode(highest));
                } else {
                    order->addBetween(currentState, order->getNode(highest), order->getNode(lowest));
                }
            }
            return std::pair<uint_fast64_t, uint_fast64_t>(numberOfStates, numberOfStates);
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendByForwardReasoning(Order *order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors) {
            // If this is the first state to add, we add it between =) and =(.
            auto succ1 = successors[0];
            auto compareSucc1 = order->compare(succ1, currentState);
            auto succ2 = successors[1];
            auto compareSucc2 = order->compare(succ2, currentState);

            if (compareSucc1 == Order::UNKNOWN && compareSucc2 == Order::UNKNOWN) {
                // ordering of succ1 and succ2 and currentState is unknown, as we have a cyclic pMC pla will not help
                return std::pair<uint_fast64_t, uint_fast8_t>(succ1, succ2);
            } else if (compareSucc1 == Order::UNKNOWN || compareSucc2 == Order::UNKNOWN) {
                if (compareSucc2 != Order::UNKNOWN) {
                    // swap them for easier implementation
                    std::swap(succ1, succ2);
                    std::swap(compareSucc1, compareSucc2);
                }
                if (compareSucc1 == Order::ABOVE) {
                    // Succ1 is above currentState, so we should add succ2 below current state
                    if (order->getNumberOfAddedStates() != numberOfStates) {
                        order->addBelow(succ2, order->getNode(currentState));
                    } else {
                        order->addRelation(currentState, succ2);
                    }
                    order->addStateToHandle(succ2);
                } else if (compareSucc1 == Order::BELOW) {
                    if (order->getNumberOfAddedStates() != numberOfStates) {
                        order->addAbove(succ2, order->getNode(currentState));
                    } else {
                        order->addRelation(succ2, currentState);
                    }
                    order->addStateToHandle(succ2);
                }
            }
            return std::pair<uint_fast64_t, uint_fast8_t>(numberOfStates, numberOfStates);
        }

        template <typename ValueType, typename ConstantType>
        Order::NodeComparison OrderExtender<ValueType, ConstantType>::addStatesBasedOnMinMax(Order *order, uint_fast64_t state1, uint_fast64_t state2) {
            assert (order->compare(state1, state2) == Order::UNKNOWN);
            if (minValues[state1] > maxValues[state2]) {
                // state 1 will always be larger than state2
                if (!order->contains(state1)) {
                    order->add(state1);
                }
                if (!order->contains(state2)) {
                    order->add(state2);
                }

                assert(order->compare(state1, state2) != Order::BELOW);
                assert(order->compare(state1, state2) != Order::SAME);
                order->addRelation(state1, state2);
                return Order::ABOVE;
            } else if (minValues[state2] > maxValues[state1]) {
                // state2 will always be larger than state1
                if (!order->contains(state1)) {
                    order->add(state1);
                }
                if (!order->contains(state2)) {
                    order->add(state2);
                }
                assert(order->compare(state2, state1) != Order::BELOW);
                assert(order->compare(state2, state1) != Order::SAME);
                order->addRelation(state2, state1);
                return Order::BELOW;
            } else {
                // Couldn't add relation between state1 and state 2 based on min/max values;
                return Order::UNKNOWN;
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::getMinMaxValues() {
            assert (!usePLA);
            // Use parameter lifting modelchecker to get initial min/max values for order creation
            modelchecker::SparseDtmcParameterLiftingModelChecker<models::sparse::Dtmc<ValueType>, ConstantType> plaModelChecker;
            std::unique_ptr<modelchecker::CheckResult> checkResult;
            auto env = Environment();

            const modelchecker::CheckTask<logic::Formula, ValueType> checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*formula);
            STORM_LOG_THROW(plaModelChecker.canHandle(model, checkTask), exceptions::NotSupportedException,"Cannot handle this formula");
            plaModelChecker.specify(env, model, checkTask, false, false);

            std::unique_ptr<modelchecker::CheckResult> minCheck = plaModelChecker.check(env, region,solver::OptimizationDirection::Minimize);
            std::unique_ptr<modelchecker::CheckResult> maxCheck = plaModelChecker.check(env, region,solver::OptimizationDirection::Maximize);

            minValues = minCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            maxValues = maxCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            usePLA = true;
        }

        template <typename ValueType, typename ConstantType>
        ValueType OrderExtender<ValueType, ConstantType>::getDerivative(ValueType function, typename OrderExtender<ValueType, ConstantType>::VariableType var) {
            if (function.isConstant()) {
                return utility::zero<ValueType>();
            }
            if ((derivatives[function]).find(var) == (derivatives[function]).end()) {
                (derivatives[function])[var] = function.derivative(var);
            }
            return (derivatives[function])[var];
        }

        template <typename ValueType, typename ConstantType>
        typename OrderExtender<ValueType, ConstantType>::Monotonicity OrderExtender<ValueType, ConstantType>::checkTransitionMonRes(ValueType function, typename OrderExtender<ValueType, ConstantType>::VariableType param) {
                std::pair<bool, bool> res = MonotonicityChecker<ValueType, ConstantType>::checkDerivative(getDerivative(function, param), region);
                if (res.first && !res.second) {
                    return Monotonicity::Incr;
                } else if (!res.first && res.second) {
                    return Monotonicity::Decr;
                } else if (res.first && res.second) {
                    return Monotonicity::Constant;
                } else {
                    return Monotonicity::Not;
                }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t s, const std::vector<uint_fast64_t>& succ, typename OrderExtender<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult) {
            uint_fast64_t succSize = succ.size();
            if (succSize == 2) {
                // In this case we can ignore the last entry, as this will have a probability of 1 - the other
                succSize = 1;
            }

            // Create + fill Vector containing the Monotonicity of the transitions to the succs
            std::vector<Monotonicity> succsMon(succSize);
            auto row = matrix.getRow(s);
            for (auto entry : row) {
                auto succState = entry.getColumn();
                auto function = entry.getValue();
                auto it = std::find(succ.begin(), succ.end(), succState);
                auto index = std::distance(succ.begin(), it);
                if (index != succSize) {
                    succsMon[index] = checkTransitionMonRes(function, param);
                }
            }

            uint_fast64_t index = 0;
            Monotonicity monCandidate = Monotonicity::Constant;
            Monotonicity temp;

            //go to first inc / dec
            while (index < succSize && monCandidate == Monotonicity::Constant) {
                temp = succsMon[index];
                if (temp != Monotonicity::Not) {
                    monCandidate = temp;
                } else {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                }
                index++;
            }
            if (index == succSize) {
                monResult->updateMonotonicityResult(param, monCandidate);
                return;
            }

            //go to first non-inc / non-dec
            while (index < succSize) {
                temp = succsMon[index];
                if (temp == Monotonicity::Not) {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                } else if (temp == Monotonicity::Constant || temp == monCandidate) {
                    index++;
                } else {
                    monCandidate = temp;
                    break;
                }
            }

            //check if it doesn't change until the end of vector
            while (index < succSize) {
                temp = succsMon[index];
                if (temp == Monotonicity::Constant || temp == monCandidate) {
                    index++;
                } else {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                }
            }

            if (monCandidate == Monotonicity::Incr) {
                monResult->updateMonotonicityResult(param, Monotonicity::Decr);
            } else {
                monResult->updateMonotonicityResult(param, Monotonicity::Incr);
            }
        }

        template class OrderExtender<RationalFunction, double>;
        template class OrderExtender<RationalFunction, RationalNumber>;
    }
}
