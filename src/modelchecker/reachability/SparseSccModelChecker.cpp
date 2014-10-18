#include "src/modelchecker/reachability/SparseSccModelChecker.h"

#include <algorithm>

#include "src/storage/parameters.h"
#include "src/storage/StronglyConnectedComponentDecomposition.h"
#include "src/properties/Prctl.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"
#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace modelchecker {
        namespace reachability {
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::optional<std::vector<std::size_t>> const& distances) {
                auto totalTimeStart = std::chrono::high_resolution_clock::now();
                auto conversionStart = std::chrono::high_resolution_clock::now();

                // Create a bit vector that represents the subsystem of states we still have to eliminate.
                storm::storage::BitVector subsystem = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
                
                // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
                FlexibleSparseMatrix<ValueType> flexibleMatrix = getFlexibleSparseMatrix(transitionMatrix);
                FlexibleSparseMatrix<ValueType> flexibleBackwardTransitions = getFlexibleSparseMatrix(backwardTransitions, true);
                auto conversionEnd = std::chrono::high_resolution_clock::now();
                
                // Then, we recursively treat all SCCs.
                storm::utility::ConstantsComparator<ValueType> comparator;
                auto modelCheckingStart = std::chrono::high_resolution_clock::now();
                std::vector<storm::storage::sparse::state_type> entryStateQueue;
                uint_fast64_t maximalDepth = treatScc(flexibleMatrix, oneStepProbabilities, initialStates, subsystem, transitionMatrix, flexibleBackwardTransitions, false, 0, storm::settings::parametricSettings().getMaximalSccSize(), entryStateQueue, comparator, distances);
                
                // If the entry states were to be eliminated last, we need to do so now.
                STORM_LOG_DEBUG("Eliminating " << entryStateQueue.size() << " entry states as a last step.");
                if (storm::settings::parametricSettings().isEliminateEntryStatesLastSet()) {
                    for (auto const& state : entryStateQueue) {
                        eliminateState(flexibleMatrix, oneStepProbabilities, state, flexibleBackwardTransitions);
                    }
                }
                
                // Make sure that we have eliminated all transitions from the initial state.
                STORM_LOG_ASSERT(flexibleMatrix.getRow(*initialStates.begin()).empty(), "The transitions of the initial states are non-empty.");
                
                auto modelCheckingEnd = std::chrono::high_resolution_clock::now();
                auto totalTimeEnd = std::chrono::high_resolution_clock::now();
                
                if (storm::settings::generalSettings().isShowStatisticsSet()) {
                    auto conversionTime = conversionEnd - conversionStart;
                    auto modelCheckingTime = modelCheckingEnd - modelCheckingStart;
                    auto totalTime = totalTimeEnd - totalTimeStart;
                    
                    STORM_PRINT_AND_LOG(std::endl);
                    STORM_PRINT_AND_LOG("Time breakdown:" << std::endl);
                    STORM_PRINT_AND_LOG("    * time for conversion: " << std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime).count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG("    * time for checking: " << std::chrono::duration_cast<std::chrono::milliseconds>(modelCheckingTime).count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG("------------------------------------------" << std::endl);
                    STORM_PRINT_AND_LOG("    * total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalTime).count() << "ms" << std::endl);
                    STORM_PRINT_AND_LOG(std::endl);
                    STORM_PRINT_AND_LOG("Other:" << std::endl);
                    STORM_PRINT_AND_LOG("    * number of states eliminated: " << transitionMatrix.getRowCount() << std::endl);
                    STORM_PRINT_AND_LOG("    * maximal depth of SCC decomposition: " << maximalDepth << std::endl);
                }
                
                // Now, we return the value for the only initial state.
                return storm::utility::simplify(oneStepProbabilities[*initialStates.begin()]);
            }
            
            template<typename ValueType>
            ValueType SparseSccModelChecker<ValueType>::computeReachabilityProbability(storm::models::Dtmc<ValueType> const& dtmc, std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> const& filterFormula) {
                
                // The first thing we need to do is to make sure the formula is of the correct form and - if so - extract
                // the bitvector representation of the atomic propositions.
                std::shared_ptr<storm::properties::prctl::Until<double>> untilFormula = std::dynamic_pointer_cast<storm::properties::prctl::Until<double>>(filterFormula->getChild());
                std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> phiStateFormula;
                std::shared_ptr<storm::properties::prctl::AbstractStateFormula<double>> psiStateFormula;
                if (untilFormula.get() != nullptr) {
                    phiStateFormula = untilFormula->getLeft();
                    psiStateFormula = untilFormula->getRight();
                } else {
                    std::shared_ptr<storm::properties::prctl::Eventually<double>> eventuallyFormula = std::dynamic_pointer_cast<storm::properties::prctl::Eventually<double>>(filterFormula->getChild());
                    STORM_LOG_THROW(eventuallyFormula.get() != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *untilFormula << " for parametric model checking. Note that only unbounded reachability properties are admitted.");
                    
                    phiStateFormula = std::shared_ptr<storm::properties::prctl::Ap<double>>(new storm::properties::prctl::Ap<double>("true"));
                    psiStateFormula = eventuallyFormula->getChild();
                }
                
                // Now we need to make sure the formulas defining the phi and psi states are just labels.
                std::shared_ptr<storm::properties::prctl::Ap<double>> phiStateFormulaApFormula = std::dynamic_pointer_cast<storm::properties::prctl::Ap<double>>(phiStateFormula);
                std::shared_ptr<storm::properties::prctl::Ap<double>> psiStateFormulaApFormula = std::dynamic_pointer_cast<storm::properties::prctl::Ap<double>>(psiStateFormula);
                STORM_LOG_THROW(phiStateFormulaApFormula.get() != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *phiStateFormula << " for parametric model checking. Note that only atomic propositions are admitted in that position.");
                STORM_LOG_THROW(psiStateFormulaApFormula.get() != nullptr, storm::exceptions::InvalidPropertyException, "Illegal formula " << *psiStateFormula << " for parametric model checking. Note that only atomic propositions are admitted in that position.");
                
                // Now retrieve the appropriate bitvectors from the atomic propositions.
                storm::storage::BitVector phiStates = phiStateFormulaApFormula->getAp() != "true" ? dtmc.getLabeledStates(phiStateFormulaApFormula->getAp()) : storm::storage::BitVector(dtmc.getNumberOfStates(), true);
                storm::storage::BitVector psiStates = dtmc.getLabeledStates(psiStateFormulaApFormula->getAp());
                
                // Do some sanity checks to establish some required properties.
                STORM_LOG_THROW(dtmc.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
                
                // Then, compute the subset of states that has a probability of 0 or 1, respectively.
                std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(dtmc, phiStates, psiStates);
                storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
                storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
                storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);
                
                // If the initial state is known to have either probability 0 or 1, we can directly return the result.
                if (dtmc.getInitialStates().isDisjointFrom(maybeStates)) {
                    STORM_LOG_DEBUG("The probability of all initial states was found in a preprocessing step.");
                    return statesWithProbability0.get(*dtmc.getInitialStates().begin()) ? storm::utility::constantZero<ValueType>() : storm::utility::constantOne<ValueType>();
                }

                // Determine the set of states that is reachable from the initial state without jumping over a target state.
                storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(dtmc.getTransitionMatrix(), dtmc.getInitialStates(), maybeStates, statesWithProbability1);
                
                // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                maybeStates &= reachableStates;

                // Create a vector for the probabilities to go to a state with probability 1 in one step.
                std::vector<ValueType> oneStepProbabilities = dtmc.getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                
                // Determine the set of initial states of the sub-DTMC.
                storm::storage::BitVector newInitialStates = dtmc.getInitialStates() % maybeStates;
                
                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = dtmc.getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                
                // To be able to apply heuristics later, we now determine the distance of each state to the initial state.
                // We start by setting up the data structures.
                std::vector<std::pair<storm::storage::sparse::state_type, std::size_t>> stateQueue;
                stateQueue.reserve(submatrix.getRowCount());
                storm::storage::BitVector statesInQueue(submatrix.getRowCount());
                std::vector<std::size_t> distances(submatrix.getRowCount());
                
                storm::storage::sparse::state_type currentPosition = 0;
                for (auto const& initialState : newInitialStates) {
                    stateQueue.emplace_back(initialState, 0);
                    statesInQueue.set(initialState);
                }
                
                // And then perform the BFS.
                while (currentPosition < stateQueue.size()) {
                    std::pair<storm::storage::sparse::state_type, std::size_t> const& stateDistancePair = stateQueue[currentPosition];
                    distances[stateDistancePair.first] = stateDistancePair.second;
                    
                    for (auto const& successorEntry : submatrix.getRow(stateDistancePair.first)) {
                        if (!statesInQueue.get(successorEntry.getColumn())) {
                            stateQueue.emplace_back(successorEntry.getColumn(), stateDistancePair.second + 1);
                            statesInQueue.set(successorEntry.getColumn());
                        }
                    }
                    ++currentPosition;
                }
                
                return computeReachabilityProbability(submatrix, oneStepProbabilities, submatrix.transpose(), newInitialStates, phiStates, psiStates, distances);
            }
            
            template<typename ValueType>
            bool hasSelfLoop(storm::storage::sparse::state_type state, FlexibleSparseMatrix<ValueType> const& matrix) {
                for (auto const& entry : matrix.getRow(state)) {
                    if (entry.getColumn() < state) {
                        continue;
                    } else if (entry.getColumn() > state) {
                        return false;
                    } else if (entry.getColumn() == state) {
                        return true;
                    }
                }
                return false;
            }
            
            template<typename ValueType>
            uint_fast64_t SparseSccModelChecker<ValueType>::treatScc(FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator, boost::optional<std::vector<std::size_t>> const& distances) {
                uint_fast64_t maximalDepth = level;
                
                // If the SCCs are large enough, we try to split them further.
                if (scc.getNumberOfSetBits() > maximalSccSize) {
                    STORM_LOG_DEBUG("SCC is large enough (" << scc.getNumberOfSetBits() << " states) to be decomposed further.");
                    
                    // Here, we further decompose the SCC into sub-SCCs.
                    storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition(forwardTransitions, scc & ~entryStates, false, false);
                    STORM_LOG_DEBUG("Decomposed SCC into " << decomposition.size() << " sub-SCCs.");
                    
                    // Store a bit vector of remaining SCCs so we can be flexible when it comes to the order in which
                    // we eliminate the SCCs.
                    storm::storage::BitVector remainingSccs(decomposition.size(), true);
                    
                    // First, get rid of the trivial SCCs.
                    std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> trivialSccs;
                    for (uint_fast64_t sccIndex = 0; sccIndex < decomposition.size(); ++sccIndex) {
                        storm::storage::StronglyConnectedComponent const& scc = decomposition.getBlock(sccIndex);
                        if (scc.isTrivial()) {
                            storm::storage::sparse::state_type onlyState = *scc.begin();
                            trivialSccs.emplace_back(onlyState, sccIndex);
                        }
                    }
                    
                    STORM_LOG_DEBUG("Eliminating " << trivialSccs.size() << " trivial SCCs.");
                    if (storm::settings::parametricSettings().isSortTrivialSccsSet()) {
                        STORM_LOG_THROW(distances, storm::exceptions::IllegalFunctionCallException, "Cannot sort according to distances because none were provided.");
                        std::vector<std::size_t> const& actualDistances = distances.get();
//                        std::sort(trivialSccs.begin(), trivialSccs.end(), [&actualDistances] (std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& state1, std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& state2) -> bool { return actualDistances[state1.first] > actualDistances[state2.first]; } );

                        std::sort(trivialSccs.begin(), trivialSccs.end(), [&oneStepProbabilities,&comparator] (std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& state1, std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& state2) -> bool { return comparator.isZero(oneStepProbabilities[state1.first]) && !comparator.isZero(oneStepProbabilities[state2.first]); } );
                    }
                    for (auto const& stateIndexPair : trivialSccs) {
                        eliminateState(matrix, oneStepProbabilities, stateIndexPair.first, backwardTransitions);
                        remainingSccs.set(stateIndexPair.second, false);
                    }
                    STORM_LOG_DEBUG("Eliminated all trivial SCCs.");
                    
                    // And then recursively treat the remaining sub-SCCs.
                    STORM_LOG_DEBUG("Eliminating " << remainingSccs.getNumberOfSetBits() << " remaining SCCs on level " << level << ".");
                    for (auto sccIndex : remainingSccs) {
                        storm::storage::StronglyConnectedComponent const& newScc = decomposition.getBlock(sccIndex);

                        // Rewrite SCC into bit vector and subtract it from the remaining states.
                        storm::storage::BitVector newSccAsBitVector(forwardTransitions.getRowCount(), newScc.begin(), newScc.end());
                        
                        // Determine the set of entry states of the SCC.
                        storm::storage::BitVector entryStates(forwardTransitions.getRowCount());
                        for (auto const& state : newScc) {
                            for (auto const& predecessor : backwardTransitions.getRow(state)) {
                                if (predecessor.getValue() != storm::utility::constantZero<ValueType>() && !newSccAsBitVector.get(predecessor.getColumn())) {
                                    entryStates.set(state);
                                }
                            }
                        }
                        
                        // Recursively descend in SCC-hierarchy.
                        uint_fast64_t depth = treatScc(matrix, oneStepProbabilities, entryStates, newSccAsBitVector, forwardTransitions, backwardTransitions, !storm::settings::parametricSettings().isEliminateEntryStatesLastSet(), level + 1, maximalSccSize, entryStateQueue, comparator, distances);
                        maximalDepth = std::max(maximalDepth, depth);
                    }
                    
                } else {
                    // In this case, we perform simple state elimination in the current SCC.
                    STORM_LOG_DEBUG("SCC of size " << scc.getNumberOfSetBits() << " is small enough to be eliminated directly.");
                    storm::storage::BitVector remainingStates = scc & ~entryStates;
                    
                    std::vector<uint_fast64_t> statesToEliminate(remainingStates.begin(), remainingStates.end());
                    if (storm::settings::parametricSettings().isSortTrivialSccsSet()) {
//                        STORM_LOG_THROW(distances, storm::exceptions::IllegalFunctionCallException, "Cannot sort according to distances because none were provided.");
//                        std::vector<std::size_t> const& actualDistances = distances.get();
//                        std::sort(statesToEliminate.begin(), statesToEliminate.end(), [&actualDistances] (storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) -> bool { return actualDistances[state1] > actualDistances[state2]; } );

                        std::sort(statesToEliminate.begin(), statesToEliminate.end(), [&oneStepProbabilities,&comparator] (storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) -> bool { return comparator.isZero(oneStepProbabilities[state1]) && !comparator.isZero(oneStepProbabilities[state2]); } );

                    }
                    
                    // Eliminate the remaining states that do not have a self-loop (in the current, i.e. modified)
                    // transition probability matrix.
                    for (auto const& state : statesToEliminate) {
//                        if (!hasSelfLoop(state, matrix)) {
                            eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
//                            remainingStates.set(state, false);
//                        }
                    }

                    STORM_LOG_DEBUG("Eliminated all states without self-loop.");
                    
                    // Eliminate the remaining states.
//                    for (auto const& state : statesToEliminate) {
//                        eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
//                    }

                    STORM_LOG_DEBUG("Eliminated all states with self-loop.");
                }
                
                // Finally, eliminate the entry states (if we are required to do so).
                STORM_LOG_DEBUG("Finally, eliminating/adding entry states.");
                if (eliminateEntryStates) {
                    for (auto state : entryStates) {
                        eliminateState(matrix, oneStepProbabilities, state, backwardTransitions);
                    }
                } else {
                    for (auto state : entryStates) {
                        entryStateQueue.push_back(state);
                    }
                }
                STORM_LOG_DEBUG("Eliminated/added entry states.");

                return maximalDepth;
            }
            
            static int chunkCounter = 0;
            static int counter = 0;
            
            template<typename ValueType>
            void SparseSccModelChecker<ValueType>::eliminateState(FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix<ValueType>& backwardTransitions) {
                
                auto eliminationStart = std::chrono::high_resolution_clock::now();
                
                ++counter;
                STORM_LOG_DEBUG("Eliminating state " << state << ".");
                if (counter > matrix.getNumberOfRows() / 10) {
                    ++chunkCounter;
                    STORM_PRINT_AND_LOG("Eliminated " << (chunkCounter * 10) << "% of the states." << std::endl);
                    counter = 0;
                }
                
                bool hasSelfLoop = false;
                ValueType loopProbability = storm::utility::constantZero<ValueType>();
                
                // Start by finding loop probability.
                typename FlexibleSparseMatrix<ValueType>::row_type& currentStateSuccessors = matrix.getRow(state);
                for (auto const& entry : currentStateSuccessors) {
                    if (entry.getColumn() >= state) {
                        if (entry.getColumn() == state) {
                            loopProbability = entry.getValue();
                            hasSelfLoop = true;
                        }
                        break;
                    }
                }
                
                std::chrono::high_resolution_clock::time_point simplifyClock;
                std::chrono::high_resolution_clock::duration simplifyTime;
                std::chrono::high_resolution_clock::time_point multiplicationClock;
                std::chrono::high_resolution_clock::duration multiplicationTime;
                std::chrono::high_resolution_clock::time_point additionClock;
                std::chrono::high_resolution_clock::duration additionTime;
                std::chrono::high_resolution_clock::time_point additionClock2;
                std::chrono::high_resolution_clock::duration additionTime2;
                
                // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
                std::size_t scaledSuccessors = 0;
                if (hasSelfLoop) {
                    loopProbability = storm::utility::constantOne<ValueType>() / (storm::utility::constantOne<ValueType>() - loopProbability);
                    storm::utility::simplify(loopProbability);
                    for (auto& entry : matrix.getRow(state)) {
                        // Only scale the non-diagonal entries.
                        if (entry.getColumn() != state) {
                            ++scaledSuccessors;
                            multiplicationClock = std::chrono::high_resolution_clock::now();
                            auto result = entry.getValue() * loopProbability;
                            multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
                            entry.setValue(result);
                        }
                    }
                    multiplicationClock = std::chrono::high_resolution_clock::now();
                    auto result = oneStepProbabilities[state] * loopProbability;
                    multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
                    oneStepProbabilities[state] = result;
                }
                
                STORM_LOG_DEBUG((hasSelfLoop ? "State has self-loop." : "State does not have a self-loop."));
                
                // Now connect the predecessors of the state being eliminated with its successors.
                typename FlexibleSparseMatrix<ValueType>::row_type& currentStatePredecessors = backwardTransitions.getRow(state);
                std::size_t numberOfPredecessors = currentStatePredecessors.size();
                std::size_t predecessorForwardTransitionCount = 0;
                for (auto const& predecessorEntry : currentStatePredecessors) {
                    uint_fast64_t predecessor = predecessorEntry.getColumn();
                    
                    // Skip the state itself as one of its predecessors.
                    if (predecessor == state) {
                        assert(hasSelfLoop);
                        continue;
                    }
                    
                    // First, find the probability with which the predecessor can move to the current state, because
                    // the other probabilities need to be scaled with this factor.
                    typename FlexibleSparseMatrix<ValueType>::row_type& predecessorForwardTransitions = matrix.getRow(predecessor);
                    predecessorForwardTransitionCount += predecessorForwardTransitions.size();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator multiplyElement = std::find_if(predecessorForwardTransitions.begin(), predecessorForwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                    
                    // Make sure we have found the probability and set it to zero.
                    STORM_LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
                    ValueType multiplyFactor = multiplyElement->getValue();
                    multiplyElement->setValue(storm::utility::constantZero<ValueType>());
                    
                    // At this point, we need to update the (forward) transitions of the predecessor.
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first1 = predecessorForwardTransitions.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last1 = predecessorForwardTransitions.end();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first2 = currentStateSuccessors.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last2 = currentStateSuccessors.end();
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type newSuccessors;
                    newSuccessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<typename FlexibleSparseMatrix<ValueType>::row_type> result(newSuccessors, newSuccessors.end());
                    
                    // Now we merge the two successor lists. (Code taken from std::set_union and modified to suit our needs).
                    for (; first1 != last1; ++result) {
                        // Skip the transitions to the state that is currently being eliminated.
                        if (first1->getColumn() == state || (first2 != last2 && first2->getColumn() == state)) {
                            if (first1->getColumn() == state) {
                                ++first1;
                            }
                            if (first2 != last2 && first2->getColumn() == state) {
                                ++first2;
                            }
                            continue;
                        }
                        
                        if (first2 == last2) {
                            std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; } );
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            multiplicationClock = std::chrono::high_resolution_clock::now();
                            auto tmpResult = *first2 * multiplyFactor;
                            multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
                            simplifyClock = std::chrono::high_resolution_clock::now();
                            *result = storm::utility::simplify(tmpResult);
                            simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
                            ++first2;
                        } else if (first1->getColumn() < first2->getColumn()) {
                            *result = *first1;
                            ++first1;
                        } else {
                            multiplicationClock = std::chrono::high_resolution_clock::now();
                            auto tmp1 = multiplyFactor * first2->getValue();
                            multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
                            simplifyClock = std::chrono::high_resolution_clock::now();
                            tmp1 = storm::utility::simplify(tmp1);
                            multiplicationClock = std::chrono::high_resolution_clock::now();
                            simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
                            additionClock = std::chrono::high_resolution_clock::now();
                            auto tmp2 = first1->getValue() + tmp1;
                            additionTime += std::chrono::high_resolution_clock::now() - additionClock;
                            simplifyClock = std::chrono::high_resolution_clock::now();
                            tmp2 = storm::utility::simplify(tmp2);
                            simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
                            *result = storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type>(first1->getColumn(), tmp2);
                            ++first1;
                            ++first2;
                        }
                    }
                    for (; first2 != last2; ++first2) {
                        if (first2->getColumn() != state) {
                            multiplicationClock = std::chrono::high_resolution_clock::now();
                            auto tmpResult = *first2 * multiplyFactor;
                            multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
                            simplifyClock = std::chrono::high_resolution_clock::now();
                            *result = storm::utility::simplify(tmpResult);
                            simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
                        }
                    }
                    
                    // Now move the new transitions in place.
                    predecessorForwardTransitions = std::move(newSuccessors);
                    
                    // Add the probabilities to go to a target state in just one step.
//                    multiplicationClock = std::chrono::high_resolution_clock::now();
//                    auto tmp1 = multiplyFactor * oneStepProbabilities[state];
//                    multiplicationTime += std::chrono::high_resolution_clock::now() - multiplicationClock;
//                    simplifyClock = std::chrono::high_resolution_clock::now();
//                    tmp1 = storm::utility::simplify(tmp1);
//                    simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
//                    auto tmp2 = oneStepProbabilities[predecessor] + tmp1;
//                    simplifyClock = std::chrono::high_resolution_clock::now();
//                    tmp2 = storm::utility::simplify(tmp2);
//                    simplifyTime += std::chrono::high_resolution_clock::now() - simplifyClock;
                    additionClock2 = std::chrono::high_resolution_clock::now();
                    oneStepProbabilities[predecessor] += storm::utility::simplify(multiplyFactor * oneStepProbabilities[state]);
                    additionTime2 += std::chrono::high_resolution_clock::now() - additionClock2;
                    
                    STORM_LOG_DEBUG("Fixed new next-state probabilities of predecessor states.");
                }
                
                // Finally, we need to add the predecessor to the set of predecessors of every successor.
                for (auto const& successorEntry : currentStateSuccessors) {
                    typename FlexibleSparseMatrix<ValueType>::row_type& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
                    
                    // Delete the current state as a predecessor of the successor state.
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator elimIt = std::find_if(successorBackwardTransitions.begin(), successorBackwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                    if (elimIt != successorBackwardTransitions.end()) {
                        successorBackwardTransitions.erase(elimIt);
                    }
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first1 = successorBackwardTransitions.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last1 = successorBackwardTransitions.end();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator first2 = currentStatePredecessors.begin();
                    typename FlexibleSparseMatrix<ValueType>::row_type::iterator last2 = currentStatePredecessors.end();
                    
                    typename FlexibleSparseMatrix<ValueType>::row_type newPredecessors;
                    newPredecessors.reserve((last1 - first1) + (last2 - first2));
                    std::insert_iterator<typename FlexibleSparseMatrix<ValueType>::row_type> result(newPredecessors, newPredecessors.end());
                    
                    
                    for (; first1 != last1; ++result) {
                        if (first2 == last2) {
                            std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; });
                            break;
                        }
                        if (first2->getColumn() < first1->getColumn()) {
                            if (first2->getColumn() != state) {
                                *result = *first2;
                            }
                            ++first2;
                        } else {
                            if (first1->getColumn() != state) {
                                *result = *first1;
                            }
                            if (first1->getColumn() == first2->getColumn()) {
                                ++first2;
                            }
                            ++first1;
                        }
                    }
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix<ValueType>::index_type, typename FlexibleSparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() != state; });
                    
                    // Now move the new predecessors in place.
                    successorBackwardTransitions = std::move(newPredecessors);
                    
                }
                STORM_LOG_DEBUG("Fixed predecessor lists of successor states.");
                
                // Clear the eliminated row to reduce memory consumption.
                currentStateSuccessors.clear();
                currentStateSuccessors.shrink_to_fit();
                currentStatePredecessors.clear();
                currentStatePredecessors.shrink_to_fit();
                
                auto eliminationEnd = std::chrono::high_resolution_clock::now();
                auto eliminationTime = eliminationEnd - eliminationStart;
                
                // If the elimination took more than 3 seconds, we print some more information and quit.
                if (std::chrono::duration_cast<std::chrono::milliseconds>(eliminationTime).count() > 3000) {
                    STORM_PRINT("Elimination took more than 3 seconds (actually took " << std::chrono::duration_cast<std::chrono::milliseconds>(eliminationTime).count() << "ms)." << std::endl);
                    STORM_PRINT("Simplification took " << std::chrono::duration_cast<std::chrono::milliseconds>(simplifyTime).count() << "ms." << std::endl);
                    STORM_PRINT("Multiplication took " << std::chrono::duration_cast<std::chrono::milliseconds>(multiplicationTime).count() << "ms." << std::endl);
                    STORM_PRINT("Addition1 took " << std::chrono::duration_cast<std::chrono::milliseconds>(additionTime).count() << "ms." << std::endl);
                    STORM_PRINT("Addition2 took " << std::chrono::duration_cast<std::chrono::milliseconds>(additionTime2).count() << "ms." << std::endl);
                    STORM_PRINT("Number of scaled successors: " << scaledSuccessors << "." << std::endl);
                    STORM_PRINT("Number of predecessors: " << numberOfPredecessors << "." << std::endl);
                    STORM_PRINT("Number of predecessor forward transitions " << predecessorForwardTransitionCount << "." << std::endl);
                }
            }
            
            template <typename ValueType>
            bool SparseSccModelChecker<ValueType>::eliminateStateInPlace(storm::storage::SparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, storm::storage::SparseMatrix<ValueType>& backwardTransitions) {
                typename storm::storage::SparseMatrix<ValueType>::iterator forwardElement = matrix.getRow(state).begin();
                typename storm::storage::SparseMatrix<ValueType>::iterator backwardElement = backwardTransitions.getRow(state).begin();
                
                if (forwardElement->getValue() != storm::utility::constantOne<ValueType>() || backwardElement->getValue() != storm::utility::constantOne<ValueType>()) {
                    return false;
                }
                
                std::cout << "eliminating " << state << std::endl;
                std::cout << "fwd element: " << *forwardElement << " and bwd element: " << *backwardElement << std::endl;
                
                // Find the element of the predecessor that moves to the state that we want to eliminate.
                typename storm::storage::SparseMatrix<ValueType>::rows forwardRow = matrix.getRow(backwardElement->getColumn());
                typename storm::storage::SparseMatrix<ValueType>::iterator multiplyElement = std::find_if(forwardRow.begin(), forwardRow.end(), [&](storm::storage::MatrixEntry<typename storm::storage::SparseMatrix<ValueType>::index_type, typename storm::storage::SparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                
                std::cout << "before fwd: " << std::endl;
                for (auto element : matrix.getRow(backwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Modify the forward probability entry of the predecessor.
                multiplyElement->setValue(multiplyElement->getValue() * forwardElement->getValue());
                multiplyElement->setColumn(forwardElement->getColumn());
                
                // Modify the one-step probability for the predecessor if necessary.
                if (oneStepProbabilities[state] != storm::utility::constantZero<ValueType>()) {
                    oneStepProbabilities[backwardElement->getColumn()] += multiplyElement->getValue() * oneStepProbabilities[state];
                }
                
                // If the forward entry is not at the right position, we need to move it there.
                if (multiplyElement != forwardRow.begin() && multiplyElement->getColumn() < (multiplyElement - 1)->getColumn()) {
                    while (multiplyElement != forwardRow.begin() && multiplyElement->getColumn() < (multiplyElement - 1)->getColumn()) {
                        std::swap(*multiplyElement, *(multiplyElement - 1));
                        --multiplyElement;
                    }
                } else if ((multiplyElement + 1) != forwardRow.end() && multiplyElement->getColumn() > (multiplyElement + 1)->getColumn()) {
                    while ((multiplyElement + 1) != forwardRow.end() && multiplyElement->getColumn() > (multiplyElement + 1)->getColumn()) {
                        std::swap(*multiplyElement, *(multiplyElement + 1));
                        ++multiplyElement;
                    }
                }
                
                std::cout << "after fwd: " << std::endl;
                for (auto element : matrix.getRow(backwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Find the backward element of the successor that moves to the state that we want to eliminate.
                typename storm::storage::SparseMatrix<ValueType>::rows backwardRow = backwardTransitions.getRow(forwardElement->getColumn());
                typename storm::storage::SparseMatrix<ValueType>::iterator backwardEntry = std::find_if(backwardRow.begin(), backwardRow.end(), [&](storm::storage::MatrixEntry<typename storm::storage::SparseMatrix<ValueType>::index_type, typename storm::storage::SparseMatrix<ValueType>::value_type> const& a) { return a.getColumn() == state; });
                
                std::cout << "before bwd" << std::endl;
                for (auto element : backwardTransitions.getRow(forwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                
                // Modify the predecessor list of the successor and add the predecessor of the state we eliminate.
                backwardEntry->setColumn(backwardElement->getColumn());
                
                // If the backward entry is not at the right position, we need to move it there.
                if (backwardEntry != backwardRow.begin() && backwardEntry->getColumn() < (backwardEntry - 1)->getColumn()) {
                    while (backwardEntry != backwardRow.begin() && backwardEntry->getColumn() < (backwardEntry - 1)->getColumn()) {
                        std::swap(*backwardEntry, *(backwardEntry - 1));
                        --backwardEntry;
                    }
                } else if ((backwardEntry + 1) != backwardRow.end() && backwardEntry->getColumn() > (backwardEntry + 1)->getColumn()) {
                    while ((backwardEntry + 1) != backwardRow.end() && backwardEntry->getColumn() > (backwardEntry + 1)->getColumn()) {
                        std::swap(*backwardEntry, *(backwardEntry + 1));
                        ++backwardEntry;
                    }
                }
                
                std::cout << "after bwd" << std::endl;
                for (auto element : backwardTransitions.getRow(forwardElement->getColumn())) {
                    std::cout << element << ", " << std::endl;
                }
                return true;
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType>::FlexibleSparseMatrix(index_type rows) : data(rows) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            void FlexibleSparseMatrix<ValueType>::reserveInRow(index_type row, index_type numberOfElements) {
                this->data[row].reserve(numberOfElements);
            }
            
            template<typename ValueType>
            typename FlexibleSparseMatrix<ValueType>::row_type& FlexibleSparseMatrix<ValueType>::getRow(index_type index) {
                return this->data[index];
            }
            
            template<typename ValueType>
            typename FlexibleSparseMatrix<ValueType>::row_type const& FlexibleSparseMatrix<ValueType>::getRow(index_type index) const {
                return this->data[index];
            }

            template<typename ValueType>
            typename FlexibleSparseMatrix<ValueType>::index_type FlexibleSparseMatrix<ValueType>::getNumberOfRows() const {
                return this->data.size();
            }
            
            template<typename ValueType>
            FlexibleSparseMatrix<ValueType> SparseSccModelChecker<ValueType>::getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) {
                FlexibleSparseMatrix<ValueType> flexibleMatrix(matrix.getRowCount());
                
                for (typename FlexibleSparseMatrix<ValueType>::index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                    typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                    flexibleMatrix.reserveInRow(rowIndex, row.getNumberOfEntries());
                    
                    for (auto const& element : row) {
                        if (setAllValuesToOne) {
                            flexibleMatrix.getRow(rowIndex).emplace_back(element.getColumn(), storm::utility::constantOne<ValueType>());
                        } else {
                            flexibleMatrix.getRow(rowIndex).emplace_back(element);
                        }
                    }
                }
                
                return flexibleMatrix;
            }
            
            template class FlexibleSparseMatrix<double>;
            template class SparseSccModelChecker<double>;
            #ifdef PARAMETRIC_SYSTEMS
            template class FlexibleSparseMatrix<RationalFunction>;
            template class SparseSccModelChecker<RationalFunction>;
            #endif
        } // namespace reachability
    } // namespace modelchecker
} // namespace storm
