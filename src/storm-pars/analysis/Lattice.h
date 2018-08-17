//
// Created by Jip Spel on 24.07.18.
//

#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include <iostream>
#include <vector>

#include "storm/models/sparse/Model.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"


namespace storm {
            namespace analysis {
                class Lattice {

                public:
                    struct Node {
                        storm::storage::BitVector states;
                        std::vector<Node *> above;
                        std::vector<Node *> below;
                    };

                    /*!
                     * Constructs a lattice with the given top node and bottom node.
                     *
                     * @param topNode The top node of the resulting lattice.
                     * @param bottomNode The bottom node of the resulting lattice.
                     */
                    Lattice(storm::storage::BitVector topStates,
                            storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates);

                    /*!
                     * Adds a node with the given state below node1 and above node2.
                     * @param state The given state.
                     * @param node1 The pointer to the node below which a new node (with state) is added
                     * @param node2 The pointer to the node above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, Node *node1, Node *node2);

                    /*!
                     * Adds state to the states of the given node.
                     * @param state The state which is added.
                     * @param node The pointer to the node to which state is added, should not be nullptr.
                     */
                    void addToNode(uint_fast64_t state, Node *node);

                    /*!
                     * Adds state between the top and bottom node of the lattice
                     * @param state The given state
                     */
                    void add(uint_fast64_t state);

                    /*!
                     * Compares the level of the nodes of the states.
                     * Behaviour unknown when one or more of the states doesnot occur at any Node in the Lattice.
                     * @param state1 The first state.
                     * @param state2 The second state.
                     * @return 0 if the nodes are on the same level;
                     * 1 if the node of the first state is closer to top then the node of the second state;
                     * 2 if the node of the second state is closer to top then the node of the first state;
                     * -1 if it is unclear from the structure of the lattice how the nodes relate.
                     */
                    int compare(uint_fast64_t state1, uint_fast64_t state2);

                    /*!
                     * Retrieves the pointer to a Node at which the state occurs.
                     * Behaviour unknown when state does not exists at any Node in the Lattice.
                     *
                     * @param state The number of the state.
                     *
                     * @return The pointer to the node of the state, nullptr if the node does not exist
                     */
                    Node *getNode(uint_fast64_t state);

                    /*!
                     * Prints a string representation of the lattice to std::cout.
                     *
                     * @param out The stream to output to.
                     */
                    void toString(std::ostream &out);

                    /*!
                     * Creates a Lattice based on the transition matrix, topStates of the Lattice and bottomStates of the Lattice
                     * @tparam ValueType Type of the probabilities
                     * @param matrix The transition matrix.
                     * @param topStates Set of topStates of the Lattice.
                     * @param bottomStates Set of bottomStates of the Lattice.
                     * @return pointer to the created Lattice.
                     */
                    template <typename ValueType>
                    static Lattice* toLattice(storm::storage::SparseMatrix<ValueType> matrix,
                                                 storm::storage::BitVector topStates,
                                                 storm::storage::BitVector bottomStates) {
                        uint_fast64_t numberOfStates = matrix.getColumnCount();

                        // Transform the transition matrix into a vector containing the states with the state to which the transition goes.
                        std::vector <State*> stateVector = std::vector<State *>({});
                        for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
                            State* state = new State();
                            state->stateNumber = i;

                            auto row = matrix.getRow(i);
                            //TODO assert that there are at most two successors
                            if ((*(row.begin())).getValue() != ValueType(1)) {
                                state->successor1 = (*(row.begin())).getColumn();
                                state->successor2 = (*(++row.begin())).getColumn();
                            } else {
                                state-> successor1 = (*(row.begin())).getColumn();
                                state-> successor2 = (*(row.begin())).getColumn();
                            }

                            stateVector.push_back(state);
                        }
                        // Start creating the Lattice
                        storm::analysis::Lattice *lattice = new storm::analysis::Lattice(topStates, bottomStates, numberOfStates);
                        storm::storage::BitVector oldStates(numberOfStates);
                        // Create a copy of the states already present in the lattice.
                        storm::storage::BitVector seenStates = topStates|= bottomStates;

                        while (oldStates != seenStates) {
                            // As long as new states are added to the lattice, continue.
                            oldStates = storm::storage::BitVector(seenStates);

                            for (auto itr = stateVector.begin(); itr != stateVector.end(); ++itr) {
                                // Iterate over all states
                                State *currentState = *itr;

                                if (!seenStates[currentState->stateNumber]
                                    && seenStates[currentState->successor1]
                                    && seenStates[currentState->successor2]) {


                                    // Otherwise, check how the two states compare, and add if the comparison is possible.
                                    uint_fast64_t successor1 = currentState->successor1;
                                    uint_fast64_t successor2 = currentState->successor2;
                                    int compareResult = lattice->compare(successor1, successor2);
                                    if (compareResult == 1) {
                                        // successor 1 is closer to top than successor 2
                                        lattice->addBetween(currentState->stateNumber, lattice->getNode(successor1),
                                                            lattice->getNode(successor2));
                                        // Add stateNumber to the set with seen states.
                                        seenStates.set(currentState->stateNumber);
                                    } else if (compareResult == 2) {
                                        // successor 2 is closer to top than successor 1
                                        lattice->addBetween(currentState->stateNumber, lattice->getNode(successor2),
                                                            lattice->getNode(successor1));
                                        // Add stateNumber to the set with seen states.
                                        seenStates.set(currentState->stateNumber);
                                    } else if (compareResult == 0) {
                                        // the successors are at the same level
                                        lattice->addToNode(currentState->stateNumber, lattice->getNode(successor1));
                                        // Add stateNumber to the set with seen states.
                                        seenStates.set(currentState->stateNumber);
                                    } else {
                                        // TODO: is this what we want?
                                        lattice->add(currentState->stateNumber);
                                    }

                                }
                            }
                        }

                        return lattice;
                    }

                private:
                    struct State {
                        uint_fast64_t stateNumber;
                        uint_fast64_t successor1;
                        uint_fast64_t successor2;
                    };

                    std::vector<Node *> nodes;

                    Node * top;

                    Node * bottom;

                    uint_fast64_t numberOfStates;

                    bool above(Node *, Node *);

                    void remove(std::vector<Node *> *nodes, Node *node);

                    void setStates(std::vector<uint_fast64_t>  states, Node *node);

                };
            }
}
#endif //LATTICE_LATTICE_H
