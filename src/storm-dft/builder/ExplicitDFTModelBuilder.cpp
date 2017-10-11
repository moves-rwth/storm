#include "ExplicitDFTModelBuilder.h"

#include <map>

#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Ctmc.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/settings/SettingsManager.h"


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
            size_t rowOffset = 0;
            ModelComponents modelComponents;
            std::vector<uint_fast64_t> tmpMarkovianStates;
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);

            if(mergeFailedStates) {
                // Introduce explicit fail state
                failedIndex = newIndex;
                newIndex++;
                transitionMatrixBuilder.newRowGroup(failedIndex);
                transitionMatrixBuilder.addNextValue(failedIndex, failedIndex, storm::utility::one<ValueType>());
                STORM_LOG_TRACE("Introduce fail state with id: " << failedIndex);
                modelComponents.exitRates.push_back(storm::utility::one<ValueType>());
                tmpMarkovianStates.push_back(failedIndex);        
            }
            
            // Explore state space
            DFTStatePointer state = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, *mStateGenerationInfo, newIndex);
            auto exploreResult = exploreStates(state, rowOffset, transitionMatrixBuilder, tmpMarkovianStates, modelComponents.exitRates);
            initialStateIndex = exploreResult.first;
            bool deterministic = exploreResult.second;
        
            // Before ending the exploration check for pseudo states which are not initialized yet
            for (auto & pseudoStatePair : mPseudoStatesMapping) {
                if (pseudoStatePair.first == 0) {
                    // Create state from pseudo state and explore
                    STORM_LOG_ASSERT(mStates.contains(pseudoStatePair.second), "Pseudo state not contained.");
                    STORM_LOG_ASSERT(mStates.getValue(pseudoStatePair.second) >= OFFSET_PSEUDO_STATE, "State is no pseudo state.");
                    STORM_LOG_TRACE("Create pseudo state from bit vector " << pseudoStatePair.second);
                    DFTStatePointer pseudoState = std::make_shared<storm::storage::DFTState<ValueType>>(pseudoStatePair.second, mDft, *mStateGenerationInfo, newIndex);
                    pseudoState->construct();
                    STORM_LOG_ASSERT(pseudoStatePair.second == pseudoState->status(), "Pseudo states do not coincide.");
                    STORM_LOG_TRACE("Explore pseudo state " << mDft.getStateString(pseudoState) << " with id " << pseudoState->getId());
                    auto exploreResult = exploreStates(pseudoState, rowOffset, transitionMatrixBuilder, tmpMarkovianStates, modelComponents.exitRates);
                    deterministic &= exploreResult.second;
                    STORM_LOG_ASSERT(pseudoStatePair.first == pseudoState->getId(), "Pseudo state ids do not coincide");
                    STORM_LOG_ASSERT(pseudoState->getId() == exploreResult.first, "Pseudo state ids do not coincide.");
                }
            }
            
            // Replace pseudo states in matrix
            std::vector<uint_fast64_t> pseudoStatesVector;
            for (auto const& pseudoStatePair : mPseudoStatesMapping) {
                pseudoStatesVector.push_back(pseudoStatePair.first);
            }
            STORM_LOG_ASSERT(std::find(pseudoStatesVector.begin(), pseudoStatesVector.end(), 0) == pseudoStatesVector.end(), "Unexplored pseudo state still contained.");
            transitionMatrixBuilder.replaceColumns(pseudoStatesVector, OFFSET_PSEUDO_STATE);
            
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
            STORM_LOG_TRACE("Exit rates: " << storm::utility::vector::toString<ValueType>(modelComponents.exitRates));
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
            storm::storage::sparse::ModelComponents<ValueType> components(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling));
                    components.exitRates = std::move(modelComponents.exitRates);
            if (deterministic) {
                components.transitionMatrix.makeRowGroupingTrivial();
                model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(components));
            } else {
                    components.markovianStates = std::move(modelComponents.markovianStates);
                    std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(std::move(components));
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
        std::pair<uint_fast64_t, bool> ExplicitDFTModelBuilder<ValueType>::exploreStates(DFTStatePointer const& state, size_t& rowOffset, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<uint_fast64_t>& markovianStates, std::vector<ValueType>& exitRates) {
            STORM_LOG_TRACE("Explore state: " << mDft.getStateString(state));

            auto explorePair = checkForExploration(state);
            if (!explorePair.first) {
                // State does not need any exploration
                return std::make_pair(explorePair.second, true);
            }

            
            // Initialization
            // TODO Matthias: set Markovian states directly as bitvector?
            std::map<uint_fast64_t, ValueType> outgoingRates;
            std::vector<std::map<uint_fast64_t, ValueType>> outgoingProbabilities;
            bool hasDependencies = state->nrFailableDependencies() > 0;
            size_t failableCount = hasDependencies ? state->nrFailableDependencies() : state->nrFailableBEs();
            size_t smallest = 0;
            ValueType exitRate = storm::utility::zero<ValueType>();
            bool deterministic = !hasDependencies;

            // Absorbing state
            if (mDft.hasFailed(state) || mDft.isFailsafe(state) || state->nrFailableBEs() == 0) {
                uint_fast64_t stateId = addState(state);
                STORM_LOG_ASSERT(stateId == state->getId(), "Ids do not coincide.");

                // Add self loop
                transitionMatrixBuilder.newRowGroup(stateId + rowOffset);
                transitionMatrixBuilder.addNextValue(stateId + rowOffset, stateId, storm::utility::one<ValueType>());
                STORM_LOG_TRACE("Added self loop for " << stateId);
                exitRates.push_back(storm::utility::one<ValueType>());
                STORM_LOG_ASSERT(exitRates.size()-1 == stateId, "No. of considered states does not match state id.");
                markovianStates.push_back(stateId);
                // No further exploration required
                return std::make_pair(stateId, true);
            }

            // Let BE fail
            while (smallest < failableCount) {
                STORM_LOG_ASSERT(!mDft.hasFailed(state), "Dft has failed.");

                // Construct new state as copy from original one
                DFTStatePointer newState = std::make_shared<storm::storage::DFTState<ValueType>>(*state);
                std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType> const>, bool> nextBEPair = newState->letNextBEFail(smallest++);
                std::shared_ptr<storm::storage::DFTBE<ValueType> const>& nextBE = nextBEPair.first;
                STORM_LOG_ASSERT(nextBE, "NextBE is null.");
                STORM_LOG_ASSERT(nextBEPair.second == hasDependencies, "Failure due to dependencies does not match.");
                STORM_LOG_TRACE("With the failure of: " << nextBE->name() << " [" << nextBE->id() << "] in " << mDft.getStateString(state));

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
                bool dftFailed = newState->hasFailed(mDft.getTopLevelIndex());
                
                while (!dftFailed && !queues.failsafePropagationDone()) {
                    DFTGatePointer next = queues.nextFailsafePropagation();
                    next->checkFailsafe(*newState, queues);
                }

                while (!dftFailed && enableDC && !queues.dontCarePropagationDone()) {
                    DFTElementPointer next = queues.nextDontCarePropagation();
                    next->checkDontCareAnymore(*newState, queues);
                }
                
                // Update failable dependencies
                if (!dftFailed) {
                    newState->updateFailableDependencies(nextBE->id());
                    newState->updateDontCareDependencies(nextBE->id());
                }
                
                uint_fast64_t newStateId;
                if(dftFailed && mergeFailedStates) {
                    newStateId = failedIndex;
                } else {
                    // Explore new state recursively
                    auto explorePair = exploreStates(newState, rowOffset, transitionMatrixBuilder, markovianStates, exitRates);
                    newStateId = explorePair.first;
                    deterministic &= explorePair.second;
                }

                // Set transitions
                if (hasDependencies) {
                    // Failure is due to dependency -> add non-deterministic choice
                    std::map<uint_fast64_t, ValueType> choiceProbabilities;
                    std::shared_ptr<storm::storage::DFTDependency<ValueType> const> dependency = mDft.getDependency(state->getDependencyId(smallest-1));
                    choiceProbabilities.insert(std::make_pair(newStateId, dependency->probability()));
                    STORM_LOG_TRACE("Added transition to " << newStateId << " with probability " << dependency->probability());

                    if (!storm::utility::isOne(dependency->probability())) {
                        // Add transition to state where dependency was unsuccessful
                        DFTStatePointer unsuccessfulState = std::make_shared<storm::storage::DFTState<ValueType>>(*state);
                        unsuccessfulState->letDependencyBeUnsuccessful(smallest-1);
                        auto explorePair = exploreStates(unsuccessfulState, rowOffset, transitionMatrixBuilder, markovianStates, exitRates);
                        uint_fast64_t unsuccessfulStateId = explorePair.first;
                        deterministic &= explorePair.second;
                        ValueType remainingProbability = storm::utility::one<ValueType>() - dependency->probability();
                        choiceProbabilities.insert(std::make_pair(unsuccessfulStateId, remainingProbability));
                        STORM_LOG_TRACE("Added transition to " << unsuccessfulStateId << " with remaining probability " << remainingProbability);
                    }
                    outgoingProbabilities.push_back(choiceProbabilities);
                } else {
                    // Set failure rate according to activation
                    bool isActive = true;
                    if (mDft.hasRepresentant(nextBE->id())) {
                        // Active must be checked for the state we are coming from as this state is responsible for the
                        // rate and not the new state we are going to
                        isActive = state->isActive(mDft.getRepresentant(nextBE->id()));
                    }
                    ValueType rate = isActive ? nextBE->activeFailureRate() : nextBE->passiveFailureRate();
                    STORM_LOG_ASSERT(!storm::utility::isZero(rate), "Rate is 0.");
                    auto resultFind = outgoingRates.find(newStateId);
                    if (resultFind != outgoingRates.end()) {
                        // Add to existing transition
                        resultFind->second += rate;
                        STORM_LOG_TRACE("Updated transition to " << resultFind->first << " with " << (isActive ? "active" : "passive") << " rate " << rate << " to new rate " << resultFind->second);
                    } else {
                        // Insert new transition
                        outgoingRates.insert(std::make_pair(newStateId, rate));
                        STORM_LOG_TRACE("Added transition to " << newStateId << " with " << (isActive ? "active" : "passive") << " rate " << rate);
                    }
                    exitRate += rate;
                }

            } // end while failing BE
            
            // Add state
            uint_fast64_t stateId = addState(state);
            STORM_LOG_ASSERT(stateId == state->getId(), "Ids do not match.");
            STORM_LOG_ASSERT(stateId == newIndex-1, "Id does not match no. of states.");
            
            if (hasDependencies) {
                // Add all probability transitions
                STORM_LOG_ASSERT(outgoingRates.empty(), "Outgoing transitions not empty.");
                transitionMatrixBuilder.newRowGroup(stateId + rowOffset);
                for (size_t i = 0; i < outgoingProbabilities.size(); ++i, ++rowOffset) {
                    STORM_LOG_ASSERT(outgoingProbabilities[i].size() == 1 || outgoingProbabilities[i].size() == 2, "No. of outgoing transitions is not valid.");
                    for (auto it = outgoingProbabilities[i].begin(); it != outgoingProbabilities[i].end(); ++it)
                    {
                        STORM_LOG_TRACE("Set transition from " << stateId << " to " << it->first << " with probability " << it->second);
                        transitionMatrixBuilder.addNextValue(stateId + rowOffset, it->first, it->second);
                    }
                }
                rowOffset--; // One increment too many
            } else {
                // Try to merge pseudo states with their instantiation
                // TODO Matthias: improve?
                for (auto it = outgoingRates.begin(); it != outgoingRates.end(); ) {
                    if (it->first >= OFFSET_PSEUDO_STATE) {
                        uint_fast64_t newId = it->first - OFFSET_PSEUDO_STATE;
                        STORM_LOG_ASSERT(newId < mPseudoStatesMapping.size(), "Id is not valid.");
                        if (mPseudoStatesMapping[newId].first > 0) {
                            // State exists already
                            newId = mPseudoStatesMapping[newId].first;
                            auto itFind = outgoingRates.find(newId);
                            if (itFind != outgoingRates.end()) {
                                // Add probability from pseudo state to instantiation
                                itFind->second += it->second;
                                STORM_LOG_TRACE("Merged pseudo state " << newId << " adding rate " << it->second << " to total rate of " << itFind->second);
                            } else {
                                // Only change id
                                outgoingRates.emplace(newId, it->second);
                                STORM_LOG_TRACE("Instantiated pseudo state " << newId << " with rate " << it->second);
                            }
                            // Remove pseudo state
                            it = outgoingRates.erase(it);
                        } else {
                            ++it;
                        }
                    } else {
                        ++it;
                    }
                }
                
                // Add all rate transitions
                STORM_LOG_ASSERT(outgoingProbabilities.empty(), "Outgoing probabilities not empty.");
                transitionMatrixBuilder.newRowGroup(state->getId() + rowOffset);
                STORM_LOG_TRACE("Exit rate for " << state->getId() << ": " << exitRate);
                for (auto it = outgoingRates.begin(); it != outgoingRates.end(); ++it)
                {
                    ValueType probability = it->second / exitRate; // Transform rate to probability
                    STORM_LOG_TRACE("Set transition from " << state->getId() << " to " << it->first << " with rate " << it->second);
                    transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, it->first, probability);
                }

                markovianStates.push_back(state->getId());
            }
            
            STORM_LOG_TRACE("Finished exploring state: " << mDft.getStateString(state));
            exitRates.push_back(exitRate);
            STORM_LOG_ASSERT(exitRates.size()-1 == state->getId(), "Id does not match no. of states.");
            return std::make_pair(state->getId(), deterministic);
        }
        
        template <typename ValueType>
        std::pair<bool, uint_fast64_t> ExplicitDFTModelBuilder<ValueType>::checkForExploration(DFTStatePointer const& state) {
            bool changed = false;
            if (mStateGenerationInfo->hasSymmetries()) {
                // Order state by symmetry
                STORM_LOG_TRACE("Check for symmetry: " << mDft.getStateString(state));
                changed = state->orderBySymmetry();
                STORM_LOG_TRACE("State " << (changed ? "changed to " : "did not change") << (changed ? mDft.getStateString(state) : ""));
            }
            
            if (mStates.contains(state->status())) {
                // State already exists
                uint_fast64_t stateId = mStates.getValue(state->status());
                STORM_LOG_TRACE("State " << mDft.getStateString(state) << " with id " << stateId << " already exists");
                
                if (changed || stateId < OFFSET_PSEUDO_STATE) {
                    // State is changed or an explored "normal" state
                    return std::make_pair(false, stateId);
                }
                
                stateId -= OFFSET_PSEUDO_STATE;
                STORM_LOG_ASSERT(stateId < mPseudoStatesMapping.size(), "Id not valid.");
                if (mPseudoStatesMapping[stateId].first > 0) {
                    // Pseudo state already explored
                    return std::make_pair(false, mPseudoStatesMapping[stateId].first);
                }
                
                STORM_LOG_ASSERT(mPseudoStatesMapping[stateId].second == state->status(), "States do not coincide.");
                STORM_LOG_TRACE("Pseudo state " << mDft.getStateString(state) << " can be explored now");
                return std::make_pair(true, stateId);
            } else {
                // State does not exists
                if (changed) {
                    // Remember state for later creation
                    state->setId(mPseudoStatesMapping.size() + OFFSET_PSEUDO_STATE);
                    mPseudoStatesMapping.push_back(std::make_pair(0, state->status()));
                    mStates.findOrAdd(state->status(), state->getId());
                    STORM_LOG_TRACE("Remember state for later creation: " << mDft.getStateString(state));
                    return std::make_pair(false, state->getId());
                } else {
                    // State needs exploration
                    return std::make_pair(true, 0);
                }
            }
        }

        template <typename ValueType>
        uint_fast64_t ExplicitDFTModelBuilder<ValueType>::addState(DFTStatePointer const& state) {
            uint_fast64_t stateId;
            // TODO remove
            bool changed = state->orderBySymmetry();
            STORM_LOG_ASSERT(!changed, "State to add has changed by applying symmetry.");
            
            // Check if state already exists
            if (mStates.contains(state->status())) {
                // State already exists
                stateId = mStates.getValue(state->status());
                STORM_LOG_TRACE("State " << mDft.getStateString(state) << " with id " << stateId << " already exists");
                
                // Check if possible pseudo state can be created now
                STORM_LOG_ASSERT(stateId >= OFFSET_PSEUDO_STATE, "State is no pseudo state.");
                stateId -= OFFSET_PSEUDO_STATE;
                STORM_LOG_ASSERT(stateId < mPseudoStatesMapping.size(), "Pseudo state not known.");
                if (mPseudoStatesMapping[stateId].first == 0) {
                    // Create pseudo state now
                    STORM_LOG_ASSERT(mPseudoStatesMapping[stateId].second == state->status(), "Pseudo states do not coincide.");
                    state->setId(newIndex++);
                    mPseudoStatesMapping[stateId].first = state->getId();
                    stateId = state->getId();
                    mStates.setOrAdd(state->status(), stateId);
                    STORM_LOG_TRACE("Now create state " << mDft.getStateString(state) << " with id " << stateId);
                    return stateId;
                } else {
                    STORM_LOG_ASSERT(false, "Pseudo state already created.");
                    return 0;
                }
            } else {
                // Create new state
                state->setId(newIndex++);
                stateId = mStates.findOrAdd(state->status(), state->getId());
                STORM_LOG_TRACE("New state: " << mDft.getStateString(state));
                return stateId;
            }
        }


        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilder<double>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilder<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


