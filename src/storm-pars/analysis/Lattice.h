//
// Created by Jip Spel on 24.07.18.
//

#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include <iostream>
#include <vector>
#include <storm/logic/Formula.h>
#include <storm/modelchecker/propositional/SparsePropositionalModelChecker.h>

#include "storm/models/sparse/Model.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"
#include "storm/utility/graph.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"


namespace storm {
            namespace analysis {
                class Lattice {

                public:
                    struct Node {
                        storm::storage::BitVector states;
                        std::set<Node *> above;
                        std::set<Node *> below;
                    };

                    /*!
                     * Constructs a lattice with the given top node and bottom node.
                     *
                     * @param topNode The top node of the resulting lattice.
                     * @param bottomNode The bottom node of the resulting lattice.
                     */
                    Lattice(storm::storage::BitVector topStates,
                            storm::storage::BitVector bottomStates, uint_fast64_t numberOfStates);

                    /*!
                     * Adds a node with the given state below node1 and above node2.
                     * @param state The given state.
                     * @param node1 The pointer to the node below which a new node (with state) is added
                     * @param node2 The pointer to the node above which a new node (with state) is added
                     */
                    void addBetween(uint_fast64_t state, Node *node1, Node *node2);

                    /*!
                     * Adds state to the states of the given node.
                     * @param state The state which is added.
                     * @param node The pointer to the node to which state is added, should not be nullptr.
                     */
                    void addToNode(uint_fast64_t state, Node *node);

                    /*!
                     * Adds state between the top and bottom node of the lattice
                     * @param state The given state
                     */
                    void add(uint_fast64_t state);

                    /*!
                     * Adds a new relation to the lattice
                     * @param above The node closest to the top Node of the Lattice.
                     * @param between The node between above and below.
                     * @param below The node closest to the bottom Node of the Lattice.
                     */
                    void addRelation(Node* above, Node* between, Node* below);

                    /*!
                     * Compares the level of the nodes of the states.
                     * Behaviour unknown when one or more of the states doesnot occur at any Node in the Lattice.
                     * @param state1 The first state.
                     * @param state2 The second state.
                     * @return 0 if the nodes are on the same level;
                     * 1 if the node of the first state is closer to top then the node of the second state;
                     * 2 if the node of the second state is closer to top then the node of the first state;
                     * -1 if it is unclear from the structure of the lattice how the nodes relate.
                     */
                    int compare(uint_fast64_t state1, uint_fast64_t state2);

                    /*!
                     * Retrieves the pointer to a Node at which the state occurs.
                     * Behaviour unknown when state does not exists at any Node in the Lattice.
                     *
                     * @param state The number of the state.
                     *
                     * @return The pointer to the node of the state, nullptr if the node does not exist
                     */
                    Node *getNode(uint_fast64_t state);

                    /*!
                     * Prints a string representation of the lattice to the output stream.
                     *
                     * @param out The stream to output to.
                     */
                    void toString(std::ostream &out);

                    /*!
                     * Prints a dot representation of the lattice to the output stream.
                     *
                     * @param out The stream to output to.
                     */
                    void toDotFile(std::ostream &out);

                    /*!
                     * Creates a Lattice based on the transition matrix, topStates of the Lattice and bottomStates of the Lattice
                     * @tparam ValueType Type of the probabilities
                     * @param model The pointer to the model
                     * @param formulas Vector with pointer to formula
                     * @return pointer to the created Lattice.
                     */
                    template <typename ValueType>
                    static Lattice* toLattice(std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel, std::vector<std::shared_ptr<storm::logic::Formula const>> formulas) {
                        STORM_LOG_THROW((++formulas.begin()) == formulas.end(), storm::exceptions::NotSupportedException, "Only one formula allowed for monotonicity analysis");
                        STORM_LOG_THROW((*(formulas[0])).isProbabilityOperatorFormula()
                                        && ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()
                                            || (*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isEventuallyFormula()), storm::exceptions::NotSupportedException, "Expecting until formula");

                        uint_fast64_t numberOfStates = sparseModel.get()->getNumberOfStates();

                        storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Model<ValueType>> propositionalChecker(*sparseModel);
                        storm::storage::BitVector phiStates;
                        storm::storage::BitVector psiStates;
                        if ((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                            phiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                            psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        } else {
                            phiStates = storm::storage::BitVector(numberOfStates, true);
                            psiStates = propositionalChecker.check((*(formulas[0])).asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                        }

                        // Get the maybeStates
                        std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(sparseModel.get()->getBackwardTransitions(), phiStates, psiStates);
                        storm::storage::BitVector topStates = statesWithProbability01.second;
                        storm::storage::BitVector bottomStates = statesWithProbability01.first;

                        STORM_LOG_THROW(topStates.begin() != topStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no 1 states");
                        STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), storm::exceptions::NotImplementedException, "Formula yields to no zero states");

                        // Transform to Lattice
                        auto matrix = sparseModel.get()->getTransitionMatrix();

                        // Transform the transition matrix into a vector containing the states with the state to which the transition goes.
                        std::vector <State*> stateVector = std::vector<State *>({});
                        for (uint_fast64_t i = 0; i < numberOfStates; ++i) {
                            State* state = new State();
                            state->stateNumber = i;

                            auto row = matrix.getRow(i);


                            if ((*(row.begin())).getValue() != ValueType(1)) {
                                STORM_LOG_THROW((row.begin() + 2) == row.end(), storm::exceptions::NotSupportedException, "Only two outgoing transitions per state allowed");
                                state->successor1 = (*(row.begin())).getColumn();
                                state->successor2 = (*(++row.begin())).getColumn();
                            } else {

                                state-> successor1 = (*(row.begin())).getColumn();
                                state-> successor2 = (*(row.begin())).getColumn();
                            }

                            stateVector.push_back(state);
                        }
                        // Start creating the Lattice
                        storm::analysis::Lattice *lattice = new storm::analysis::Lattice(topStates, bottomStates, numberOfStates);
                        storm::storage::BitVector oldStates(numberOfStates);
                        // Create a copy of the states already present in the lattice.
                        storm::storage::BitVector seenStates = topStates|= bottomStates;

                        while (oldStates != seenStates) {
                            // As long as new states are added to the lattice, continue.
                            oldStates = storm::storage::BitVector(seenStates);

                            for (auto itr = stateVector.begin(); itr != stateVector.end(); ++itr) {
                                // Iterate over all states
                                State *currentState = *itr;

                                if (!seenStates[currentState->stateNumber]
                                    && seenStates[currentState->successor1]
                                    && seenStates[currentState->successor2]) {

                                    // Otherwise, check how the two states compare, and add if the comparison is possible.
                                    uint_fast64_t successor1 = currentState->successor1;
                                    uint_fast64_t successor2 = currentState->successor2;
                                    int compareResult = lattice->compare(successor1, successor2);
                                    if (compareResult == 1) {
                                        // successor 1 is closer to top than successor 2
                                        lattice->addBetween(currentState->stateNumber, lattice->getNode(successor1),
                                                            lattice->getNode(successor2));
                                    } else if (compareResult == 2) {
                                        // successor 2 is closer to top than successor 1
                                        lattice->addBetween(currentState->stateNumber, lattice->getNode(successor2),
                                                            lattice->getNode(successor1));
                                    } else if (compareResult == 0) {
                                        // the successors are at the same level
                                        lattice->addToNode(currentState->stateNumber, lattice->getNode(successor1));
                                    } else {
                                        // TODO: is this what we want?
                                        lattice->add(currentState->stateNumber);
                                    }
                                    seenStates.set(currentState->stateNumber);

                                }
                            }
                        }

                        return lattice;
                    }

                private:
                    struct State {
                        uint_fast64_t stateNumber;
                        uint_fast64_t successor1;
                        uint_fast64_t successor2;
                    };

                    std::vector<Node*> nodes;

                    Node* top;

                    Node* bottom;

                    uint_fast64_t numberOfStates;

                    bool above(Node *, Node *);

                    void setStates(std::vector<uint_fast64_t>  states, Node *node);

                };
            }
}
#endif //LATTICE_LATTICE_H
