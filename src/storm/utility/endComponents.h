#pragma once

#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/graph.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/utility/Stopwatch.h"
#include "storm/storage/MaximalEndComponentDecomposition.h"

namespace storm {
    namespace utility {
        namespace endComponents {
            
            
            // Checks whether there is an End Component that
            // 1. contains at least one of the specified choices and
            // 2. only contains states given by the specified subsystem.
            template <typename T>
            bool checkIfECWithChoiceExists(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& choices) {
                
                STORM_LOG_THROW(subsystem.size() == transitionMatrix.getRowGroupCount(), storm::exceptions::InvalidArgumentException, "Invalid size of subsystem");
                STORM_LOG_THROW(choices.size() == transitionMatrix.getRowCount(), storm::exceptions::InvalidArgumentException, "Invalid size of choice vector");
                
                if (subsystem.empty() || choices.empty()) {
                    return false;
                }
                
                storm::storage::BitVector statesWithChoice(transitionMatrix.getRowGroupCount(), false);
                uint_fast64_t state = 0;
                for (auto const& choice : choices) {
                    // Get the correct state
                    while (choice >= transitionMatrix.getRowGroupIndices()[state + 1]) {
                        ++state;
                    }
                    assert(choice >= transitionMatrix.getRowGroupIndices()[state]);
                    // make sure that the choice originates from the subsystem and also stays within the subsystem
                    if (subsystem.get(state)) {
                        bool choiceStaysInSubsys = true;
                        for (auto const& entry : transitionMatrix.getRow(choice)) {
                            if (!subsystem.get(entry.getColumn())) {
                                choiceStaysInSubsys = false;
                                break;
                            }
                        }
                        if (choiceStaysInSubsys) {
                            statesWithChoice.set(state, true);
                        }
                    }
                }
                
                // Initialize candidate states that satisfy some necessary conditions for being part of an EC with a specified choice:
                
                // Get the states for which a policy can enforce that a choice is reached while staying inside the subsystem
                storm::storage::BitVector candidateStates = storm::utility::graph::performProb1E(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, subsystem, statesWithChoice);
                
                // Only keep the states that can stay in the set of candidates forever
                candidateStates = storm::utility::graph::performProb0E(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, candidateStates, ~candidateStates);
                
                // Only keep the states that can be reached after performing one of the specified choices
                statesWithChoice &= candidateStates;
                storm::storage::BitVector choiceTargets(transitionMatrix.getRowGroupCount(), false);
                for (auto const& state : statesWithChoice) {
                    for (uint_fast64_t choice = choices.getNextSetIndex(transitionMatrix.getRowGroupIndices()[state]); choice < transitionMatrix.getRowGroupIndices()[state + 1]; choice = choices.getNextSetIndex(choice + 1)) {
                        bool choiceStaysInCandidateSet = true;
                        for (auto const& entry : transitionMatrix.getRow(choice)) {
                            if (!candidateStates.get(entry.getColumn())) {
                                choiceStaysInCandidateSet = false;
                                break;
                            }
                        }
                        if (choiceStaysInCandidateSet) {
                            for (auto const& entry : transitionMatrix.getRow(choice)) {
                                choiceTargets.set(entry.getColumn(), true);
                            }
                        }
                    }
                }
                candidateStates = storm::utility::graph::getReachableStates(transitionMatrix, choiceTargets, candidateStates, storm::storage::BitVector(candidateStates.size(), false));
                
                storm::utility::Stopwatch fixPointSw(true);
                // At this point we know that every candidate state can reach a choice at least once without leaving the set of candidate states.
                // We now compute the states that can reach a choice at least twice, three times, four times, ... until a fixpoint is reached.
                while (!candidateStates.empty()) {
                    // Update the states with a choice that stays within the set of candidates
                    statesWithChoice &= candidateStates;
                    for (auto const& state : statesWithChoice) {
                        bool stateHasChoice = false;
                        for (uint_fast64_t choice = choices.getNextSetIndex(transitionMatrix.getRowGroupIndices()[state]); choice < transitionMatrix.getRowGroupIndices()[state + 1]; choice = choices.getNextSetIndex(choice + 1)) {
                            bool choiceStaysInCandidateSet = true;
                            for (auto const& entry : transitionMatrix.getRow(choice)) {
                                if (!candidateStates.get(entry.getColumn())) {
                                    choiceStaysInCandidateSet = false;
                                    break;
                                }
                            }
                            if (choiceStaysInCandidateSet) {
                                stateHasChoice = true;
                                break;
                            }
                        }
                        if (!stateHasChoice) {
                            statesWithChoice.set(state, false);
                        }
                    }
                   
                    // Update the candidates
                    storm::storage::BitVector newCandidates = storm::utility::graph::performProb1E(transitionMatrix, transitionMatrix.getRowGroupIndices(), backwardTransitions, candidateStates, statesWithChoice);
                    
                    // Check if conferged
                    if (newCandidates == candidateStates) {
                        assert(!candidateStates.empty());
                        // return true;
                        break;
                    }
                }
                // return false;
                
                fixPointSw.stop();
                bool result = !candidateStates.empty();
                
                storm::utility::Stopwatch ecDecompSw(true);
                bool otherresult = false;
                auto mecDecomposition = storm::storage::MaximalEndComponentDecomposition<T>(transitionMatrix, backwardTransitions, candidateStates);
                for (auto& mec : mecDecomposition) {
                    for (auto const& stateActionsPair : mec) {
                        for (auto const& action : stateActionsPair.second) {
                            if (choices.get(action)) {
                                otherresult = true;
                                break;
                            }
                        }
                    }
                }
                
                STORM_LOG_THROW(result == otherresult, storm::exceptions::InvalidArgumentException, "Wrong result! Fixpoint says " << result << " mec says differently");
                ecDecompSw.stop();
                std::cout << "EC check: Fixpoint took " << fixPointSw << " seconds and MEC took " << ecDecompSw << " seconds." << std::endl;
                return result;

            }
        }
    }
}