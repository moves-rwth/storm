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

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

#include <set>

namespace storm {
    namespace analysis {

        template <typename ValueType>
        LatticeExtender<ValueType>::LatticeExtender(){//std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> model) {
            //TODO
        }

//        template <typename ValueType>
//        storm::analysis::Lattice* LatticeExtender<ValueType>::toLattice(std::vector<std::shared_ptr<storm::logic::Formula const>> formulas) {
//            STORM_LOG_THROW((++formulas.begin()) == formulas.end(), storm::exceptions::NotSupportedException, "Only one formula allowed for monotonicity analysis");
//            STORM_LOG_THROW((*(formulas[0])).isProbabilityOperatorFormula()
//                            && ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()
//                                || (*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()), storm::exceptions::NotSupportedException, "Expecting until formula");
//
//            uint_fast64_t numberOfStates = this->model.getNumberOfStates();
//
//            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<ValueType>> propositionalChecker(this->model);
//            storm::storage::BitVector phiStates;
//            storm::storage::BitVector psiStates;
//            if ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
//                phiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//                psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//            } else {
//                phiStates = storm::storage::BitVector(numberOfStates, true);
//                psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
//            }
//
//            // Get the maybeStates
//            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->model.getBackwardTransitions(), phiStates, psiStates);
//            storm::storage::BitVector topStates = statesWithProbability01.second;
//            storm::storage::BitVector bottomStates = statesWithProbability01.first;
//
//            STORM_LOG_THROW(topStates.begin() != topStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no 1 states");
//            STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no zero states");
//
//            // Transform to Lattice
//            auto matrix = this->model.getTransitionMatrix();
//
//            for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
//                stateMap[i] = storm::storage::BitVector(numberOfStates, false);
//
//                auto row = matrix.getRow(i);
//                for (auto rowItr = row.begin(); rowItr != row.end(); ++rowItr) {
//                    stateMap[i].set(rowItr->getColumn(), true);
//                }
//                // TODO: allow more than 2 transitions? or fix this in preprocessing?
//                STORM_LOG_THROW(stateMap[i].getNumberOfSetBits() <= 2, storm::exceptions::NotSupportedException, "Only two outgoing transitions per state allowed");
//            }
//
//            // Create the Lattice
//            storm::analysis::Lattice *lattice = new storm::analysis::Lattice(topStates, bottomStates, numberOfStates);
//            return this->extendLattice(lattice, std::set<storm::expressions::Expression>({}));
//        }
//
//        template <typename ValueType>
//        storm::analysis::Lattice* LatticeExtender<ValueType>::extendLattice(storm::analysis::Lattice* lattice, std::set<storm::expressions::Expression> assumptions) {
//            uint_fast64_t numberOfStates = this->model.getNumberOfStates();
//
//            storm::storage::BitVector oldStates(numberOfStates);
//
//            // Create a copy of the states already present in the lattice.
//            storm::storage::BitVector seenStates = lattice->getAddedStates();
//
//            while (oldStates != seenStates) {
//                // As long as new states are added to the lattice, continue.
//                oldStates = storm::storage::BitVector(seenStates);
//
//                for (auto stateItr = stateMap.begin(); stateItr != stateMap.end(); ++stateItr) {
//                    // Iterate over all states
//                    auto stateNumber = stateItr->first;
//                    storm::storage::BitVector successors = stateItr->second;
//
//                    // Check if current state has not been added yet, and all successors have
//                    bool check = !seenStates[stateNumber];
//                    for (auto succIndex = successors.getNextSetIndex(0); check && succIndex != numberOfStates; succIndex = successors.getNextSetIndex(++succIndex)) {
//                        check &= seenStates[succIndex];
//                    }
//
//                    if (check && successors.getNumberOfSetBits() == 1) {
//                        // As there is only one successor the current state and its successor must be at the same nodes.
//                        lattice->addToNode(stateNumber, lattice->getNode(successors.getNextSetIndex(0)));
//                        seenStates.set(stateNumber);
//                    } else if (check && successors.getNumberOfSetBits() > 1) {
//                        // TODO: allow more than 2 transitions?
//
//                        // Otherwise, check how the two states compare, and add if the comparison is possible.
//                        uint_fast64_t successor1 = successors.getNextSetIndex(0);
//                        uint_fast64_t successor2 = successors.getNextSetIndex(successor1 + 1);
//                        int compareResult = lattice->compare(successor1, successor2);
//                        if (compareResult == 1) {
//                            // successor 1 is closer to top than successor 2
//                            lattice->addBetween(stateNumber, lattice->getNode(successor1),
//                                                lattice->getNode(successor2));
//                        } else if (compareResult == 2) {
//                            // successor 2 is closer to top than successor 1
//                            lattice->addBetween(stateNumber, lattice->getNode(successor2),
//                                                lattice->getNode(successor1));
//                        } else if (compareResult == 0) {
//                            // the successors are at the same level
//                            lattice->addToNode(stateNumber, lattice->getNode(successor1));
//                        } else {
//                            // TODO: create critical pair
//                        }
//                        seenStates.set(stateNumber);
//                    }
//                }
//            }
//            return lattice;
//        }
    }
}