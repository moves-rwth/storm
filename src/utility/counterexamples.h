/*
 * vector.h
 *
 *  Created on: 06.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_COUNTEREXAMPLE_H_
#define STORM_UTILITY_COUNTEREXAMPLE_H_

#include <queue>

namespace storm {
    namespace utility {
        namespace counterexamples {
            
            /*!
             * Computes a set of action labels that is visited along all paths from any state to a target state.
             *
             * @return The set of action labels that is visited on all paths from any state to a target state.
             */
            template <typename T>
            std::vector<storm::storage::VectorSet<uint_fast64_t>> getGuaranteedLabelSets(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, storm::storage::VectorSet<uint_fast64_t> const& relevantLabels) {
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<storm::storage::VectorSet<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();

                // Now we compute the set of labels that is present on all paths from the initial to the target states.
                std::vector<storm::storage::VectorSet<uint_fast64_t>> analysisInformation(labeledMdp.getNumberOfStates(), relevantLabels);
                std::queue<std::pair<uint_fast64_t, uint_fast64_t>> worklist;
                
                // Initially, put all predecessors of target states in the worklist and empty the analysis information them.
                for (auto state : psiStates) {
                    analysisInformation[state] = std::set<uint_fast64_t>();
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(state), predecessorIte = backwardTransitions.constColumnIteratorEnd(state); predecessorIt != predecessorIte; ++predecessorIt) {
                        if (*predecessorIt != state) {
                            worklist.push(std::make_pair(*predecessorIt, state));
                        }
                    }
                }

                // Iterate as long as the worklist is non-empty.
                uint_fast64_t iters = 0;
                while (!worklist.empty()) {
                    ++iters;
                    std::pair<uint_fast64_t, uint_fast64_t> const& currentStateTargetStatePair = worklist.front();
                    uint_fast64_t currentState = currentStateTargetStatePair.first;
                    uint_fast64_t targetState = currentStateTargetStatePair.second;
                    
                    // Iterate over the successor states for all choices and compute new analysis information.
                    storm::storage::VectorSet<uint_fast64_t> intersection(analysisInformation[currentState]);
                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        storm::storage::VectorSet<uint_fast64_t> tmpIntersection;
                        storm::storage::VectorSet<uint_fast64_t> tmpUnion;
                        bool choiceTargetsTargetState = false;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(currentChoice), successorIte = transitionMatrix.constColumnIteratorEnd(currentChoice); successorIt != successorIte; ++successorIt) {
                            // If we can reach the target state with this choice, we need to intersect the current
                            // analysis information with the union of the new analysis information of the target state
                            // and the choice labels.
                            if (*successorIt == targetState) {
                                choiceTargetsTargetState = true;
                                std::set_union(analysisInformation[targetState].begin(), analysisInformation[targetState].end(), choiceLabeling[currentChoice].begin(), choiceLabeling[currentChoice].end(), std::inserter(tmpUnion, tmpUnion.end()));
                                break;
                            }
                        }
                        if (choiceTargetsTargetState) {
                            std::set_intersection(intersection.begin(), intersection.end(), tmpUnion.begin(), tmpUnion.end(), std::inserter(tmpIntersection, tmpIntersection.end()));
                            std::swap(intersection, tmpIntersection);
                        }
                    }
                    
                    // If the analysis information changed, we need to update it and put all the predecessors of this
                    // state in the worklist.
                    if (analysisInformation[currentState].size() != intersection.size()) {
                        analysisInformation[currentState] = std::move(intersection);
                        
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(currentState), predecessorIte = backwardTransitions.constColumnIteratorEnd(currentState); predecessorIt != predecessorIte; ++predecessorIt) {
                            worklist.push(std::make_pair(*predecessorIt, currentState));
                        }
                    }
                    
                    worklist.pop();
                }
 
                return analysisInformation;
            }
            
            /*!
             * Computes a set of action labels that is visited along all paths from an initial state to a target state.
             *
             * @return The set of action labels that is visited on all paths from an initial state to a target state.
             */
            template <typename T>
            storm::storage::VectorSet<uint_fast64_t> getGuaranteedLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, storm::storage::VectorSet<uint_fast64_t> const& relevantLabels) {
                std::vector<storm::storage::VectorSet<uint_fast64_t>> guaranteedLabels = getGuaranteedLabelSets(labeledMdp, psiStates, relevantLabels);
                
                storm::storage::VectorSet<uint_fast64_t> knownLabels(relevantLabels);
                storm::storage::VectorSet<uint_fast64_t> tempIntersection;
                for (auto initialState : labeledMdp.getInitialStates()) {
                    tempIntersection = knownLabels.intersect(guaranteedLabels[initialState]);
                    std::swap(knownLabels, tempIntersection);
                }

                return knownLabels;
            }
            
        } // namespace counterexample
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_COUNTEREXAMPLE_H_ */
