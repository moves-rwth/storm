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

            for (auto const &state : *initialMiddleStates) {
                add(state);
            }
        }

        Lattice::Lattice(Lattice* lattice) {
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
            assert(*addedStates == *(lattice->getAddedStates()));

            // set all states above and below
            for (auto itr = oldNodes.begin(); itr != oldNodes.end(); ++itr) {
                Node *oldNode = (*itr);
                if (oldNode != nullptr) {
                    Node *newNode = getNode(*(oldNode->states.begin()));
                    newNode->statesAbove = storm::storage::BitVector((oldNode->statesAbove));
                }
            }
        }

        void Lattice::addBetween(uint_fast64_t state, Node *above, Node *below) {
//            std::cout << "Adding " << state << " between " << *(above->states.begin()) << " and " << *(below->states.begin()) << std::endl;
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

        void Lattice::addToNode(uint_fast64_t state, Node *node) {
//            std::cout << "Adding " << state << " to " << *(node->states.begin()) << std::endl;
            assert(!(*addedStates)[state]);
            node->states.insert(state);
            nodes[state] = node;
            addedStates->set(state);
        }

        void Lattice::add(uint_fast64_t state) {
            assert(!(*addedStates)[state]);
            addBetween(state, top, bottom);
        }

        void Lattice::addRelationNodes(Lattice::Node *above, Lattice::Node * below) {
            assert (compare(above, below) == UNKNOWN);
            for (auto const& state : above->states) {
                below->statesAbove.set(state);
            }
            below->statesAbove|=((above->statesAbove));
            assert (compare(above, below) == ABOVE);
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
            // TODO improve
            auto numberOfSetBits = states->getNumberOfSetBits();
            auto stateSize = states->size();
            auto result = std::vector<uint_fast64_t>(numberOfSetBits, stateSize);
            for (auto state : *states) {
                if (result[0] == stateSize) {
                    result[0] = state;
                } else {
                    auto i = 0;
                    bool added = false;
                    while (i < numberOfSetBits && !added) {
                        if (result[i] == stateSize) {
                            result[i] = state;
                            added = true;
                        } else {
                            auto compareRes = compare(state, result[i]);
                            if (compareRes == Lattice::ABOVE) {
                                auto temp = result[i];
                                result[i] = state;
                                for (auto j = i + 1; j < numberOfSetBits && result[j + 1] != stateSize; j++) {
                                    auto temp2 = result[j];
                                    result[j] = temp;
                                    temp = temp2;
                                }
                                added = true;
                            } else if (compareRes == Lattice::UNKNOWN) {
                                break;
                            } else if (compareRes == Lattice::SAME) {
                                ++i;
                                auto temp = result[i];
                                result[i] = state;
                                for (auto j = i + 1; j < numberOfSetBits && result[j + 1] != stateSize; j++) {
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

        void Lattice::toString(std::ostream &out) {
            std::vector<Node*> printedNodes = std::vector<Node*>({});
            for (auto itr = nodes.begin(); itr != nodes.end(); ++itr) {

                if ((*itr) != nullptr && std::find(printedNodes.begin(), printedNodes.end(), (*itr)) == printedNodes.end()) {
                    Node *node = *itr;
                    printedNodes.push_back(*itr);
                    out << "Node: {";
                    for (auto const & state:node->states) {
                        out << state << "; ";

                    }
                    out << "}" << "\n";
                    out << "  Address: " << node << "\n";
                    out << "    Above: {";

                    auto statesAbove = node->statesAbove;
                    for (auto const & state:(node->statesAbove)) {
                        out << state << "; ";
                    }
                    out << "}" << "\n";
                }
            }
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

        bool Lattice::above(storm::analysis::Lattice::Node *node1, storm::analysis::Lattice::Node *node2,
                            storm::analysis::Lattice::Node *nodePrev, storm::storage::BitVector *statesSeen) {
            bool found = false;
            for (auto const& state : node1->states) {
                found = ((node2->statesAbove))[state];
                if (found) {
                    break;
                }
            }
            if (!found) {
                // TODO: kan dit niet anders?
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

        void Lattice::mergeNodes(storm::analysis::Lattice::Node *node1, storm::analysis::Lattice::Node *node2) {
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
    }
}
