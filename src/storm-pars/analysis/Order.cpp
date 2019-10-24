#include <iostream>
#include <fstream>
#include <algorithm>

#include "Order.h"

namespace storm {
    namespace analysis {
        Order::Order(storm::storage::BitVector* topStates,
                         storm::storage::BitVector* bottomStates,
                         storm::storage::BitVector* initialMiddleStates,
                         uint_fast64_t numberOfStates,
                         std::vector<uint_fast64_t>* statesSorted) {
            nodes = std::vector<Node *>(numberOfStates);

            this->numberOfStates = numberOfStates;
            this->addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = false;
            assert (statesSorted != nullptr);
            this->statesSorted = *statesSorted;
            this->statesToHandle = initialMiddleStates;

            top = new Node();
            bottom = new Node();

            top->statesAbove = storm::storage::BitVector(numberOfStates);
            bottom->statesAbove = storm::storage::BitVector(numberOfStates);

            for (auto const& i : *topStates) {
                addedStates->set(i);
                bottom->statesAbove.set(i);
                top->states.insert(i);
                nodes[i] = top;
            }

            for (auto const& i : *bottomStates) {
                addedStates->set(i);
                bottom->states.insert(i);
                nodes[i] = bottom;
            }

            for (auto const &state : *initialMiddleStates) {
                add(state);
            }
        }

        Order::Order(uint_fast64_t topState, uint_fast64_t bottomState, uint_fast64_t numberOfStates, std::vector<uint_fast64_t>* statesSorted) {
            nodes = std::vector<Node *>(numberOfStates);

            this->numberOfStates = numberOfStates;
            this->addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = false;
            this->statesSorted = *statesSorted;
            this->statesToHandle = new storm::storage::BitVector(numberOfStates);

            top = new Node();
            bottom = new Node();

            top->statesAbove = storm::storage::BitVector(numberOfStates);
            bottom->statesAbove = storm::storage::BitVector(numberOfStates);

            addedStates->set(topState);
            bottom->statesAbove.set(topState);
            top->states.insert(topState);
            nodes[topState] = top;

            addedStates->set(bottomState);
            bottom->states.insert(bottomState);
            nodes[bottomState] = bottom;
            assert (addedStates->getNumberOfSetBits() == 2);
        }

        Order::Order(Order* order) {
            numberOfStates = order->getAddedStates()->size();
            nodes = std::vector<Node *>(numberOfStates);
            addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = order->getDoneBuilding();

            auto oldNodes = order->getNodes();
            // Create nodes
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode = new Node();
                    newNode->states = oldNode->states;
                    for (auto const& i : newNode->states) {
                        addedStates->set(i);
                        nodes[i] = newNode;
                    }
                    if (oldNode == order->getTop()) {
                        top = newNode;
                    } else if (oldNode == order->getBottom()) {
                        bottom = newNode;
                    }
                }
            }
            assert(*addedStates == *(order->getAddedStates()));

            // set all states above and below
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode = getNode(*(oldNode->states.begin()));
                    newNode->statesAbove = storm::storage::BitVector((oldNode->statesAbove));
                }
            }

            auto statesSortedOrder = order->getStatesSorted();
            for (auto itr = statesSortedOrder.begin(); itr != statesSortedOrder.end(); ++itr) {
                this->statesSorted.push_back(*itr);
            }
            this->statesToHandle = new storm::storage::BitVector(*(order->statesToHandle));
        }

        void Order::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(!(*addedStates)[state]);
            assert(compare(above, below) == ABOVE);
            Node *newNode = new Node();
            nodes[state] = newNode;

            newNode->states.insert(state);
            newNode->statesAbove = storm::storage::BitVector((above->statesAbove));
            for (auto const& state : above->states) {
                newNode->statesAbove.set(state);
            }
            below->statesAbove.set(state);
            addedStates->set(state);
        }

        void Order::addBetween(uint_fast64_t state, uint_fast64_t above, uint_fast64_t below) {
            assert(!(*addedStates)[state]);
            assert(compare(above, below) == ABOVE);

            assert (getNode(below)->states.find(below) != getNode(below)->states.end());
            assert (getNode(above)->states.find(above) != getNode(above)->states.end());
            addBetween(state, getNode(above), getNode(below));

        }

        void Order::addToNode(uint_fast64_t state, Node *node) {
            assert(!(*addedStates)[state]);
            node->states.insert(state);
            nodes[state] = node;
            addedStates->set(state);
        }

        void Order::add(uint_fast64_t state) {
            assert(!(*addedStates)[state]);
            addBetween(state, top, bottom);
        }

        void Order::addRelationNodes(Order::Node *above, Order::Node * below) {
            assert (compare(above, below) == UNKNOWN);
            for (auto const& state : above->states) {
                below->statesAbove.set(state);
            }
            below->statesAbove|=((above->statesAbove));
            assert (compare(above, below) == ABOVE);
        }

        void Order::addRelation(uint_fast64_t above, uint_fast64_t below) {
            addRelationNodes(getNode(above), getNode(below));
        }

        Order::NodeComparison Order::compare(uint_fast64_t state1, uint_fast64_t state2) {
            return compare(getNode(state1), getNode(state2));
        }

        Order::NodeComparison Order::compare(Node* node1, Node* node2) {
            if (node1 != nullptr && node2 != nullptr) {
                if (node1 == node2) {
                    return SAME;
                }

                if (above(node1, node2)) {
                    assert(!above(node2, node1));
                    return ABOVE;
                }

                if (above(node2, node1)) {
                    return BELOW;
                }

                // tweak for cyclic pmcs
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
                }
            }
            return UNKNOWN;
        }


        bool Order::contains(uint_fast64_t state) {
            return state < addedStates->size() && addedStates->get(state);
        }

        Order::Node *Order::getNode(uint_fast64_t stateNumber) {
            assert (stateNumber < numberOfStates);
            return nodes.at(stateNumber);
        }

        Order::Node *Order::getTop() {
            return top;
        }

        Order::Node *Order::getBottom() {
            return bottom;
        }

        std::vector<Order::Node*> Order::getNodes() {
            return nodes;
        }

        storm::storage::BitVector* Order::getAddedStates() {
            return addedStates;
        }

        bool Order::getDoneBuilding() {
            return doneBuilding;
        }

        void Order::setDoneBuilding(bool done) {
            doneBuilding = done;
        }

        std::vector<uint_fast64_t> Order::sortStates(storm::storage::BitVector* states) {
            uint_fast64_t numberOfSetBits = states->getNumberOfSetBits();
            auto stateSize = states->size();
            auto result = std::vector<uint_fast64_t>(numberOfSetBits, stateSize);
            for (auto state : *states) {
                if (result[0] == stateSize) {
                    result[0] = state;
                } else {
                    uint_fast64_t i = 0;
                    bool added = false;
                    while (i < numberOfSetBits && !added) {
                        if (result[i] == stateSize) {
                            result[i] = state;
                            added = true;
                        } else {
                            auto compareRes = compare(state, result[i]);
                            if (compareRes == ABOVE) {
                                auto temp = result[i];
                                result[i] = state;
                                for (uint_fast64_t j = i + 1; j < numberOfSetBits && result[j + 1] != stateSize; j++) {
                                    auto temp2 = result[j];
                                    result[j] = temp;
                                    temp = temp2;
                                }
                                added = true;
                            } else if (compareRes == UNKNOWN) {
                                break;
                            } else if (compareRes == SAME) {
                                ++i;
                                auto temp = result[i];
                                result[i] = state;
                                for (uint_fast64_t j = i + 1; j < numberOfSetBits && result[j + 1] != stateSize; j++) {
                                    auto temp2 = result[j];
                                    result[j] = temp;
                                    temp = temp2;
                                }
                                added = true;
                            }
                        }
                        ++i;
                    }
                }
            }

            return result;
        }

        bool Order::above(Node *node1, Node *node2) {
            bool found = false;
            for (auto const& state : node1->states) {
                found = ((node2->statesAbove))[state];
                if (found) {
                    break;
                }
            }

            if (!found && !doneBuilding) {
                storm::storage::BitVector statesSeen((node2->statesAbove));
                for (auto const &i: (node2->statesAbove)) {
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

        bool Order::above(storm::analysis::Order::Node *node1, storm::analysis::Order::Node *node2,
                            storm::analysis::Order::Node *nodePrev, storm::storage::BitVector *statesSeen) {
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

        void Order::mergeNodes(storm::analysis::Order::Node *node1, storm::analysis::Order::Node *node2) {
            // Merges node2 into node 1
            // everything above n2 also above n1
            node1->statesAbove|=((node2->statesAbove));
            // everything below node 2 also below node 1

            // add states of node 2 to node 1
            node1->states.insert(node2->states.begin(), node2->states.end());

            for(auto const& i : node2->states) {
                nodes[i] = node1;
            }
        }

        void Order::merge(uint_fast64_t var1, uint_fast64_t var2) {
            mergeNodes(getNode(var1), getNode(var2));
        }

        std::vector<uint_fast64_t> Order::getStatesSorted() {
            return statesSorted;
        }

        uint_fast64_t Order::getNextSortedState() {
            if (statesSorted.begin() != statesSorted.end()) {
                return *(statesSorted.begin());
            } else {
                return numberOfStates;
            }
        }

        void Order::removeFirstStatesSorted() {
            statesSorted.erase(statesSorted.begin());
        }

        void Order::removeStatesSorted(uint_fast64_t state) {
            assert(containsStatesSorted(state));
            statesSorted.erase(std::find(statesSorted.begin(), statesSorted.end(), state));
        }

        bool Order::containsStatesSorted(uint_fast64_t state) {
            return std::find(statesSorted.begin(), statesSorted.end(), state) != statesSorted.end();
        }
    }
}
