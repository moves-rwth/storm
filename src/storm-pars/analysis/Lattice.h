//
// Created by Jip Spel on 24.07.18.
//

#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include <iostream>
#include <vector>

#include "storm/storage/BitVector.h"

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
                     * @param node1 The pointer to the node below which a new node is added.
                     * @param node2 The pointer to the node above which a new node is added.
                     */
                    void addBetween(uint_fast64_t state, Node *node1, Node *node2);

                    /*!
                     * Adds state to the states of the given node.
                     * @param state The state which is added.
                     * @param node The pointer to the node to which state is added.
                     */
                    void addToNode(uint_fast64_t state, Node *node);

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

//                    std::vector<uint_fast64_t> addedStates;

                private:
                    std::vector<Node *> nodes;

                    uint_fast64_t numberOfStates;

                    bool above(Node *, Node *);

                    void remove(std::vector<Node *> *nodes, Node *node);

                    void setStates(std::vector<uint_fast64_t>  states, Node *node);

                };
            }
}
#endif //LATTICE_LATTICE_H
