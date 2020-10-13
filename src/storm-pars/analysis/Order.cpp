#include "Order.h"

#include <iostream>
#include <storm/utility/macros.h>


namespace storm {
    namespace analysis {
        Order::Order(storm::storage::BitVector* topStates, storm::storage::BitVector* bottomStates, uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted) {
            init(numberOfStates, decomposition);
            this->numberOfAddedStates = 0;
            this->onlyBottomTopOrder = true;
            for (auto const& i : *topStates) {
                this->addedStates.set(i);
                this->doneStates.set(i);
                this->bottom->statesAbove.set(i);
                this->top->states.insert(i);
                this->nodes[i] = top;
                numberOfAddedStates++;
            }
            this->statesSorted = statesSorted;

            for (auto const& i : *bottomStates) {
                this->addedStates.set(i);
                this->doneStates.set(i);
                this->bottom->states.insert(i);
                this->nodes[i] = bottom;
                numberOfAddedStates++;
            }
            assert (addedStates.getNumberOfSetBits() == (topStates->getNumberOfSetBits() + bottomStates->getNumberOfSetBits()));
        }

        Order::Order(uint_fast64_t topState, uint_fast64_t bottomState, uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, std::vector<uint_fast64_t> statesSorted) {
            init(numberOfStates, decomposition);

            this->onlyBottomTopOrder = true;
            this->addedStates.set(topState);
            this->doneStates.set(topState);

            this->bottom->statesAbove.set(topState);
            this->top->states.insert(topState);
            this->nodes[topState] = top;

            this->addedStates.set(bottomState);
            this->doneStates.set(bottomState);

            this->bottom->states.insert(bottomState);
            this->nodes[bottomState] = bottom;
            this->numberOfAddedStates = 2;
            this->statesSorted = statesSorted;
            assert (addedStates.getNumberOfSetBits() == 2);
        }

        Order::Order(){

        }

        /*** Modifying the order ***/

        void Order::add(uint_fast64_t state) {
            assert (!(addedStates)[state]);
            addBetween(state, top, bottom);
            addStateToHandle(state);
        }

        void Order::addAbove(uint_fast64_t state, Node *node) {
            assert (!(addedStates)[state]);
            Node *newNode = new Node();
            nodes[state] = newNode;

            newNode->states.insert(state);
            newNode->statesAbove = storm::storage::BitVector((top->statesAbove));
            for (auto const &state : top->states) {
                newNode->statesAbove.set(state);
            }
            node->statesAbove.set(state);
            addedStates.set(state);
            numberOfAddedStates++;
            onlyBottomTopOrder = false;
        }

        void Order::addBelow(uint_fast64_t state, Node *node) {
            assert (!(addedStates)[state]);
            Node *newNode = new Node();
            nodes[state] = newNode;
            newNode->states.insert(state);
            newNode->statesAbove = storm::storage::BitVector((node->statesAbove));
            for (auto statesAbove : node->states) {
                newNode->statesAbove.set(statesAbove);
            }
            bottom->statesAbove.set(state);
            addedStates.set(state);
            numberOfAddedStates++;
            onlyBottomTopOrder = false;
        }

        void Order::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(compare(above, below) == ABOVE);
            assert (above != nullptr && below != nullptr);
            if (!(addedStates)[state]) {
                // State is not in the order yet
                Node *newNode = new Node();
                nodes[state] = newNode;

                newNode->states.insert(state);
                newNode->statesAbove = storm::storage::BitVector((above->statesAbove));
                for (auto aboveStates : above->states) {
                    newNode->statesAbove.set(aboveStates);
                }
                below->statesAbove.set(state);
                addedStates.set(state);
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

        void Order::addRelation(uint_fast64_t above, uint_fast64_t below, bool allowMerge) {
            assert (getNode(above) != nullptr && getNode(below) != nullptr);
            addRelationNodes(getNode(above), getNode(below), allowMerge);
        }

        void Order::addRelationNodes(Order::Node *above, Order::Node * below, bool allowMerge) {
            assert (allowMerge || compare(above, below) != BELOW);
            if (allowMerge) {
                if (compare(below, above) == ABOVE) {
                    mergeNodes(above, below);
                    return;
                }
            }
            for (auto const &state : above->states) {
                below->statesAbove.set(state);
            }
            below->statesAbove |= ((above->statesAbove));
            assert (compare(above, below) == ABOVE);
        }

        void Order::addToNode(uint_fast64_t state, Node *node) {
            if (!(addedStates)[state]) {
                // State is not in the order yet
                node->states.insert(state);
                nodes[state] = node;
                addedStates.set(state);
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

        }

        void Order::merge(uint_fast64_t var1, uint_fast64_t var2) {
            mergeNodes(getNode(var1), getNode(var2));
        }

        /*** Checking on the order ***/

        Order::NodeComparison Order::compare(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis){
            auto res = compare(getNode(state1), getNode(state2), hypothesis);
            return res;
        }

        Order::NodeComparison Order::compareFast(uint_fast64_t state1, uint_fast64_t state2, NodeComparison hypothesis){
            auto res = compareFast(getNode(state1), getNode(state2), hypothesis);
            return res;
        }

        Order::NodeComparison Order::compareFast(Node* node1, Node* node2, NodeComparison hypothesis) {
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
            }  else if (node1 == top || node2 == bottom) {
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

                // tweak for cyclic pmcs it might be possible that we missed something due to forward reasoning
                //TODO: fix this
                if (!trivialStates.full() && doneBuilding) {
                    doneBuilding = false;
                    if ((hypothesis == UNKNOWN || hypothesis == ABOVE) && above(node1, node2)) {
                        assert(!above(node2, node1));
                        doneBuilding = true;
                        return ABOVE;
                    }
                    if ((hypothesis == UNKNOWN || hypothesis == BELOW) || above(node2, node1)) {
                        doneBuilding = true;
                        return BELOW;
                    }
                    doneBuilding = true;
                }
            } else if (node1 == top || node2 == bottom) {
                return ABOVE;
            } else if (node2 == top || node1 == bottom) {
                return BELOW;
            }
            return UNKNOWN;
        }

        bool Order::contains(uint_fast64_t state) const {
            return state < addedStates.size() && (addedStates)[state];
        }

        storm::storage::BitVector Order::getAddedStates() const {
            return addedStates;
        }

        Order::Node *Order::getBottom() const {
            return bottom;
        }

        bool Order::getDoneBuilding() const {
            return doneBuilding;
        }

        uint_fast64_t Order::getNextAddedState(uint_fast64_t state) const {
            return addedStates.getNextSetIndex(state + 1);
        }

        uint_fast64_t Order::getNextDoneState(uint_fast64_t state) const {
            return doneStates.getNextSetIndex(state + 1);
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

        std::pair<std::pair<uint_fast64_t,uint_fast64_t>, std::vector<uint_fast64_t>> Order::sortStatesForForward(uint_fast64_t currentState, std::vector<uint_fast64_t> const& states) {
            std::vector<uint_fast64_t> statesSorted;
            statesSorted.push_back(currentState);
            // Go over all states
            bool oneUnknown = false;
            bool unknown = false;
            uint_fast64_t s1 = numberOfStates;
            uint_fast64_t s2 = numberOfStates;
            for (auto & state : states) {
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
                if (!added ) {
                    // State will be last in the list
                    statesSorted.push_back(state);
                }
                if (unknown && oneUnknown) {
                    break;
                }
            }
            if (!unknown && oneUnknown) {
                assert (statesSorted.size() == states.size());
                s2 = numberOfStates;
            }
            assert (s1 == numberOfStates || (s1 != numberOfStates && s2 == numberOfStates && statesSorted.size() == states.size()) || (s1 !=numberOfStates && s2 != numberOfStates && statesSorted.size() < states.size()));

            return {{s1, s2}, statesSorted};
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

            assert (result.size() == numberOfStatesToSort);
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

        std::shared_ptr<Order> Order::copy() const {
            std::shared_ptr<Order> copiedOrder = std::make_shared<Order>();
            copiedOrder->nodes = std::vector<Node *>(numberOfStates, nullptr);
            copiedOrder->onlyBottomTopOrder = this->isOnlyBottomTopOrder();
            copiedOrder->numberOfStates = this->getNumberOfStates();
            copiedOrder->addedStates = storm::storage::BitVector(addedStates);
            copiedOrder->statesSorted = std::vector<uint_fast64_t>(this->statesSorted);
            copiedOrder->trivialStates = storm::storage::BitVector(trivialStates);
            copiedOrder->doneStates = storm::storage::BitVector(doneStates);

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
                        copiedOrder->addedStates.set(i);
                        copiedOrder->nodes[i] = newNode;
                    }
                    newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
                }
                copiedOrder->numberOfAddedStates = this->addedStates.getNumberOfSetBits();
            }

            copiedOrder->setDoneBuilding(this->getDoneBuilding());
            return copiedOrder;
        }

        /*** Setters ***/

        void Order::setDoneBuilding(bool done) {
            assert (!done || addedStates.full());
            doneBuilding = done;
        }

        void Order::setAddedState(uint_fast64_t stateNumber) {
            doneStates.set(stateNumber);
        }

        /*** Output ***/

        void Order::toDotOutput() const {
            // Graphviz Output start
            STORM_PRINT("Dot Output:" << std::endl << "digraph model {" << std::endl);

            // Vertices of the digraph
            storm::storage::BitVector stateCoverage = storm::storage::BitVector(addedStates);
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

        void Order::init(uint_fast64_t numberOfStates, storage::Decomposition<storage::StronglyConnectedComponent> decomposition, bool doneBuilding) {
            this->numberOfStates = numberOfStates;
            this->nodes = std::vector<Node *>(numberOfStates, nullptr);
            this->addedStates = storm::storage::BitVector(numberOfStates, false);
            this->doneStates = storm::storage::BitVector(numberOfStates, false);
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
            this->doneBuilding = doneBuilding;
            this->top = new Node();
            this->bottom = new Node();
            this->top->statesAbove = storm::storage::BitVector(numberOfStates);
            this->bottom->statesAbove = storm::storage::BitVector(numberOfStates);
            this->dummySCC = storage::StronglyConnectedComponent();

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

        bool Order::existsNextState() {
            return !doneStates.full();
        }

        bool Order::isTrivial(uint_fast64_t state) {
            return trivialStates[state];
        }

        std::pair<uint_fast64_t, bool> Order::getNextStateNumber() {
            if (!statesSorted.empty()) {
                auto state = statesSorted.back();
                statesSorted.pop_back();
                return {state, true};
            } else {
                return {numberOfStates, true};
            }
        }

        std::pair<uint_fast64_t, bool> Order::getStateToHandle() {
            assert (existsStateToHandle());
            auto state = statesToHandle.back();
            statesToHandle.pop_back();
            while (!statesToHandle.empty() && doneStates[state]) {
                state = statesToHandle.back();
                statesToHandle.pop_back();
            }
            return {state, false};
        }

        bool Order::existsStateToHandle() {
            return !statesToHandle.empty();
        }

        void Order::addStateToHandle(uint_fast64_t state) {
            statesToHandle.push_back(state);
        }

        void Order::addStateSorted(uint_fast64_t state) {
            statesSorted.push_back(state);
        }

        std::pair<bool, bool> Order::allAboveBelow(std::vector<uint_fast64_t> const states, uint_fast64_t state) {
            auto allAbove = true;
            auto allBelow = true;
            for (auto & checkState : states) {
                auto comp = compare(checkState, state);
                allAbove &= (comp == ABOVE || comp == SAME);
                allBelow &= (comp == BELOW || comp == SAME);
            }
            return {allAbove, allBelow};
        }

        uint_fast64_t Order::getNumberOfDoneStates() const {
            return doneStates.getNumberOfSetBits();
        }
    }
}
