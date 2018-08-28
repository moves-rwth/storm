//
// Created by Jip Spel on 24.07.18.
//

#include <iostream>
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
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
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
            node->states.set(state);
            nodes.at(state) = node;
            addedStates.set(state);
        }

        void Lattice::add(uint_fast64_t state) {
            addBetween(state, top, bottom);
        }

        void Lattice::addRelation(storm::analysis::Lattice::Node *above, storm::analysis::Lattice::Node *between,
                                  storm::analysis::Lattice::Node *below) {
            above->below.insert(between);
            between->above.insert(above);
            between->below.insert(below);
            below->above.insert(between);
        }

        int Lattice::compare(uint_fast64_t state1, uint_fast64_t state2) {
            Node *node1 = getNode(state1);
            Node *node2 = getNode(state2);

            // TODO: Wat als above(node1, node2) en above(node2, node1), dan moeten ze samengevoegd?
            if (node1 != nullptr && node2 != nullptr) {
                if (node1 == node2) {
                    return 0;
                }

                if (above(node1, node2)) {
                    return 1;
                }

                if (above(node2, node1)) {
                    return 2;
                }
            }

            return -1;
        }

        Lattice::Node *Lattice::getNode(uint_fast64_t stateNumber) {
            return nodes.at(stateNumber);
        }

        storm::storage::BitVector Lattice::getAddedStates() {
            return addedStates;
        }

        void Lattice::toString(std::ostream &out) {
            std::vector<Node*> printedNodes = std::vector<Node*>({});
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {

                if (std::find(printedNodes.begin(), printedNodes.end(), (*itr)) == printedNodes.end()) {
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
                        out << "{";
                        index = above->states.getNextSetIndex(0);
                        while (index < numberOfStates) {
                            out << index;
                            index = above->states.getNextSetIndex(index + 1);
                            if (index < numberOfStates) {
                                out << ", ";
                            }
                        }

                        out << "}";
                        if ((++itr2) != node->above.end()) {
                            out << ", ";
                        }
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
                        if ((++itr2) != node->below.end()) {
                            out << ", ";
                        }
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
                if (find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
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
                if (find(printed.begin(), printed.end(), (*itr)) == printed.end()) {
                    auto below = (*itr)->below;
                    for (auto itr2 = below.begin(); itr2 != below.end(); ++itr2) {
                        out << "\t\"" << (*itr) << "\" -> \"" << (*itr2) << "\";" << std::endl;
                    }
                    printed.push_back(*itr);
                }
            }

            out << "}" << std::endl;
        }

        bool Lattice::above(Node *node1, Node *node2) {
            bool result = !node1->below.empty() && std::find(node1->below.begin(), node1->below.end(), node2) != node1->below.end();

            for (auto itr = node1->below.begin(); !result && node1->below.end() != itr; ++itr) {
                result |= above(*itr, node2);
            }
            return result;
        }
    }
}
