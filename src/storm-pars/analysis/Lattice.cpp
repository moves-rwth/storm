//
// Created by Jip Spel on 24.07.18.
//

#include <iostream>
#include <fstream>
#include "Lattice.h"

namespace storm {
    namespace analysis {
        Lattice::Lattice(storm::storage::BitVector* topStates,
                         storm::storage::BitVector* bottomStates,
                         storm::storage::BitVector* initialMiddleStates,
                         uint_fast64_t numberOfStates) {
//            assert(topStates->getNumberOfSetBits() != 0);
//            assert(bottomStates->getNumberOfSetBits() != 0);
//            assert((*topStates & *bottomStates).getNumberOfSetBits() == 0);
            nodes = std::vector<Node *>(numberOfStates);

            this->numberOfStates = numberOfStates;
            this->addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = false;

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


//            setStatesBelow(top, bottomStates, false);

//            bottom->statesBelow = storm::storage::BitVector(numberOfStates);
//            setStatesAbove(bottom, topStates, false);


            for (auto const &state : *initialMiddleStates) {
                add(state);
            }
        }

        Lattice::Lattice(Lattice* lattice) {
//            assert (false);
            numberOfStates = lattice->getAddedStates()->size();
            nodes = std::vector<Node *>(numberOfStates);
            addedStates = new storm::storage::BitVector(numberOfStates);
            this->doneBuilding = lattice->getDoneBuilding();

            auto oldNodes = lattice->getNodes();
            // Create nodes
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode = new Node();
                    // TODO: gaat dit goed of moet er een constructor om
                    newNode->states = oldNode->states;
                    for (auto const& i : newNode->states) {
                        addedStates->set(i);
                        nodes[i] = newNode;
                    }
                    if (oldNode == lattice->getTop()) {
                        top = newNode;
                    } else if (oldNode == lattice->getBottom()) {
                        bottom = newNode;
                    }
                }
            }

            assert(addedStates == lattice->getAddedStates());

            // set all states above and below
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr && oldNode != lattice->getTop() && oldNode != lattice->getBottom()) {
                    Node *newNode = getNode(*(oldNode->states.begin()));
                    newNode->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
//                    setStatesAbove(newNode, oldNode->statesAbove, false);
//                    setStatesBelow(newNode, oldNode->statesBelow, false);
                } else if (oldNode != nullptr && oldNode == lattice->getBottom()) {
                    bottom->statesAbove = storm::storage::BitVector(oldNode->statesAbove);
//                    setStatesAbove(bottom, lattice->getBottom()->statesAbove, false);
//                    bottom->statesBelow = storm::storage::BitVector(numberOfStates);
//                } else if (oldNode != nullptr && oldNode == lattice->getTop()) {
//                    top->statesAbove = storm::storage::BitVector(numberOfStates);
//                    setStatesBelow(top, lattice->getTop()->statesBelow, false);
                }
            }
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(!(*addedStates)[state]);
            assert(compare(above, below) == ABOVE);

            Node *newNode = new Node();
            nodes[state] = newNode;

//            newNode->states = storm::storage::BitVector(numberOfStates);
            newNode->states.insert(state);
            newNode->statesAbove = above->statesAbove;
            for (auto const& state : above->states) {
                newNode->statesAbove.set(state);
            }
            below->statesAbove.set(state);
//            comparisons[state] = comparisons[above->states.getNextSetIndex(0)];
//            setStatesAbove(newNode, above->statesAbove | above->states, false);
//            setStatesBelow(newNode, below->statesBelow | below->states, false);
//            setStatesBelow(above, state, true);
//            setStatesAbove(below, state, true);

//            for (auto i = below->statesBelow.getNextSetIndex(0); i < below->statesBelow.size(); i = below->statesBelow.getNextSetIndex(i + 1)) {
//                setStatesAbove(getNode(i), state, true);
//            }
//
//            for (auto i = above->statesAbove.getNextSetIndex(0); i < above->statesAbove.size(); i = above->statesAbove.getNextSetIndex(i + 1)) {
//                setStatesBelow(getNode(i), state, true);
//            }

            addedStates->set(state);

        }

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
            assert(!(*addedStates)[state]);
            node->states.insert(state);
            nodes[state] = node;
            addedStates->set(state);
//            comparisons[state] = comparisons[node->states.getNextSetIndex(0)];

//            for (auto i = node->statesBelow.getNextSetIndex(0); i < node->statesBelow.size(); i = node->statesBelow.getNextSetIndex(i + 1)) {
//                setStatesAbove(getNode(i), state, true);
//            }
//
//            for (auto i = node->statesAbove.getNextSetIndex(0); i < node->statesAbove.size(); i = node->statesAbove.getNextSetIndex(i + 1)) {
//                setStatesBelow(getNode(i), state, true);
//            }
        }

        void Lattice::add(uint_fast64_t state) {
            assert(!(*addedStates)[state]);
            addBetween(state, top, bottom);
        }

        void Lattice::addRelationNodes(Lattice::Node *above, Lattice::Node * below) {
            assert (compare(above, below) == UNKNOWN);
            // TODO: welke assert
//            assert ((above->statesAbove & below->statesBelow).getNumberOfSetBits() == 0);
//            setStatesBelow(above, below->states | below->statesBelow, true);
//            setStatesAbove(below, above->states | above->statesAbove, true);
            for (auto const& state : above->states) {
                below->statesAbove.set(state);
            }
            below->statesAbove |= above->statesAbove;
            // TODO: comparisons

//            for (auto i = below->statesBelow.getNextSetIndex(0); i < below->statesBelow.size(); i = below->statesBelow.getNextSetIndex(i + 1)) {
//                setStatesAbove(getNode(i), above->states | above->statesAbove, true);
//            }
//
//            for (auto i = above->statesAbove.getNextSetIndex(0); i < above->statesAbove.size(); i = above->statesAbove.getNextSetIndex(i + 1)) {
//                setStatesBelow(getNode(i), below->states | below->statesBelow, true);
//            }

        }

        int Lattice::compare(uint_fast64_t state1, uint_fast64_t state2) {
            return compare(getNode(state1), getNode(state2));
        }

        int Lattice::compare(Node* node1, Node* node2) {
            if (node1 != nullptr && node2 != nullptr) {
                if (node1 == node2) {
                    return SAME;
                }

//                auto state1 = node1->states.getNextSetIndex(0);
//                auto state2 = node2->states.getNextSetIndex(0);
//
//                auto mapEntry = comparisons[state1];
//                if (mapEntry.find(state2) != mapEntry.end()) {
//                    return mapEntry[state2];
//                }

                if (above(node1, node2)) {
                    assert(!above(node2, node1));
//                    comparisons[state1][state2] = ABOVE;
//                    comparisons[state2][state1] = BELOW;
                    return ABOVE;
                }

                if (above(node2, node1)) {
//                    comparisons[state1][state2] = BELOW;
//                    comparisons[state2][state1] = ABOVE;
                    return BELOW;
                }
            }
            return UNKNOWN;
        }

        Lattice::Node *Lattice::getNode(uint_fast64_t stateNumber) {
            return nodes.at(stateNumber);
        }

        Lattice::Node *Lattice::getTop() {
            return top;
        }

        Lattice::Node *Lattice::getBottom() {
            return bottom;
        }

        std::vector<Lattice::Node*> Lattice::getNodes() {
            return nodes;
        }

        storm::storage::BitVector* Lattice::getAddedStates() {
            return addedStates;
        }

        bool Lattice::getDoneBuilding() {
            return doneBuilding;
        }

        void Lattice::setDoneBuilding(bool done) {
            doneBuilding = done;
        }

        std::vector<uint_fast64_t> Lattice::sortStates(storm::storage::BitVector* states) {
            auto numberOfSetBits = states->getNumberOfSetBits();
            auto result = std::vector<uint_fast64_t>(numberOfSetBits, -1);
            for (auto state : *states) {
                if (result[0] == -1) {
                    result[0] = state;
                } else {
                    for (auto i = 0; i < numberOfSetBits && result[i] != -1; i++) {
                        auto compareRes = compare(state, result[i]);
                        if (compareRes == Lattice::ABOVE) {
                            for (auto j = i; j < numberOfSetBits -1 && result[j+1] != -1; j++) {
                                auto temp = result[j];
                                result[j] = state;
                                result[j+1] = temp;
                            }
                        } else if (compareRes == Lattice::UNKNOWN) {
                            break;
                        } else if (compareRes == Lattice::SAME) {
                            for (auto j = i+1; j < numberOfSetBits -1 && result[j+1] != -1; j++) {
                                auto temp = result[j];
                                result[j] = state;
                                result[j+1] = temp;
                            }
                        }
                    }
                }
            }

            return result;
        }

//        std::set<Lattice::Node*> Lattice::getAbove(uint_fast64_t state) {
//            return getAbove(getNode(state));
//        }
//
//        std::set<Lattice::Node*> Lattice::getBelow(uint_fast64_t state) {
//            return getBelow(getNode(state));
//        }
//
//        std::set<Lattice::Node*> Lattice::getAbove(Lattice::Node* node) {
//            std::set<Lattice::Node*> result({});
//            for (auto i = node->statesAbove.getNextSetIndex(0); i < node->statesAbove.size(); i = node->statesAbove.getNextSetIndex(i + 1)) {
//             result.insert(getNode(i));
//            }
//            return result;
//        }
//
//        std::set<Lattice::Node*> Lattice::getBelow(Lattice::Node* node) {
//            std::set<Lattice::Node*> result({});
//            for (auto i = node->statesBelow.getNextSetIndex(0); i < node->statesBelow.size(); i = node->statesBelow.getNextSetIndex(i + 1)) {
//                result.insert(getNode(i));
//            }
//            return result;
//        }

        void Lattice::toString(std::ostream &out) {
//            assert (false);
//            std::vector<Node*> printedNodes = std::vector<Node*>({});
//            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
//
//                if ((*itr) != nullptr && std::find(printedNodes.begin(), printedNodes.end(), (*itr)) == printedNodes.end()) {
//                    Node *node = *itr;
//                    printedNodes.push_back(*itr);
//                    out << "Node: {";
//                    uint_fast64_t index = node->states.getNextSetIndex(0);
//                    while (index < node->states.size()) {
//                        out << index;
//                        index = node->states.getNextSetIndex(index + 1);
//                        if (index < node->states.size()) {
//                            out << ", ";
//                        }
//                    }
//                    out << "}" << "\n";
//                    out << "  Address: " << node << "\n";
//                    out << "    Above: {";
//
//                    auto statesAbove = node->statesAbove;
//                    for (auto above : statesAbove) {
//                        Node* nodeAbove = getNode(above);
//                        index = nodeAbove->states.getNextSetIndex(0);
//                        out << "{";
//                        while (index < nodeAbove->states.size()) {
//                            out << index;
//                            index = nodeAbove->states.getNextSetIndex(index + 1);
//                            if (index < nodeAbove->states.size()) {
//                                out << ", ";
//                            }
//                        }
//
//                        out << "}";
//                    }
//                    out << "}" << "\n";


//                    out << "    Below: {";
//                    auto statesBelow = getBelow(node);
//                    for (auto itr2 = statesBelow.begin(); itr2 != statesBelow.end(); ++itr2) {
//                        Node *below = *itr2;
//                        out << "{";
//                        index = below->states.getNextSetIndex(0);
//                        while (index < below->states.size()) {
//                            out << index;
//                            index = below->states.getNextSetIndex(index + 1);
//                            if (index < below->states.size()) {
//                                out << ", ";
//                            }
//                        }
//
//                        out << "}";
//                    }
//                    out << "}" << "\n";
//                }
//            }
        }

        void Lattice::toDotFile(std::ostream &out) {
            assert (false);
//            out << "digraph \"Lattice\" {" << std::endl;
//
//            // print all nodes
//            std::vector<Node*> printed;
//            out << "\t" << "node [shape=ellipse]" << std::endl;
//            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
//
//                if ((*itr) != nullptr && find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
//                    out << "\t\"" << (*itr) << "\" [label = \"";
//                    uint_fast64_t index = (*itr)->states.getNextSetIndex(0);
//                    while (index < (*itr)->states.size()) {
//                        out << index;
//                        index = (*itr)->states.getNextSetIndex(index + 1);
//                        if (index < (*itr)->states.size()) {
//                            out << ", ";
//                        }
//                    }
//
//                    out << "\"]" << std::endl;
//                    printed.push_back(*itr);
//                }
//            }
//
//            // print arcs
//            printed.clear();
//            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
//                if ((*itr) != nullptr && find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
//                    auto below = getBelow(*itr);
//                    for (auto itr2 = below.begin(); itr2 != below.end(); ++itr2) {
//                        out << "\t\"" << (*itr) << "\" -> \"" << (*itr2) << "\";" << std::endl;
//                    }
//                    printed.push_back(*itr);
//                }
//            }
//
//            out << "}" << std::endl;
        }


        bool Lattice::above(Node *node1, Node *node2) {
            // ligt node 1 boven node 2 ?
            // oftewel is er een state in node2.above die met een state in node1 matched
            bool found = false;
            for (auto const& state : node1->states) {
                found = node2->statesAbove[state];
                if (found) {
                    break;
                }
            }

            if (!found && !doneBuilding) {
                storm::storage::BitVector statesSeen(node2->statesAbove);
                for (auto const &i: node2->statesAbove) {
                    auto nodeI = getNode(i);
                    if ((nodeI->statesAbove & statesSeen) != nodeI->statesAbove) {
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

        bool Lattice::above(storm::analysis::Lattice::Node *node1, storm::analysis::Lattice::Node *node2,
                            storm::analysis::Lattice::Node *nodePrev, storm::storage::BitVector *statesSeen) {
//            bool found = (node2->statesAbove & node1->states).getNumberOfSetBits() != 0;
            bool found = false;
            for (auto const& state : node1->states) {
                found = node2->statesAbove[state];
                if (found) {
                    break;
                }
            }
//            bool found = !(node2->statesAbove & node1->states).empty();
            if (!found) {
                // TODO: kan dit niet anders?
                nodePrev->statesAbove |= node2->statesAbove;
                auto complement = (statesSeen->operator~());

                storm::storage::BitVector states = storm::storage::BitVector(
                        (node2->statesAbove & complement));
                statesSeen->operator|=(node2->statesAbove);

                for (auto const &i: states) {
//                assert (!statesSeen[i]);
                    auto nodeI = getNode(i);
                if (!(nodeI->statesAbove & complement).empty()) {
                    found = above(node1, nodeI, node2, statesSeen);
                }
                    if (found) {
                        break;
                    }

                }
            }
            return found;
        }
//
//        void Lattice::setStatesAbove(Lattice::Node *node, uint_fast64_t state, bool alreadyInitialized) {
//            assert (!node->states[state]);
//
//            if (!alreadyInitialized) {
//                node->statesAbove = storm::storage::BitVector(numberOfStates);
//            }
//
//            assert (!node->statesBelow[state]);
//            node->statesAbove.set(state);
//        }
//
//        void Lattice::setStatesBelow(Lattice::Node *node, uint_fast64_t state, bool alreadyInitialized) {
//            assert (!node->states.get(state));
//
//            if (!alreadyInitialized) {
//                node->statesBelow = storm::storage::BitVector(numberOfStates);
//            }
//            assert (!node->statesAbove[state]);
//            node->statesBelow.set(state);
//        }
//
//        void Lattice::setStatesAbove(Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
//            assert((states.getNumberOfSetBits() - (node->states & states).getNumberOfSetBits()) != 0);
//            // the states to add to the above state of the current node shouldnt occur in either statesbelow or states of ndoe
//
//            assert ((node->states & states).getNumberOfSetBits() ==0);
//            if (alreadyInitialized) {
//                assert ((node->statesBelow & states).getNumberOfSetBits() == 0);
//
//                node->statesAbove |= (states);
//            } else {
//                node->statesAbove = (storm::storage::BitVector(states));
//            }
//        }
//
//        void Lattice::setStatesBelow(Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
//            assert((states.getNumberOfSetBits() - (node->states & states).getNumberOfSetBits()) != 0);
//
//            assert ((node->states & states).getNumberOfSetBits() ==0);
//            if (alreadyInitialized) {
//                assert ((node->statesAbove & states).getNumberOfSetBits() == 0);
//                node->statesBelow |= (states);
//            } else {
//                node->statesBelow = (storm::storage::BitVector(states));
//            }
//        }

        void Lattice::mergeNodes(storm::analysis::Lattice::Node *node1, storm::analysis::Lattice::Node *node2) {
//            assert (false);
            // Merges node2 into node 1
            // everything above n2 also above n1
            node1->statesAbove |= node2->statesAbove;
            // everything below node 2 also below node 1
//            node1->statesBelow |= node2->statesBelow;

            // add states of node 2 to node 1
            node1->states.insert(node2->states.begin(), node2->states.end());
            for(auto const& i : node2->states) {
                nodes[i] = node1;
            }

//            // Add all states of combined node to states Above of all states Below of node1
//            for (auto i = node1->statesBelow.getNextSetIndex(0); i < node1->statesBelow.size(); i= node1->statesBelow.getNextSetIndex(i+1)) {
//                getNode(i)->statesAbove |= node1->states | node1->statesAbove;
//            }

            // Add all states of combined node to states Below of all states Above of node1
//            for (auto i = node1->statesAbove.getNextSetIndex(0); i < node1->statesAbove.size(); i= node1->statesAbove.getNextSetIndex(i+1)) {
//                getNode(i)->statesBelow |= node1->states | node1->statesBelow;
//            }

        }
    }
}
