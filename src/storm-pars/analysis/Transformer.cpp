//
// Created by Jip Spel on 26.07.18.
//

#include "Transformer.h"
namespace storm {
    namespace analysis {
        Lattice *Transformer::toLattice(storm::storage::SparseMatrix<storm::RationalFunction> matrix,
                                        storm::storage::BitVector const &initialStates,
                                        storm::storage::BitVector topStates,
                                        storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates) {
            // Transform the transition matrix into a vector containing the states with the state to which the transition goes.
            std::vector<State*> stateVector = toStateVector(matrix, initialStates);

            // Start creating the Lattice.
            Lattice::Node top = {topStates};
            Lattice::Node bottom = {bottomStates};
            Lattice *lattice = new Lattice(&top, &bottom, numberOfStates);
            storm::storage::BitVector oldStates(numberOfStates);
            // Create a copy of the states already present in the lattice.
            storm::storage::BitVector seenStates = topStates|=bottomStates;

            matrix.printAsMatlabMatrix(std::cout);

            while (oldStates != seenStates) {
                // As long as new states are discovered, continue.
                oldStates = storm::storage::BitVector(seenStates);

                for (auto itr = stateVector.begin(); itr != stateVector.end(); ++itr) {
                    // Iterate over all states
                    State *currentState = *itr;

                    if (!seenStates[currentState->stateNumber]
                        && seenStates[currentState->successor1]
                        && seenStates[currentState->successor2]) {

                        // Check if the current state number has not been added, but its successors have been added.
                        if (currentState->successor1 == currentState->successor2) {
                            // If there is only one successor, the state should be added to the same Node as its successor
                            lattice->addToNode(currentState->stateNumber, lattice->getNode(currentState->successor1));
                            // Add stateNumber to the set with seen states.
                            seenStates.set(currentState->stateNumber);
                        } else {
                            // Otherwise, check how the two states compare, and add if the comparison is possible.
                            uint_fast64_t successor1 = currentState->successor1;
                            uint_fast64_t successor2 = currentState->successor2;
                            int compareResult = lattice->compare(successor1, successor2);
                            if (compareResult == 1) {
                                //TODO dit in een aparte methode doen
                                Lattice::Node *above = lattice->getNode(successor1);
                                Lattice::Node *below = lattice->getNode(successor2);
                                std::vector<Lattice::Node *> states1 = above->below;
                                std::vector<Lattice::Node *> states2 = below->above;
                                bool added = false;
                                for (auto itr1 = states1.begin(); itr1 < states1.end(); ++itr1) {
                                    for (auto itr2 = states2.begin(); itr2 < states2.end(); ++itr2) {
                                        if ((*itr1)->states == (*itr2)->states) {
                                            storm::RationalFunction prob1 = getProbability(currentState->stateNumber, successor1, matrix);
                                            storm::RationalFunction prob2 = getProbability(currentState->stateNumber, successor2, matrix);
                                            if (prob1 != storm::RationalFunction(1)
                                                && prob2 != storm::RationalFunction(1)
                                                && getProbability((*itr1)->states, above->states, matrix) == prob1
                                                && getProbability((*itr1)->states, below->states, matrix) == prob2) {

                                                std::cout << "Van: " << currentState-> stateNumber << " naar: " << successor1 << std::endl;
                                                std::cout << prob1 << std::endl;
                                                std::cout << "Van: " << currentState-> stateNumber << " naar: " << successor2 << std::endl;
                                                std::cout << prob2 << std::endl;

                                                lattice->addToNode(currentState->stateNumber, (*itr1));
                                                seenStates.set(currentState->stateNumber);
                                                added = true;
                                            }
                                        }

                                    }
                                }

                                if (!added) {
                                    // successor 1 is closer to top than successor 2
                                    lattice->addBetween(currentState->stateNumber, lattice->getNode(successor1),
                                                        lattice->getNode(successor2));
                                    // Add stateNumber to the set with seen states.
                                    seenStates.set(currentState->stateNumber);
                                }
                            } else if (compareResult == 2) {
                                //TODO dit in een aparte methode doen
                                // als er in de below van successor 2 en de above van succesor 1 een overlap is met een state, dan moet je kijken naar de kans
                                Lattice::Node *above = lattice->getNode(successor2);
                                Lattice::Node *below = lattice->getNode(successor1);
                                std::vector<Lattice::Node *> states1 = above->below;
                                std::vector<Lattice::Node *> states2 = below->above;
                                bool added = false;
                                for (auto itr1 = states1.begin(); itr1 < states1.end(); ++itr1) {
                                    for (auto itr2 = states2.begin(); itr2 < states2.end(); ++itr2) {
                                        if ((*itr1)->states == (*itr2)->states) {
                                            storm::RationalFunction prob1 = getProbability(currentState->stateNumber, successor2, matrix);
                                            storm::RationalFunction prob2 = getProbability(currentState->stateNumber, successor1, matrix);
                                            if (prob1 != storm::RationalFunction(1)
                                                && prob2 != storm::RationalFunction(1)
                                                && getProbability((*itr1)->states, above->states, matrix) == prob1
                                                && getProbability((*itr1)->states, below->states, matrix) == prob2) {

                                                std::cout << "Van: " << currentState-> stateNumber << " naar: " << successor2 << std::endl;
                                                std::cout << prob1 << std::endl;
                                                std::cout << "Van: " << currentState-> stateNumber << " naar: " << successor1 << std::endl;
                                                std::cout << prob2 << std::endl;

                                                lattice->addToNode(currentState->stateNumber, (*itr1));
                                                seenStates.set(currentState->stateNumber);
                                                added = true;
                                            }
                                        }
                                    }
                                }

                                if (!added) {
                                    // successor 2 is closer to top than successor 1
                                    lattice->addBetween(currentState->stateNumber, lattice->getNode(successor2),
                                                        lattice->getNode(successor1));
                                    // Add stateNumber to the set with seen states.
                                    seenStates.set(currentState->stateNumber);
                                }




                            } else if (compareResult == 0) {
                                // the successors are at the same level
                                lattice->addToNode(currentState->stateNumber, lattice->getNode(successor1));
                                // Add stateNumber to the set with seen states.
                                seenStates.set(currentState->stateNumber);
                            } else {
                                // TODO: what to do?
                                STORM_LOG_DEBUG("Failed to add" << currentState->stateNumber << "\n");
                            }

                        }
                    }
                }

            }

            return lattice;
        }

        std::vector<Transformer::State *>
        Transformer::toStateVector(storm::storage::SparseMatrix<storm::RationalFunction> transitionMatrix,
                                   storm::storage::BitVector const &initialStates) {
            std::vector < State *> states = std::vector<State *>({});
            std::vector <uint_fast64_t> stack(initialStates.begin(), initialStates.end());
            std::vector <uint_fast64_t> seenStates(initialStates.begin(), initialStates.end());
            uint_fast64_t currentState;

            while (!stack.empty()) {
                currentState = stack.back();
                stack.pop_back();
                std::vector <uint_fast64_t> successorStates(0, 2);

                // Assume there are at most 2 successors
                for (auto const &successor : transitionMatrix.getRowGroup(currentState)) {
                    if (!storm::utility::isZero(successor.getValue())) {
                        // Only explore the state if the transition was actually there.
                        uint_fast64_t successorNumber = successor.getColumn();
                        if (std::find(seenStates.begin(), seenStates.end(), successorNumber) == seenStates.end()) {
                            stack.push_back(successorNumber);
                            seenStates.push_back(successorNumber);
                        }
                        successorStates.push_back(successorNumber);
                    }
                }

                State *state = new State();
                state->stateNumber = currentState;
                state->successor1 = successorStates.back();
                successorStates.pop_back();
                if (!successorStates.empty()) {
                    state->successor2 = successorStates.back();
                    successorStates.pop_back();
                } else {
                    state->successor2 = state->successor1;
                }
                states.push_back(state);
            }
            return states;
        }

        void Transformer::print(storm::storage::BitVector vector, std::string message) {
            uint_fast64_t index = vector.getNextSetIndex(0);
            std::cout << message <<": {";
            while (index < vector.size()) {
                std::cout << index;
                index = vector.getNextSetIndex(index+1);
                if (index < vector.size()) {
                    std::cout << ", ";
                }
            }
            std::cout << "}" << std::endl;
        }

        std::vector<uint_fast64_t> Transformer::getNumbers(storm::storage::BitVector vector) {
            std::vector<uint_fast64_t> result = std::vector<uint_fast64_t>({});
            uint_fast64_t index = vector.getNextSetIndex(0);
            while (index < vector.size()) {
                result.push_back(index);
                index = vector.getNextSetIndex(index+1);

            }
            return result;
        }

        storm::RationalFunction Transformer::getProbability(storm::storage::BitVector state, storm::storage::BitVector successor, storm::storage::SparseMatrix<storm::RationalFunction>  matrix) {
            std::vector<uint_fast64_t> successorNumbers = getNumbers(successor);
            storm::RationalFunction result = storm::RationalFunction(1);
            for (auto itr = successorNumbers.begin(); itr < successorNumbers.end() && result == storm::RationalFunction(1); ++itr) {
                result = getProbability(state, (*itr), matrix);
            }
            return result;
        }

        storm::RationalFunction Transformer::getProbability(storm::storage::BitVector state, uint_fast64_t successor, storm::storage::SparseMatrix<storm::RationalFunction>  matrix) {
            std::vector<uint_fast64_t> stateNumbers = getNumbers(state);
            storm::RationalFunction result = storm::RationalFunction(1);
            for (auto itr = stateNumbers.begin(); itr < stateNumbers.end() && result == storm::RationalFunction(1); ++itr) {
                result = getProbability((*itr), successor, matrix);
            }
            return result;
        }

        storm::RationalFunction Transformer::getProbability(uint_fast64_t state, uint_fast64_t successor, storm::storage::SparseMatrix<storm::RationalFunction>  matrix) {
            storm::RationalFunction result = storm::RationalFunction(1);
            // Iterate over all row groups.
            auto row = matrix.getRow(state);

            for (auto itr = row.begin(); itr < row.end() && result == storm::RationalFunction(1); ++itr) {
                if ((*itr).getColumn() == successor) {
                    std::cout << "Tralala" << std::endl;
                    // TODO: nog checken dat ie wel met state te doen heeft
                    result = (*itr).getValue();
                    std::cout << "Van: " << state << " naar: " << successor << std::endl;
                    std::cout << result << std::endl;
                }

            }

            return result;
        }
    }
}
