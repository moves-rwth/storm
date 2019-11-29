#ifndef ORDER_ORDER_H
#define ORDER_ORDER_H

#include <iostream>
#include <set>
#include <vector>
#include <unordered_map>
#include <boost/container/flat_set.hpp>

#include "storm/storage/BitVector.h"

namespace storm {
            namespace analysis {

                class Order {

                public:
                    /*!
                     * Constants for comparison of nodes/states
                     */
                    enum NodeComparison {
                        UNKNOWN,
                        BELOW,
                        ABOVE,
                        SAME,
                    };
                    struct Node {
                        boost::container::flat_set<uint_fast64_t> states;
                        storm::storage::BitVector statesAbove;
                    };

                    /*!
                     * Constructs an order with the given top node and bottom node.
                     *
                     * @param topNode The top node of the resulting order.
                     * @param bottomNode The bottom node of the resulting order.
                     */
                    Order(storm::storage::BitVector* topStates,
                            storm::storage::BitVector* bottomStates,
                            storm::storage::BitVector* initialMiddleStates,
                            uint_fast64_t numberOfStates,
                            std::vector<uint_fast64_t>* statesSorted);

                    /*!
                     * Constructs an order with the given top state and bottom state.
                     *
                     * @param top The top state of the resulting order.
                     * @param bottom The bottom state of the resulting order.
                     * @param numberOfStates Max number of states in order.
                     */
                    Order(uint_fast64_t top,
                            uint_fast64_t bottom,
                            uint_fast64_t numberOfStates,
                            std::vector<uint_fast64_t>* statesSorted);

                    /*!
                     * Constructs a copy of the given order.
                     *
                     * @param order The original order.
                     */
                    Order(Order* order);

                    /*!
                     * Adds a node with the given state below node1 and above node2.
                     * @param state The given state.
                     * @param node1 The pointer to the node below which a new node (with state) is added
                     * @param node2 The pointer to the node above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, Node *node1, Node *node2);

                    /*!
                     * Adds a node with the given state between the nodes of below and above.
                     * Result: below -> state -> above
                     * @param state The given state.
                     * @param above The state number of the state below which a new node (with state) is added
                     * @param below The state number of the state above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below);

                    /*!
                     * Adds state to the states of the given node.
                     * @param state The state which is added.
                     * @param node The pointer to the node to which state is added, should not be nullptr.
                     */
                    void addToNode(uint_fast64_t state, Node *node);

                    /*!
                     * Adds state between the top and bottom node of the order
                     * @param state The given state
                     */
                    void add(uint_fast64_t state);

                    /*!
                     * Adds a new relation between two nodes to the order
                     * @param above The node closest to the top Node of the Order.
                     * @param below The node closest to the bottom Node of the Order.
                     */
                    void addRelationNodes(storm::analysis::Order::Node *above, storm::analysis::Order::Node * below);

                    /*!
                    * Adds a new relation between two states to the order
                    * @param above The state closest to the top Node of the Order.
                    * @param below The state closest to the bottom Node of the Order.
                    */
                    void addRelation(uint_fast64_t above, uint_fast64_t below);

                    /*!
                     * Compares the level of the nodes of the states.
                     * Behaviour unknown when one or more of the states doesnot occur at any Node in the Order.
                     * @param state1 The first state.
                     * @param state2 The second state.
                     * @return SAME if the nodes are on the same level;
                     * ABOVE if the node of the first state is closer to top then the node of the second state;
                     * BELOW if the node of the second state is closer to top then the node of the first state;
                     * UNKNOWN if it is unclear from the structure of the order how the nodes relate.
                     */
                    Order::NodeComparison compare(uint_fast64_t state1, uint_fast64_t state2);

                    /*!
                     * Check if state is already in order
                     * @param state
                     * @return
                     */
                    bool contains(uint_fast64_t state);

                    /*!
                     * Retrieves the pointer to a Node at which the state occurs.
                     *
                     * @param state The number of the state.
                     *
                     * @return The pointer to the node of the state, nullptr if the node does not exist.
                     */
                    Node *getNode(uint_fast64_t state);

                    /*!
                     * Retrieves the top node of the order.
                     *
                     * @return The top node.
                     */
                    Node* getTop();

                    /*!
                     * Retrieves the bottom node of the order.
                     *
                     * @return The bottom node.
                     */
                    Node* getBottom();

                    /*!
                     * Returns the vector with the nodes of the order.
                     * Each index in the vector refers to a state.
                     * When the state is not yet added at a node, it will contain the nullptr.
                     *
                     * @return The vector with nodes of the order.
                     */
                    std::vector<Node*> getNodes();

                    /*!
                     * Returns a BitVector in which all added states are set.
                     *
                     * @return The BitVector with all added states.
                     */
                    storm::storage::BitVector* getAddedStates();

                    /*!
                     * Returns true if done building the order.
                     * @return
                     */
                    bool getDoneBuilding();

                    /*!
                     * Compares two nodes in the order
                     * @param node1
                     * @param node2
                     * @return BELOW, ABOVE, SAME or UNKNOWN
                     */
                    NodeComparison compare(Node* node1, Node* node2);

                    /*!
                     * Sorts the given stats if possible.
                     *
                     * @param states Bitvector of the states to sort
                     * @return Vector with states sorted, length equals number of states to sort.
                     * If states cannot be sorted, last state of the vector will always equal the length of the BitVector
                     */
                    std::vector<uint_fast64_t> sortStates(storm::storage::BitVector* states);

                    /*!
                     * If the order is fully build, this can be set to true.
                     */
                    void setDoneBuilding(bool done);

                    /*!
                     * Prints a string representation of the order to the output stream.
                     *
                     * @param out The stream to output to.
                     */
                    void toString(std::ostream &out);

                    /*!
                     * Merges node2 into node1
                     * @param node1
                     * @param node2
                     */
                    void mergeNodes(Node* node1, Node* node2);
                    /*!
                     * Merges node of var2 into node of var1
                     * @param var1
                     * @param var2
                     */
                    void merge(uint_fast64_t var1, uint_fast64_t var2);

                    storm::storage::BitVector* statesToHandle;

                    uint_fast64_t getNextSortedState();

                    bool containsStatesSorted(uint_fast64_t state);

                    void removeFirstStatesSorted();

                    void removeStatesSorted(uint_fast64_t state);

                protected:
                    std::vector<uint_fast64_t> getStatesSorted();

                private:
                    std::vector<Node*> nodes;

                    std::vector<uint_fast64_t> statesSorted;

                    storm::storage::BitVector* addedStates;

                    Node* top;

                    Node* bottom;

                    uint_fast64_t numberOfStates;

                    bool above(Node * node1, Node * node2);

                    bool above(Node * node1, Node * node2, storm::analysis::Order::Node *nodePrev, storm::storage::BitVector *statesSeen);

                    bool doneBuilding;
                };
            }
}
#endif //ORDER_ORDER_H
