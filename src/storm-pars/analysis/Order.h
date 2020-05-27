#ifndef ORDER_ORDER_H
#define ORDER_ORDER_H

#include <boost/container/flat_set.hpp>
#include <iostream>
#include <vector>

#include "storm/storage/BitVector.h"

namespace storm {
            namespace analysis {
                // TODO: @Svenja change pointers to Order and Node* to shared_ptr

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
                     * Constructs an order with the given top state and bottom state.
                     *
                     * @param topStates a Bitvector with the top states of the resulting order
                     * @param bottomStates a Bitvector with the bottom states of the resulting order
                     * @param numberOfStates maximum number of states in order
                     * @param statesSorted pointer to a vector which contains the states which still need to added to the order
                     */
                    Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted);

                    /*!
                     * Constructs an order with the given top state and bottom state.
                     *
                     * @param top the top state of the resulting order
                     * @param bottom the bottom state of the resulting order
                     * @param numberOfStates maximum number of states in order
                     * @param statesSorted pointer to a vector which contains the states which still need to added to the order
                     */
                    Order(uint_fast64_t top, uint_fast64_t bottom, uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted);

                    /*!
                     * Constructs a copy of the given order.
                     *
                     * @param order the original order
                     */
                    Order(Order* order);

                    /*!
                     * Adds state between the top and bottom node of the order
                     * @param state the given state
                     */
                    void add(uint_fast64_t state);

                    /*!
                    * Adds a node with the given state above the given node.
                     *
                    * @param state the state which is added
                    * @param node the pointer to the node above which the state is added, should not be nullptr
                    */
                    void addAbove(uint_fast64_t state, Node *node);

                    /*!
                    * Adds a node with the given state below the given node.
                     *
                    * @param state the state which is added
                    * @param node the pointer to the node below which the state is added, should not be nullptr
                    */
                    void addBelow(uint_fast64_t state, Node *node);

                    /*!
                     * Adds a node with the given state below node1 and above node2.
                     *
                     * @param state the given state
                     * @param node1 the pointer to the node below which a new node (with state) is added
                     * @param node2 the pointer to the node above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, Node *node1, Node *node2);

                    /*!
                     * Adds a node with the given state between the nodes of below and above.
                     * Result: below -> state -> above
                     *
                     * @param state the given state
                     * @param above the state number of the state below which a new node (with state) is added
                     * @param below the state number of the state above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below);

                    /*!
                     * Adds a new relation between two nodes to the order
                     *
                     * @param above the node closest to the top Node of the Order
                     * @param below the node closest to the bottom Node of the Order
                     */
                    void addRelationNodes(storm::analysis::Order::Node *above, storm::analysis::Order::Node * below);

                    /*!
                    * Adds a new relation between two states to the order
                     *
                    * @param above the state closest to the top Node of the Order
                    * @param below the state closest to the bottom Node of the Order
                    */
                    void addRelation(uint_fast64_t above, uint_fast64_t below);

                    /*!
                     * Adds state to the states of the given node.
                     *
                     * @param state the state which is added
                     * @param node the pointer to the node to which state is added, should not be nullptr
                     */
                    void addToNode(uint_fast64_t state, Node *node);

                    /*!
                     * Merges node2 into node1.
                     */
                    void mergeNodes(Node* node1, Node* node2);

                    /*!
                     * Merges node of var2 into node of var1.
                     */
                    void merge(uint_fast64_t var1, uint_fast64_t var2);

                    /*!
                     * Compares the level of the nodes of the states.
                     * Behaviour unknown when one or more of the states does not occur at any Node in the Order.
                     *
                     * @param state1 the first state
                     * @param state2 the second state
                     * @return SAME if the nodes are on the same level;
                     *         ABOVE if the node of the first state is closer to top than the node of the second state;
                     *         BELOW if the node of the second state is closer to top than the node of the first state;
                     *         UNKNOWN if it is unclear from the structure of the order how the nodes relate
                     */
                    Order::NodeComparison compare(uint_fast64_t state1, uint_fast64_t state2);

                    /*!
                     * Compares the level of the two nodes.
                     *
                     * @param node1 the first node
                     * @param node2 the second node
                     * @return SAME if the nodes are on the same level;
                     *         ABOVE if node1 is closer to top than node2;
                     *         BELOW if node2 is closer to top than node1;
                     *         UNKNOWN if it is unclear from the structure of the order how the nodes relate
                     */
                    NodeComparison compare(Node* node1, Node* node2);

                    /*!
                     * Check if state is already contained in order
                     */
                    bool contains(uint_fast64_t state);

                    /*!
                     * Returns a BitVector in which all added states are set.
                     *
                     * @return the BitVector with all added states
                     */
                    storm::storage::BitVector* getAddedStates();

                    /*!
                     * Retrieves the bottom node of the order.
                     *
                     * @return the bottom node
                     */
                    Node* getBottom();

                    /*!
                     * Returns true if done building the order.
                     */
                    bool getDoneBuilding();

                    /*!
                     * Returns the next added state of the order, returns the number of state if end of added states is reached.
                     */
                    uint_fast64_t getNextAddedState(uint_fast64_t state);

                    /*!
                     * Retrieves the pointer to a Node at which the state occurs.
                     *
                     * @param state The number of the state.
                     * @return The pointer to the node of the state, nullptr if the node does not exist.
                     */
                    Node *getNode(uint_fast64_t state);

                    /*!
                     * Returns the vector with the nodes of the order.
                     * Each index in the vector refers to a state.
                     * When the state is not yet added at a node, it will contain the nullptr.
                     *
                     * @return the vector with nodes of the order
                     */
                    std::vector<Node*> getNodes();

                    /*!
                     * Retrieves the top node of the order.
                     *
                     * @return the top node
                     */
                    Node* getTop();

                    /*!
                     * Returns the number of added states.
                     */
                    uint_fast64_t getNumberOfAddedStates();

                    /*!
                     * Returns the number of possible states in the order.
                     */
                    uint_fast64_t getNumberOfStates();

                    /*!
                     * Returns if the order only consists of bottom and top states (so no in-between nodes).
                     */
                    bool isOnlyBottomTopOrder();

                    /*!
                     * Sorts the given states if possible.
                     *
                     * @param states vector of the states to sort
                     * @return vector with states sorted, length equals number of states to sort.
                     * If states cannot be sorted, last state of the vector will always equal the length of the BitVector
                     */
                    std::vector<uint_fast64_t> sortStates(std::vector<uint_fast64_t>* states);

                    /*!
                     * Sorts the given states if possible.
                     *
                     * @param states Bitvector of the states to sort
                     * @return vector with states sorted, length equals number of states to sort.
                     * If states cannot be sorted, last state of the vector will always equal the length of the BitVector
                     */
                    std::vector<uint_fast64_t> sortStates(storm::storage::BitVector* states);

                    /*!
                     * Adds a state to the list of states which should be handled,
                     * before continuing with the normally sorted states.
                     *
                     * @param state number of the state to handle first
                     */
                    void addStateToHandle(uint_fast64_t state);

                    /*!
                     * Returns the next state to handle, if the statesToHandle list is empty, it continues with the sorted states list
                     * @return
                     */
                    uint_fast64_t getNextSortedState();


                    /*!
                     * If the order is fully build, this can be set to true.
                     */
                    void setDoneBuilding(bool done = true);


                    /*!
                     * Prints to normal STORM_PRINT the dot output.
                     */
                    void toDotOutput();

                    /*!
                     * Writes dotoutput to the file.
                     * @param dotOutfile
                     */
                    void dotOutputToFile(std::ofstream& dotOutfile);

                protected:
                    std::vector<uint_fast64_t> getStatesSorted();

                    std::vector<uint_fast64_t> getStatesToHandle();

                private:
                    bool above(Node * node1, Node * node2);

                    bool above(Node * node1, Node * node2, storm::analysis::Order::Node *nodePrev, storm::storage::BitVector *statesSeen);

                    bool aboveFast(Node * node1, Node * node2);

                    void init(uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted, bool doneBuilding = false);

                    std::string nodeName(Node n);

                    std::string nodeLabel(Node n);

                    void nodeOutput();

                    bool doneBuilding;

                    bool onlyBottomTopOrder;

                    storm::storage::BitVector* addedStates;

                    std::vector<Node*> nodes;

                    std::vector<uint_fast64_t> statesSorted;

                    std::vector<uint_fast64_t> statesToHandle;

                    Node* top;

                    Node* bottom;

                    uint_fast64_t numberOfStates;

                    uint_fast64_t numberOfAddedStates;
                };
            }
}
#endif //ORDER_ORDER_H
