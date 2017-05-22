#ifndef STORM_UTILITY_COUNTEREXAMPLE_H_
#define STORM_UTILITY_COUNTEREXAMPLE_H_

#include <queue>
#include <utility>

#include "storm/storage/sparse/PrismChoiceOrigins.h"

namespace storm {
    namespace utility {
        namespace counterexamples {
            
            /*!
             * Computes a set of Prism commands that is executed along all paths from any state to a target state.
             *
             * @return The set of Prism commands that is visited on all paths from any state to a target state.
             */
            template <typename T>
            std::vector<boost::container::flat_set<uint_fast64_t>> getGuaranteedCommandSets(storm::models::sparse::Mdp<T> const& mdp, storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& relevantCommands) {
                // Get some data from the MDP for convenient access.
                storm::storage::SparseMatrix<T> const& transitionMatrix = mdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = mdp.getNondeterministicChoiceIndices();
                storm::storage::SparseMatrix<T> backwardTransitions = mdp.getBackwardTransitions();

                // Now we compute the set of commands that is present on all paths from the initial to the target states.
                std::vector<boost::container::flat_set<uint_fast64_t>> analysisInformation(mdp.getNumberOfStates(), relevantCommands);
                
                std::queue<uint_fast64_t> worklist;
                storm::storage::BitVector statesInWorkList(mdp.getNumberOfStates());
                storm::storage::BitVector markedStates(mdp.getNumberOfStates());
                
                // Initially, put all predecessors of target states in the worklist and empty the analysis information them.
                for (auto state : psiStates) {
                    analysisInformation[state] = boost::container::flat_set<uint_fast64_t>();
                    for (auto const& predecessorEntry : backwardTransitions.getRow(state)) {
                        if (predecessorEntry.getColumn() != state && !statesInWorkList.get(predecessorEntry.getColumn()) && !psiStates.get(predecessorEntry.getColumn())) {
                            worklist.push(predecessorEntry.getColumn());
                            statesInWorkList.set(predecessorEntry.getColumn());
                            markedStates.set(state);
                        }
                    }
                }

                uint_fast64_t iters = 0;
                while (!worklist.empty()) {
                    ++iters;
                    uint_fast64_t const& currentState = worklist.front();
                    
                    size_t analysisInformationSizeBefore = analysisInformation[currentState].size();
                    
                    // Iterate over the successor states for all choices and compute new analysis information.
                    for (uint_fast64_t currentChoice = nondeterministicChoiceIndices[currentState]; currentChoice < nondeterministicChoiceIndices[currentState + 1]; ++currentChoice) {
                        bool modifiedChoice = false;
                        
                        for (auto const& entry : transitionMatrix.getRow(currentChoice)) {
                            if (markedStates.get(entry.getColumn())) {
                                modifiedChoice = true;
                                break;
                            }
                        }
                        
                        // If we can reach the target state with this choice, we need to intersect the current
                        // analysis information with the union of the new analysis information of the target state
                        // and the choice commands.
                        if (modifiedChoice) {
                            for (auto const& entry : transitionMatrix.getRow(currentChoice)) {
                                if (markedStates.get(entry.getColumn())) {
                                    boost::container::flat_set<uint_fast64_t> tmpIntersection;
                                    std::set_intersection(analysisInformation[currentState].begin(), analysisInformation[currentState].end(), analysisInformation[entry.getColumn()].begin(), analysisInformation[entry.getColumn()].end(), std::inserter(tmpIntersection, tmpIntersection.begin()));
                                    std::set_intersection(analysisInformation[currentState].begin(), analysisInformation[currentState].end(), choiceOrigins.getCommandSet(currentChoice).begin(), choiceOrigins.getCommandSet(currentChoice).end(), std::inserter(tmpIntersection, tmpIntersection.begin()));
                                    analysisInformation[currentState] = std::move(tmpIntersection);
                                }
                            }
                        }
                    }
                    
                    // If the analysis information changed, we need to update it and put all the predecessors of this
                    // state in the worklist.
                    if (analysisInformation[currentState].size() != analysisInformationSizeBefore) {
                        for (auto const& predecessorEntry : backwardTransitions.getRow(currentState)) {
                            // Only put the predecessor in the worklist if it's not already a target state.
                            if (!psiStates.get(predecessorEntry.getColumn()) && !statesInWorkList.get(predecessorEntry.getColumn())) {
                                worklist.push(predecessorEntry.getColumn());
                                statesInWorkList.set(predecessorEntry.getColumn());
                            }
                        }
                        markedStates.set(currentState, true);
                    } else {
                        markedStates.set(currentState, false);
                    }
                    
                    worklist.pop();
                    statesInWorkList.set(currentState, false);
                }
                
                return analysisInformation;
            }
            
            /*!
             * Computes a set of prism commands that is executed along all paths from an initial state to a target state.
             *
             * @return The set of prism commands that is executed on all paths from an initial state to a target state.
             */
            template <typename T>
            boost::container::flat_set<uint_fast64_t> getGuaranteedCommandSet(storm::models::sparse::Mdp<T> const& mdp, storm::storage::sparse::PrismChoiceOrigins const& choiceOrigins, storm::storage::BitVector const& psiStates, boost::container::flat_set<uint_fast64_t> const& relevantCommands) {
                std::vector<boost::container::flat_set<uint_fast64_t>> guaranteedCommands = getGuaranteedCommandSets(mdp, choiceOrigins, psiStates, relevantCommands);
                
                boost::container::flat_set<uint_fast64_t> knownCommands(relevantCommands);
                boost::container::flat_set<uint_fast64_t> tempIntersection;
                for (auto initialState : mdp.getInitialStates()) {
                    std::set_intersection(knownCommands.begin(), knownCommands.end(), guaranteedCommands[initialState].begin(), guaranteedCommands[initialState].end(), std::inserter(tempIntersection, tempIntersection.end()));
                    std::swap(knownCommands, tempIntersection);
                }

                return knownCommands;
            }
            
        } // namespace counterexample
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_COUNTEREXAMPLE_H_ */
