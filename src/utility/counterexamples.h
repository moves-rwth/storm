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
            std::vector<boost::container::flat_set<uint_fast64_t>> getGuaranteedLabelSets(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& relevantLabels) {
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<boost::container::flat_set<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                storm::storage::SparseMatrix<T> backwardTransitions = labeledMdp.getBackwardTransitions();

                // Now we compute the set of labels that is present on all paths from the initial to the target states.
                std::vector<boost::container::flat_set<uint_fast64_t>> analysisInformation(labeledMdp.getNumberOfStates(), relevantLabels);
                std::queue<std::pair<uint_fast64_t, uint_fast64_t>> worklist;
                
                // Initially, put all predecessors of target states in the worklist and empty the analysis information them.
                for (auto state : psiStates) {
                    analysisInformation[state] = boost::container::flat_set<uint_fast64_t>();
                    for (auto& predecessorEntry : backwardTransitions.getRow(state)) {
                        if (predecessorEntry.first != state) {
                            worklist.push(std::make_pair(predecessorEntry.first, state));
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
                    
                    size_t analysisInformationSizeBefore = analysisInformation[currentState].size();
                    
                    // Iterate over the successor states for all choices and compute new analysis information.
                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        boost::container::flat_set<uint_fast64_t> tmpIntersection;
                        bool choiceTargetsTargetState = false;
                        
                        for (auto& entry : transitionMatrix.getRow(currentChoice)) {
                            if (entry.first == targetState) {
                                choiceTargetsTargetState = true;
                                break;
                            }
                        }
                        
                        // If we can reach the target state with this choice, we need to intersect the current
                        // analysis information with the union of the new analysis information of the target state
                        // and the choice labels.
                        if (choiceTargetsTargetState) {
                            std::set_intersection(analysisInformation[currentState].begin(), analysisInformation[currentState].end(), analysisInformation[targetState].begin(), analysisInformation[targetState].end(), std::inserter(tmpIntersection, tmpIntersection.end()));
                            std::set_intersection(analysisInformation[currentState].begin(), analysisInformation[currentState].end(), choiceLabeling[currentChoice].begin(), choiceLabeling[currentChoice].end(), std::inserter(tmpIntersection, tmpIntersection.end()));
                            analysisInformation[currentState] = std::move(tmpIntersection);
                        }
                    }
                    
                    // If the analysis information changed, we need to update it and put all the predecessors of this
                    // state in the worklist.
                    if (analysisInformation[currentState].size() != analysisInformationSizeBefore) {
                        for (auto& predecessorEntry : backwardTransitions.getRow(currentState)) {
                            // Only put the predecessor in the worklist if it's not already a target state.
                            if (!psiStates.get(predecessorEntry.first)) {
                                worklist.push(std::make_pair(predecessorEntry.first, currentState));
                            }
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
            boost::container::flat_set<uint_fast64_t> getGuaranteedLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& relevantLabels) {
                std::vector<boost::container::flat_set<uint_fast64_t>> guaranteedLabels = getGuaranteedLabelSets(labeledMdp, psiStates, relevantLabels);
                
                boost::container::flat_set<uint_fast64_t> knownLabels(relevantLabels);
                boost::container::flat_set<uint_fast64_t> tempIntersection;
                for (auto initialState : labeledMdp.getInitialStates()) {
                    std::set_intersection(knownLabels.begin(), knownLabels.end(), guaranteedLabels[initialState].begin(), guaranteedLabels[initialState].end(), std::inserter(tempIntersection, tempIntersection.end()));
                    std::swap(knownLabels, tempIntersection);
                }

                return knownLabels;
            }
            
        } // namespace counterexample
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_COUNTEREXAMPLE_H_ */
