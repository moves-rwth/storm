#include "src/builder/ExplicitDFTModelBuilder.h"
#include <src/models/sparse/MarkovAutomaton.h>
#include <src/models/sparse/Ctmc.h>
#include <src/utility/constants.h>
#include <src/utility/vector.h>
#include <src/exceptions/UnexpectedException.h>
#include <map>

namespace storm {
    namespace builder {

        template <typename ValueType>
        ExplicitDFTModelBuilder<ValueType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), markovianStates(), exitRates(), choiceLabeling() {
            // Intentionally left empty.
        }
        
        template <typename ValueType>
        ExplicitDFTModelBuilder<ValueType>::ExplicitDFTModelBuilder(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTIndependentSymmetries const& symmetries, bool enableDC) : mDft(dft), mStates(((mDft.stateVectorSize() / 64) + 1) * 64, INITIAL_BUCKETSIZE), enableDC(enableDC) {
            // stateVectorSize is bound for size of bitvector

            mStateGenerationInfo = std::make_shared<storm::storage::DFTStateGenerationInfo>(mDft.buildStateGenerationInfo(symmetries));
        }


        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilder<ValueType>::buildModel(LabelOptions const& labelOpts) {
            // Initialize
            bool deterministicModel = false;
            ModelComponents modelComponents;
            std::vector<uint_fast64_t> tmpMarkovianStates;
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);
            

            if(mergeFailedStates) {
                newIndex++;
                
                transitionMatrixBuilder.newRowGroup(failedIndex);
                transitionMatrixBuilder.addNextValue(failedIndex, failedIndex, storm::utility::one<ValueType>());
                STORM_LOG_TRACE("Added self loop for " << failedIndex);
                modelComponents.exitRates.push_back(storm::utility::one<ValueType>());
                tmpMarkovianStates.push_back(failedIndex);        
            }
            
            DFTStatePointer state = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, *mStateGenerationInfo, newIndex++);
            mStates.findOrAdd(state->status(), state->getId());
            initialStateIndex = state->getId();
            std::queue<DFTStatePointer> stateQueue;
            stateQueue.push(state);
            
            // Begin model generation
            bool deterministic = exploreStates(stateQueue, transitionMatrixBuilder, tmpMarkovianStates, modelComponents.exitRates);
            STORM_LOG_DEBUG("Generated " << mStates.size() + (mergeFailedStates ? 1 : 0) << " states");
            STORM_LOG_DEBUG("Model is " << (deterministic ? "deterministic" : "non-deterministic"));

            size_t stateSize = mStates.size() + (mergeFailedStates ? 1 : 0);
            // Build Markov Automaton
            modelComponents.markovianStates = storm::storage::BitVector(stateSize, tmpMarkovianStates);
            // Build transition matrix
            modelComponents.transitionMatrix = transitionMatrixBuilder.build(stateSize, stateSize);
            if (stateSize <= 15) {
                STORM_LOG_TRACE("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
            } else {
                STORM_LOG_TRACE("Transition matrix: too big to print");
            }
            STORM_LOG_TRACE("Exit rates: " << modelComponents.exitRates);
            STORM_LOG_TRACE("Markovian states: " << modelComponents.markovianStates);
            
            // Build state labeling
            modelComponents.stateLabeling = storm::models::sparse::StateLabeling(mStates.size() + (mergeFailedStates ? 1 : 0));
            // Initial state is always first state without any failure
            modelComponents.stateLabeling.addLabel("init");
            modelComponents.stateLabeling.addLabelToState("init", initialStateIndex);
            // Label all states corresponding to their status (failed, failsafe, failed BE)
            if(labelOpts.buildFailLabel) {
                modelComponents.stateLabeling.addLabel("failed");
            } 
            if(labelOpts.buildFailSafeLabel) {
                modelComponents.stateLabeling.addLabel("failsafe");
            }
            
            // Collect labels for all BE
            std::vector<std::shared_ptr<storage::DFTBE<ValueType>>> basicElements = mDft.getBasicElements();
            for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                if(labelOpts.beLabels.count(elem->name()) > 0) {
                    modelComponents.stateLabeling.addLabel(elem->name() + "_fail");
                }
            }

            if(mergeFailedStates) {
                modelComponents.stateLabeling.addLabelToState("failed", failedIndex);
            }
            for (auto const& stateIdPair : mStates) {
                storm::storage::BitVector state = stateIdPair.first;
                size_t stateId = stateIdPair.second;
                if (!mergeFailedStates && labelOpts.buildFailLabel && mDft.hasFailed(state, *mStateGenerationInfo)) {
                    modelComponents.stateLabeling.addLabelToState("failed", stateId);
                }
                if (labelOpts.buildFailSafeLabel && mDft.isFailsafe(state, *mStateGenerationInfo)) {
                    modelComponents.stateLabeling.addLabelToState("failsafe", stateId);
                };
                // Set fail status for each BE
                for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                    if (labelOpts.beLabels.count(elem->name()) > 0 && storm::storage::DFTState<ValueType>::hasFailed(state, mStateGenerationInfo->getStateIndex(elem->id())) ) {
                        modelComponents.stateLabeling.addLabelToState(elem->name() + "_fail", stateId);
                    }
                }
            }

            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
            
            if (deterministic) {
                // Turn the probabilities into rates by multiplying each row with the exit rate of the state.
                // TODO Matthias: avoid transforming back and forth
                storm::storage::SparseMatrix<ValueType> rateMatrix(modelComponents.transitionMatrix);
                for (uint_fast64_t row = 0; row < rateMatrix.getRowCount(); ++row) {
                    assert(row < modelComponents.markovianStates.size());
                    if (modelComponents.markovianStates.get(row)) {
                        for (auto& entry : rateMatrix.getRow(row)) {
                            entry.setValue(entry.getValue() * modelComponents.exitRates[row]);
                        }
                    }
                }
                model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(rateMatrix), std::move(modelComponents.stateLabeling));
            } else {
                std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.markovianStates), std::move(modelComponents.exitRates), true);
                if (ma->hasOnlyTrivialNondeterminism()) {
                 // Markov automaton can be converted into CTMC
                    model = ma->convertToCTMC();
                } else {
                    model = ma;
                }
            }
            
            return model;
        }

        template <typename ValueType>
        bool ExplicitDFTModelBuilder<ValueType>::exploreStates(std::queue<DFTStatePointer>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<uint_fast64_t>& markovianStates, std::vector<ValueType>& exitRates) {

            // TODO Matthias: set Markovian states directly as bitvector?
            std::map<uint_fast64_t, ValueType> outgoingTransitions;
            size_t rowOffset = 0; // Captures number of non-deterministic choices
            bool deterministic = true;
            
            
            while (!stateQueue.empty()) {
                // Initialization
                outgoingTransitions.clear();
                ValueType exitRate = storm::utility::zero<ValueType>();

                // Consider next state
                DFTStatePointer state = stateQueue.front();
                stateQueue.pop();

                bool hasDependencies = state->nrFailableDependencies() > 0;
                deterministic &= !hasDependencies;
                size_t failableCount = hasDependencies ? state->nrFailableDependencies() : state->nrFailableBEs();
                size_t smallest = 0;

                transitionMatrixBuilder.newRowGroup(state->getId() + rowOffset);

                // Add self loop for target states
                if (mDft.hasFailed(state) || mDft.isFailsafe(state)) {
                    transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, state->getId(), storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Added self loop for " << state->getId());
                    exitRates.push_back(storm::utility::one<ValueType>());
                    assert(exitRates.size()-1 == state->getId());
                    markovianStates.push_back(state->getId());
                    // No further exploration required
                    continue;
                } else {
                    STORM_LOG_THROW(state->nrFailableBEs() > 0, storm::exceptions::UnexpectedException, "State " << state->getId() << " is no target state but behaves like one");
                }

                // Let BE fail
                while (smallest < failableCount) {
                    assert(!mDft.hasFailed(state));
                    STORM_LOG_TRACE("exploring from: " << mDft.getStateString(state));

                    // Construct new state as copy from original one
                    DFTStatePointer newState = std::make_shared<storm::storage::DFTState<ValueType>>(*state);
                    std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType> const>, bool> nextBEPair = newState->letNextBEFail(smallest++);
                    std::shared_ptr<storm::storage::DFTBE<ValueType> const>& nextBE = nextBEPair.first;
                    assert(nextBE);
                    assert(nextBEPair.second == hasDependencies);
                    STORM_LOG_TRACE("with the failure of: " << nextBE->name() << " [" << nextBE->id() << "]");

                    // Propagate failures
                    storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;

                    for (DFTGatePointer parent : nextBE->parents()) {
                        if (newState->isOperational(parent->id())) {
                            queues.propagateFailure(parent);
                        }
                    }
                    for (DFTRestrictionPointer restr : nextBE->restrictions()) {
                        queues.checkRestrictionLater(restr);
                    }

                    while (!queues.failurePropagationDone()) {
                        DFTGatePointer next = queues.nextFailurePropagation();
                        next->checkFails(*newState, queues);
                        newState->updateFailableDependencies(next->id());
                    }
                    
                    while(!queues.restrictionChecksDone()) {
                        DFTRestrictionPointer next = queues.nextRestrictionCheck();
                        next->checkFails(*newState, queues);
                        newState->updateFailableDependencies(next->id());
                    }
                    
                    if(newState->isInvalid()) {
                        continue;
                    }

                    while (!queues.failsafePropagationDone()) {
                        DFTGatePointer next = queues.nextFailsafePropagation();
                        next->checkFailsafe(*newState, queues);
                    }

                    while (enableDC && !queues.dontCarePropagationDone()) {
                        DFTElementPointer next = queues.nextDontCarePropagation();
                        next->checkDontCareAnymore(*newState, queues);
                    }
                    
                    // Update failable dependencies
                    newState->updateFailableDependencies(nextBE->id());
                    
                    bool dftFailed = newState->hasFailed(mDft.getTopLevelIndex());
                    uint_fast64_t newStateId;
                    if(dftFailed && mergeFailedStates) {
                        newStateId = failedIndex;
                    } else {
                        // Order state by symmetry
                        STORM_LOG_TRACE("Check for symmetry: " << mDft.getStateString(newState));
                        bool changed = newState->orderBySymmetry();
                        STORM_LOG_TRACE("State " << (changed ? "changed to " : "did not change") << (changed ? mDft.getStateString(newState) : ""));
                        
                        // Check if state already exists
                        if (mStates.contains(newState->status())) {
                            // State already exists
                            newStateId = mStates.getValue(newState->status());
                            STORM_LOG_TRACE("State " << mDft.getStateString(newState) << " with id " << newStateId << " already exists");
                            
                            // Check if possible pseudo state can be created now
                            if (!changed && newStateId >= OFFSET_PSEUDO_STATE) {
                                newStateId = newStateId - OFFSET_PSEUDO_STATE;
                                assert(newStateId < mPseudoStatesMapping.size());
                                if (mPseudoStatesMapping[newStateId].first == 0) {
                                    // Create pseudo state now
                                    assert(mPseudoStatesMapping[newStateId].second == newState->status());
                                    newState->setId(newIndex++);
                                    mPseudoStatesMapping[newStateId].first = newState->getId();
                                    newStateId = newState->getId();
                                    mStates.setOrAdd(newState->status(), newStateId);
                                    stateQueue.push(newState);
                                    STORM_LOG_TRACE("Now create state " << mDft.getStateString(newState) << " with id " << newStateId);
                                }
                            }
                        } else {
                            // New state
                            if (changed) {
                                // Remember to create state later on
                                newState->setId(mPseudoStatesMapping.size() + OFFSET_PSEUDO_STATE);
                                mPseudoStatesMapping.push_back(std::make_pair(0, newState->status()));
                                newStateId = mStates.findOrAdd(newState->status(), newState->getId());
                                STORM_LOG_TRACE("New state for later creation: " << mDft.getStateString(newState));
                            } else {
                                // Create new state
                                newState->setId(newIndex++);
                                newStateId = mStates.findOrAdd(newState->status(), newState->getId());
                                STORM_LOG_TRACE("New state: " << mDft.getStateString(newState));
                                stateQueue.push(newState);
                            }
                        }
                    }

                    // Set transitions
                    if (hasDependencies) {
                        // Failure is due to dependency -> add non-deterministic choice
                        std::shared_ptr<storm::storage::DFTDependency<ValueType> const> dependency = mDft.getDependency(state->getDependencyId(smallest-1));
                        transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, newStateId, dependency->probability());
                        STORM_LOG_TRACE("Added transition from " << state->getId() << " to " << newStateId << " with probability " << dependency->probability());

                        if (!storm::utility::isOne(dependency->probability())) {
                            // TODO Matthias: use symmetry as well
                            // Add transition to state where dependency was unsuccessful
                            DFTStatePointer unsuccessfulState = std::make_shared<storm::storage::DFTState<ValueType>>(*state);
                            unsuccessfulState->letDependencyBeUnsuccessful(smallest-1);
                            uint_fast64_t unsuccessfulStateId;
                            if (mStates.contains(unsuccessfulState->status())) {
                                // Unsuccessful state already exists
                                unsuccessfulStateId = mStates.getValue(unsuccessfulState->status());
                                STORM_LOG_TRACE("State " << mDft.getStateString(unsuccessfulState) << " with id " << unsuccessfulStateId << " already exists");
                            } else {
                                // New unsuccessful state
                                unsuccessfulState->setId(newIndex++);
                                unsuccessfulStateId = mStates.findOrAdd(unsuccessfulState->status(), unsuccessfulState->getId());
                                STORM_LOG_TRACE("New state " << mDft.getStateString(unsuccessfulState));
                                
                                // Add unsuccessful state to search queue
                                stateQueue.push(unsuccessfulState);
                            }

                            ValueType remainingProbability = storm::utility::one<ValueType>() - dependency->probability();
                            transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, unsuccessfulStateId, remainingProbability);
                            STORM_LOG_TRACE("Added transition from " << state->getId() << " to " << unsuccessfulStateId << " with probability " << remainingProbability);
                        } else {
                            // Self-loop with probability one cannot be eliminated later one.
                            assert(state->getId() != newStateId);
                        }
                        ++rowOffset;

                    } else {
                        // Set failure rate according to activation
                        bool isActive = true;
                        if (mDft.hasRepresentant(nextBE->id())) {
                            // Active must be checked for the state we are coming from as this state is responsible for the
                            // rate and not the new state we are going to
                            isActive = state->isActive(mDft.getRepresentant(nextBE->id())->id());
                        }
                        STORM_LOG_TRACE("BE " << nextBE->name() << " is " << (isActive ? "active" : "not active"));
                        ValueType rate = isActive ? nextBE->activeFailureRate() : nextBE->passiveFailureRate();
                        auto resultFind = outgoingTransitions.find(newStateId);
                        if (resultFind != outgoingTransitions.end()) {
                            // Add to existing transition
                            resultFind->second += rate;
                            STORM_LOG_TRACE("Updated transition from " << state->getId() << " to " << resultFind->first << " with rate " << rate << " to new rate " << resultFind->second);
                        } else {
                            // Insert new transition
                            outgoingTransitions.insert(std::make_pair(newStateId, rate));
                            STORM_LOG_TRACE("Added transition from " << state->getId() << " to " << newStateId << " with rate " << rate);
                        }
                        exitRate += rate;
                    }

                } // end while failing BE

                if (hasDependencies) {
                    rowOffset--; // One increment to many
                } else {
                    // Try to merge pseudo states with their instantiation
                    // TODO Matthias: improve?
                    auto it = outgoingTransitions.begin();
                    while (it != outgoingTransitions.end()) {
                        if (it->first >= OFFSET_PSEUDO_STATE) {
                            uint_fast64_t newId = it->first - OFFSET_PSEUDO_STATE;
                            assert(newId < mPseudoStatesMapping.size());
                            if (mPseudoStatesMapping[newId].first > 0) {
                                // State exists already
                                newId = mPseudoStatesMapping[newId].first;
                                auto itFind = outgoingTransitions.find(newId);
                                if (itFind != outgoingTransitions.end()) {
                                    // Add probability from pseudo state to instantiation
                                    itFind->second += it->second;
                                    STORM_LOG_TRACE("Merged pseudo state " << newId << " adding rate " << it->second << " to total rate of " << itFind->second);
                                } else {
                                    // Only change id
                                    outgoingTransitions.emplace(newId, it->second);
                                    STORM_LOG_TRACE("Instantiated pseudo state " << newId << " with rate " << it->second);
                                }
                                // Remove pseudo state
                                auto itErase = it;
                                ++it;
                                outgoingTransitions.erase(itErase);
                            } else {
                                ++it;
                            }
                        } else {
                            ++it;
                        }
                    }
                    
                    // Add all probability transitions
                    for (auto it = outgoingTransitions.begin(); it != outgoingTransitions.end(); ++it)
                    {
                        ValueType probability = it->second / exitRate; // Transform rate to probability
                        transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, it->first, probability);
                    }

                    markovianStates.push_back(state->getId());
                }
                exitRates.push_back(exitRate);
                assert(exitRates.size()-1 == state->getId());

                if (stateQueue.empty()) {
                    // Before ending the exploration check for pseudo states which are no be initialized yet
                    for (auto & pseudoStatePair : mPseudoStatesMapping) {
                        if (pseudoStatePair.first == 0) {
                            // Create state from pseudo state and explore
                            assert(mStates.contains(pseudoStatePair.second));
                            assert(mStates.getValue(pseudoStatePair.second) >= OFFSET_PSEUDO_STATE);
                            STORM_LOG_TRACE("Create pseudo state from bit vector " << pseudoStatePair.second);
                            DFTStatePointer pseudoState = std::make_shared<storm::storage::DFTState<ValueType>>(pseudoStatePair.second, mDft, *mStateGenerationInfo, newIndex++);
                            assert(pseudoStatePair.second == pseudoState->status());
                            pseudoStatePair.first = pseudoState->getId();
                            mStates.setOrAdd(pseudoState->status(), pseudoState->getId());
                            stateQueue.push(pseudoState);
                            STORM_LOG_TRACE("Explore pseudo state " << mDft.getStateString(pseudoState) << " with id " << pseudoState->getId());
                            break;
                        }
                    }
                }
                
            } // end while queue
            
            std::vector<uint_fast64_t> pseudoStatesVector;
            for (auto const& pseudoStatePair : mPseudoStatesMapping) {
                pseudoStatesVector.push_back(pseudoStatePair.first);
            }
            assert(std::find(pseudoStatesVector.begin(), pseudoStatesVector.end(), 0) == pseudoStatesVector.end());
            
            // Replace pseudo states in matrix
            transitionMatrixBuilder.replaceColumns(pseudoStatesVector, OFFSET_PSEUDO_STATE);
            
            assert(!deterministic || rowOffset == 0);
            return deterministic;
        }

        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilder<double>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilder<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


