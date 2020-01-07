//
// Created by Jip Spel on 28.08.18.
//

#include "OrderExtender.h"
#include "storm/utility/macros.h"
#include "storm/utility/graph.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/graph.h"
#include <storm/logic/Formula.h>
#include <storm/modelchecker/propositional/SparsePropositionalModelChecker.h>
#include "storm/models/sparse/Model.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"


#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

#include <set>
#include <boost/container/flat_set.hpp>
#include "storm/storage/StronglyConnectedComponentDecomposition.h"
#include "storm/storage/StronglyConnectedComponent.h"

#include "storm/storage/BitVector.h"
#include "storm/utility/macros.h"
#include "storm/utility/Stopwatch.h"


namespace storm {
    namespace analysis {

        template<typename ValueType>
        OrderExtender<ValueType>::OrderExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model) {
            this->model = model;
            this->matrix = model->getTransitionMatrix();
            this->assumptionSeen = false;
            uint_fast64_t numberOfStates = this->model->getNumberOfStates();

            // Build stateMap
            for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
                stateMap[i] = new storm::storage::BitVector(numberOfStates, false);
                auto row = matrix.getRow(i);
                for (auto rowItr = row.begin(); rowItr != row.end(); ++rowItr) {
                    // ignore self-loops when there are more transitions
                    if (i != rowItr->getColumn() || row.getNumberOfEntries() == 1) {
                        stateMap[i]->set(rowItr->getColumn(), true);
                    }
                }
            }

            // Check if MC contains cycles
            storm::storage::StronglyConnectedComponentDecompositionOptions const options;
            this->sccs = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);
            acyclic = true;
            for (size_t i = 0; acyclic && i < sccs.size(); ++i) {
                acyclic &= sccs.getBlock(i).size() <= 1;
            }
        }

        template <typename ValueType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType>::toOrder(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas) {
            STORM_LOG_THROW((++formulas.begin()) == formulas.end(), storm::exceptions::NotSupportedException, "Only one formula allowed for monotonicity analysis");
            STORM_LOG_THROW((*(formulas[0])).isProbabilityOperatorFormula()
                            && ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()
                                || (*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()), storm::exceptions::NotSupportedException, "Expecting until or eventually formula");

            uint_fast64_t numberOfStates = this->model->getNumberOfStates();

            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<ValueType>> propositionalChecker(*model);
            storm::storage::BitVector phiStates;
            storm::storage::BitVector psiStates;
            if ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                phiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
            } else {
                phiStates = storm::storage::BitVector(numberOfStates, true);
                psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
            }


            // Get the maybeStates
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->model->getBackwardTransitions(), phiStates, psiStates);
            storm::storage::BitVector topStates = statesWithProbability01.second;
            storm::storage::BitVector bottomStates = statesWithProbability01.first;

            STORM_LOG_THROW(topStates.begin() != topStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no 1 states");
            STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no zero states");

            // Transform to Order
            auto matrix = this->model->getTransitionMatrix();

            auto initialMiddleStates = storm::storage::BitVector(numberOfStates);
            // Add possible cycle breaking states
            if (!acyclic) {
                for (size_t i = 0; i < sccs.size(); ++i) {
                    auto scc = sccs.getBlock(i);
                    if (scc.size() > 1) {
                        auto states = scc.getStates();
                        // check if the state has already one successor in bottom of top, in that case pick it
                        for (auto const& state : states) {
                            auto successors = stateMap[state];
                            if (successors->getNumberOfSetBits() == 2) {
                                auto succ1 = successors->getNextSetIndex(0);
                                auto succ2 = successors->getNextSetIndex(succ1 + 1);
                                auto intersection = bottomStates | topStates;
                                if (intersection[succ1] || intersection[succ2]) {
                                    initialMiddleStates.set(state);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(matrix);
            Order *order = new Order(&topStates, &bottomStates, &initialMiddleStates, numberOfStates, &statesSorted);

            return this->extendOrder(order);
        }


        template <typename ValueType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType>::toOrder(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, std::vector<double> minValues, std::vector<double> maxValues) {
            uint_fast64_t numberOfStates = this->model->getNumberOfStates();
            uint_fast64_t bottom = numberOfStates;
            uint_fast64_t top = numberOfStates;
            std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(matrix);
            Order *order = nullptr;

            for (auto state : statesSorted) {
                if ((minValues[numberOfStates - 1 - state] == 1 || maxValues[numberOfStates - 1 - state] == 0)
                        && minValues[numberOfStates - 1 - state] == maxValues[numberOfStates - 1 - state]) {
                    if (maxValues[numberOfStates - 1 - state] == 0) {
                        assert (bottom == numberOfStates);
                        bottom = state;
                    }
                    if (minValues[numberOfStates - 1 - state] == 1) {
                        assert (top == numberOfStates);
                        top = state;
                    }
                    if (bottom != numberOfStates && top != numberOfStates) {
                        order = new Order(top, bottom, numberOfStates, &statesSorted);
                    }

                } else {
                    assert (order != nullptr);
                    auto successors = stateMap[state];
                    if (successors->getNumberOfSetBits() == 1) {
                        auto succ = successors->getNextSetIndex(0);
                        if (succ != state) {
                            if (!order->contains(succ)) {
                                order->add(succ);
                            }
                            order->addToNode(state, order->getNode(succ));
                        }
                    } else if (successors->getNumberOfSetBits() > 1) {
                        uint_fast64_t min = numberOfStates;
                        uint_fast64_t max = numberOfStates;
                        bool allSorted = true;

                        for (auto succ = successors->getNextSetIndex(0);
                             succ < numberOfStates; succ = successors->getNextSetIndex(succ + 1)) {
                            if (min == numberOfStates) {
                                assert (max == numberOfStates);
                                min = succ;
                                max = succ;
                            } else {
                                if (minValues[numberOfStates - 1 - succ] > maxValues[numberOfStates - 1 - max]) {
                                    max = succ;
                                } else if (maxValues[numberOfStates - 1 - succ] < minValues[numberOfStates - 1 - min]) {
                                    min = succ;
                                } else {
                                    allSorted = false;
                                    break;
                                }
                            }
                        }

                        if (allSorted && min != max) {
                            if (order->contains(min) && order->contains(max)) {
                                assert (order->compare(min,max) == Order::UNKNOWN || order->compare(min,max) == Order::BELOW);
                                if (order->compare(min, max) == Order::UNKNOWN) {
                                    order->addRelation(max, min);
                                }
                            }
                            if (!order->contains(min)) {
                                if (order->contains(max)) {
                                    order->addBetween(min, order->getNode(max), order->getBottom());
                                } else {
                                    order->add(min);
                                }
                            }
                            if (!order->contains(max)) {
                                // Because of construction min is in the order
                                order->addBetween(max, order->getTop(), order->getNode(min));
                            }
                            assert (order->compare(max, min) == Order::ABOVE);
                            if (order->contains(state)) {
                                if (order->compare(max, state) == Order::UNKNOWN) {
                                    order->addRelation(max, state);
                                }
                                if (order->compare(state, min) == Order::UNKNOWN) {
                                    order->addRelation(state, min);
                                }
                            } else {
                                order->addBetween(state, max, min);
                            }
                            assert (order->compare(max, state) == Order::ABOVE);
                            assert (order->compare(state, min) == Order::ABOVE);
                        }
                    }
                }
            }

            assert (order != nullptr);

            // Handle sccs
            auto addedStates = order->getAddedStates();
            for (auto scc : sccs) {
                if (scc.size() > 1) {
                    auto states = scc.getStates();
                    auto candidate = -1;
                    for (auto const& state : states) {
                        if (addedStates->get(state)) {
                            candidate = -1;
                            break;
                            // if there is a state of the scc already present in the order, there is no need to add one.
                        }
                        auto successors = stateMap[state];
                        if (candidate == -1 && successors->getNumberOfSetBits() == 2) {
                            auto succ1 = successors->getNextSetIndex(0);
                            auto succ2 = successors->getNextSetIndex(succ1 + 1);
                            if (addedStates->get(succ1) || addedStates->get(succ2)) {
                                candidate = state;
                            }
                        }
                    }
                    if (candidate != -1) {
                        order->add(candidate);
                        order->statesToHandle->set(candidate);
                    }
                }
             }
            return this->extendOrder(order);
        }


        template <typename ValueType>
        void OrderExtender<ValueType>::handleAssumption(Order* order, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            assert (assumption != nullptr);
            assumptionSeen = true;

            storm::expressions::BinaryRelationExpression expr = *assumption;
            assert (expr.getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Greater
                || expr.getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal);

            if (expr.getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::Equal) {
                assert (expr.getFirstOperand()->isVariable() && expr.getSecondOperand()->isVariable());
                storm::expressions::Variable var1 = expr.getFirstOperand()->asVariableExpression().getVariable();
                storm::expressions::Variable var2 = expr.getSecondOperand()->asVariableExpression().getVariable();
                auto val1 = std::stoul(var1.getName(), nullptr, 0);
                auto val2 = std::stoul(var2.getName(), nullptr, 0);
                auto comp = order->compare(val1, val2);

                assert (comp == Order::UNKNOWN);
                Order::Node *n1 = order->getNode(val1);
                Order::Node *n2 = order->getNode(val2);

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
                assert (expr.getFirstOperand()->isVariable() && expr.getSecondOperand()->isVariable());
                storm::expressions::Variable largest = expr.getFirstOperand()->asVariableExpression().getVariable();
                storm::expressions::Variable smallest = expr.getSecondOperand()->asVariableExpression().getVariable();
                auto val1 = std::stoul(largest.getName(), nullptr, 0);
                auto val2 = std::stoul(smallest.getName(), nullptr, 0);
                auto compareRes = order->compare(val1, val2);

                assert(compareRes == Order::UNKNOWN);
                Order::Node *n1 = order->getNode(val1);
                Order::Node *n2 = order->getNode(val2);

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

        template <typename ValueType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType>::extendAllSuccAdded(Order* order, uint_fast64_t const & stateNumber, storm::storage::BitVector* successors) {
            auto numberOfStates = successors->size();
            assert (order->getAddedStates()->size() == numberOfStates);

            if (successors->getNumberOfSetBits() == 1) {
                // As there is only one successor the current state and its successor must be at the same nodes.
                order->addToNode(stateNumber, order->getNode(successors->getNextSetIndex(0)));
            } else if (successors->getNumberOfSetBits() == 2) {
                // Otherwise, check how the two states compare, and add if the comparison is possible.
                uint_fast64_t successor1 = successors->getNextSetIndex(0);
                uint_fast64_t successor2 = successors->getNextSetIndex(successor1 + 1);

                int compareResult = order->compare(successor1, successor2);
                if (compareResult == Order::ABOVE) {
                    // successor 1 is closer to top than successor 2
                    order->addBetween(stateNumber, order->getNode(successor1),
                                        order->getNode(successor2));
                } else if (compareResult == Order::BELOW) {
                    // successor 2 is closer to top than successor 1
                    order->addBetween(stateNumber, order->getNode(successor2),
                                        order->getNode(successor1));
                } else if (compareResult == Order::SAME) {
                    // the successors are at the same level
                    order->addToNode(stateNumber, order->getNode(successor1));
                } else {
                    assert(order->compare(successor1, successor2) == Order::UNKNOWN);
                    return std::make_tuple(order, successor1, successor2);
                }
            } else if (successors->getNumberOfSetBits() > 2) {
                for (auto const& i : *successors) {
                    for (auto j = successors->getNextSetIndex(i+1); j < numberOfStates; j = successors->getNextSetIndex(j+1)) {
                        if (order->compare(i,j) == Order::UNKNOWN) {
                            return std::make_tuple(order, i, j);
                        }
                    }
                }

                auto highest = successors->getNextSetIndex(0);
                auto lowest = highest;
                for (auto i = successors->getNextSetIndex(highest+1); i < numberOfStates; i = successors->getNextSetIndex(i+1)) {
                    if (order->compare(i, highest) == Order::ABOVE) {
                        highest = i;
                    }
                    if (order->compare(lowest, i) == Order::ABOVE) {
                        lowest = i;
                    }
                }
                if (lowest == highest) {
                    order->addToNode(stateNumber, order->getNode(highest));
                } else {
                    order->addBetween(stateNumber, order->getNode(highest), order->getNode(lowest));
                }
            }
            return std::make_tuple(order, numberOfStates, numberOfStates);
        }



        template <typename ValueType>
        std::tuple<Order*, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType>::extendOrder(Order* order, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            auto numberOfStates = this->model->getNumberOfStates();

            if (assumption != nullptr) {
                handleAssumption(order, assumption);
            }

            auto oldNumberSet = numberOfStates;
            while (oldNumberSet != order->getAddedStates()->getNumberOfSetBits()) {
                oldNumberSet = order->getAddedStates()->getNumberOfSetBits();

                // Forward reasoning for cycles;
                if (!acyclic) {
                    auto statesToHandle = order->statesToHandle;
                    auto stateNumber = statesToHandle->getNextSetIndex(0);

                    while (stateNumber != numberOfStates) {
                        storm::storage::BitVector *successors = stateMap[stateNumber];
                        // Checking for states which are already added to the order, and only have one successor left which haven't been added yet
                        auto succ1 = successors->getNextSetIndex(0);
                        auto succ2 = successors->getNextSetIndex(succ1 + 1);

                        assert (order->contains(stateNumber));
                        if (successors->getNumberOfSetBits() == 1) {
                            if (!order->contains(succ1)) {
                                order->addToNode(succ1, order->getNode(stateNumber));
                                statesToHandle->set(succ1, true);
                                if (order->containsStatesSorted(succ1)) {
                                    order->removeStatesSorted(succ1);
                                }
                            }
                            statesToHandle->set(stateNumber, false);
                            stateNumber = statesToHandle->getNextSetIndex(0);
                        } else if (successors->getNumberOfSetBits() == 2
                                   && ((order->contains(succ1) && !order->contains(succ2))
                                       || (!order->contains(succ1) && order->contains(succ2)))) {

                            if (!order->contains(succ1)) {
                                std::swap(succ1, succ2);
                            }

                            auto compare = order->compare(stateNumber, succ1);
                            if (compare == Order::ABOVE) {
                                if (order->containsStatesSorted(succ2)) {
                                    order->removeStatesSorted(succ2);
                                }
                                order->addBetween(succ2, order->getTop(), order->getNode(stateNumber));
                                statesToHandle->set(succ2);
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            } else if (compare == Order::BELOW) {
                                if (order->containsStatesSorted(succ2)) {
                                    order->removeStatesSorted(succ2);
                                }
                                order->addBetween(succ2, order->getNode(stateNumber), order->getBottom());
                                statesToHandle->set(succ2);
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            } else {
                                // We don't know positions, so we set the current state number to false
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            }

                        } else if (!((order->contains(succ1) && !order->contains(succ2))
                                     || (!order->contains(succ1) && order->contains(succ2)))) {
                            stateNumber = statesToHandle->getNextSetIndex(stateNumber + 1);
                        } else {
                            statesToHandle->set(stateNumber, false);
                            stateNumber = statesToHandle->getNextSetIndex(0);
                        }
                    }

                }

                // Normal backwardreasoning
                auto stateNumber = order->getNextSortedState();
                while (stateNumber != numberOfStates && order->contains(stateNumber)) {
                    order->removeFirstStatesSorted();
                    stateNumber = order->getNextSortedState();

                    if (stateNumber != numberOfStates && order->contains(stateNumber)) {
                        auto resAllAdded = allSuccAdded(order, stateNumber);
                        if (!std::get<0>(resAllAdded)) {
                            return std::make_tuple(order, std::get<1>(resAllAdded), std::get<2>(resAllAdded));
                        }
                    }
                }

                if (stateNumber != numberOfStates && !order->contains(stateNumber)) {
                    auto successors = stateMap[stateNumber];

                    auto result = extendAllSuccAdded(order, stateNumber, successors);
                    if (std::get<1>(result) != numberOfStates) {
                        // So we don't know the relation between all successor states
                        return result;
                    } else {
                        assert (order->getNode(stateNumber) != nullptr);
                        if (!acyclic) {
                            order->statesToHandle->set(stateNumber);
                        }
                        order->removeFirstStatesSorted();
                    }
                }
                assert (stateNumber == numberOfStates || order->getNode(stateNumber) != nullptr);
                assert (stateNumber == numberOfStates || order->contains(stateNumber));

            }
            assert (order->getAddedStates()->getNumberOfSetBits() == numberOfStates);
            order->setDoneBuilding(true);
            return std::make_tuple(order, numberOfStates, numberOfStates);
        }

        template <typename ValueType>
        std::tuple<bool, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType>::allSuccAdded(storm::analysis::Order *order, uint_fast64_t stateNumber) {
            auto successors = stateMap[stateNumber];
            auto numberOfStates = successors->size();

            if (successors->getNumberOfSetBits() == 1) {
                auto succ = successors->getNextSetIndex(0);
                return std::make_tuple(order->contains(succ), succ, succ);
            } else if (successors->getNumberOfSetBits() > 2) {
                for (auto const& i : *successors) {
                    for (auto j = successors->getNextSetIndex(i+1); j < numberOfStates; j = successors->getNextSetIndex(j+1)) {
                        if (order->compare(i,j) == Order::UNKNOWN) {
                            return std::make_tuple(false, i, j);
                        }
                    }
                }
            }
            return std::make_tuple(true, numberOfStates, numberOfStates);

        }

        template class OrderExtender<storm::RationalFunction>;
    }
}
