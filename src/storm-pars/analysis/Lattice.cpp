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
            bottom = new Node();
            bottom->states = bottomStates;
            top->below.insert(bottom);
            bottom->above.insert(top);

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
            bottom = new Node();
            bottom->states = storm::storage::BitVector(lattice->getBottom()->states);
            numberOfStates = top->states.size();
            nodes = std::vector<Node *>(numberOfStates);
            addedStates = storm::storage::BitVector(numberOfStates);
            addedStates.operator|=(top->states);
            addedStates.operator|=(bottom->states);

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
                    for (auto i = newNode->states.getNextSetIndex(0);
                         i < numberOfStates; i = newNode->states.getNextSetIndex(i + 1)) {
                        addedStates.set(i);
                        nodes.at(i) = newNode;
                    }
                }
            }

            // Create transitions
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node* oldNode = (*itr);
                if (oldNode != nullptr) {
                    auto state = (*itr)->states.getNextSetIndex(0);
                    for (auto itr2 = (*itr)->below.begin(); itr2 != (*itr)->below.end(); ++itr2) {
                        auto stateBelow = (*itr2)->states.getNextSetIndex(0);
                        addRelationNodes(getNode((state)), getNode((stateBelow)));
                    }
                }
            }
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
            assert(!addedStates[state]);
            auto res = compare(above, below);
            assert(res == ABOVE || res == UNKNOWN);
            Node *newNode = new Node();
            newNode->states = storm::storage::BitVector(numberOfStates);
            newNode->states.set(state);
            newNode->above = std::set<Node *>({above});
            newNode->below = std::set<Node *>({below});
            below->above.erase(above);
            above->below.erase(below);
            (below->above).insert(newNode);
            above->below.insert(newNode);
            nodes.at(state) = newNode;
            addedStates.set(state);
        }

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
            assert(!addedStates[state]);
            node->states.set(state);
            nodes.at(state) = node;
            addedStates.set(state);
        }

        void Lattice::add(uint_fast64_t state) {
            addBetween(state, top, bottom);
        }

        void Lattice::addRelationNodes(storm::analysis::Lattice::Node *above, storm::analysis::Lattice::Node * below) {
            assert(compare(above, below) == UNKNOWN || compare(above, below) == ABOVE);
            above->below.insert(below);
            below->above.insert(above);
        }

        void Lattice::mergeNodes(storm::analysis::Lattice::Node *n1, storm::analysis::Lattice::Node * n2) {
            // TODO
        }

        int Lattice::compare(uint_fast64_t state1, uint_fast64_t state2) {
            return compare(getNode(state1), getNode(state2));
        }

        int Lattice::compare(Node* node1, Node* node2) {
            if (node1 != nullptr && node2 != nullptr) {
                if (node1 == node2) {
                    return SAME;
                }

                if (above(node1, node2, new std::set<Node*>({}))) {
                    assert(!above(node2, node1, new std::set<Node*>({})));
                    return ABOVE;
                }

                if (above(node2, node1, new std::set<Node*>({}))) {
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

        bool Lattice::above(Node *node1, Node *node2, std::set<Node *>* seenNodes) {
            bool result = !node1->below.empty() && std::find(node1->below.begin(), node1->below.end(), node2) != node1->below.end();
            for (auto itr = node1->below.begin(); !result && node1->below.end() != itr; ++itr) {
                if (std::find(seenNodes->begin(), seenNodes->end(), (*itr)) == seenNodes->end()) {
                    seenNodes->insert(*itr);
                    result |= above(*itr, node2, seenNodes);
                }
            }
            return result;
        }
    }
}
