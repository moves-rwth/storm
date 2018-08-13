//
// Created by Jip Spel on 24.07.18.
//

#include <iostream>
#include "Lattice.h"
namespace storm {
    namespace analysis {
        Lattice::Lattice(Node *topNode, Node *bottomNode, uint_fast64_t numberOfStates) {
            Node *top = new Node();
            top->states = topNode->states;
            Node *bottom = new Node();
            bottom->states = bottomNode->states;
            top->below.push_back(bottom);
            bottom->above.push_back(top);
            nodes = std::vector<Node *>({top, bottom});
            this->numberOfStates = numberOfStates;
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
            Node *newNode = new Node();
            newNode->states = storm::storage::BitVector(numberOfStates);
            newNode->states.set(state);
            newNode->above = std::vector<Node *>({above});
            newNode->below = std::vector<Node *>({below});
            remove(&(below->above), above);
            remove(&(above->below), below);
            (below->above).push_back(newNode);
            above->below.push_back(newNode);
            nodes.push_back(newNode);
        }

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
            node->states.set(state);
        }

        int Lattice::compare(uint_fast64_t state1, uint_fast64_t state2) {
            Node *node1 = getNode(state1);
            Node *node2 = getNode(state2);

            if (node1 == node2) {
                return 0;
            }

            if (above(node1, node2)) {
                return 1;
            }

            if (below(node1, node2)) {
                return 2;
            }

            return -1;
        }

        Lattice::Node *Lattice::getNode(uint_fast64_t stateNumber) {
            // TODO: might return nullptr, what to do with it?
            Node *node;
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
                storm::storage::BitVector states = (*itr)->states;

                if (states[stateNumber]) return (*itr);
            }
            return node;
        }

        void Lattice::toString(std::ostream &out) {
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {
                Node *node = *itr;
                out << "Node: {";
                uint_fast64_t index = node->states.getNextSetIndex(0);
                while (index < numberOfStates) {
                    out << index;
                    index = node->states.getNextSetIndex(index+1);
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
                        index = above->states.getNextSetIndex(index+1);
                        if (index < numberOfStates) {
                            out << ", ";
                        }
                    }

                    out << "}";
                    if (itr2 + 1 != node->above.end()) {
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
                        index = below->states.getNextSetIndex(index+1);
                        if (index < numberOfStates) {
                            out << ", ";
                        }
                    }

                    out << "}";
                    if (itr2 + 1 != node->below.end()) {
                        out << ", ";
                    }
                }
                out << "}" << "\n";
            }
        }

        bool Lattice::above(Node *node1, Node *node2) {
            if (node1->below.empty()) {
                return false;
            }

            if (std::find(node1->below.begin(), node1->below.end(), node2) != node1->below.end()) {
                return true;
            }

            bool result = false;
            for (auto itr = node1->below.begin(); node1->below.end() != itr; ++itr) {
                result |= above(*itr, node2);
            }
            return result;
        }

        bool Lattice::below(Node *node1, Node *node2) {
            if (node1->above.empty()) {
                return false;
            }

            if (std::find(node1->above.begin(), node1->above.end(), node2) != node1->above.end()) {
                return true;
            }

            bool result = false;
            for (auto itr = node1->above.begin(); node1->above.end() != itr; ++itr) {
                result |= below(*itr, node2);
            }
            return result;
        }

        void Lattice::remove(std::vector<Node *> *nodes, Node *node) {
            auto index = std::find(nodes->begin(), nodes->end(), node);
            if (index != nodes->end()) {
                nodes->erase(index);
            }
        };

        void Lattice::setStates(std::vector<uint_fast64_t> states, Node *node) {
            for (auto itr = states.begin(); itr < states.end(); ++itr) {
                node->states.set(*itr);
            }
        }
    }
}
