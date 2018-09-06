//
// Created by Jip Spel on 24.07.18.
//

#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include <iostream>
#include <set>
#include <vector>

#include "storm/storage/BitVector.h"

namespace storm {
            namespace analysis {
                class Lattice {

                public:
                    struct Node {
                        storm::storage::BitVector states;
                        std::set<Node *> above;
                        std::set<Node *> below;
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
                     * Adds a new relation to the lattice
                     * @param above The node closest to the top Node of the Lattice.
                     * @param between The node between above and below.
                     * @param below The node closest to the bottom Node of the Lattice.
                     */
                    void addRelation(Node* above, Node* between, Node* below);

                    void addRelationNodes(storm::analysis::Lattice::Node *above, storm::analysis::Lattice::Node * below);

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

                    Node* getTop();

                    Node* getBottom();

                    storm::storage::BitVector getAddedStates();

                    /*!
                     * Prints a string representation of the lattice to the output stream.
                     *
                     * @param out The stream to output to.
                     */
                    void toString(std::ostream &out);

                    /*!
                     * Prints a dot representation of the lattice to the output stream.
                     *
                     * @param out The stream to output to.
                     */
                    void toDotFile(std::ostream &out);

                    Lattice* deepCopy();

                    static const int UNKNOWN = -1;
                    static const int BELOW = 2;
                    static const int ABOVE = 1;
                    static const int SAME = 0;

                protected:
                    void addBelow(uint_fast64_t state, Node* node);

                    void addAbove(uint_fast64_t state, Node* node);

                private:
                    std::vector<Node*> nodes;

                    storm::storage::BitVector addedStates;

                    Node* top;

                    Node* bottom;

                    uint_fast64_t numberOfStates;

                    /**
                     * Check if node1 lies above node2
                     * @param node1
                     * @param node2
                     * @param seenNodes
                     * @return
                     */
                    bool above(Node * node1, Node * node2, std::set<Node*>* seenNodes);
                };
            }
}
#endif //LATTICE_LATTICE_H
