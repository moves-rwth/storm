#include "Order.h"

#include <iostream>
#include <storm/utility/macros.h>

namespace storm {
    namespace analysis {
        Order::Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted) {
            assert (statesSorted != nullptr);
            init(numberOfStates, statesSorted);
            this->numberOfAddedStates = 0;
            this->onlyBottomTopOrder = true;
            for (auto const& i : *topStates) {
                this->addedStates->set(i);
                this->bottom->statesAbove.set(i);
                this->top->states.insert(i);
                this->nodes[i] = top;
                numberOfAddedStates++;
            }

            for (auto const& i : *bottomStates) {
                this->addedStates->set(i);
                this->bottom->states.insert(i);
                this->nodes[i] = bottom;
                numberOfAddedStates++;

            }
            assert (addedStates->getNumberOfSetBits() == (topStates->getNumberOfSetBits() + bottomStates->getNumberOfSetBits()));
        }

        Order::Order(uint_fast64_t topState, uint_fast64_t bottomState, uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted) {
            init(numberOfStates, statesSorted);

            this->onlyBottomTopOrder = true;
            this->addedStates->set(topState);
            this->bottom->statesAbove.set(topState);
            this->top->states.insert(topState);
            this->nodes[topState] = top;

            this->addedStates->set(bottomState);
            this->bottom->states.insert(bottomState);
            this->nodes[bottomState] = bottom;
            this->numberOfAddedStates = 2;

            assert (addedStates->getNumberOfSetBits() == 2);
        }

        Order::Order(std::shared_ptr<Order> order) {
            auto copyStatesSorted = std::vector<uint_fast64_t>(order->getStatesSorted());
            init(order->getAddedStates()->size(), &copyStatesSorted, order->getDoneBuilding());
            onlyBottomTopOrder = order->isOnlyBottomTopOrder();

            this->statesToHandle = std::vector<uint_fast64_t>(order->getStatesToHandle());
            auto oldNodes = order->getNodes();
            // Create nodes, and set the states in this node
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode;
                    if (oldNode == order->getTop()) {
                        newNode = this->top;
                    } else if (oldNode == order->getBottom()) {
                        newNode = this->bottom;
                    } else {
                        newNode = new Node();
                    }
                    newNode->states = oldNode->states;
                    for (auto const &i : newNode->states) {
                        this->addedStates->set(i);
                        this->nodes[i] = newNode;
                    }
                    newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
                }
                numberOfAddedStates = this->addedStates->getNumberOfSetBits();

            }
            assert(*addedStates == *(order->getAddedStates()));
        }

        Order::Order(){

        }

        /*** Modifying the order ***/

        void Order::add(uint_fast64_t state) {
            assert (!(*addedStates)[state]);
            addBetween(state, top, bottom);
        }

        void Order::addAbove(uint_fast64_t state, Node *node) {
            assert (!(*addedStates)[state]);
            Node *newNode = new Node();
            nodes[state] = newNode;

            newNode->states.insert(state);
            newNode->statesAbove = storm::storage::BitVector((top->statesAbove));
            for (auto const &state : top->states) {
                newNode->statesAbove.set(state);
            }
            node->statesAbove.set(state);
            addedStates->set(state);
            numberOfAddedStates++;
            onlyBottomTopOrder = false;
        }

        void Order::addBelow(uint_fast64_t state, Node *node) {
            assert (!(*addedStates)[state]);
            Node *newNode = new Node();
            nodes[state] = newNode;
            newNode->states.insert(state);
            newNode->statesAbove = storm::storage::BitVector((node->statesAbove));
            for (auto statesAbove : node->states) {
                newNode->statesAbove.set(statesAbove);
            }
            bottom->statesAbove.set(state);
            addedStates->set(state);
            numberOfAddedStates++;
            onlyBottomTopOrder = false;
        }

        void Order::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(compare(above, below) == ABOVE);
            assert (above != nullptr && below != nullptr);
            if (!(*addedStates)[state]) {
                // State is not in the order yet
                Node *newNode = new Node();
                nodes[state] = newNode;

                newNode->states.insert(state);
                newNode->statesAbove = storm::storage::BitVector((above->statesAbove));
                for (auto aboveStates : above->states) {
                    newNode->statesAbove.set(aboveStates);
                }
                below->statesAbove.set(state);
                addedStates->set(state);
                numberOfAddedStates++;
                onlyBottomTopOrder = false;
            } else {
                // State is in the order already, so we add the new relations
                addRelationNodes(above, nodes[state]);
                addRelationNodes(nodes[state], below);
            }
        }

        void Order::addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below) {
            assert (compare(above, below) == ABOVE);
            assert (getNode(below)->states.find(below) != getNode(below)->states.end());
            assert (getNode(above)->states.find(above) != getNode(above)->states.end());

            addBetween(state, getNode(above), getNode(below));
        }

        void Order::addRelation(uint_fast64_t above, uint_fast64_t below) {
            assert (getNode(above) != nullptr && getNode(below) != nullptr);
            addRelationNodes(getNode(above), getNode(below));
        }

        void Order::addRelationNodes(Order::Node *above, Order::Node * below) {
            assert (compare(above, below) != BELOW);

            for (auto const &state : above->states) {
                below->statesAbove.set(state);
            }
            below->statesAbove |= ((above->statesAbove));

            assert (compare(above, below) == ABOVE);
        }

        void Order::addToNode(uint_fast64_t state, Node *node) {
            if (!(*addedStates)[state]) {
                // State is not in the order yet
                node->states.insert(state);
                nodes[state] = node;
                addedStates->set(state);
                numberOfAddedStates++;
            } else {
                // State is in the order already, so we merge the nodes
                mergeNodes(nodes[state], node);
            }
        }

        void Order::mergeNodes(storm::analysis::Order::Node *node1, storm::analysis::Order::Node *node2) {
            // Merges node2 into node 1
            // everything above n2 also above n1
            node1->statesAbove |= ((node2->statesAbove));

            // add states of node 2 to node 1
            node1->states.insert(node2->states.begin(), node2->states.end());

            for(auto const& i : node2->states) {
                nodes[i] = node1;
            }
        }

        void Order::merge(uint_fast64_t var1, uint_fast64_t var2) {
            mergeNodes(getNode(var1), getNode(var2));
        }

        /*** Checking on the order ***/

        Order::NodeComparison Order::compare(uint_fast64_t state1, uint_fast64_t state2){
            return compare(getNode(state1), getNode(state2));
        }

        Order::NodeComparison Order::compare(Node* node1, Node* node2) {
            if (node1 != nullptr && node2 != nullptr) {
                if (node1 == node2) {
                    return SAME;
                }

                if (aboveFast(node1, node2)) {
                    return ABOVE;
                }

                if (aboveFast(node2, node1)) {
                    return BELOW;
                }
                if (above(node1, node2)) {
                    assert(!above(node2, node1));
                    return ABOVE;
                }

                if (above(node2, node1)) {
                    return BELOW;
                }

                // tweak for cyclic pmcs it might be possible that we missed something due to forward reasoning
                //TODO: fix this
                if (doneBuilding) {
                    doneBuilding = false;
                    if (above(node1, node2)) {
                        assert(!above(node2, node1));
                        doneBuilding = true;
                        return ABOVE;
                    }
                    if (above(node2, node1)) {
                        doneBuilding = true;
                        return BELOW;
                    }
                    doneBuilding = true;
                }
            }
            return UNKNOWN;
        }

        bool Order::contains(uint_fast64_t state) const {
            return state < addedStates->size() && (*addedStates)[state];
        }

        storm::storage::BitVector* Order::getAddedStates() const {
            return addedStates;
        }

        Order::Node *Order::getBottom() const {
            return bottom;
        }

        bool Order::getDoneBuilding() const {
            return doneBuilding;
        }

        uint_fast64_t Order::getNextAddedState(uint_fast64_t state) const {
            return addedStates->getNextSetIndex(state + 1);
        }

        Order::Node *Order::getNode(uint_fast64_t stateNumber) const {
            assert (stateNumber < numberOfStates);
            return nodes[stateNumber];
        }

        std::vector<Order::Node*> Order::getNodes() const {
            return nodes;
        }

        Order::Node *Order::getTop() const {
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
            assert (states != nullptr);
            uint_fast64_t numberOfStatesToSort = states->size();
            std::vector<uint_fast64_t> result;
            // Go over all states
            for (auto state : *states) {
                bool unknown = false;
                if (result.size() == 0) {
                    result.push_back(state);
                } else {
                    bool added = false;
                    for (auto itr = result.begin();  itr != result.end(); ++itr) {
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
            assert (result.size() == numberOfStatesToSort);
            return result;
        }

        std::pair<std::pair<uint_fast64_t ,uint_fast64_t>,std::vector<uint_fast64_t>> Order::sortStatesUnorderedPair(const std::vector<uint_fast64_t>* states) {
            assert (states != nullptr);
            uint_fast64_t numberOfStatesToSort = states->size();
            std::vector<uint_fast64_t> result;
            // Go over all states
            for (auto state : *states) {
                bool unknown = false;
                if (result.size() == 0) {
                    result.push_back(state);
                } else {
                    bool added = false;
                    for (auto itr = result.begin();  itr != result.end(); ++itr) {
                        auto compareRes = compare(state, (*itr));
                        if (compareRes == ABOVE || compareRes == SAME) {
                            // insert at current pointer (while keeping other values)
                            result.insert(itr, state);
                            added = true;
                            break;
                        } else if (compareRes == UNKNOWN) {
                            return std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>>(std::pair<uint_fast64_t, uint_fast64_t>((*itr), state), result);
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

            assert (result.size() == numberOfStatesToSort);
            return std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::vector<uint_fast64_t>>(std::pair<uint_fast64_t, uint_fast64_t>(numberOfStates, numberOfStates), result);
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
                    for (auto itr = result.begin();  itr != result.end(); ++itr) {
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
            assert (result.size() == numberOfStatesToSort);
            return result;
        }

        /*** Checking on helpfunctionality for building of order ***/

        void Order::addStateToHandle(uint_fast64_t state) {
            statesToHandle.push_back(state);
        }

        std::vector<uint_fast64_t> Order::getStatesSorted() const {
            return statesSorted;
        }

        std::vector<uint_fast64_t> Order::getStatesToHandle() const {
            return statesToHandle;
        }

        uint_fast64_t Order::getNextSortedState() {
            auto state = numberOfStates;
            if (!statesToHandle.empty()) {
                state = statesToHandle.back();
                statesToHandle.pop_back();
            } else if (!statesSorted.empty()) {
                state = statesSorted.back();
                while (!statesSorted.empty() && (*addedStates)[state]) {
                    statesSorted.pop_back();
                    if (!statesSorted.empty()) {
                        state = statesSorted.back();
                    }
                }
                if (statesSorted.empty()) {
                    state = numberOfStates;
                } else {
                    statesSorted.pop_back();
                }
            }
            return state;
        }

        std::shared_ptr<Order> Order::copy() const {
            // TODO Maybe put some of this into the empty constructor?
            std::shared_ptr<Order> copiedOrder = std::make_shared<Order>();

            copiedOrder->nodes = std::vector<Node *>(numberOfStates, nullptr);
            copiedOrder->onlyBottomTopOrder = this->isOnlyBottomTopOrder();
            copiedOrder->numberOfStates = this->getNumberOfStates();
            copiedOrder->statesToHandle = std::vector<uint_fast64_t>(this->getStatesToHandle());
            copiedOrder->statesSorted = std::vector<uint_fast64_t>(this->getStatesSorted());
            copiedOrder->addedStates = new storm::storage::BitVector(numberOfStates);

            //copy nodes
            copiedOrder->top = new Node();
            copiedOrder->bottom = new Node();
            copiedOrder->top->statesAbove = storm::storage::BitVector(numberOfStates);
            copiedOrder->bottom->statesAbove = storm::storage::BitVector(numberOfStates);

            auto oldNodes = this->getNodes();
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode;
                    if (oldNode == this->getTop()) {
                        newNode = copiedOrder->top;
                    } else if (oldNode == this->getBottom()) {
                        newNode = copiedOrder->bottom;
                    } else {
                        newNode = new Node();
                    }
                    newNode->states = oldNode->states;
                    for (auto const &i : newNode->states) {
                        copiedOrder->addedStates->set(i);
                        copiedOrder->nodes[i] = newNode;
                    }
                    newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
                }
                copiedOrder->numberOfAddedStates = this->addedStates->getNumberOfSetBits();

            }

            copiedOrder->setDoneBuilding(this->getDoneBuilding());

            return copiedOrder;
        }

        /*** Setters ***/

        void Order::setDoneBuilding(bool done) {
            assert (!done || addedStates->full());
            doneBuilding = done;
        }

        /*** Output ***/

        void Order::toDotOutput() const {
            // Graphviz Output start
            STORM_PRINT("Dot Output:" << std::endl << "digraph model {" << std::endl);

            // Vertices of the digraph
            storm::storage::BitVector stateCoverage = storm::storage::BitVector(*addedStates);
            for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i!= numberOfStates; i= stateCoverage.getNextSetIndex(i+1)) {
                for (uint_fast64_t j = i + 1; j < numberOfStates; j++) {
                    if (getNode(j) == getNode(i)) stateCoverage.set(j, false);
                }
                STORM_PRINT("\t" << nodeName(*getNode(i)) << " [ label = \"" << nodeLabel(*getNode(i)) << "\" ];" << std::endl);
            }

            // Edges of the digraph
            for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i!= numberOfStates; i= stateCoverage.getNextSetIndex(i+1)) {
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
                            STORM_PRINT("\t" << nodeName(*currentNode) << " ->  " << nodeName(*getNode(state)) << ";" << std::endl);
                        }
                    }
                }
            }
            // Graphviz Output end
            STORM_PRINT("}" << std::endl);
        }

        void Order::dotOutputToFile(std::ofstream& dotOutfile) const {
            // Graphviz Output start
            dotOutfile << "Dot Output:" << std::endl << "digraph model {" << std::endl;

            // Vertices of the digraph
            storm::storage::BitVector stateCoverage = storm::storage::BitVector(numberOfStates, true);
            for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i!= numberOfStates; i= stateCoverage.getNextSetIndex(i+1)) {
                if (getNode(i) == NULL) {
                    continue;
                }
                for (uint_fast64_t j = i + 1; j < numberOfStates; j++) {
                    if (getNode(j) == getNode(i)) stateCoverage.set(j, false);
                }

                dotOutfile << "\t" << nodeName(*getNode(i)) << " [ label = \"" << nodeLabel(*getNode(i)) << "\" ];" << std::endl;
            }

            // Edges of the digraph
            for (uint_fast64_t i = stateCoverage.getNextSetIndex(0); i!= numberOfStates; i= stateCoverage.getNextSetIndex(i+1)) {
                storm::storage::BitVector v = storm::storage::BitVector(numberOfStates, false);
                Node* currentNode = getNode(i);
                if (currentNode == NULL){
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
                            dotOutfile << "\t" << nodeName(*currentNode) << " ->  " << nodeName(*getNode(state)) << ";" << std::endl;
                        }

                    }
                }

            }

            // Graphviz Output end
            dotOutfile << "}" << std::endl;
        }

        /*** Private methods ***/

        void Order::init(uint_fast64_t numberOfStates, std::vector<uint_fast64_t> *statesSorted, bool doneBuilding) {
            this->numberOfStates = numberOfStates;
            this->nodes = std::vector<Node *>(numberOfStates, nullptr);
            this->addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = doneBuilding;
            this->statesSorted = *statesSorted;
            this->top = new Node();
            this->bottom = new Node();
            this->top->statesAbove = storm::storage::BitVector(numberOfStates);
            this->bottom->statesAbove = storm::storage::BitVector(numberOfStates);
        }

        bool Order::aboveFast(Node* node1, Node* node2) {
            bool found = false;
            for (auto const& state : node1->states) {
                found = ((node2->statesAbove))[state];
                if (found) {
                    break;
                }
            }
            return found;
        }

        bool Order::above(Node *node1, Node *node2) {
            bool found = false;
            if (!doneBuilding) {
                storm::storage::BitVector statesSeen((node2->statesAbove));
                for (auto const &i : (node2->statesAbove)) {
                    auto nodeI = getNode(i);
                    if (((nodeI->statesAbove) & statesSeen) != (nodeI->statesAbove)) {
                        found = above(node1, nodeI, node2, &statesSeen);
                    }
                    if (found) {
                        for (auto const& state:node1->states) {
                            node2->statesAbove.set(state);
                        }
                        break;
                    }
                }
            }
            return found;
        }

        bool Order::above(storm::analysis::Order::Node *node1, storm::analysis::Order::Node *node2, storm::analysis::Order::Node *nodePrev, storm::storage::BitVector *statesSeen) {
            bool found = false;
            for (auto const& state : node1->states) {
                found = ((node2->statesAbove))[state];
                if (found) {
                    break;
                }
            }

            if (!found) {
                nodePrev->statesAbove|=((node2->statesAbove));
                statesSeen->operator|=(((node2->statesAbove)));

                for (auto const &i: node2->statesAbove) {
                    if (!(*statesSeen)[i]) {
                        auto nodeI = getNode(i);
                        if (((nodeI->statesAbove) & *statesSeen) != (nodeI->statesAbove)) {
                            found = above(node1, nodeI, node2, statesSeen);
                        }
                    }
                    if (found) {
                        break;
                    }
                }
            }
            return found;
        }

        std::string Order::nodeName(Node n) const {
            auto itr = n.states.begin();
            std::string name = "n" + std::to_string(*itr);
            return name;
        }

        std::string Order::nodeLabel(Node n) const {
            if (n.states == top->states) return "=)";
            if (n.states == bottom->states) return "=(";
            auto itr = n.states.begin();
            std::string label = "s" + std::to_string(*itr);
            ++itr;
            if (itr != n.states.end()) label = "[" + label + "]";
            return label;
        }

        bool Order::existsNextSortedState() {
            return !statesToHandle.empty() || !statesSorted.empty();
        }

    }
}
