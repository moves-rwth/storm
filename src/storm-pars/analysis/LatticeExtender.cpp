//
// Created by Jip Spel on 28.08.18.
//

#include "LatticeExtender.h"
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
        LatticeExtender<ValueType>::LatticeExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model) {
            this->model = model;
            this->matrix = model->getTransitionMatrix();
            this->assumptionSeen = false;
            uint_fast64_t numberOfStates = this->model->getNumberOfStates();

            // Build stateMap
            // TODO: is dit wel nodig
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
            for (auto i = 0; acyclic && i < sccs.size(); ++i) {
                acyclic &= sccs.getBlock(i).size() <= 1;
            }
        }

        template <typename ValueType>
        std::tuple<Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas) {
            STORM_LOG_THROW((++formulas.begin()) == formulas.end(), storm::exceptions::NotSupportedException, "Only one formula allowed for monotonicity analysis");
            STORM_LOG_THROW((*(formulas[0])).isProbabilityOperatorFormula()
                            && ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()
                                || (*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()), storm::exceptions::NotSupportedException, "Expecting until or eventually formula");

            uint_fast64_t numberOfStates = this->model->getNumberOfStates();

            // TODO: dit moet anders kunnen
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

            // Transform to Lattice
            auto matrix = this->model->getTransitionMatrix();

            auto initialMiddleStates = storm::storage::BitVector(numberOfStates);
            // Check if MC contains cycles
            storm::storage::StronglyConnectedComponentDecompositionOptions const options;

            // Create the Lattice

            if (!acyclic) {
                for (auto i = 0; i < sccs.size(); ++i) {
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
            Lattice *lattice = new Lattice(&topStates, &bottomStates, &initialMiddleStates, numberOfStates, &statesSorted);

            return this->extendLattice(lattice);
        }


        template <typename ValueType>
        std::tuple<Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas, std::vector<double> minValues, std::vector<double> maxValues) {
            uint_fast64_t numberOfStates = this->model->getNumberOfStates();

            // Compare min/max for all states
            STORM_LOG_THROW((++formulas.begin()) == formulas.end(), storm::exceptions::NotSupportedException, "Only one formula allowed for monotonicity analysis");
            STORM_LOG_THROW((*(formulas[0])).isProbabilityOperatorFormula()
                            && ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()
                                || (*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()), storm::exceptions::NotSupportedException, "Expecting until or eventually formula");

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

            uint_fast64_t bottom = bottomStates.getNextSetIndex(0);
            uint_fast64_t top = topStates.getNextSetIndex(0);
            std::vector<uint_fast64_t> statesSorted = storm::utility::graph::getTopologicalSort(matrix);
            Lattice *lattice = new Lattice(top, bottom, numberOfStates, &statesSorted);


            for (auto state : *(lattice->statesSorted)) {
                if (state != bottom && state != top) {
                    assert (lattice != nullptr);
                    auto successors = stateMap[state];
                    if (successors->size() > 1) {
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
                                if (minValues[succ] > maxValues[max]) {
                                    max = succ;
                                } else if (maxValues[succ] < minValues[min]) {
                                    min = succ;
                                } else {
                                    allSorted = false;
                                    break;
                                }
                            }
                        }

                        if (allSorted && min != max) {
                            if (lattice->contains(min) && lattice->contains(max)) {
                                assert (lattice->compare(min,max) == Lattice::UNKNOWN || lattice->compare(min,max) == Lattice::BELOW);
                                if (lattice->compare(min, max) == Lattice::UNKNOWN) {
                                    lattice->addRelation(max, min);
                                }
                            }
                            if (!lattice->contains(min)) {
                                if (lattice->contains(max)) {
                                    lattice->addBetween(min, lattice->getNode(max), lattice->getBottom());
                                } else {
                                    lattice->add(min);
                                }
                            }
                            if (!lattice->contains(max)) {
                                // Because of construction min is in the lattice
                                lattice->addBetween(max, lattice->getNode(min), lattice->getTop());
                            }
                            assert (lattice->compare(max, min) == Lattice::ABOVE);
                            lattice->addBetween(state, max, min);
                        }
                    }
                }
            }

            // Handle sccs
            auto addedStates = lattice->getAddedStates();
            for (auto scc : sccs) {
                if (scc.size() > 1) {
                    auto states = scc.getStates();
                    auto candidate = -1;
                    for (auto const& state : states) {
                        if (addedStates->get(state)) {
                            candidate = -1;
                            break;
                            // if there is a state of the scc already present in the lattice, there is no need to add one.
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
                        lattice->add(candidate);
                        lattice->statesToHandle->set(candidate);
                    }
                }
             }
            return this->extendLattice(lattice);
        }


        template <typename ValueType>
        void LatticeExtender<ValueType>::handleAssumption(Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
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
                auto comp = lattice->compare(val1, val2);

                assert (comp == Lattice::UNKNOWN);
                Lattice::Node *n1 = lattice->getNode(val1);
                Lattice::Node *n2 = lattice->getNode(val2);

                if (n1 != nullptr && n2 != nullptr) {
                    lattice->mergeNodes(n1,n2);
                } else if (n1 != nullptr) {
                    lattice->addToNode(val2, n1);
                } else if (n2 != nullptr) {
                    lattice->addToNode(val1, n2);
                } else {
                    lattice->add(val1);
                    lattice->addToNode(val2, lattice->getNode(val1));
                }
            } else {
                assert (expr.getFirstOperand()->isVariable() && expr.getSecondOperand()->isVariable());
                storm::expressions::Variable largest = expr.getFirstOperand()->asVariableExpression().getVariable();
                storm::expressions::Variable smallest = expr.getSecondOperand()->asVariableExpression().getVariable();
                auto val1 = std::stoul(largest.getName(), nullptr, 0);
                auto val2 = std::stoul(smallest.getName(), nullptr, 0);
                auto compareRes = lattice->compare(val1, val2);

                assert(compareRes == Lattice::UNKNOWN);
                Lattice::Node *n1 = lattice->getNode(val1);
                Lattice::Node *n2 = lattice->getNode(val2);

                if (n1 != nullptr && n2 != nullptr) {
                    lattice->addRelationNodes(n1, n2);
                } else if (n1 != nullptr) {
                    lattice->addBetween(val2, n1, lattice->getBottom());
                } else if (n2 != nullptr) {
                    lattice->addBetween(val1, lattice->getTop(), n2);
                } else {
                    lattice->add(val1);
                    lattice->addBetween(val2, lattice->getNode(val1), lattice->getBottom());
                }
            }
        }

        template <typename ValueType>
        std::tuple<Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::extendAllSuccAdded(Lattice* lattice, uint_fast64_t const & stateNumber, storm::storage::BitVector* successors) {
            auto numberOfStates = successors->size();
            assert (lattice->getAddedStates()->size() == numberOfStates);

            if (successors->getNumberOfSetBits() == 1) {
                // As there is only one successor the current state and its successor must be at the same nodes.
                lattice->addToNode(stateNumber, lattice->getNode(successors->getNextSetIndex(0)));
            } else if (successors->getNumberOfSetBits() == 2) {
                // Otherwise, check how the two states compare, and add if the comparison is possible.
                uint_fast64_t successor1 = successors->getNextSetIndex(0);
                uint_fast64_t successor2 = successors->getNextSetIndex(successor1 + 1);

                int compareResult = lattice->compare(successor1, successor2);
                if (compareResult == Lattice::ABOVE) {
                    // successor 1 is closer to top than successor 2
                    lattice->addBetween(stateNumber, lattice->getNode(successor1),
                                        lattice->getNode(successor2));
                } else if (compareResult == Lattice::BELOW) {
                    // successor 2 is closer to top than successor 1
                    lattice->addBetween(stateNumber, lattice->getNode(successor2),
                                        lattice->getNode(successor1));
                } else if (compareResult == Lattice::SAME) {
                    // the successors are at the same level
                    lattice->addToNode(stateNumber, lattice->getNode(successor1));
                } else {
                    assert(lattice->compare(successor1, successor2) == Lattice::UNKNOWN);
                    return std::make_tuple(lattice, successor1, successor2);
                }
            } else if (successors->getNumberOfSetBits() > 2) {
                for (auto const& i : *successors) {
                    for (auto j = successors->getNextSetIndex(i+1); j < numberOfStates; j = successors->getNextSetIndex(j+1)) {
                        if (lattice->compare(i,j) == Lattice::UNKNOWN) {
                            return std::make_tuple(lattice, i, j);
                        }
                    }
                }

                auto highest = successors->getNextSetIndex(0);
                auto lowest = highest;
                for (auto i = successors->getNextSetIndex(highest+1); i < numberOfStates; i = successors->getNextSetIndex(i+1)) {
                    if (lattice->compare(i, highest) == Lattice::ABOVE) {
                        highest = i;
                    }
                    if (lattice->compare(lowest, i) == Lattice::ABOVE) {
                        lowest = i;
                    }
                }
                if (lowest == highest) {
                    lattice->addToNode(stateNumber, lattice->getNode(highest));
                } else {
                    lattice->addBetween(stateNumber, lattice->getNode(highest), lattice->getNode(lowest));
                }
            }
            return std::make_tuple(lattice, numberOfStates, numberOfStates);
        }



        template <typename ValueType>
        std::tuple<Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::extendLattice(Lattice* lattice, std::shared_ptr<storm::expressions::BinaryRelationExpression> assumption) {
            auto numberOfStates = this->model->getNumberOfStates();

            if (assumption != nullptr) {
                handleAssumption(lattice, assumption);
            }

            auto statesSorted = lattice->statesSorted;

            auto oldNumberSet = numberOfStates;
            while (oldNumberSet != lattice->getAddedStates()->getNumberOfSetBits()) {
                oldNumberSet = lattice->getAddedStates()->getNumberOfSetBits();

                // Forward reasoning for cycles;
                if (!acyclic) {
                    auto statesToHandle = lattice->statesToHandle;
                    auto stateNumber = statesToHandle->getNextSetIndex(0);

                    while (stateNumber != numberOfStates) {
                        storm::storage::BitVector *successors = stateMap[stateNumber];
                        // Checking for states which are already added to the lattice, and only have one successor left which haven't been added yet
                        auto succ1 = successors->getNextSetIndex(0);
                        auto succ2 = successors->getNextSetIndex(succ1 + 1);

                        assert (lattice->contains(stateNumber));
                        if (successors->getNumberOfSetBits() == 1) {
                            if (!lattice->contains(succ1)) {
                                lattice->addToNode(succ1, lattice->getNode(stateNumber));
                                statesToHandle->set(succ1, true);
                                auto itr = std::find(statesSorted->begin(), statesSorted->end(), succ1);
                                if (itr != statesSorted->end()) {
                                    statesSorted->erase(itr);
                                }
                            }
                            statesToHandle->set(stateNumber, false);
                            stateNumber = statesToHandle->getNextSetIndex(0);
                        } else if (successors->getNumberOfSetBits() == 2
                                   && ((lattice->contains(succ1) && !lattice->contains(succ2))
                                       || (!lattice->contains(succ1) && lattice->contains(succ2)))) {

                            if (!lattice->contains(succ1)) {
                                std::swap(succ1, succ2);
                            }

                            auto compare = lattice->compare(stateNumber, succ1);
                            if (compare == Lattice::ABOVE) {
                                auto itr = std::find(statesSorted->begin(), statesSorted->end(), succ2);
                                if (itr != statesSorted->end()) {
                                    statesSorted->erase(itr);
                                }
                                lattice->addBetween(succ2, lattice->getTop(), lattice->getNode(stateNumber));
                                statesToHandle->set(succ2);
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            } else if (compare == Lattice::BELOW) {
                                auto itr = std::find(statesSorted->begin(), statesSorted->end(), succ2);
                                if (itr != statesSorted->end()) {
                                    statesSorted->erase(itr);
                                }
                                lattice->addBetween(succ2, lattice->getNode(stateNumber), lattice->getBottom());
                                statesToHandle->set(succ2);
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            } else {
                                // We don't know positions, so we set the current state number to false
                                statesToHandle->set(stateNumber, false);
                                stateNumber = statesToHandle->getNextSetIndex(0);
                            }

                        } else if (!((lattice->contains(succ1) && !lattice->contains(succ2))
                                     || (!lattice->contains(succ1) && lattice->contains(succ2)))) {
                            stateNumber = statesToHandle->getNextSetIndex(stateNumber + 1);
                        } else {
                            statesToHandle->set(stateNumber, false);
                            stateNumber = statesToHandle->getNextSetIndex(0);
                        }
                    }

                }

                // Normal backwardreasoning
                if (statesSorted->size() > 0) {
                    auto stateNumber = *(statesSorted->begin());
                    while (lattice->contains(stateNumber) && statesSorted->size() > 1) {
                        // states.size()>1 such that there is at least one state left after erase
                        statesSorted->erase(statesSorted->begin());
                        stateNumber = *(statesSorted->begin());

                        if (lattice->contains(stateNumber)) {
                            auto resAllAdded = allSuccAdded(lattice, stateNumber);
                            if (!std::get<0>(resAllAdded)) {
                                return std::make_tuple(lattice, std::get<1>(resAllAdded), std::get<2>(resAllAdded));
                            }
                        }
                    }

                    if (!lattice->contains(stateNumber)) {
                        auto successors = stateMap[stateNumber];

                        auto result = extendAllSuccAdded(lattice, stateNumber, successors);
                        if (std::get<1>(result) != numberOfStates) {
                            // So we don't know the relation between all successor states
                            return result;
                        } else {
                            assert (lattice->getNode(stateNumber) != nullptr);
                            if (!acyclic) {
                                lattice->statesToHandle->set(stateNumber);
                            }
                            statesSorted->erase(statesSorted->begin());
                        }
                    }
                    assert (lattice->getNode(stateNumber) != nullptr);
                    assert (lattice->contains(stateNumber));
                }

            }
            assert (lattice->getAddedStates()->getNumberOfSetBits() == numberOfStates);
            lattice->setDoneBuilding(true);
            return std::make_tuple(lattice, numberOfStates, numberOfStates);
        }

        template <typename ValueType>
        std::tuple<bool, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::allSuccAdded(storm::analysis::Lattice *lattice, uint_fast64_t stateNumber) {
            auto successors = stateMap[stateNumber];
            auto numberOfStates = successors->size();

            if (successors->getNumberOfSetBits() == 1) {
                auto succ = successors->getNextSetIndex(0);
                return std::make_tuple(lattice->contains(succ), succ, succ);
            } else if (successors->getNumberOfSetBits() > 2) {
                for (auto const& i : *successors) {
                    for (auto j = successors->getNextSetIndex(i+1); j < numberOfStates; j = successors->getNextSetIndex(j+1)) {
                        if (lattice->compare(i,j) == Lattice::UNKNOWN) {
                            return std::make_tuple(false, i, j);
                        }
                    }
                }
            }
            return std::make_tuple(true, numberOfStates, numberOfStates);

        }

        template class LatticeExtender<storm::RationalFunction>;
    }
}
