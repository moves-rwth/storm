#include "Order.h"

#include <iostream>
#include <queue>
#include <storm/utility/macros.h>

namespace storm {
namespace analysis {
Order::Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates,
             storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted) {
    init(numberOfStates, decomposition);
    this->numberOfAddedStates = 0;
    this->onlyBottomTopOrder = true;
    for (auto const& i : *topStates) {
        this->doneStates.set(i);
        this->bottom->statesAbove.set(i);
        this->top->states.insert(i);
        this->nodes[i] = top;
        numberOfAddedStates++;
    }
    this->statesSorted = statesSorted;

    for (auto const& i : *bottomStates) {
        this->doneStates.set(i);
        this->bottom->states.insert(i);
        this->nodes[i] = bottom;
        numberOfAddedStates++;
    }
    assert(numberOfAddedStates <= numberOfStates);
    assert(doneStates.getNumberOfSetBits() == (topStates->getNumberOfSetBits() + bottomStates->getNumberOfSetBits()));
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = doneStates.full();
    }
}

Order::Order(uint_fast64_t topState, uint_fast64_t bottomState, uint_fast64_t numberOfStates,
             storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted) {
    init(numberOfStates, decomposition);

    this->onlyBottomTopOrder = true;
    this->doneStates.set(topState);

    this->bottom->statesAbove.set(topState);
    this->top->states.insert(topState);
    this->nodes[topState] = top;

    this->doneStates.set(bottomState);

    this->bottom->states.insert(bottomState);
    this->nodes[bottomState] = bottom;
    this->numberOfAddedStates = 2;
    assert(numberOfAddedStates <= numberOfStates);

    this->statesSorted = statesSorted;
    assert(doneStates.getNumberOfSetBits() == 2);
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = doneStates.full();
    }
}

Order::Order() {
    this->invalid = false;
}

/*** Modifying the order ***/

void Order::add(uint_fast64_t state) {
    assert(nodes[state] == nullptr);
    addBetween(state, top, bottom);
    addStateToHandle(state);
}

void Order::addAbove(uint_fast64_t state, Node* node) {
    STORM_LOG_INFO("Add " << state << " above " << *node->states.begin() << '\n');

    assert(nodes[state] == nullptr);
    Node* newNode = new Node();
    nodes[state] = newNode;

    newNode->states.insert(state);
    newNode->statesAbove = storm::storage::BitVector(numberOfStates, false);
    for (auto const& state : top->states) {
        newNode->statesAbove.set(state);
    }
    node->statesAbove.set(state);
    numberOfAddedStates++;
    onlyBottomTopOrder = false;
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = doneStates.full();
    }
}

void Order::addBelow(uint_fast64_t state, Node* node) {
    STORM_LOG_INFO("Add " << state << " below " << *node->states.begin() << '\n');

    assert(nodes[state] == nullptr);
    Node* newNode = new Node();
    nodes[state] = newNode;
    newNode->states.insert(state);
    newNode->statesAbove = storm::storage::BitVector((node->statesAbove));
    for (auto statesAbove : node->states) {
        newNode->statesAbove.set(statesAbove);
    }
    bottom->statesAbove.set(state);
    numberOfAddedStates++;
    onlyBottomTopOrder = false;
    if (numberOfAddedStates == numberOfStates) {
        doneBuilding = doneStates.full();
    }
    assert(numberOfAddedStates <= numberOfStates);
}

void Order::addBetween(uint_fast64_t state, Node* above, Node* below) {
    STORM_LOG_INFO("Add " << state << " between (above) " << *above->states.begin() << " and " << *below->states.begin() << '\n');

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
        onlyBottomTopOrder = false;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = doneStates.full();
        }
        assert(numberOfAddedStates <= numberOfStates);
    } else {
        // State is in the order already, so we add the new relations
        addRelationNodes(above, nodes[state]);
        addRelationNodes(nodes[state], below);
    }
}

void Order::addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below) {
    assert(compare(above, below) == ABOVE);
    assert(getNode(below)->states.find(below) != getNode(below)->states.end());
    assert(getNode(above)->states.find(above) != getNode(above)->states.end());

    addBetween(state, getNode(above), getNode(below));
}

void Order::addRelation(uint_fast64_t above, uint_fast64_t below, bool allowMerge) {
    assert(getNode(above) != nullptr && getNode(below) != nullptr);
    addRelationNodes(getNode(above), getNode(below), allowMerge);
}

void Order::addRelationNodes(Order::Node* above, Order::Node* below, bool allowMerge) {
    assert(allowMerge || compare(above, below) != BELOW);

    STORM_LOG_INFO("Add relation between (above) " << *above->states.begin() << " and " << *below->states.begin() << '\n');

    if (allowMerge) {
        if (compare(below, above) == ABOVE) {
            mergeNodes(above, below);
            return;
        }
    }
    below->statesAbove |= ((above->statesAbove));
    for (auto const& state : above->states) {
        below->statesAbove.set(state);
    }
    assert(compare(above, below) == ABOVE);
}

void Order::addToNode(uint_fast64_t state, Node* node) {
    STORM_LOG_INFO("Add " << state << " to between (above) " << *node->states.begin() << '\n');

    if (nodes[state] == nullptr) {
        // State is not in the order yet
        node->states.insert(state);
        nodes[state] = node;
        numberOfAddedStates++;
        if (numberOfAddedStates == numberOfStates) {
            doneBuilding = doneStates.full();
        }
        assert(numberOfAddedStates <= numberOfStates);

    } else {
        // State is in the order already, so we merge the nodes
        mergeNodes(nodes[state], node);
    }
}

bool Order::mergeNodes(storm::analysis::Order::Node* node1, storm::analysis::Order::Node* node2) {
    STORM_LOG_INFO("Merge " << *node1->states.begin() << " and " << *node2->states.begin() << '\n');

    // Merges node2 into node 1
    // everything above n2 also above n1
    node1->statesAbove |= ((node2->statesAbove));
    // add states of node 2 to node 1
    node1->states.insert(node2->states.begin(), node2->states.end());

    for (auto const& i : node2->states) {
        nodes[i] = node1;
    }

    for (auto const& node : nodes) {
        if (node != nullptr) {
            for (auto state2 : node2->states) {
                if (node->statesAbove[state2]) {
                    for (auto state1 : node1->states) {
                        node->statesAbove.set(state1);
                    }
                    break;
                }
            }
        }
    }
    for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
        for (uint_fast64_t j = i + 1; j < numberOfStates; ++j) {
            auto comp1 = compare(i, j);
            auto comp2 = compare(j, i);
            if (!((comp1 == BELOW && comp2 == ABOVE) || (comp1 == ABOVE && comp2 == BELOW) || (comp1 == UNKNOWN && comp2 == UNKNOWN) ||
                  (comp1 == SAME && comp2 == SAME))) {
                invalid = true;
                return false;
            }
        }
    }
    return !invalid;
}

bool Order::merge(uint_fast64_t var1, uint_fast64_t var2) {
    return mergeNodes(getNode(var1), getNode(var2));
}

/*** Checking on the order ***/

Order::NodeComparison Order::compare(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis) {
    return compare(getNode(state1), getNode(state2), hypothesis);
}

Order::NodeComparison Order::compareFast(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis) const {
    auto res = compareFast(getNode(state1), getNode(state2), hypothesis);
    return res;
}

Order::NodeComparison Order::compareFast(Node* node1, Node* node2, NodeComparison hypothesis) const {
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
    } else if (node1 == top || node2 == bottom) {
        return ABOVE;
    } else if (node2 == top || node1 == bottom) {
        return BELOW;
    }
    return UNKNOWN;
}

Order::NodeComparison Order::compare(Node* node1, Node* node2, NodeComparison hypothesis) {
    if (node1 != nullptr && node2 != nullptr) {
        auto comp = compareFast(node1, node2, hypothesis);
        if (comp != UNKNOWN) {
            return comp;
        }
        if ((hypothesis == UNKNOWN || hypothesis == ABOVE) && above(node1, node2)) {
            assert(!above(node2, node1));
            return ABOVE;
        }

        if ((hypothesis == UNKNOWN || hypothesis == BELOW) && above(node2, node1)) {
            return BELOW;
        }
    } else if (node1 == top || node2 == bottom) {
        return ABOVE;
    } else if (node2 == top || node1 == bottom) {
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
    assert(!doneStates.full() || numberOfAddedStates == numberOfStates);
    return doneStates.full();
}

uint_fast64_t Order::getNextDoneState(uint_fast64_t state) const {
    return doneStates.getNextSetIndex(state + 1);
}

Order::Node* Order::getNode(uint_fast64_t stateNumber) const {
    assert(stateNumber < numberOfStates);
    return nodes[stateNumber];
}

std::vector<Order::Node*> Order::getNodes() const {
    return nodes;
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
    auto states = top->states;
    return states.find(state) != states.end();
}

bool Order::isOnlyBottomTopOrder() const {
    return onlyBottomTopOrder;
}

std::vector<uint_fast64_t> Order::sortStates(std::vector<uint_fast64_t>* states) {
    assert(states != nullptr);
    uint_fast64_t numberOfStatesToSort = states->size();
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
    while (result.size() < numberOfStatesToSort) {
        result.push_back(numberOfStates);
    }
    assert(result.size() == numberOfStatesToSort);
    return result;
}

std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> Order::sortStatesForForward(uint_fast64_t currentState,
                                                                                                           std::vector<uint_fast64_t> const& states) {
    std::vector<uint_fast64_t> statesSorted;
    statesSorted.push_back(currentState);
    // Go over all states
    bool oneUnknown = false;
    bool unknown = false;
    uint_fast64_t s1 = numberOfStates;
    uint_fast64_t s2 = numberOfStates;
    for (auto& state : states) {
        unknown = false;
        bool added = false;
        for (auto itr = statesSorted.begin(); itr != statesSorted.end(); ++itr) {
            auto compareRes = compare(state, (*itr));
            if (compareRes == ABOVE || compareRes == SAME) {
                if (!contains(state) && compareRes == ABOVE) {
                    add(state);
                }
                added = true;
                // insert at current pointer (while keeping other values)
                statesSorted.insert(itr, state);
                break;
            } else if (compareRes == UNKNOWN && !oneUnknown) {
                // We miss state in the result.
                s1 = state;
                s2 = *itr;
                oneUnknown = true;
                added = true;
                break;
            } else if (compareRes == UNKNOWN && oneUnknown) {
                unknown = true;
                added = true;
                break;
            }
        }
        if (!added) {
            // State will be last in the list
            statesSorted.push_back(state);
        }
        if (unknown && oneUnknown) {
            break;
        }
    }
    if (!unknown && oneUnknown) {
        assert(statesSorted.size() == states.size());
        s2 = numberOfStates;
    }
    assert(s1 == numberOfStates || (s1 != numberOfStates && s2 == numberOfStates && statesSorted.size() == states.size()) ||
           (s1 != numberOfStates && s2 != numberOfStates && statesSorted.size() < states.size()));

    return {{s1, s2}, statesSorted};
}

std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>> Order::sortStatesUnorderedPair(const std::vector<uint_fast64_t>* states) {
    assert(states != nullptr);
    uint_fast64_t numberOfStatesToSort = states->size();
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
                    return {{(*itr), state}, std::move(result)};
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

    assert(result.size() == numberOfStatesToSort);
    return {{numberOfStates, numberOfStates}, std::move(result)};
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
    assert(!isInvalid());
    std::shared_ptr<Order> copiedOrder = std::make_shared<Order>();
    copiedOrder->nodes = std::vector<Node*>(numberOfStates, nullptr);
    copiedOrder->onlyBottomTopOrder = this->isOnlyBottomTopOrder();
    copiedOrder->numberOfStates = this->getNumberOfStates();
    copiedOrder->statesSorted = std::vector<uint_fast64_t>(this->statesSorted);
    copiedOrder->statesToHandle = std::vector<uint_fast64_t>(this->statesToHandle);
    copiedOrder->trivialStates = storm::storage::BitVector(trivialStates);
    copiedOrder->doneStates = storm::storage::BitVector(doneStates);
    copiedOrder->numberOfAddedStates = this->numberOfAddedStates;
    copiedOrder->doneBuilding = this->doneBuilding;

    auto seenStates = storm::storage::BitVector(numberOfStates, false);
    // copy nodes
    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
        Node* oldNode = nodes.at(state);
        if (oldNode != nullptr) {
            if (!seenStates[*(oldNode->states.begin())]) {
                Node* newNode = new Node();
                if (oldNode == this->getTop()) {
                    copiedOrder->top = newNode;
                } else if (oldNode == this->getBottom()) {
                    copiedOrder->bottom = newNode;
                }
                newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
                for (size_t i = 0; i < oldNode->statesAbove.size(); ++i) {
                    assert(newNode->statesAbove[i] == oldNode->statesAbove[i]);
                }
                for (auto const& i : oldNode->states) {
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

    return copiedOrder;
}

/*** Setters ***/
void Order::setDoneState(uint_fast64_t stateNumber) {
    doneStates.set(stateNumber);
}

/*** Output ***/

void Order::toDotOutput() const {
    // Graphviz Output start
    STORM_PRINT("Dot Output:\n"
                << "digraph model {\n");

    // Vertices of the digraph
    storm::storage::BitVector stateCoverage = storm::storage::BitVector(doneStates);
    for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
        if (nodes[i] != nullptr) {
            stateCoverage.set(i);
        }
    }
    for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i != numberOfStates; i = stateCoverage.getNextSetIndex(i + 1)) {
        for (uint_fast64_t j = i + 1; j < numberOfStates; j++) {
            if (getNode(j) == getNode(i))
                stateCoverage.set(j, false);
        }
        STORM_PRINT("\t" << nodeName(*getNode(i)) << " [ label = \"" << nodeLabel(*getNode(i)) << "\" ];\n");
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
                    STORM_PRINT("\t" << nodeName(*currentNode) << " ->  " << nodeName(*getNode(state)) << ";\n");
                }
            }
        }
    }
    // Graphviz Output end
    STORM_PRINT("}\n");
}

void Order::dotOutputToFile(std::ofstream& dotOutfile) const {
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

        dotOutfile << "\t" << nodeName(*getNode(i)) << " [ label = \"" << nodeLabel(*getNode(i)) << "\" ];\n";
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
                    dotOutfile << "\t" << nodeName(*currentNode) << " ->  " << nodeName(*getNode(state)) << ";\n";
                }
            }
        }
    }

    // Graphviz Output end
    dotOutfile << "}\n";
}

/*** Private methods ***/

void Order::init(uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, bool doneBuilding) {
    this->numberOfStates = numberOfStates;
    this->invalid = false;
    this->nodes = std::vector<Node*>(numberOfStates, nullptr);
    this->doneStates = storm::storage::BitVector(numberOfStates, false);
    assert(doneStates.getNumberOfSetBits() == 0);
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
    assert(!aboveFast(node1, node2));
    // Check whether node1 is above node2 by going over all states that are above state 2
    bool above = false;
    // Only do this when we have to deal with forward reasoning or we are not yet done with the building of the order
    if (!trivialStates.full() || !doneBuilding) {
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
    }
    if (above) {
        for (auto state : node1->states) {
            node2->statesAbove.set(state);
        }
    }
    return above;
}

std::string Order::nodeName(Node n) const {
    auto itr = n.states.begin();
    std::string name = "n" + std::to_string(*itr);
    return name;
}

std::string Order::nodeLabel(Node n) const {
    if (n.states == top->states)
        return "=)";
    if (n.states == bottom->states)
        return "=(";
    auto itr = n.states.begin();
    std::string label = "s" + std::to_string(*itr);
    ++itr;
    if (itr != n.states.end())
        label = "[" + label + "]";
    return label;
}

bool Order::existsNextState() {
    return !doneStates.full();
}

bool Order::isTrivial(uint_fast64_t state) {
    return trivialStates[state];
}

std::pair<uint_fast64_t, bool> Order::getNextStateNumber() {
    while (!statesSorted.empty()) {
        auto state = statesSorted.back();
        statesSorted.pop_back();
        if (!doneStates[state]) {
            return {state, true};
        }
    }
    return {numberOfStates, true};
}

std::pair<uint_fast64_t, bool> Order::getStateToHandle() {
    assert(existsStateToHandle());
    auto state = statesToHandle.back();
    statesToHandle.pop_back();
    return {state, false};
}

bool Order::existsStateToHandle() {
    while (!statesToHandle.empty() && doneStates[statesToHandle.back()]) {
        statesToHandle.pop_back();
    }
    return !statesToHandle.empty();
}

void Order::addStateToHandle(uint_fast64_t state) {
    if (!doneStates[state]) {
        statesToHandle.push_back(state);
    }
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

uint_fast64_t Order::getNumberOfDoneStates() const {
    return doneStates.getNumberOfSetBits();
}

bool Order::isInvalid() const {
    return invalid;
}
}  // namespace analysis
}  // namespace storm
