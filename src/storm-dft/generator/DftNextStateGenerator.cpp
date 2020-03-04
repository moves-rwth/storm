#include "DftNextStateGenerator.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/settings/SettingsManager.h"
#include "storm-dft/settings/modules/FaultTreeSettings.h"

namespace storm {
    namespace generator {

        template<typename ValueType, typename StateType>
        DftNextStateGenerator<ValueType, StateType>::DftNextStateGenerator(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTStateGenerationInfo const& stateGenerationInfo) : mDft(dft), mStateGenerationInfo(stateGenerationInfo), state(nullptr), uniqueFailedState(false) {
            deterministicModel = !mDft.canHaveNondeterminism();
            mTakeFirstDependency = storm::settings::getModule<storm::settings::modules::FaultTreeSettings>().isTakeFirstDependency();
        }

        template<typename ValueType, typename StateType>
        bool DftNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
            return deterministicModel;
        }

        template<typename ValueType, typename StateType>
        std::vector<StateType> DftNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
            DFTStatePointer initialState = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);
            size_t constFailedBeCounter = 0;
            std::shared_ptr<storm::storage::DFTBE<ValueType> const> constFailedBE = nullptr;
            for (auto &be : mDft.getBasicElements()) {
                if (be->type() == storm::storage::DFTElementType::BE_CONST) {
                    auto constBe = std::static_pointer_cast<storm::storage::BEConst<ValueType> const>(be);
                    if (constBe->failed()) {
                        constFailedBeCounter++;
                        STORM_LOG_THROW(constFailedBeCounter < 2, storm::exceptions::NotSupportedException,
                                        "DFTs with more than one constantly failed BE are not supported. Try using the option '--uniquefailedbe'.");
                        constFailedBE = constBe;
                    }
                }
            }
            StateType id;
            if (constFailedBeCounter == 0) {
                // Register initial state
                id = stateToIdCallback(initialState);
            } else {
                initialState->letNextBEFail(constFailedBE->id(), false);
                // Propagate the constant failure to reach the real initial state
                storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;
                propagateFailure(initialState, constFailedBE, queues);

                if (initialState->hasFailed(mDft.getTopLevelIndex()) && uniqueFailedState) {
                    propagateFailsafe(initialState, constFailedBE, queues);

                    // Update failable dependencies
                    initialState->updateFailableDependencies(constFailedBE->id());
                    initialState->updateDontCareDependencies(constFailedBE->id());
                    initialState->updateFailableInRestrictions(constFailedBE->id());

                    // Unique failed state
                    id = 0;
                } else {
                    propagateFailsafe(initialState, constFailedBE, queues);

                    // Update failable dependencies
                    initialState->updateFailableDependencies(constFailedBE->id());
                    initialState->updateDontCareDependencies(constFailedBE->id());
                    initialState->updateFailableInRestrictions(constFailedBE->id());

                    id = stateToIdCallback(initialState);
                }
            }

            initialState->setId(id);

            return {id};
        }

        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::load(storm::storage::BitVector const& state) {
            // Load the state from bitvector
            size_t id = 0; //TODO: set correct id
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
            // Initialization
            bool hasDependencies = state->getFailableElements().hasDependencies();
            return exploreState(stateToIdCallback, hasDependencies, mTakeFirstDependency);
        }

        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::exploreState(StateToIdCallback const& stateToIdCallback, bool exploreDependencies, bool takeFirstDependency) {
            // Prepare the result, in case we return early.
            StateBehavior<ValueType, StateType> result;

            //size_t failableCount = hasDependencies ? state->nrFailableDependencies() : state->nrFailableBEs();
            //size_t currentFailable = 0;
            state->getFailableElements().init(exploreDependencies);

            // Check for absorbing state:
            // - either no relevant event remains (i.e., all relevant events have failed already), or
            // - no BE can fail
            if (!state->hasOperationalRelevantEvent() || state->getFailableElements().isEnd()) {
                Choice<ValueType, StateType> choice(0, true);
                // Add self loop
                choice.addProbability(state->getId(), storm::utility::one<ValueType>());
                STORM_LOG_TRACE("Added self loop for " << state->getId());
                // No further exploration required
                result.addChoice(std::move(choice));
                result.setExpanded();
                return result;
            }

            Choice<ValueType, StateType> choice(0, !exploreDependencies);

            // Let BE fail
            bool isFirst = true;
            while (!state->getFailableElements().isEnd()) {
                //TODO outside
                if (takeFirstDependency && exploreDependencies && !isFirst) {
                    // We discard further exploration as we already chose one dependent event
                    break;
                }
                isFirst = false;

                // Construct new state as copy from original one
                DFTStatePointer newState = state->copy();
                std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType> const>, bool> nextBEPair = newState->letNextBEFail(state->getFailableElements().get(), exploreDependencies);
                std::shared_ptr<storm::storage::DFTBE<ValueType> const>& nextBE = nextBEPair.first;
                STORM_LOG_ASSERT(nextBE, "NextBE is null.");
                STORM_LOG_ASSERT(nextBEPair.second == exploreDependencies, "Failure due to dependencies does not match.");
                STORM_LOG_TRACE("With the failure of: " << nextBE->name() << " [" << nextBE->id() << "] in " << mDft.getStateString(state));

                // Propagate
                storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;

                propagateFailure(newState, nextBE, queues);

                bool transient = false;
                if (nextBE->type() == storm::storage::DFTElementType::BE_EXP) {
                    auto beExp = std::static_pointer_cast<storm::storage::BEExponential<ValueType> const>(nextBE);
                    transient = beExp->isTransient();
                }

                if(newState->isInvalid() || (transient && !newState->hasFailed(mDft.getTopLevelIndex()))) {
                    // Continue with next possible state
                    state->getFailableElements().next();
                    STORM_LOG_TRACE("State is ignored because " << (newState->isInvalid() ? "it is invalid" : "the transient fault is ignored"));
                    continue;
                }

                // Get the id of the successor state
                StateType newStateId;
                if (newState->hasFailed(mDft.getTopLevelIndex()) && uniqueFailedState) {
                    // Use unique failed state
                    newStateId = 0;
                } else {
                    propagateFailsafe(newState, nextBE, queues);

                    // Update failable dependencies
                    newState->updateFailableDependencies(nextBE->id());
                    newState->updateDontCareDependencies(nextBE->id());
                    newState->updateFailableInRestrictions(nextBE->id());

                    // Add new state
                    newStateId = stateToIdCallback(newState);
                }

                // Set transitions
                if (exploreDependencies) {
                    // Failure is due to dependency -> add non-deterministic choice if necessary
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
                    STORM_LOG_THROW(nextBE->type() == storm::storage::DFTElementType::BE_EXP, storm::exceptions::NotSupportedException, "BE of type '" << nextBE->type() << "' is not supported.");
                    auto beExp = std::static_pointer_cast<storm::storage::BEExponential<ValueType> const>(nextBE);
                    bool isActive = true;
                    if (mDft.hasRepresentant(beExp->id())) {
                        // Active must be checked for the state we are coming from as this state is responsible for the rate
                        isActive = state->isActive(mDft.getRepresentant(beExp->id()));
                    }
                    ValueType rate = isActive ? beExp->activeFailureRate() : beExp->passiveFailureRate();
                    STORM_LOG_ASSERT(!storm::utility::isZero(rate), "Rate is 0.");
                    choice.addProbability(newStateId, rate);
                    STORM_LOG_TRACE("Added transition to " << newStateId << " with " << (isActive ? "active" : "passive") << " failure rate " << rate);
                }
                STORM_LOG_ASSERT(newStateId != state->getId(), "Self loop was added for " << newStateId << " and failure of " << nextBE->name());

                state->getFailableElements().next();
            } // end while failing BE

            if (exploreDependencies) {
                if (result.empty()) {
                    // Dependencies might have been prevented from sequence enforcer
                    // -> explore BEs now
                    return exploreState(stateToIdCallback, false, takeFirstDependency);
                }
            } else {
                if (choice.size() == 0) {
                    // No transition was generated
                    STORM_LOG_TRACE("No transitions were generated.");
                    // Add self loop
                    choice.addProbability(state->getId(), storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Added self loop for " << state->getId());
                }
                STORM_LOG_ASSERT(choice.size() > 0, "At least one choice should have been generated.");
                // Add all rates as one choice
                result.addChoice(std::move(choice));
            }

            STORM_LOG_TRACE("Finished exploring state: " << mDft.getStateString(state));
            result.setExpanded();
            return result;
        }

        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::propagateFailure(DFTStatePointer newState,
                                                                           std::shared_ptr<storm::storage::DFTBE<ValueType> const> &nextBE,
                                                                           storm::storage::DFTStateSpaceGenerationQueues<ValueType> &queues) {
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
                newState->updateFailableInRestrictions(next->id());
            }

            // Check restrictions
            for (DFTRestrictionPointer restr : nextBE->restrictions()) {
                queues.checkRestrictionLater(restr);
            }
            // Check restrictions
            while (!queues.restrictionChecksDone()) {
                DFTRestrictionPointer next = queues.nextRestrictionCheck();
                next->checkFails(*newState, queues);
                newState->updateFailableDependencies(next->id());
                newState->updateFailableInRestrictions(next->id());
            }

        }

        template<typename ValueType, typename StateType>
        void DftNextStateGenerator<ValueType, StateType>::propagateFailsafe(DFTStatePointer newState,
                                                                            std::shared_ptr<storm::storage::DFTBE<ValueType> const> &nextBE,
                                                                            storm::storage::DFTStateSpaceGenerationQueues<ValueType> &queues) {
            // Propagate failsafe
            while (!queues.failsafePropagationDone()) {
                DFTGatePointer next = queues.nextFailsafePropagation();
                next->checkFailsafe(*newState, queues);
            }

            // Propagate dont cares
            // Relevance is considered for each element independently
            while (!queues.dontCarePropagationDone()) {
                DFTElementPointer next = queues.nextDontCarePropagation();
                next->checkDontCareAnymore(*newState, queues);
            }
        }

        template<typename ValueType, typename StateType>
        StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::createMergeFailedState(StateToIdCallback const& stateToIdCallback) {
            this->uniqueFailedState = true;
            // Introduce explicit fail state with id 0
            DFTStatePointer failedState = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);
            size_t failedStateId = stateToIdCallback(failedState);
            STORM_LOG_ASSERT(failedStateId == 0, "Unique failed state has not id 0.");
            STORM_LOG_TRACE("Introduce fail state with id 0.");

            // Add self loop
            Choice<ValueType, StateType> choice(0, true);
            choice.addProbability(0, storm::utility::one<ValueType>());

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
