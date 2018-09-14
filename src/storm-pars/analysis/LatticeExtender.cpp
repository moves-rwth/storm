//
// Created by Jip Spel on 28.08.18.
//

#include "LatticeExtender.h"
#include "storm/utility/macros.h"
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

#include "storm/storage/BitVector.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace analysis {

        template<typename ValueType>
        LatticeExtender<ValueType>::LatticeExtender(std::shared_ptr<storm::models::sparse::Model<ValueType>> model) : model(model) {
            // intentionally left empty
        }

        template <typename ValueType>
        std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas) {
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

            // Transform to Lattice
            auto matrix = this->model->getTransitionMatrix();

            for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
                stateMap[i] = storm::storage::BitVector(numberOfStates, false);

                auto row = matrix.getRow(i);
                for (auto rowItr = row.begin(); rowItr != row.end(); ++rowItr) {
                    stateMap[i].set(rowItr->getColumn(), true);
                }
            }

            // Create the Lattice
            storm::analysis::Lattice *lattice = new storm::analysis::Lattice(topStates, bottomStates, numberOfStates);
            return this->extendLattice(lattice);
        }

        template <typename ValueType>
        std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::extendLattice(storm::analysis::Lattice* lattice) {
            std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions;
            return this->extendLattice(lattice, assumptions);
        }

        template <typename ValueType>
        std::tuple<storm::analysis::Lattice*, uint_fast64_t, uint_fast64_t> LatticeExtender<ValueType>::extendLattice(storm::analysis::Lattice* lattice, std::set<std::shared_ptr<storm::expressions::BinaryRelationExpression>> assumptions) {
            auto numberOfStates = this->model->getNumberOfStates();
            // First handle assumptions
            for (auto itr = assumptions.begin(); itr != assumptions.end(); ++itr) {
                storm::expressions::BinaryRelationExpression expr = *(*itr);
                STORM_LOG_THROW(expr.getRelationType() == storm::expressions::BinaryRelationExpression::RelationType::GreaterOrEqual, storm::exceptions::NotImplementedException, "Only greater assumptions allowed");
                if (expr.getFirstOperand()->isVariable() && expr.getSecondOperand()->isVariable()) {
                    storm::expressions::Variable largest = expr.getFirstOperand()->asVariableExpression().getVariable();
                    storm::expressions::Variable smallest = expr.getSecondOperand()->asVariableExpression().getVariable();
                    if (lattice->compare(std::stoul(largest.getName(), nullptr, 0), std::stoul(smallest.getName(), nullptr, 0)) != storm::analysis::Lattice::ABOVE) {
                        storm::analysis::Lattice::Node* n1 = lattice->getNode(std::stoul(largest.getName(), nullptr, 0));
                        storm::analysis::Lattice::Node* n2 = lattice->getNode(std::stoul(smallest.getName(), nullptr, 0));

                        if (n1 != nullptr && n2 != nullptr) {
                            lattice->addRelationNodes(n1, n2);
                        } else if (n1 != nullptr) {
                            lattice->addBetween(std::stoul(smallest.getName(), nullptr, 0), n1, lattice->getBottom());
                        } else if (n2 != nullptr) {
                            lattice->addBetween(std::stoul(largest.getName(), nullptr, 0), lattice->getTop(), n2);
                        } else {
                            lattice->add(std::stoul(largest.getName(), nullptr, 0));
                            lattice->addBetween(std::stoul(smallest.getName(), nullptr, 0), lattice->getNode(std::stoul(largest.getName(), nullptr, 0)),
                                                lattice->getBottom());
                        }
                    }
                }
            }

            auto oldNumberSet = lattice->getAddedStates().getNumberOfSetBits();
            while (lattice->getAddedStates().getNumberOfSetBits() != numberOfStates) {
                for (auto stateItr = stateMap.begin(); stateItr != stateMap.end(); ++stateItr) {
                    storm::storage::BitVector seenStates = (lattice->getAddedStates());
                    // Iterate over all states
                    auto stateNumber = stateItr->first;
                    storm::storage::BitVector successors = stateItr->second;

                    // Check if current state has not been added yet, and all successors have
                    bool check = !seenStates[stateNumber];
                    for (auto succIndex = successors.getNextSetIndex(0); check && succIndex != numberOfStates; succIndex = successors.getNextSetIndex(++succIndex)) {
                        check &= seenStates[succIndex];
                    }

                    if (check && successors.getNumberOfSetBits() == 1) {
                        // As there is only one successor the current state and its successor must be at the same nodes.
                        lattice->addToNode(stateNumber, lattice->getNode(successors.getNextSetIndex(0)));
                    } else if (check && successors.getNumberOfSetBits() == 2) {
                        // Otherwise, check how the two states compare, and add if the comparison is possible.
                        uint_fast64_t successor1 = successors.getNextSetIndex(0);
                        uint_fast64_t successor2 = successors.getNextSetIndex(successor1 + 1);

                        int compareResult = lattice->compare(successor1, successor2);
                        if (compareResult == storm::analysis::Lattice::ABOVE) {
                            // successor 1 is closer to top than successor 2
                            lattice->addBetween(stateNumber, lattice->getNode(successor1),
                                                lattice->getNode(successor2));
                        } else if (compareResult == storm::analysis::Lattice::BELOW) {
                            // successor 2 is closer to top than successor 1
                            lattice->addBetween(stateNumber, lattice->getNode(successor2),
                                                lattice->getNode(successor1));
                        } else if (compareResult == storm::analysis::Lattice::SAME) {
                            // the successors are at the same level
                            lattice->addToNode(stateNumber, lattice->getNode(successor1));
                        } else {
                            return std::make_tuple(lattice, successor1, successor2);
                        }
                    } else if (check && successors.getNumberOfSetBits() > 2) {
                        for (auto i = successors.getNextSetIndex(0); i < successors.size(); i = successors.getNextSetIndex(i+1)) {
                            for (auto j = successors.getNextSetIndex(i+1); j < successors.size(); j = successors.getNextSetIndex(j+1)) {
                                if (lattice->compare(i,j) == storm::analysis::Lattice::UNKNOWN) {
                                    return std::make_tuple(lattice, i, j);
                                }
                            }
                        }

                        auto highest = successors.getNextSetIndex(0);
                        auto lowest = highest;
                        for (auto i = successors.getNextSetIndex(highest+1); i < successors.size(); i = successors.getNextSetIndex(i+1)) {
                            if (lattice->compare(i, highest) == storm::analysis::Lattice::ABOVE) {
                                highest = i;
                            }
                            if (lattice->compare(lowest, i) == storm::analysis::Lattice::ABOVE) {
                                lowest = i;
                            }
                        }
                        lattice->addBetween(stateNumber, lattice->getNode(highest), lattice->getNode(lowest));
                    }
                }
                // Nothing changed and not done yet
                if (oldNumberSet == lattice->getAddedStates().getNumberOfSetBits()) {
                    // add the first unset state to the lattice between top and bottom
                    lattice->add(lattice->getAddedStates().getNextUnsetIndex(0));
                }
                oldNumberSet = lattice->getAddedStates().getNumberOfSetBits();
            }
            return std::make_tuple(lattice, numberOfStates, numberOfStates);
        }

        template class LatticeExtender<storm::RationalFunction>;
    }
}
