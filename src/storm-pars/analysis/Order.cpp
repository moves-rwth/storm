#include "Order.h"

#include <iostream>
#include <queue>
#include <storm/utility/macros.h>

namespace storm {
namespace analysis {
Order::Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates,
             storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted, bool deterministic) {
    STORM_LOG_ASSERT(bottomStates->getNumberOfSetBits() > 0, "Expecting order to contain at least one bottom state");
    init(numberOfStates, decomposition, deterministic);
    this->numberOfAddedStates = 0;
    this->onlyInitialOrder = true;
    if (!topStates->empty()) {
        for (auto const& i : *topStates) {
            this->sufficientForState.set(i);
            this->doneForState.set(i);
            this->bottom->statesAbove.set(i);
            this->top->states.insert(i);
            this->nodes[i] = top;
            numberOfAddedStates++;
        }
    } else {
        top = nullptr;
    }

    this->statesSorted = statesSorted;

    for (auto const& i : *bottomStates) {
        this->sufficientForState.set(i);
        this->doneForState.set(i);
        this->bottom->states.insert(i);
        this->nodes[i] = bottom;
        numberOfAddedStates++;
    }
    assert(numberOfAddedStates <= numberOfStates);
    assert(sufficientForState.getNumberOfSetBits() == (topStates->getNumberOfSetBits() + bottomStates->getNumberOfSetBits()));
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = sufficientForState.full();
    }
    changed = true;
}

Order::Order(uint_fast64_t topState, uint_fast64_t bottomState, uint_fast64_t numberOfStates,
             storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted, bool deterministic) {
    init(numberOfStates, decomposition, deterministic);

    this->onlyInitialOrder = true;
    this->sufficientForState.set(topState);
    this->doneForState.set(topState);

    this->bottom->statesAbove.set(topState);
    this->top->states.insert(topState);
    this->nodes[topState] = top;

    this->sufficientForState.set(bottomState);
    this->doneForState.set(bottomState);

    this->bottom->states.insert(bottomState);
    this->nodes[bottomState] = bottom;
    this->numberOfAddedStates = 2;
    assert(numberOfAddedStates <= numberOfStates);

    this->statesSorted = statesSorted;
    assert(sufficientForState.getNumberOfSetBits() == 2);
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = sufficientForState.full();
    }
    changed = true;
}

Order::Order() {
    this->changed = false;
}

/*** Modifying the order ***/

void Order::add(uint_fast64_t state) {
    STORM_LOG_ASSERT(nodes[state] == nullptr, "Cannot add state that is already in the order");
    if (top == nullptr) {
        addAbove(state, bottom);
    } else {
        addBetween(state, top, bottom);
    }
}

void Order::addAbove(uint_fast64_t state, Node* node) {
    STORM_LOG_ASSERT(node != nullptr, "Expecting node to not be a nullptr");
    STORM_LOG_ASSERT(state < numberOfStates, "Invalid state number");
    STORM_LOG_INFO("Add " << state << " above " << *node->states.begin());

    if (nodes[state] == nullptr) {
        Node* newNode = new Node();
        nodes[state] = newNode;

        newNode->states.insert(state);
        newNode->statesAbove = storm::storage::BitVector(numberOfStates, false);
        if (top != nullptr) {
            for (auto const& state : top->states) {
                newNode->statesAbove.set(state);
            }
        }
        node->statesAbove.set(state);
        numberOfAddedStates++;
        onlyInitialOrder = false;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = sufficientForState.full();
        }
    } else {
        addRelationNodes(getNode(state), node);
    }
}

void Order::addBelow(uint_fast64_t state, Node* node) {
    STORM_LOG_INFO("Add " << state << " below " << *node->states.begin() << std::endl);
    STORM_LOG_ASSERT(node != nullptr, "Expecting node to not be a nullptr");
    STORM_LOG_ASSERT(state < numberOfStates, "Invalid state number");
    if (!contains(state)) {
        Node* newNode = new Node();
        nodes[state] = newNode;
        newNode->states.insert(state);
        newNode->statesAbove = storm::storage::BitVector((node->statesAbove));
        for (auto statesAbove : node->states) {
            newNode->statesAbove.set(statesAbove);
        }
        bottom->statesAbove.set(state);
        numberOfAddedStates++;
        onlyInitialOrder = false;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = sufficientForState.full();
        }
    } else {
        addRelationNodes(node, getNode(state));
    }
    assert(numberOfAddedStates <= numberOfStates);
}

void Order::addBetween(uint_fast64_t state, Node* above, Node* below) {
    STORM_LOG_INFO("Add " << state << " between (above) " << *above->states.begin() << " and " << *below->states.begin());

    STORM_LOG_ASSERT(above != below, "Cannot add between the same nodes");
    if (above == nullptr) {
        addAbove(state, below);
        return;
    }

    assert(compare(above, below) == ABOVE);
    assert(above != nullptr && below != nullptr);
    if (nodes[state] == nullptr) {
        // State is not in the order yet
        Node* newNode = new Node();
        nodes[state] = newNode;

        newNode->states.insert(state);
        newNode->statesAbove = storm::storage::BitVector(above->statesAbove);
        for (auto aboveStates : above->states) {
            newNode->statesAbove.set(aboveStates);
        }
        below->statesAbove.set(state);
        numberOfAddedStates++;
        onlyInitialOrder = false;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = sufficientForState.full();
        }
        assert(numberOfAddedStates <= numberOfStates);
    } else {
        // State is in the order already, so we add the new relations
        addRelationNodes(above, nodes[state]);
        addRelationNodes(nodes[state], below);
    }
}

void Order::addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below) {
    // is done in the other addBetween anyway
    // assert (compare(above, below) == ABOVE);
    assert(getNode(below)->states.find(below) != getNode(below)->states.end());
    assert(getNode(above)->states.find(above) != getNode(above)->states.end());

    addBetween(state, getNode(above), getNode(below));
}

void Order::addRelation(uint_fast64_t above, uint_fast64_t below, bool allowMerge) {
    //            assert (getNode(above) != nullptr && getNode(below) != nullptr);
    addRelationNodes(getNode(above), getNode(below));
}

void Order::addRelationNodes(Order::Node* above, Order::Node* below) {
    STORM_LOG_INFO("Add relation between (above) " << *above->states.begin() << " and " << *below->states.begin());

    below->statesAbove |= ((above->statesAbove));
    for (auto const& state : above->states) {
        below->statesAbove.set(state);
    }
}

void Order::addToNode(uint_fast64_t state, Node* node) {
    STORM_LOG_INFO("Add " << state << " to " << *node->states.begin());

    if (nodes[state] == nullptr) {
        // State is not in the order yet
        node->states.insert(state);
        nodes[state] = node;
        numberOfAddedStates++;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = sufficientForState.full();
        }
        assert(numberOfAddedStates <= numberOfStates);

    } else {
        // State is in the order already, so we merge the nodes
        mergeNodes(nodes[state], node);
    }
}

void Order::mergeNodes(Order::Node* node1, Order::Node* node2) {
    if (invalid) {
        STORM_LOG_INFO("Not merging " << *node1->states.begin() << " and " << *node2->states.begin() << " as order is invalid");
        return;
    }
    STORM_LOG_INFO("Merge " << *node1->states.begin() << " and " << *node2->states.begin());
    if (node1 == node2) {
        return;
    }

    // if node1 and node2 are already ordered, the order is invalid
    auto compareRes = compare(node1, node2);
    if (compareRes == ABOVE || compareRes == BELOW) {
        setInvalid();
    }

    if (compareRes == BELOW) {
        // To make life easier later on
        std::swap(node1, node2);
    }

    // Merges node2 into node 1
    // 1) Every node which has node2 above it, should now have (the states in) node1 above it
    // First doing this, and then adding states from node2 to node1 to prevent adding states from node2 again to the states above node
    for (auto const& node : nodes) {
        if (node != node1 && node != node2 && compareFast(node2, node) == ABOVE) {
            for (auto state1 : node1->states) {
                node->statesAbove.set(state1);
            }
        }
    }
    // 2) Add the states from node2 to node1
    node1->states.insert(node2->states.begin(), node2->states.end());
    for (auto const& i : node2->states) {
        nodes[i] = node1;
    }
    // 3) All states that are above node2 should also be above node1
    node1->statesAbove |= ((node2->statesAbove));
    // 4) states cannot be both above the node and in the node
    for (auto state : node1->states) {
        STORM_LOG_ASSERT(!node1->statesAbove[state] || this->isInvalid(), "Expecting the order to be invalid if there are states which are both in the node and above the node");
    }

    if (compareRes == BELOW || compareRes == ABOVE) {
        STORM_LOG_ASSERT(this->isInvalid(), "Expecting the order to be invalid if there are nodes between node1 and node2");
        // We need to merge all states between node1 and node2 as well
        // node1 will be above node2 right now, as we did swapping
        // so we collect all nodes above node2 that are below node1
        // we only do this for the ones directly above node2, as the other ones will be considered recursively
        std::set<Node*> nodesAbove;
        for (auto state : node2->statesAbove) {
            nodesAbove.insert(getNode(state));
        }
        for (auto node : nodesAbove) {
            if (compare(node1, node) == ABOVE) {
                mergeNodes(node1, node);
            }
        }
    }
    STORM_LOG_ASSERT(compare(node1, bottom) == ABOVE, "Something went wrong with merging the nodes, the resulting node is not above the bottom node");
    STORM_LOG_ASSERT(top == nullptr || compare(node1, top) == BELOW, "Something went wrong with merging the nodes, the resulting node is not below the top node");
}

void Order::merge(uint_fast64_t state1, uint_fast64_t state2) {
    mergeNodes(getNode(state1), getNode(state2));
}

/*** Checking on the order ***/

Order::NodeComparison Order::compare(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis) {
    return compare(getNode(state1), getNode(state2), hypothesis);
}

Order::NodeComparison Order::compareFast(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis) const {
    return compareFast(getNode(state1), getNode(state2), hypothesis);
}

Order::NodeComparison Order::compareFast(Node* node1, Node* node2, NodeComparison hypothesis) const {
    if ((node1 == top && node1 != nullptr) || node2 == bottom) {
        return ABOVE;
    }
    if (node1 == bottom || (node2 == top && node2 != nullptr)) {
        return BELOW;
    }

    if (node1 != nullptr && node2 != nullptr) {
        if (node1 == node2) {
            return SAME;
        }
        if ((hypothesis == UNKNOWN || hypothesis == ABOVE) && ((node1 == top || node2 == bottom) || aboveFast(node1, node2))) {
            return ABOVE;
        }

        if ((hypothesis == UNKNOWN || hypothesis == BELOW) && ((node2 == top || node1 == bottom) || aboveFast(node2, node1))) {
            return BELOW;
        }
    } else if (deterministic && ((top != nullptr && node1 == top) || node2 == bottom)) {
        return ABOVE;
    } else if (deterministic && ((top != nullptr && node1 == top) || node1 == bottom)) {
        return BELOW;
    }
    return UNKNOWN;
}

Order::NodeComparison Order::compare(Node* node1, Node* node2, NodeComparison hypothesis) {
    if (node1 != nullptr && node1 == node2) {
        return SAME;
    }
    if (node1 != nullptr && node2 != nullptr) {
        auto comp = compareFast(node1, node2, hypothesis);
        if (comp != UNKNOWN) {
            if (compareFast(node2, node1, UNKNOWN) == comp) {
                mergeNodes(node1, node2);
                return SAME;
            }
            return comp;
        }
        if ((hypothesis == UNKNOWN || hypothesis == ABOVE) && above(node1, node2)) {
            return ABOVE;
        }

        if ((hypothesis == UNKNOWN || hypothesis == BELOW) && above(node2, node1)) {
            return BELOW;
        }
    } else if ((top != nullptr && node1 == top) || node2 == bottom) {
        return ABOVE;
    } else if ((top != nullptr && node2 == top) || node1 == bottom) {
        return BELOW;
    }
    return UNKNOWN;
}

bool Order::contains(uint_fast64_t state) const {
    return state < numberOfStates && nodes[state] != nullptr;
}

Order::Node* Order::getBottom() const {
    return bottom;
}

bool Order::getDoneBuilding() const {
    return sufficientForState.full();
}

uint_fast64_t Order::getNextSufficientState(uint_fast64_t state) const {
    return sufficientForState.getNextSetIndex(state + 1);
}

Order::Node* Order::getNode(uint_fast64_t stateNumber) const {
    assert(stateNumber < numberOfStates);
    return nodes[stateNumber];
}

std::vector<uint_fast64_t>& Order::getStatesSorted() {
    return statesSorted;
}

Order::Node* Order::getTop() const {
    return top;
}

uint_fast64_t Order::getNumberOfAddedStates() const {
    return numberOfAddedStates;
}

uint_fast64_t Order::getNumberOfStates() const {
    return numberOfStates;
}

bool Order::isBottomState(uint_fast64_t state) const {
    auto states = bottom->states;
    return states.find(state) != states.end();
}

bool Order::isTopState(uint_fast64_t state) const {
    if (top == nullptr) {
        return false;
    }
    auto states = top->states;
    return states.find(state) != states.end();
}

bool Order::isOnlyInitialOrder() const {
    return onlyInitialOrder;
}

std::vector<uint_fast64_t> Order::sortStates(std::vector<uint_fast64_t> const& states) {
    assert(states.size() > 0);
    uint_fast64_t numberOfStatesToSort = states.size();
    std::vector<uint_fast64_t> result;
    // Go over all states
    for (auto state : states) {
        bool unknown = false;
        if (result.size() == 0) {
            result.push_back(state);
        } else {
            bool added = false;
            for (auto itr = result.begin(); itr != result.end(); ++itr) {
                auto compareRes = compare(state, (*itr));
                if (compareRes == ABOVE || compareRes == SAME) {
                    // insert at current pointer (while keeping other values)
                    result.insert(itr, state);
                    added = true;
                    break;
                } else if (compareRes == UNKNOWN) {
                    unknown = true;
                    break;
                }
            }
            if (unknown) {
                break;
            }
            if (!added) {
                result.push_back(state);
            }
        }
    }
    while (result.size() < numberOfStatesToSort) {
        result.push_back(numberOfStates);
    }
    assert(result.size() == numberOfStatesToSort);
    return result;
}

std::vector<uint_fast64_t> Order::sortStates(boost::container::flat_set<uint_fast64_t> const& states) {
    assert(states.size() > 0);
    uint_fast64_t numberOfStatesToSort = states.size();
    std::vector<uint_fast64_t> result;
    // Go over all states
    for (auto state : states) {
        bool unknown = false;
        if (result.size() == 0) {
            result.push_back(state);
        } else {
            bool added = false;
            for (auto itr = result.begin(); itr != result.end(); ++itr) {
                auto compareRes = compare(state, (*itr));
                if (compareRes == ABOVE || compareRes == SAME) {
                    // insert at current pointer (while keeping other values)
                    result.insert(itr, state);
                    added = true;
                    break;
                } else if (compareRes == UNKNOWN) {
                    unknown = true;
                    break;
                }
            }
            if (unknown) {
                break;
            }
            if (!added) {
                result.push_back(state);
            }
        }
    }
    while (result.size() < numberOfStatesToSort) {
        result.push_back(numberOfStates);
    }
    assert(result.size() == numberOfStatesToSort);
    return result;
}

std::vector<uint_fast64_t> Order::sortStates(storm::storage::BitVector* states) {
    uint_fast64_t numberOfStatesToSort = states->getNumberOfSetBits();
    std::vector<uint_fast64_t> result;
    // Go over all states
    for (auto state : *states) {
        bool unknown = false;
        if (result.size() == 0) {
            result.push_back(state);
        } else {
            bool added = false;
            for (auto itr = result.begin(); itr != result.end(); ++itr) {
                auto compareRes = compare(state, (*itr));
                if (compareRes == ABOVE || compareRes == SAME) {
                    // insert at current pointer (while keeping other values)
                    result.insert(itr, state);
                    added = true;
                    break;
                } else if (compareRes == UNKNOWN) {
                    unknown = true;
                    break;
                }
            }
            if (unknown) {
                break;
            }
            if (!added) {
                result.push_back(state);
            }
        }
    }
    auto i = 0;
    while (result.size() < numberOfStatesToSort) {
        result.push_back(numberOfStates);
        ++i;
    }
    assert(result.size() == numberOfStatesToSort);
    return result;
}

/*** Checking on helpfunctionality for building of order ***/

std::shared_ptr<Order> Order::copy() const {
    assert(!this->isInvalid());
    std::shared_ptr<Order> copiedOrder = std::make_shared<Order>();
    copiedOrder->nodes = std::vector<Node*>(numberOfStates, nullptr);
    copiedOrder->onlyInitialOrder = isOnlyInitialOrder();
    copiedOrder->numberOfStates = getNumberOfStates();
    copiedOrder->statesSorted = std::vector<uint_fast64_t>(this->statesSorted);
    copiedOrder->statesToHandle = std::vector<uint_fast64_t>(this->statesToHandle);
    copiedOrder->specialStatesToHandle = std::vector<uint_fast64_t>(this->specialStatesToHandle);
    copiedOrder->trivialStates = storm::storage::BitVector(trivialStates);
    copiedOrder->sufficientForState = storm::storage::BitVector(sufficientForState);
    copiedOrder->doneForState = storm::storage::BitVector(doneForState);
    copiedOrder->numberOfAddedStates = numberOfAddedStates;
    copiedOrder->doneBuilding = doneBuilding;
    copiedOrder->setChanged(this->getChanged());
    copiedOrder->setInvalid(this->invalid);
    copiedOrder->deterministic = deterministic;

    auto seenStates = storm::storage::BitVector(numberOfStates, false);
    // copy nodes
    if (this->top == nullptr) {
        copiedOrder->top = nullptr;
    }
    for (uint64_t state = 0; state < numberOfStates; ++state) {
        Node* oldNode = nodes.at(state);
        if (oldNode != nullptr) {
            if (!seenStates[*(oldNode->states.begin())]) {
                Node* newNode = new Node();
                if (oldNode == this->top) {
                    copiedOrder->top = newNode;
                } else if (oldNode == this->bottom) {
                    copiedOrder->bottom = newNode;
                }
                newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
                for (uint64_t i = 0; i < oldNode->statesAbove.size(); ++i) {
                    assert(newNode->statesAbove[i] == oldNode->statesAbove[i]);
                }
                for (uint64_t const& i : oldNode->states) {
                    assert(!seenStates[i]);
                    newNode->states.insert(i);
                    seenStates.set(i);
                    copiedOrder->nodes[i] = newNode;
                }
            }
        } else {
            assert(copiedOrder->nodes[state] == nullptr);
        }
    }
    if (!deterministic) {
        copiedOrder->mdpScheduler = std::vector<uint64_t>(numberOfStates, std::numeric_limits<uint64_t>::max());
        for (auto i = 0; i < numberOfStates; ++i) {
            if (this->isActionSetAtState(i)) {
                copiedOrder->addToMdpScheduler(i, this->getActionAtState(i));
            }
        }
    }

    return copiedOrder;
}

/*** Setters ***/
void Order::setSufficientForState(uint_fast64_t stateNumber) {
    sufficientForState.set(stateNumber);
}

void Order::setDoneForState(uint_fast64_t stateNumber) {
    STORM_LOG_ASSERT(sufficientForState[stateNumber], "Cannot set state " << stateNumber << " as done as the order is not yet sufficient for the state");
    STORM_LOG_ASSERT(contains(stateNumber), "Cannot set state " << stateNumber << " as done as the order does not contain the state");
    doneForState.set(stateNumber);
}

void Order::setInvalid(bool invalid) {
    this->invalid = invalid;
}

/*** Output ***/

void Order::toDotOutput(bool print) const {
    // Graphviz Output start
    std::string result = "Dot Output:\n digraph model {\n";

    // Vertices of the digraph
    storm::storage::BitVector stateCoverage = storm::storage::BitVector(numberOfStates);
    for (auto i = 0; i < numberOfStates; ++i) {
        if (nodes[i] != nullptr) {
            stateCoverage.set(i);
        }
    }
    for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i != numberOfStates; i = stateCoverage.getNextSetIndex(i + 1)) {
        for (uint_fast64_t j = i + 1; j < numberOfStates; j++) {
            if (getNode(j) == getNode(i))
                stateCoverage.set(j, false);
        }
        result += "\t" + nodeName(getNode(i)) + " [ label = \"" + nodeLabel(getNode(i)) + "\" ];\n";
    }

    // Edges of the digraph
    for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i != numberOfStates; i = stateCoverage.getNextSetIndex(i + 1)) {
        storm::storage::BitVector v = storm::storage::BitVector(numberOfStates, false);
        Node* currentNode = getNode(i);
        for (uint_fast64_t s1 : getNode(i)->statesAbove) {
            v |= (currentNode->statesAbove & getNode(s1)->statesAbove);
        }

        std::set<Node*> seenNodes;
        for (uint_fast64_t state : currentNode->statesAbove) {
            Node* n = getNode(state);
            if (std::find(seenNodes.begin(), seenNodes.end(), n) == seenNodes.end()) {
                seenNodes.insert(n);
                if (!v[state]) {
                    result += "\t" + nodeName(currentNode) + " ->  " + nodeName(getNode(state)) + ";\n";
                }
            }
        }
    }
    // Graphviz Output end
    result += "}\n";
    if (print) {
        STORM_PRINT(result);
    } else {
        STORM_LOG_INFO(result);
    }
}

void Order::dotOutputToFile(std::ostream& dotOutfile) const {
    // Graphviz Output start
    dotOutfile << "Dot Output:\n"
               << "digraph model {\n";

    // Vertices of the digraph
    storm::storage::BitVector stateCoverage = storm::storage::BitVector(numberOfStates, true);
    for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i != numberOfStates; i = stateCoverage.getNextSetIndex(i + 1)) {
        if (getNode(i) == NULL) {
            continue;
        }
        for (uint_fast64_t j = i + 1; j < numberOfStates; j++) {
            if (getNode(j) == getNode(i))
                stateCoverage.set(j, false);
        }

        dotOutfile << "\t" << nodeName(getNode(i)) << " [ label = \"" << nodeLabel(getNode(i)) << "\" ];" << std::endl;
    }

    // Edges of the digraph
    for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i != numberOfStates; i = stateCoverage.getNextSetIndex(i + 1)) {
        storm::storage::BitVector v = storm::storage::BitVector(numberOfStates, false);
        Node* currentNode = getNode(i);
        if (currentNode == NULL) {
            continue;
        }

        for (uint_fast64_t s1 : getNode(i)->statesAbove) {
            v |= (currentNode->statesAbove & getNode(s1)->statesAbove);
        }

        std::set<Node*> seenNodes;
        for (uint_fast64_t state : currentNode->statesAbove) {
            Node* n = getNode(state);
            if (std::find(seenNodes.begin(), seenNodes.end(), n) == seenNodes.end()) {
                seenNodes.insert(n);
                if (!v[state]) {
                    dotOutfile << "\t" << nodeName(currentNode) << " ->  " << nodeName(getNode(state)) << ";" << std::endl;
                }
            }
        }
    }

    // Graphviz Output end
    dotOutfile << "}\n";
}

/*** Private methods ***/

void Order::init(uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, bool deterministic) {
    this->numberOfStates = numberOfStates;
    this->nodes = std::vector<Node*>(numberOfStates, nullptr);
    this->sufficientForState = storm::storage::BitVector(numberOfStates, false);
    this->doneForState = storm::storage::BitVector(numberOfStates, false);
    if (decomposition.size() == 0) {
        this->trivialStates = storm::storage::BitVector(numberOfStates, true);
    } else {
        this->trivialStates = storm::storage::BitVector(numberOfStates, false);
        for (auto& scc : decomposition) {
            if (scc.size() == 1) {
                trivialStates.set(*(scc.begin()));
            }
        }
    }
    this->top = new Node();
    this->bottom = new Node();
    this->top->statesAbove = storm::storage::BitVector(numberOfStates, false);
    this->bottom->statesAbove = storm::storage::BitVector(numberOfStates, false);
    this->doneBuilding = doneBuilding;
    this->invalid = false;
    this->deterministic = deterministic;
    if (!this->deterministic) {
        mdpScheduler = std::vector<uint64_t>(numberOfStates, std::numeric_limits<uint64_t>::max());
    }
}

bool Order::aboveFast(Node* node1, Node* node2) const {
    bool found = false;
    for (auto const& state : node1->states) {
        found = ((node2->statesAbove))[state];
        if (found) {
            break;
        }
    }
    return found;
}

bool Order::above(Node* node1, Node* node2) {
    // Check whether node1 is above node2 by going over all states that are above state 2
    bool above = false;
    // Only do this when we have to deal with forward reasoning or we are not yet done with the building of the order
    //    if (!trivialStates.full() || !doneBuilding) {
    // First gather all states that are above node 2;

    storm::storage::BitVector statesSeen((node2->statesAbove));
    std::queue<uint_fast64_t> statesToHandle;
    for (auto state : statesSeen) {
        statesToHandle.push(state);
    }
    while (!above && !statesToHandle.empty()) {
        // Get first item from the queue
        auto state = statesToHandle.front();
        statesToHandle.pop();
        auto node = getNode(state);
        if (aboveFast(node1, node)) {
            above = true;
            continue;
        }
        for (auto newState : node->statesAbove) {
            if (!statesSeen[newState]) {
                statesToHandle.push(newState);
                statesSeen.set(newState);
            }
        }
    }
    //    }
    if (above) {
        for (auto state : node1->states) {
            node2->statesAbove.set(state);
        }
    }
    return above;
}

std::string Order::nodeName(Node* n) const {
    auto itr = n->states.begin();
    std::string name = "n" + std::to_string(*itr);
    return name;
}

std::string Order::nodeLabel(Node* n) const {
    if (n == top)
        return "=)";
    // if topstates is empty, we have a reward formula, so we don't want =) or =(
    if (n == bottom && top != nullptr && !top->states.empty())
        return "=(";
    auto itr = n->states.begin();
    std::string label = "s" + std::to_string(*itr);
    ++itr;
    while (itr != n->states.end()) {
        label += ", s" + std::to_string(*itr);
        ++itr;
    }
    return label;
}

bool Order::existsNextState() {
    return !sufficientForState.full();
}

bool Order::isTrivial(uint_fast64_t state) {
    return trivialStates[state];
}

std::pair<uint_fast64_t, bool> Order::getNextStateNumber() {
    assert(statesToHandle.empty());
    while (!statesSorted.empty()) {
        auto state = statesSorted.back();
        statesSorted.pop_back();
        if (!doneForState[state]) {
            return {state, true};
        }
        assert(sufficientForState[state]);
    }
    return {numberOfStates, true};
}

std::pair<uint_fast64_t, bool> Order::getStateToHandle() {
    if (!specialStatesToHandle.empty()) {
        auto state = specialStatesToHandle.back();
        specialStatesToHandle.pop_back();
        return {state, false};
    }
    while (!statesToHandle.empty()) {
        auto state = statesToHandle.back();
        statesToHandle.pop_back();
        if (!doneForState[state]) {
            return {state, false};
        }
    }
    return getNextStateNumber();
}

bool Order::existsStateToHandle() {
    if (!specialStatesToHandle.empty()) {
        return true;
    }
    while (!statesToHandle.empty() && contains(statesToHandle.back()) && sufficientForState[statesToHandle.back()]) {
        statesToHandle.pop_back();
    }
    return !statesToHandle.empty();
}

void Order::addStateToHandle(uint_fast64_t state) {
    STORM_LOG_INFO("Adding " << state << " to states to handle");
    if (!sufficientForState[state]) {
        statesToHandle.push_back(state);
    }
}

void Order::addSpecialStateToHandle(uint_fast64_t state) {
    STORM_LOG_INFO("Adding " << state << " to special states to handle");
    specialStatesToHandle.push_back(state);
}

void Order::addStateSorted(uint_fast64_t state) {
    statesSorted.push_back(state);
}

std::pair<bool, bool> Order::allAboveBelow(std::vector<uint_fast64_t> const states, uint_fast64_t state) {
    auto allAbove = true;
    auto allBelow = true;
    for (auto& checkState : states) {
        auto comp = compare(checkState, state);
        allAbove &= (comp == ABOVE || comp == SAME);
        allBelow &= (comp == BELOW || comp == SAME);
    }
    return {allAbove, allBelow};
}

uint_fast64_t Order::getNumberOfSufficientStates() const {
    return sufficientForState.getNumberOfSetBits();
}

void Order::addToMdpScheduler(uint_fast64_t state, uint_fast64_t action) {
    if (!deterministic) {
        mdpScheduler[state] = action;
    } else {
        STORM_LOG_WARN("Trying to add a action to the scheduler for a deterministic model");
    }
}

uint64_t Order::getActionAtState(uint_fast64_t state) const {
    if (deterministic) {
        return 0;
    }
    if (this->isTopState(state) || this->isBottomState(state)) {
        return 0;
    }
    STORM_LOG_ASSERT(mdpScheduler.size() > state, "Cannot get action for a state which is outside the mdpscheduler range");
    return mdpScheduler.at(state);
}

bool Order::isActionSetAtState(uint_fast64_t state) const {
    STORM_LOG_THROW(deterministic || state < mdpScheduler.size(), storm::exceptions::InvalidOperationException,
                    "Expecting model to be deterministic or state to be a valid statenumber");
    if (deterministic || this->isTopState(state) || this->isBottomState(state)) {
        return true;
    }
    return mdpScheduler.at(state) != std::numeric_limits<uint64_t>::max();
}

bool Order::isSufficientForState(uint_fast64_t state) const {
    return sufficientForState[state];
}

bool Order::isDoneForState(uint_fast64_t stateNumber) const {
    return doneForState[stateNumber];
}

void Order::setChanged(bool changed) {
    this->changed = changed;
}

bool Order::getChanged() const {
    return changed;
}

bool Order::isInvalid() const {
    return invalid;
}
}  // namespace analysis
}  // namespace storm
