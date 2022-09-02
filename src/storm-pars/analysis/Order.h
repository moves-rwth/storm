#ifndef ORDER_ORDER_H
#define ORDER_ORDER_H

#include <boost/container/flat_set.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "storm/storage/BitVector.h"
#include "storm/storage/StronglyConnectedComponent.h"

namespace storm {
namespace analysis {
class Order {
   public:
    /*!
     * Constants for comparison of nodes/states.
     */
    enum NodeComparison {
        UNKNOWN,
        BELOW,
        ABOVE,
        SAME,
    };

    /*!
     * Nodes of the Order. Contains all states in the same equivalence class.
     */
    struct Node {
        boost::container::flat_set<uint_fast64_t> states;
        storm::storage::BitVector statesAbove;
    };

    /*!
     * Constructs an order with the given top and bottom states.
     *
     * @param topStates A Bitvector with the top states of the resulting order.
     * @param bottomStates A Bitvector with the bottom states of the resulting order.
     * @param numberOfStates Maximum number of states in order.
     * @param sccsSorted TODO ??
     * @param statesSorted Pointer to a vector which contains the states which still need to added to the order.
     */
    Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates,
          storage::Decomposition<storage::StronglyConnectedComponent> sccsSorted, std::vector<uint_fast64_t> statesSorted);

    /*!
     * Constructs an order with the given top state and bottom state.
     *
     * @param top The top state of the resulting order.
     * @param bottom The bottom state of the resulting order.
     * @param numberOfStates Maximum number of states in order.
     * @param sccsSorted TODO ??
     * @param statesSorted Pointer to a vector which contains the states which still need to added to the order.
     */
    Order(uint_fast64_t top, uint_fast64_t bottom, uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> sccsSorted,
          std::vector<uint_fast64_t> statesSorted);

    /*!
     * Constructs a new Order.
     */
    Order();

    /*!
     * Adds state between the top and bottom node of the order.
     *
     * @param state The given state.
     */
    void add(uint_fast64_t state);

    /*!
     * Adds a node with the given state above the given node.
     *
     * @param state The state which is added.
     * @param node The pointer to the node above which the state is added, should not be nullptr.
     */
    void addAbove(uint_fast64_t state, Node* node);

    /*!
     * Adds a node with the given state below the given node.
     *
     * @param state The state which is added.
     * @param node The pointer to the node below which the state is added, should not be nullptr.
     */
    void addBelow(uint_fast64_t state, Node* node);

    /*!
     * Adds a node with the given state below node1 and above node2.
     *
     * @param state The given state.
     * @param node1 The pointer to the node below which a new node (with state) is added.
     * @param node2 The pointer to the node above which a new node (with state) is added.
     */
    void addBetween(uint_fast64_t state, Node* node1, Node* node2);

    /*!
     * Adds a node with the given state between the nodes of below and above.
     * Result: below -> state -> above
     *
     * @param state The given state.
     * @param above The state number of the state below which a new node (with state) is added.
     * @param below The state number of the state above which a new node (with state) is added.
     */
    void addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below);

    /*!
     * Adds a new relation between two nodes to the order.
     *
     * @param above The node closest to the top Node of the Order.
     * @param below The node closest to the bottom Node of the Order.
     */
    void addRelationNodes(storm::analysis::Order::Node* above, storm::analysis::Order::Node* below);

    /*!
     * Adds a new relation between two states to the order.
     *
     * @param above The state closest to the top Node of the Order.
     * @param below The state closest to the bottom Node of the Order.
     * @param allowMerge Whether merging of nodes is allowed or not (should only happen when using assumptions)
     */
    void addRelation(uint_fast64_t above, uint_fast64_t below, bool allowMerge = false);

    /*!
     * Adds state to the states of the given node.
     *
     * @param state The state which is added.
     * @param node The pointer to the node to which state is added, should not be nullptr.
     */
    void addToNode(uint_fast64_t state, Node* node);

    /*!
     * Merges node2 into node1.
     *
     * @param node1 The first node
     * @param node2 The second node
     */
    void mergeNodes(Node* node1, Node* node2);

    /*!
     * Merges node of var2 into node of var1.
     * @param var1 The first state
     * @param var2 The second state
     */
    void merge(uint_fast64_t state1, uint_fast64_t state2);

    /*!
     * Compares the level of the nodes of the states.
     * Behaviour unknown when one or more of the states does not occur at any Node in the Order.
     *
     * @param state1 The first state.
     * @param state2 The second state.
     * @param hypothesis An optional hypothesis on the relation between the states
     * @return SAME if the nodes are on the same level;
     *         ABOVE if the node of the first state is closer to top than the node of the second state;
     *         BELOW if the node of the second state is closer to top than the node of the first state;
     *         UNKNOWN if it is unclear from the structure of the order how the nodes relate.
     */
    Order::NodeComparison compare(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis = UNKNOWN);

    /*!
     * Compares the level of the nodes of two states but only considers direct states above the nodes
     *
     * @param state1 The first state
     * @param state2 The second state
     * @param hypothesis An optional hypothesis on the relation between the states
     * @return The NodeComparison describing the relation between the nodes that could be determined via the direct states above
     */
    Order::NodeComparison compareFast(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis = UNKNOWN) const;

    /*!
     * Compares the level of the two nodes.
     *
     * @param node1 The first node.
     * @param node2 The second node.
     * @param hypothesis An optional hypothesis on the relation between the nodes
     * @return SAME if the nodes are on the same level;
     *         ABOVE if node1 is closer to top than node2;
     *         BELOW if node2 is closer to top than node1;
     *         UNKNOWN if it is unclear from the structure of the order how the nodes relate.
     */
    NodeComparison compare(Node* node1, Node* node2, NodeComparison hypothesis = UNKNOWN);

    /*!
     * Compares the level of two nodes but only considers direct states above the nodes
     * @param node1 The first node
     * @param node2 The second node
     * @param hypothesis An optional hypothesis on the relation between the nodes
     * @return The NodeComparison describing the relation between the nodes that could be determined via the direct states above
     */
    NodeComparison compareFast(Node* node1, Node* node2, NodeComparison hypothesis = UNKNOWN) const;

    /*!
     * Check if state is already contained in order.
     *
     * @param state The state
     * @return true iff the state is in the order.
     */
    bool contains(uint_fast64_t state) const;

    /*!
     * Retrieves the bottom node of the order.
     *
     * @return The bottom node.
     */
    Node* getBottom() const;

    /*!
     * Checks if this order is done being built
     *
     * @return true if done building the order.
     */
    bool getDoneBuilding() const;

    /*!
     * Returns the next state for which the order is sufficient, returns the number of state if end of sufficient states is reached.
     *
     * @param state TODO ??
     */
    uint_fast64_t getNextSufficientState(uint_fast64_t state) const;

    /*!
     * Retrieves the number of states for which the order is sufficient
     *
     * @return The number of states for which the order is sufficient
     */
    uint_fast64_t getNumberOfSufficientStates() const;

    /*!
     * Retrieves the pointer to a Node at which the state occurs.
     *
     * @param state The number of the state.
     * @return The pointer to the node of the state, nullptr if the node does not exist.
     */
    Node* getNode(uint_fast64_t state) const;

    /*!
     * Returns the vector with the nodes of the order.
     * Each index in the vector refers to a state.
     * When the state is not yet added at a node, it will contain the nullptr.
     *
     * @return The vector with nodes of the order.
     */
    // std::vector<Node*> getNodes() const;

    /*!
     * TODO ??
     * @return
     */
    std::vector<uint_fast64_t>& getStatesSorted();

    /*!
     * Retrieves the top node of the order.
     *
     * @return The top node.
     */
    Node* getTop() const;

    /*!
     * Retrieves the number of states added to the order so far
     *
     * @return The numebr of Added States
     */
    uint_fast64_t getNumberOfAddedStates() const;

    /*!
     * Returns the number of states in the corresponding model, i.e. the possible number of states in the order.
     *
     * @return The possible number of states
     */
    uint_fast64_t getNumberOfStates() const;

    /*!
     * Checks if the given state is a bottom state.
     *
     * @param state The considered state
     * @return true if the state is a bottom state
     */
    bool isBottomState(uint_fast64_t state) const;

    /*!
     * Checks if the given state is a top state.
     *
     * @param state The considered state
     * @return true if the state is a top state
     */
    bool isTopState(uint_fast64_t state) const;

    /*!
     * Checks if the order only consists of bottom and top states (so no in-between nodes).
     *
     * @return true iff there is only a top node and a bottom node
     */
    bool isOnlyInitialOrder() const;

    /*!
     * Sorts the given states if possible.
     *
     * @param states Vector of the states to be sorted.
     * @return Vector with states sorted, length equals number of states to sort.
     * If states cannot be sorted, last state of the vector will always equal the length of the BitVector.
     */
    std::vector<uint_fast64_t> sortStates(std::vector<uint_fast64_t> const& states);
    std::vector<uint_fast64_t> sortStates(boost::container::flat_set<uint_fast64_t> const& states);

    /*!
     * Checks if a set of states are all above (or equal) or all below (or equal) a certain state
     * @param states A vector containing the states to check
     * @param state The state to which their relation is tested
     * @return <true, true> if the states are all equal in this order
     *         <true, false> if all of the states are above or equal to the state
     *         <false, true> if all of the states are below or equal to the state
     *         <false, false> if there are states above and below the state
     */
    std::pair<bool, bool> allAboveBelow(std::vector<uint_fast64_t> const states, uint_fast64_t state);

    /*!
     * Sorts the given states if possible.
     *
     * @param states Vector of the states to be sorted.
     * @return s1, s2, vector
     * if s1 == numberOfStates, all states could be sorted including current
     * if s1 < numberOfStates && s2 == numberOfStates, all states excluding s1 could be sorted, forward reasonging can be continued
     * else assumption is needed
     */
    // std::pair<std::pair<uint_fast64_t,uint_fast64_t>, std::vector<uint_fast64_t>> sortStatesForForward(uint_fast64_t currentState, std::vector<uint_fast64_t>
    // const& successors);

    /*!
     * Sorts the given states if possible.
     *
     * @param states Bitvector of the states to be sorted.
     * @return vector with states sorted, length equals number of states to sort.
     * If states cannot be sorted, last state of the vector will always equal the length of the BitVector.
     */
    std::vector<uint_fast64_t> sortStates(storm::storage::BitVector* states);

    /*!
     * Checks if a state is trivial / is in a trivial SCC with just itself
     *
     * @param state The considered state
     * @return true if the state is trivial / is in a trivial SCC with just itself
     */
    bool isTrivial(uint_fast64_t state);

    /*!
     * Retrieves the next state that has not been added yet.
     *
     * @return The next non-added state
     */
    std::pair<uint_fast64_t, bool> getNextStateNumber();

    //                    bool existsNextSCC();

    /*!
     * Checks if there exists a state that has yet to be added
     *
     * @return true if there is a state for which the order is not yet sufficient
     */
    bool existsNextState();

    /*!
     * Checks if TODO ??
     *
     * @return true if
     */
    bool existsStateToHandle();

    /*!
     * TODO ??
     * @return
     */
    std::pair<uint_fast64_t, bool> getStateToHandle();

    /*!
     * TODO ??
     * @param state
     */
    void addStateToHandle(uint_fast64_t state);
    void addSpecialStateToHandle(uint_fast64_t state);

    /*!
     * TODO ??
     * @param state
     */
    void addStateSorted(uint_fast64_t state);

    /*!
     * If the order is fully built, this can be set to true.
     */
    // void setDoneBuilding(bool done = true);

    /*!
     * Prints the dot output to normal STORM_PRINT.
     */
    void toDotOutput(bool print) const;

    /*!
     * Writes dot output to the given file.
     *
     * @param dotOutfile
     */
    void dotOutputToFile(std::ostream& dotOutfile) const;

    /*!
     * Creates a copy of the calling Order.
     *
     * @return Pointer to the copy.
     */
    std::shared_ptr<Order> copy() const;

    //                    void setAddedSCC(uint_fast64_t sccNumber);

    /*!
     * Marks the order as sufficient for the specified state
     *
     * @param sccNumber The considered state
     */
    void setSufficientForState(uint_fast64_t stateNumber);
    void setDoneForState(uint_fast64_t stateNumber);

    /*!
     * Adds an action for a state in an mdp
     *
     * @param state the considered state
     * @param action the action that should be taken by this scheduler at the state
     */
    void addToMdpScheduler(uint_fast64_t state, uint_fast64_t action);

    /*!
     * Gives the best action for a state in an mdp
     *
     * @param state the considered state
     * @return the best action to be taken according to mdpScheduler
     */
    uint64_t getActionAtState(uint_fast64_t state) const;

    bool isActionSetAtState(uint_fast64_t state) const;
    bool isSufficientForState(uint_fast64_t state) const;
    bool isDoneForState(uint_fast64_t state) const;

    void setChanged(bool changed);

    bool getChanged() const;

   protected:
    // storage::Decomposition<storage::StronglyConnectedComponent> getDecomposition() const;

   private:
    /*!
     * Checks if a node is above another node
     *
     * @param node1 The first node
     * @param node2 The second node
     * @return true iff node1 is above node2 in the order
     */
    bool above(Node* node1, Node* node2);

    // bool above(Node * node1, Node * node2, storm::analysis::Order::Node *nodePrev, storm::storage::BitVector *statesSeen);

    /*!
     * Checks if a node is above another one based on the direct states above the second node
     *
     * @param node1 The first node
     * @param node2 The second node
     * @return true iff node1 is in the statesAbove bitvector of node2
     */
    bool aboveFast(Node* node1, Node* node2) const;

    /*!
     * Initializes the basic attributes of the order
     *
     * @param numberOfStates numberOfStates The number of states the corresponding model has / the order will possibly have
     * @param decomposition TODO ??
     * @param doneBuilding
     */
    void init(uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, bool doneBuilding = false);

    /*!
     * Creates a name for a node in the dotOutput that represents a node in the order
     *
     * @param n The considered node in the order
     * @return The name for the node in the dotOutput
     */
    std::string nodeName(Node* n) const;

    /*!
     * Creates the name with which a node will be labeled in the dotOutput
     *
     * @param n The considered node in the order
     * @return The label for the node in the dotOutput
     */
    std::string nodeLabel(Node* n) const;

    // void nodeOutput();

    /*** Order Information ***/
    // True if the order is finished
    bool doneBuilding;

    // True if the order only consists of a bottom and a top node
    bool onlyInitialOrder;

    // Potential number of states the order can have aka the number of states of the model
    uint_fast64_t numberOfStates;

    // The number of states already in the order
    uint_fast64_t numberOfAddedStates;

    // The vector of nodes in this order
    std::vector<Node*> nodes;
    std::set<Node*> nodesSet;

    // The top node
    Node* top;

    // The bottom node
    Node* bottom;

    /*** Important sets of states: ***/

    // States in reversed topological order. If a state is added to the order, it is removed from this. TODO maybe find a better name?
    std::vector<uint_fast64_t> statesSorted;
    // States whose successors can be ordered by this order
    storm::storage::BitVector sufficientForState;
    // States whose successors can be ordered by this order, and state itself can be ordered by backward/forward reasoning
    storm::storage::BitVector doneForState;
    // States that are their own SCC
    storm::storage::BitVector trivialStates;
    // States for Forward Reasoning
    std::vector<uint_fast64_t> statesToHandle;
    std::vector<uint_fast64_t> specialStatesToHandle;

    boost::optional<std::vector<uint64_t>> mdpScheduler;
    bool changed;
};
}  // namespace analysis
}  // namespace storm
#endif  // ORDER_ORDER_H