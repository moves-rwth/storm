#include "DftNextStateGenerator.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/settings/SettingsManager.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        DftNextStateGenerator<ValueType, StateType>::DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo, bool enableDC, bool mergeFailedStates) : mDft(dft), mStateGenerationInfo(stateGenerationInfo), state(nullptr), enableDC(enableDC), mergeFailedStates(mergeFailedStates) {
            deterministicModel = !mDft.canHaveNondeterminism();
        }
        
        template<typename ValueType, typename StateType>
        bool DftNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            return deterministicModel;
        }
        
        template<typename ValueType, typename StateType>
        std::vector<StateType> DftNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            DFTStatePointer initialState = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);

            // Register initial state
            StateType id = stateToIdCallback(initialState);

            initialState->setId(id);
            return {id};
        }

        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::load(storm::storage::BitVector const& state) {
            // Load the state from bitvector
            size_t id = 0; //TODO Matthias: set correct id
            this->state = std::make_shared<storm::storage::DFTState<ValueType>>(state, mDft, mStateGenerationInfo, id);
        }

        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::load(DFTStatePointer const& state) {
            // Store a pointer to the state itself, because we need to be able to access it when expanding it.
            this->state = state;
        }
        
        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
            STORM_LOG_DEBUG("Explore state: " << mDft.getStateString(state));

            // Prepare the result, in case we return early.
            StateBehavior<ValueType, StateType> result;

            // Initialization
            bool hasDependencies = state->getFailableElements().hasDependencies();
            //size_t failableCount = hasDependencies ? state->nrFailableDependencies() : state->nrFailableBEs();
            //size_t currentFailable = 0;
            state->getFailableElements().init(hasDependencies);

            // Check for absorbing state
            if (mDft.hasFailed(state) || mDft.isFailsafe(state) || state->getFailableElements().isEnd()) {
                Choice<ValueType, StateType> choice(0, true);
                // Add self loop
                choice.addProbability(state->getId(), storm::utility::one<ValueType>());
                STORM_LOG_TRACE("Added self loop for " << state->getId());
                // No further exploration required
                result.addChoice(std::move(choice));
                result.setExpanded();
                return result;
            }

            Choice<ValueType, StateType> choice(0, !hasDependencies);

            // Let BE fail
            bool isFirst = true;
            while (!state->getFailableElements().isEnd()) {
                if (storm::settings::getModule<storm::settings::modules::FaultTreeSettings>().isTakeFirstDependency() && hasDependencies && !isFirst) {
                    // We discard further exploration as we already chose one dependent event
                    break;
                }
                STORM_LOG_ASSERT(!mDft.hasFailed(state), "Dft has failed.");
                isFirst = false;

                // Construct new state as copy from original one
                DFTStatePointer newState = state->copy();
                std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType> const>, bool> nextBEPair = newState->letNextBEFail(state->getFailableElements().get());
                std::shared_ptr<storm::storage::DFTBE<ValueType> const>& nextBE = nextBEPair.first;
                STORM_LOG_ASSERT(nextBE, "NextBE is null.");
                STORM_LOG_ASSERT(nextBEPair.second == hasDependencies, "Failure due to dependencies does not match.");
                STORM_LOG_TRACE("With the failure of: " << nextBE->name() << " [" << nextBE->id() << "] in " << mDft.getStateString(state));

                // Propagate
                storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;

                // Propagate failure
                for (DFTGatePointer parent : nextBE->parents()) {
                    if (newState->isOperational(parent->id())) {
                        queues.propagateFailure(parent);
                    }
                }
                // Propagate failures
                while (!queues.failurePropagationDone()) {
                    DFTGatePointer next = queues.nextFailurePropagation();
                    next->checkFails(*newState, queues);
                    newState->updateFailableDependencies(next->id());
                }

                // Check restrictions
                for (DFTRestrictionPointer restr : nextBE->restrictions()) {
                    queues.checkRestrictionLater(restr);
                }
                // Check restrictions
                while(!queues.restrictionChecksDone()) {
                    DFTRestrictionPointer next = queues.nextRestrictionCheck();
                    next->checkFails(*newState, queues);
                    newState->updateFailableDependencies(next->id());
                }

                if(newState->isInvalid() || (nextBE->isTransient() && !newState->hasFailed(mDft.getTopLevelIndex()))) {
                    // Continue with next possible state
                    state->getFailableElements().next();
                    STORM_LOG_TRACE("State is ignored because " << (newState->isInvalid() ? "it is invalid" : "the transient fault is ignored"));
                    continue;
                }

                // Get the id of the successor state
                StateType newStateId;
                if (newState->hasFailed(mDft.getTopLevelIndex()) && mergeFailedStates) {
                    // Use unique failed state
                    newStateId = mergeFailedStateId;
                } else {
                    // Propagate failsafe
                    while (!queues.failsafePropagationDone()) {
                        DFTGatePointer next = queues.nextFailsafePropagation();
                        next->checkFailsafe(*newState, queues);
                    }

                    // Propagate dont cares
                    while (enableDC && !queues.dontCarePropagationDone()) {
                        DFTElementPointer next = queues.nextDontCarePropagation();
                        next->checkDontCareAnymore(*newState, queues);
                    }

                    // Update failable dependencies
                    newState->updateFailableDependencies(nextBE->id());
                    newState->updateDontCareDependencies(nextBE->id());

                    // Add new state
                    newStateId = stateToIdCallback(newState);
                }

                // Set transitions
                if (hasDependencies) {
                    // Failure is due to dependency -> add non-deterministic choice
                    ValueType probability = mDft.getDependency(state->getFailableElements().get())->probability();
                    choice.addProbability(newStateId, probability);
                    STORM_LOG_TRACE("Added transition to " << newStateId << " with probability " << probability);

                    if (!storm::utility::isOne(probability)) {
                        // Add transition to state where dependency was unsuccessful
                        DFTStatePointer unsuccessfulState = state->copy();
                        unsuccessfulState->letDependencyBeUnsuccessful(state->getFailableElements().get());
                        // Add state
                        StateType unsuccessfulStateId = stateToIdCallback(unsuccessfulState);
                        ValueType remainingProbability = storm::utility::one<ValueType>() - probability;
                        choice.addProbability(unsuccessfulStateId, remainingProbability);
                        STORM_LOG_TRACE("Added transition to " << unsuccessfulStateId << " with remaining probability " << remainingProbability);
                        STORM_LOG_ASSERT(unsuccessfulStateId != state->getId(), "Self loop was added (through PDEP) for " << unsuccessfulStateId << " and failure of " << nextBE->name());
                    }
                    result.addChoice(std::move(choice));
                } else {
                    // Failure is due to "normal" BE failure
                    // Set failure rate according to activation
                    bool isActive = true;
                    if (mDft.hasRepresentant(nextBE->id())) {
                        // Active must be checked for the state we are coming from as this state is responsible for the rate
                        isActive = state->isActive(mDft.getRepresentant(nextBE->id()));
                    }
                    ValueType rate = isActive ? nextBE->activeFailureRate() : nextBE->passiveFailureRate();
                    STORM_LOG_ASSERT(!storm::utility::isZero(rate), "Rate is 0.");
                    choice.addProbability(newStateId, rate);
                    STORM_LOG_TRACE("Added transition to " << newStateId << " with " << (isActive ? "active" : "passive") << " failure rate " << rate);
                }
                STORM_LOG_ASSERT(newStateId != state->getId(), "Self loop was added for " << newStateId << " and failure of " << nextBE->name());

                state->getFailableElements().next();
            } // end while failing BE
            
            if (!hasDependencies) {
                // Add all rates as one choice
                result.addChoice(std::move(choice));
            }

            STORM_LOG_TRACE("Finished exploring state: " << mDft.getStateString(state));
            result.setExpanded();
            return result;
        }

        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::createMergeFailedState(StateToIdCallback const& stateToIdCallback) {
            STORM_LOG_ASSERT(mergeFailedStates, "No unique failed state used.");
            // Introduce explicit fail state
            DFTStatePointer failedState = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);
            mergeFailedStateId = stateToIdCallback(failedState);
            STORM_LOG_TRACE("Introduce fail state with id: " << mergeFailedStateId);

            // Add self loop
            Choice<ValueType, StateType> choice(0, true);
            choice.addProbability(mergeFailedStateId, storm::utility::one<ValueType>());

            // No further exploration required
            StateBehavior<ValueType, StateType> result;
            result.addChoice(std::move(choice));
            result.setExpanded();
            return result;
        }

        template class DftNextStateGenerator<double>;

#ifdef STORM_HAVE_CARL
        template class DftNextStateGenerator<storm::RationalFunction>;
#endif
    }
}
