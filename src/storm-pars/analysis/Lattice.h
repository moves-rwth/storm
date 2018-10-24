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
                        storm::storage::BitVector statesAbove;
                        storm::storage::BitVector statesBelow;
                        storm::storage::BitVector recentlyAddedAbove;
                        storm::storage::BitVector recentlyAddedBelow;
                        std::set<Lattice::Node*> above;
                        std::set<Lattice::Node*> below;
                    };

                    /*!
                     * Constructs a lattice with the given top node and bottom node.
                     *
                     * @param topNode The top node of the resulting lattice.
                     * @param bottomNode The bottom node of the resulting lattice.
                     */
                    Lattice(storm::storage::BitVector topStates,
                            storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates);

                    Lattice(Lattice* lattice);

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
                   * Adds a new relation between two nodes to the lattice
                   * @param above The node closest to the top Node of the Lattice.
                   * @param below The node closest to the bottom Node of the Lattice.
                   */
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
                     *
                     * @param state The number of the state.
                     *
                     * @return The pointer to the node of the state, nullptr if the node does not exist.
                     */
                    Node *getNode(uint_fast64_t state);

                    /*!
                     * Retrieves the top node of the lattice.
                     *
                     * @return The top node.
                     */
                    Node* getTop();

                    /*!
                     * Retrieves the bottom node of the lattice.
                     *
                     * @return The bottom node.
                     */
                    Node* getBottom();

                    /*!
                     * Returns the vector with the nodes of the lattice.
                     * Each index in the vector refers to a state.
                     * When the state is not yet added at a node, it will contain the nullptr.
                     *
                     * @return The vector with nodes of the lattice.
                     */
                    std::vector<Node*> getNodes();

                    std::set<Node*> getNodesBelow(Node* node);
                    std::set<Node*> getNodesAbove(Node* node);

                    /*!
                     * Returns a BitVector in which all added states are set.
                     *
                     * @return The BitVector with all added states.
                     */
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

                    /*!
                     * Constants for comparison of nodes/states
                     */
                     enum {
                         UNKNOWN = -1,
                         BELOW = 2,
                         ABOVE = 1,
                         SAME = 0,
                     };

                private:
                    std::vector<Node*> nodes;

                    storm::storage::BitVector addedStates;

                    Node* top;

                    Node* bottom;

                    uint_fast64_t numberOfStates;

                    bool above(Node * node1, Node * node2, std::shared_ptr<std::set<Node *>> seenNodes);

                    int compare(Node* node1, Node* node2);

                    void setStatesAbove(Node* node, uint_fast64_t state);

                    void setStatesBelow(Node* node, uint_fast64_t state);

                    void setStatesAbove(storm::analysis::Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized);

                    void setStatesBelow(storm::analysis::Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized);
                };
            }
}
#endif //LATTICE_LATTICE_H
