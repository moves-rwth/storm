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
            top = new Node();
            top->states = topStates;
            setStatesAbove(top, storm::storage::BitVector(numberOfStates), false);
            setStatesBelow(top, bottomStates, false);

            bottom = new Node();
            bottom->states = bottomStates;
            setStatesBelow(bottom, storm::storage::BitVector(numberOfStates), false);
            setStatesAbove(bottom, topStates, false);

            nodes = std::vector<Node *>(numberOfStates);
            for (auto i = topStates.getNextSetIndex(0); i < numberOfStates; i = topStates.getNextSetIndex(i+1)) {
                nodes.at(i) = top;

            }

            for (auto i = bottomStates.getNextSetIndex(0); i < numberOfStates; i = bottomStates.getNextSetIndex(i+1)) {
                nodes.at(i) = bottom;
            }

            this->numberOfStates = numberOfStates;
            this->addedStates = storm::storage::BitVector(numberOfStates);
            this->addedStates |= (topStates);
            this->addedStates |= (bottomStates);
        }

        Lattice::Lattice(Lattice* lattice) {
            top = new Node();
            top->states = storm::storage::BitVector(lattice->getTop()->states);
            setStatesAbove(top, lattice->getTop()->statesAbove, false);
            setStatesBelow(top, lattice->getTop()->statesBelow, false);

            bottom = new Node();
            bottom->states = storm::storage::BitVector(lattice->getBottom()->states);
            setStatesAbove(bottom, lattice->getBottom()->statesAbove, false);
            setStatesBelow(bottom, lattice->getBottom()->statesBelow, false);

            numberOfStates = top->states.size();
            nodes = std::vector<Node *>(numberOfStates);
            addedStates = storm::storage::BitVector(numberOfStates);
            addedStates |= (top->states);
            addedStates |= (bottom->states);

            for (auto i = top->states.getNextSetIndex(0); i < numberOfStates; i = top->states.getNextSetIndex(i+1)) {
                nodes.at(i) = top;
            }

            for (auto i =  bottom->states.getNextSetIndex(0); i < numberOfStates; i =  bottom->states.getNextSetIndex(i+1)) {
                nodes.at(i) = bottom;
            }

            auto oldNodes = lattice->getNodes();
            // Create nodes
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node* oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode = new Node();
                    newNode->states = storm::storage::BitVector(oldNode->states);
                    setStatesAbove(newNode, oldNode->statesAbove, false);
                    setStatesBelow(newNode, oldNode->statesBelow, false);
                    for (auto i = newNode->states.getNextSetIndex(0);
                         i < numberOfStates; i = newNode->states.getNextSetIndex(i + 1)) {
                        addedStates.set(i);
                        nodes.at(i) = newNode;
                    }
                }
            }

            assert(addedStates == lattice->getAddedStates());
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(!addedStates[state]);
            auto res = compare(above, below);
            assert(res == ABOVE || res == UNKNOWN);

            Node *newNode = new Node();
            newNode->states = storm::storage::BitVector(numberOfStates);
            newNode->states.set(state);
            setStatesAbove(newNode, above->statesAbove | above->states, false);
            setStatesBelow(newNode, below->statesBelow | below->states, false);

            setStatesBelow(above, state);
            setStatesAbove(below, state);

            auto nodesBelow = getNodesBelow(below);
            for (auto itr = nodesBelow.begin(); itr != nodesBelow.end(); ++itr) {
                assert((*itr)->statesAbove.size() == numberOfStates);
                setStatesAbove((*itr), state);
            }

            auto nodesAbove = getNodesAbove(above);
            for (auto itr = nodesAbove.begin(); itr != nodesAbove.end(); ++itr) {
                assert((*itr)->statesBelow.size() == numberOfStates);
                setStatesBelow((*itr), state);
            }
            nodes.at(state) = newNode;
            addedStates.set(state);
        }

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
            assert(!addedStates[state]);
            node->states.set(state);
            nodes.at(state) = node;
            addedStates.set(state);
            auto nodesBelow = getNodesBelow(node);
            for (auto itr = nodesBelow.begin(); itr != nodesBelow.end(); ++itr) {
                setStatesAbove((*itr), state);
            }
            auto nodesAbove = getNodesAbove(node);
            for (auto itr = nodesAbove.begin(); nodesAbove.size() != 0 &&itr != nodesAbove.end(); ++itr) {
                setStatesBelow((*itr), state);
            }
        }

        void Lattice::add(uint_fast64_t state) {
            addBetween(state, top, bottom);
        }

        void Lattice::addRelationNodes(storm::analysis::Lattice::Node *above, storm::analysis::Lattice::Node * below) {
            setStatesBelow(above, below->states, true);
            setStatesAbove(below, above->states, true);
            auto nodesBelow = getNodesBelow(below);
            for (auto itr = nodesBelow.begin(); itr != nodesBelow.end(); ++itr) {
                setStatesAbove((*itr), above->states, true);
            }
            auto nodesAbove = getNodesAbove(above);
            for (auto itr = nodesAbove.begin()  ; itr != nodesAbove.end(); ++itr) {
                setStatesBelow((*itr), below->states, true);
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

                if (above(node1, node2, std::make_shared<std::set<Node*>>(std::set<Node*>({})))) {
                    assert(!above(node2, node1, std::make_shared<std::set<Node*>>(std::set<Node*>({}))));
                    return ABOVE;
                }

                if (above(node2, node1, std::make_shared<std::set<Node*>>(std::set<Node*>({})))) {
                    return BELOW;
                }
            }
            return UNKNOWN;
        }

        Lattice::Node *Lattice::getNode(uint_fast64_t stateNumber) {
            return nodes.at(stateNumber);
        }

        std::set<Lattice::Node*> Lattice::getNodesAbove(Lattice::Node * node) {
            if (node->recentlyAddedAbove.getNumberOfSetBits() != 0) {
                for (auto i = node->recentlyAddedAbove.getNextSetIndex(0); i < node->recentlyAddedAbove.size(); i = node->recentlyAddedAbove.getNextSetIndex(i+1)) {
                    node->above.insert(getNode(i));
                }
                node->recentlyAddedAbove.clear();
            }
            return node->above;
        }

        std::set<Lattice::Node*> Lattice::getNodesBelow(Lattice::Node * node) {
            if (node->recentlyAddedBelow.getNumberOfSetBits() != 0) {
                for (auto i = node->recentlyAddedBelow.getNextSetIndex(0); i < node->recentlyAddedBelow.size(); i = node->recentlyAddedBelow.getNextSetIndex(i+1)) {
                    node->below.insert(getNode(i));
                }
                node->recentlyAddedBelow.clear();
            }
            return node->above;
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
                    while (index < numberOfStates) {
                        out << index;
                        index = node->states.getNextSetIndex(index + 1);
                        if (index < numberOfStates) {
                            out << ", ";
                        }
                    }
                    out << "}" << "\n";
                    out << "  Address: " << node << "\n";
                    out << "    Above: {";

                    getNodesAbove(node); // such that it is updated
                    for (auto itr2 = node->above.begin(); itr2 != node->above.end(); ++itr2) {
                        Node *above = *itr2;
                        index = above->states.getNextSetIndex(0);
                        out << "{";
                        while (index < numberOfStates) {
                            out << index;
                            index = above->states.getNextSetIndex(index + 1);
                            if (index < numberOfStates) {
                                out << ", ";
                            }
                        }

                        out << "}";
                    }
                    out << "}" << "\n";


                    out << "    Below: {";
                    getNodesBelow(node); // To make sure it is updated
                    // TODO verbeteren
                    for (auto itr2 = node->below.begin(); itr2 != node->below.end(); ++itr2) {
                        Node *below = *itr2;
                        out << "{";
                        index = below->states.getNextSetIndex(0);
                        while (index < numberOfStates) {
                            out << index;
                            index = below->states.getNextSetIndex(index + 1);
                            if (index < numberOfStates) {
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
                    while (index < numberOfStates) {
                        out << index;
                        index = (*itr)->states.getNextSetIndex(index + 1);
                        if (index < numberOfStates) {
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
                    for (auto itr2 = below.begin(); itr2 != below.end(); ++itr2) {
                        out << "\t\"" << (*itr) << "\" -> \"" << (*itr2) << "\";" << std::endl;
                    }
                    printed.push_back(*itr);
                }
            }

            out << "}" << std::endl;
        }

        bool Lattice::above(Node *node1, Node *node2, std::shared_ptr<std::set<Node *>> seenNodes) {
            return node1->statesBelow.get(node2->states.getNextSetIndex(0));
        }

        void Lattice::setStatesAbove(storm::analysis::Lattice::Node *node, uint_fast64_t state) {
            node->statesAbove.set(state);
            node->recentlyAddedAbove.set(state);
        }

        void Lattice::setStatesBelow(storm::analysis::Lattice::Node *node, uint_fast64_t state) {
            node->statesBelow.set(state);
            node->recentlyAddedBelow.set(state);
        }

        void Lattice::setStatesAbove(storm::analysis::Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
            if (alreadyInitialized) {
                node->statesAbove |= states;
                node->recentlyAddedAbove |= states;
            } else {
                node->statesAbove = storm::storage::BitVector(states);
                node->recentlyAddedAbove = storm::storage::BitVector(states);
            }
        }

        void Lattice::setStatesBelow(storm::analysis::Lattice::Node *node, storm::storage::BitVector states, bool alreadyInitialized) {
            if (alreadyInitialized) {
                node->statesBelow |= states;
                node->recentlyAddedBelow |= states;
            } else {
                node->statesBelow = storm::storage::BitVector(states);
                node->recentlyAddedBelow = storm::storage::BitVector(states);
            }
        }
    }
}
