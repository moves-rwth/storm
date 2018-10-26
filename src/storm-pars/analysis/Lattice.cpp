//
// Created by Jip Spel on 24.07.18.
//

#include <iostream>
#include <fstream>
#include "Lattice.h"

namespace storm {
    namespace analysis {
        Lattice::Lattice(storm::storage::BitVector topStates,
                         storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates) {
            assert(topStates.getNumberOfSetBits() != 0);
            assert(bottomStates.getNumberOfSetBits() != 0);
            assert((topStates & bottomStates).getNumberOfSetBits() == 0);
            nodes = std::vector<Node *>(numberOfStates);

            top = new Node();
            top->states = topStates;
            for (auto i = topStates.getNextSetIndex(0); i < topStates.size(); i = topStates.getNextSetIndex(i+1)) {
                nodes.at(i) = top;
            }

            bottom = new Node();
            bottom->states = bottomStates;
            for (auto i = bottomStates.getNextSetIndex(0); i < bottomStates.size(); i = bottomStates.getNextSetIndex(i+1)) {
                nodes.at(i) = bottom;
            }

            top->above = new std::set<Lattice::Node*>({});
            top->statesAbove = storm::storage::BitVector(numberOfStates);
            setStatesBelow(top, bottomStates, false);
            assert(top->statesAbove.size() == numberOfStates);
            assert(top->statesBelow.size() == numberOfStates);
            assert(top->statesAbove.getNumberOfSetBits() == 0);
            assert(top->statesBelow.getNumberOfSetBits() == bottomStates.getNumberOfSetBits());
            assert(top->above->size() == 0);
            assert(top->below->size() == bottomStates.getNumberOfSetBits());

            bottom->below = new std::set<Lattice::Node*>({});
            bottom->statesBelow = storm::storage::BitVector(numberOfStates);
            setStatesAbove(bottom, topStates, false);
            assert(bottom->statesAbove.size() == numberOfStates);
            assert(bottom->statesBelow.size() == numberOfStates);
            assert(bottom->statesBelow.getNumberOfSetBits() == 0);
            assert(bottom->statesAbove.getNumberOfSetBits() == topStates.getNumberOfSetBits());
            assert(bottom->below->size() == 0);
            assert(bottom->above->size() == topStates.getNumberOfSetBits());

            this->numberOfStates = numberOfStates;
            this->addedStates = storm::storage::BitVector(numberOfStates);
            this->addedStates |= (topStates);
            this->addedStates |= (bottomStates);
        }

        Lattice::Lattice(Lattice* lattice) {
            numberOfStates = lattice->getAddedStates().size();
            nodes = std::vector<Node *>(numberOfStates);

            top = new Node();
            top->states = storm::storage::BitVector(lattice->getTop()->states);
            for (auto i = top->states.getNextSetIndex(0); i < top->states.size(); i = top->states.getNextSetIndex(i+1)) {
                nodes.at(i) = top;
            }

            bottom = new Node();
            bottom->states = storm::storage::BitVector(lattice->getBottom()->states);
            for (auto i =  bottom->states.getNextSetIndex(0); i < bottom->states.size(); i =  bottom->states.getNextSetIndex(i+1)) {
                nodes.at(i) = bottom;
            }

            addedStates = storm::storage::BitVector(numberOfStates);
            addedStates |= (top->states);
            addedStates |= (bottom->states);

            auto oldNodes = lattice->getNodes();
            // Create nodes
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr && oldNode != lattice->getTop() && oldNode != lattice->getBottom()) {
                    Node *newNode = new Node();
                    newNode->states = storm::storage::BitVector(oldNode->states);
                    for (auto i = newNode->states.getNextSetIndex(0);
                         i < newNode->states.size(); i = newNode->states.getNextSetIndex(i + 1)) {
                        addedStates.set(i);
                        nodes.at(i) = newNode;
                    }
                }
            }
            assert(addedStates == lattice->getAddedStates());

            // set all states above and below
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr && oldNode != lattice->getTop() && oldNode != lattice->getBottom()) {
                    Node *newNode = getNode(oldNode->states.getNextSetIndex(0));
                    setStatesAbove(newNode, oldNode->statesAbove, false);
                    setStatesBelow(newNode, oldNode->statesBelow, false);
                } else if (oldNode != nullptr && oldNode == lattice->getBottom()) {
                    setStatesAbove(bottom, lattice->getBottom()->statesAbove, false);
                    assert(lattice->getBottom()->statesBelow.getNumberOfSetBits() == 0);
                    bottom->below = new std::set<Lattice::Node*>({});
                    bottom->statesBelow = storm::storage::BitVector(numberOfStates);
                    assert(bottom->statesAbove.size() == numberOfStates);
                    assert(bottom->statesBelow.size() == numberOfStates);
                } else if (oldNode != nullptr && oldNode == lattice->getTop()) {
                    top->above = new std::set<Lattice::Node*>({});
                    top->statesAbove = storm::storage::BitVector(numberOfStates);
                    assert(lattice->getTop()->statesAbove.getNumberOfSetBits() == 0);
                    setStatesBelow(top, lattice->getTop()->statesBelow, false);
                    assert(top->statesAbove.size() == numberOfStates);
                    assert(top->statesBelow.size() == numberOfStates);
                }

                if (oldNode!= nullptr) {
                    Node *newNode = getNode(oldNode->states.getNextSetIndex(0));
                    assert((newNode->statesAbove & newNode->statesBelow).getNumberOfSetBits() == 0);
                    assert(newNode->statesAbove == oldNode->statesAbove);
                    assert(newNode->statesBelow == oldNode->statesBelow);
                    assert(newNode->above->size() == oldNode->above->size());
                    assert(newNode->below->size() == oldNode->below->size());
                }
            }
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(!addedStates[state]);
            assert(compare(above, below) == ABOVE);

            Node *newNode = new Node();
            nodes.at(state) = newNode;

            newNode->states = storm::storage::BitVector(numberOfStates);
            newNode->states.set(state);
            setStatesAbove(newNode, above->statesAbove | above->states, false);
            setStatesBelow(newNode, below->statesBelow | below->states, false);
            setStatesBelow(above, state, true);
            setStatesAbove(below, state, true);

            for (auto i = below->statesBelow.getNextSetIndex(0); i < below->statesBelow.size(); i = below->statesBelow.getNextSetIndex(i + 1)) {
                setStatesAbove(getNode(i), state, true);
            }

            for (auto i = above->statesAbove.getNextSetIndex(0); i < above->statesAbove.size(); i = above->statesAbove.getNextSetIndex(i + 1)) {
                setStatesBelow(getNode(i), state, true);
            }
            addedStates.set(state);
        }

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
            assert(!addedStates[state]);
            node->states.set(state);
            nodes.at(state) = node;
            addedStates.set(state);
            for (auto i = node->statesBelow.getNextSetIndex(0); i < node->statesBelow.size(); i = node->statesBelow.getNextSetIndex(i + 1)) {
                setStatesAbove(getNode(i), state, true);
            }

            for (auto i = node->statesAbove.getNextSetIndex(0); i < node->statesAbove.size(); i = node->statesAbove.getNextSetIndex(i + 1)) {
                setStatesBelow(getNode(i), state, true);
            }
        }

        void Lattice::add(uint_fast64_t state) {
            addBetween(state, top, bottom);
        }

        void Lattice::addRelationNodes(Lattice::Node *above, Lattice::Node * below) {
            assert(compare(above, below) == UNKNOWN);
            assert((above->statesAbove & below->statesBelow).getNumberOfSetBits() == 0);

            setStatesBelow(above, below->states, true);
            setStatesAbove(below, above->states, true);

            for (auto i = below->statesBelow.getNextSetIndex(0); i < below->statesBelow.size(); i = below->statesBelow.getNextSetIndex(i + 1)) {
                setStatesAbove(getNode(i), above->states, true);
            }

            for (auto i = above->statesAbove.getNextSetIndex(0); i < above->statesAbove.size(); i = above->statesAbove.getNextSetIndex(i + 1)) {
                setStatesBelow(getNode(i), below->states, true);
            }
        }

        int Lattice::compare(uint_fast64_t state1, uint_fast64_t state2) {
            return compare(getNode(state1), getNode(state2));
        }

        int Lattice::compare(Node* node1, Node* node2) {
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

        storm::storage::BitVector Lattice::getAddedStates() {
            return addedStates;
        }

        void Lattice::toString(std::ostream &out) {
            std::vector<Node*> printedNodes = std::vector<Node*>({});
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {

                if ((*itr) != nullptr && std::find(printedNodes.begin(), printedNodes.end(), (*itr)) == printedNodes.end()) {
                    Node *node = *itr;
                    printedNodes.push_back(*itr);
                    out << "Node: {";
                    uint_fast64_t index = node->states.getNextSetIndex(0);
                    while (index < node->states.size()) {
                        out << index;
                        index = node->states.getNextSetIndex(index + 1);
                        if (index < node->states.size()) {
                            out << ", ";
                        }
                    }
                    out << "}" << "\n";
                    out << "  Address: " << node << "\n";
                    out << "    Above: {";

                    for (auto itr2 = node->above->begin(); itr2 != node->above->end(); ++itr2) {
                        Node *above = *itr2;
                        index = above->states.getNextSetIndex(0);
                        out << "{";
                        while (index < above->states.size()) {
                            out << index;
                            index = above->states.getNextSetIndex(index + 1);
                            if (index < above->states.size()) {
                                out << ", ";
                            }
                        }

                        out << "}";
                    }
                    out << "}" << "\n";


                    out << "    Below: {";
                    for (auto itr2 = node->below->begin(); itr2 != node->below->end(); ++itr2) {
                        Node *below = *itr2;
                        out << "{";
                        index = below->states.getNextSetIndex(0);
                        while (index < below->states.size()) {
                            out << index;
                            index = below->states.getNextSetIndex(index + 1);
                            if (index < below->states.size()) {
                                out << ", ";
                            }
                        }

                        out << "}";
                    }
                    out << "}" << "\n";
                }
            }
        }

        void Lattice::toDotFile(std::ostream &out) {
            out << "digraph \"Lattice\" {" << std::endl;

            // print all nodes
            std::vector<Node*> printed;
            out << "\t" << "node [shape=ellipse]" << std::endl;
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {

                if ((*itr) != nullptr && find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
                    out << "\t\"" << (*itr) << "\" [label = \"";
                    uint_fast64_t index = (*itr)->states.getNextSetIndex(0);
                    while (index < (*itr)->states.size()) {
                        out << index;
                        index = (*itr)->states.getNextSetIndex(index + 1);
                        if (index < (*itr)->states.size()) {
                            out << ", ";
                        }
                    }

                    out << "\"]" << std::endl;
                    printed.push_back(*itr);
                }
            }

            // print arcs
            printed.clear();
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
                if ((*itr) != nullptr && find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
                    auto below = (*itr)->below;
                    for (auto itr2 = below->begin(); itr2 != below->end(); ++itr2) {
                        out << "\t\"" << (*itr) << "\" -> \"" << (*itr2) << "\";" << std::endl;
                    }
                    printed.push_back(*itr);
                }
            }

            out << "}" << std::endl;
        }

        bool Lattice::above(Node *node1, Node *node2) {
            return node1->statesBelow.get(node2->states.getNextSetIndex(0));
        }

        void Lattice::setStatesAbove(Lattice::Node *node, uint_fast64_t state, bool alreadyInitialized) {
            assert (!node->states.get(state));
            if (!alreadyInitialized) {
                node->statesAbove = storm::storage::BitVector(numberOfStates);
                node->above = new std::set<Lattice::Node*>({});
            }
            node->statesAbove.set(state);
            node->above->insert(getNode(state));
        }

        void Lattice::setStatesBelow(Lattice::Node *node, uint_fast64_t state, bool alreadyInitialized) {
            assert (!node->states.get(state));
            if (!alreadyInitialized) {
                node->statesBelow = storm::storage::BitVector(numberOfStates);
                node->below = new std::set<Lattice::Node*>({});
            }
            node->statesBelow.set(state);
            node->below->insert(getNode(state));
        }

        void Lattice::setStatesAbove(Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
            assert((states.getNumberOfSetBits() - (node->states & states).getNumberOfSetBits()) != 0);

            if (alreadyInitialized) {
                node->statesAbove |= states;
            } else {
                node->statesAbove = storm::storage::BitVector(states);
                node->above = new std::set<Lattice::Node*>({});
            }
            for (auto i = states.getNextSetIndex(0); i < states.size(); i = states.getNextSetIndex(i + 1)) {
                if (node->states.get(i)) {
                    node->statesAbove.set(i, false);
                } else {
                    node->above->insert(getNode(i));
                }
            }
        }

        void Lattice::setStatesBelow(Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
            assert((states.getNumberOfSetBits() - (node->states & states).getNumberOfSetBits()) != 0);
            if (alreadyInitialized) {
                node->statesBelow |= states;
            } else {
                node->statesBelow = storm::storage::BitVector(states);
                node->below = new std::set<Lattice::Node*>({});
            }
            for (auto i = states.getNextSetIndex(0); i < states.size(); i = states.getNextSetIndex(i + 1)) {
                if (node->states.get(i)) {
                    node->statesBelow.set(i, false);
                } else {
                    node->below->insert(getNode(i));
                }
            }
        }
    }
}
