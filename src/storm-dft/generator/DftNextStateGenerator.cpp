#include "DftNextStateGenerator.h"

#include "storm-dft/settings/modules/FaultTreeSettings.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/settings/SettingsManager.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm::dft {
namespace generator {

template<typename ValueType, typename StateType>
DftNextStateGenerator<ValueType, StateType>::DftNextStateGenerator(storm::dft::storage::DFT<ValueType> const& dft,
                                                                   storm::dft::storage::DFTStateGenerationInfo const& stateGenerationInfo)
    : mDft(dft), mStateGenerationInfo(stateGenerationInfo), state(nullptr), uniqueFailedState(false) {
    deterministicModel = !mDft.canHaveNondeterminism();
    mTakeFirstDependency = storm::settings::getModule<storm::dft::settings::modules::FaultTreeSettings>().isTakeFirstDependency();
}

template<typename ValueType, typename StateType>
bool DftNextStateGenerator<ValueType, StateType>::isDeterministicModel() const {
    return deterministicModel;
}

template<typename ValueType, typename StateType>
typename DftNextStateGenerator<ValueType, StateType>::DFTStatePointer DftNextStateGenerator<ValueType, StateType>::createInitialState() const {
    return std::make_shared<storm::dft::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);
}

template<typename ValueType, typename StateType>
std::vector<StateType> DftNextStateGenerator<ValueType, StateType>::getInitialStates(StateToIdCallback const& stateToIdCallback) {
    DFTStatePointer initialState = createInitialState();

    // Check whether constant failed BEs are present
    size_t constFailedBeCounter = 0;
    std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const> constFailedBE = nullptr;
    for (auto& be : mDft.getBasicElements()) {
        if (be->beType() == storm::dft::storage::elements::BEType::CONSTANT) {
            auto constBe = std::static_pointer_cast<storm::dft::storage::elements::BEConst<ValueType> const>(be);
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
        initialState->letBEFail(constFailedBE, nullptr);
        // Propagate the constant failure to reach the real initial state
        storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType> queues;
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
    size_t id = 0;  // TODO: set correct id
    this->state = std::make_shared<storm::dft::storage::DFTState<ValueType>>(state, mDft, mStateGenerationInfo, id);
}

template<typename ValueType, typename StateType>
void DftNextStateGenerator<ValueType, StateType>::load(DFTStatePointer const& state) {
    // Store a pointer to the state itself, because we need to be able to access it when expanding it.
    this->state = state;
}

template<typename ValueType, typename StateType>
storm::generator::StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::expand(StateToIdCallback const& stateToIdCallback) {
    STORM_LOG_DEBUG("Explore state: " << mDft.getStateString(state));
    // Initialization
    bool hasDependencies = state->getFailableElements().hasDependencies();
    return exploreState(stateToIdCallback, hasDependencies, mTakeFirstDependency);
}

template<typename ValueType, typename StateType>
storm::generator::StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::exploreState(StateToIdCallback const& stateToIdCallback,
                                                                                                                bool exploreDependencies,
                                                                                                                bool takeFirstDependency) {
    // Prepare the result, in case we return early.
    storm::generator::StateBehavior<ValueType, StateType> result;

    STORM_LOG_TRACE("Currently failable: " << state->getFailableElements().getCurrentlyFailableString());
    // size_t failableCount = hasDependencies ? state->nrFailableDependencies() : state->nrFailableBEs();
    // size_t currentFailable = 0;
    // TODO remove exploreDependencies
    auto iterFailable = state->getFailableElements().begin(!exploreDependencies);

    // Check for absorbing state:
    // - either no relevant event remains (i.e., all relevant events have failed already), or
    // - no BE can fail
    if (!state->hasOperationalRelevantEvent() || iterFailable == state->getFailableElements().end(!exploreDependencies)) {
        storm::generator::Choice<ValueType, StateType> choice(0, true);
        // Add self loop
        choice.addProbability(state->getId(), storm::utility::one<ValueType>());
        STORM_LOG_TRACE("Added self loop for " << state->getId());
        // No further exploration required
        result.addChoice(std::move(choice));
        result.setExpanded();
        return result;
    }

    storm::generator::Choice<ValueType, StateType> choice(0, !exploreDependencies);

    // Let BE fail
    for (; iterFailable != state->getFailableElements().end(!exploreDependencies); ++iterFailable) {
        // Get BE which fails next
        std::pair<std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>,
                  std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const>>
            nextBEPair = iterFailable.getFailBE(mDft);
        std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const> nextBE = nextBEPair.first;
        std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const> dependency = nextBEPair.second;
        STORM_LOG_ASSERT(nextBE, "NextBE is null.");
        STORM_LOG_ASSERT(iterFailable.isFailureDueToDependency() == exploreDependencies, "Failure due to dependencies does not match.");
        STORM_LOG_ASSERT((dependency != nullptr) == exploreDependencies, "Failure due to dependencies does not match.");

        // Obtain successor state by propagating failure
        DFTStatePointer newState = createSuccessorState(state, nextBE, dependency);

        if (newState->isInvalid() || newState->isTransient()) {
            STORM_LOG_TRACE("State is ignored because " << (newState->isInvalid() ? "it is invalid" : "the transient fault is ignored"));
            // Continue with next possible state
            continue;
        }

        StateType newStateId;
        if (newState->hasFailed(mDft.getTopLevelIndex()) && uniqueFailedState) {
            // Use unique failed state
            newStateId = 0;
        } else {
            // Add new state
            newStateId = stateToIdCallback(newState);
        }

        // Set transitions
        if (exploreDependencies) {
            // Failure is due to dependency -> add non-deterministic choice if necessary
            ValueType probability = dependency->probability();
            choice.addProbability(newStateId, probability);
            STORM_LOG_TRACE("Added transition to " << newStateId << " with probability " << probability);

            if (!storm::utility::isOne(probability)) {
                // Add transition to state where dependency was unsuccessful
                DFTStatePointer unsuccessfulState = createSuccessorState(state, nextBE, dependency, false);
                // Add state
                StateType unsuccessfulStateId = stateToIdCallback(unsuccessfulState);
                ValueType remainingProbability = storm::utility::one<ValueType>() - probability;
                choice.addProbability(unsuccessfulStateId, remainingProbability);
                STORM_LOG_TRACE("Added transition to " << unsuccessfulStateId << " with remaining probability " << remainingProbability);
                STORM_LOG_ASSERT(unsuccessfulStateId != state->getId(),
                                 "Self loop was added (through PDEP) for " << unsuccessfulStateId << " and failure of " << nextBE->name());
            }
            result.addChoice(std::move(choice));
        } else {
            // Failure is due to "normal" BE failure
            // Set failure rate according to activation
            ValueType rate = state->getBERate(nextBE->id());
            STORM_LOG_ASSERT(!storm::utility::isZero(rate), "Rate is 0.");
            choice.addProbability(newStateId, rate);
            STORM_LOG_TRACE("Added transition to " << newStateId << " with failure rate " << rate);
        }
        STORM_LOG_ASSERT(newStateId != state->getId(), "Self loop was added for " << newStateId << " and failure of " << nextBE->name());

        // Handle premature stop for dependencies
        if (iterFailable.isFailureDueToDependency() && !iterFailable.isConflictingDependency()) {
            // We only explore the first non-conflicting dependency because we can fix an order.
            break;
        }
        if (takeFirstDependency && exploreDependencies) {
            // We discard further exploration as we already chose one dependent event
            break;
        }
    }  // end iteration of failing BE

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
typename DftNextStateGenerator<ValueType, StateType>::DFTStatePointer DftNextStateGenerator<ValueType, StateType>::createSuccessorState(
    DFTStatePointer const state, std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& failedBE,
    std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const>& triggeringDependency, bool dependencySuccessful) const {
    // Construct new state as copy from original one
    DFTStatePointer newState = state->copy();

    if (!dependencySuccessful) {
        // Dependency was unsuccessful -> no BE fails
        STORM_LOG_ASSERT(triggeringDependency != nullptr, "Dependency is not given");
        STORM_LOG_TRACE("With the unsuccessful triggering of PDEP " << triggeringDependency->name() << " [" << triggeringDependency->id() << "]"
                                                                    << " in " << mDft.getStateString(state));
        newState->letDependencyBeUnsuccessful(triggeringDependency);
        return newState;
    }

    STORM_LOG_TRACE("With the failure of " << failedBE->name() << " [" << failedBE->id() << "]"
                                           << (triggeringDependency != nullptr ? " (through dependency " + triggeringDependency->name() + " [" +
                                                                                     std::to_string(triggeringDependency->id()) + ")]"
                                                                               : "")
                                           << " in " << mDft.getStateString(state));

    newState->letBEFail(failedBE, triggeringDependency);

    // Propagate
    storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType> queues;
    propagateFailure(newState, failedBE, queues);

    // Check whether transient failure lead to TLE failure
    // TODO handle for all types of BEs.
    if (failedBE->beType() == storm::dft::storage::elements::BEType::EXPONENTIAL) {
        auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<ValueType> const>(failedBE);
        if (beExp->isTransient() && !newState->hasFailed(mDft.getTopLevelIndex())) {
            newState->markAsTransient();
        }
    }

    // Check whether fail safe propagation can be discarded
    bool discardFailSafe = false;
    discardFailSafe |= newState->isInvalid();
    discardFailSafe |= newState->isTransient();
    discardFailSafe |= (newState->hasFailed(mDft.getTopLevelIndex()) && uniqueFailedState);

    // Propagate fail safe (if necessary)
    if (!discardFailSafe) {
        propagateFailsafe(newState, failedBE, queues);

        // Update failable dependencies
        newState->updateFailableDependencies(failedBE->id());
        newState->updateDontCareDependencies(failedBE->id());
        newState->updateFailableInRestrictions(failedBE->id());
    }
    return newState;
}

template<typename ValueType, typename StateType>
void DftNextStateGenerator<ValueType, StateType>::propagateFailure(DFTStatePointer newState,
                                                                   std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& nextBE,
                                                                   storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType>& queues) const {
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
                                                                    std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& nextBE,
                                                                    storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType>& queues) const {
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
storm::generator::StateBehavior<ValueType, StateType> DftNextStateGenerator<ValueType, StateType>::createMergeFailedState(
    StateToIdCallback const& stateToIdCallback) {
    this->uniqueFailedState = true;
    // Introduce explicit fail state with id 0
    DFTStatePointer failedState = std::make_shared<storm::dft::storage::DFTState<ValueType>>(mDft, mStateGenerationInfo, 0);
    size_t failedStateId = stateToIdCallback(failedState);
    STORM_LOG_ASSERT(failedStateId == 0, "Unique failed state has not id 0.");
    STORM_LOG_TRACE("Introduce fail state with id 0.");

    // Add self loop
    storm::generator::Choice<ValueType, StateType> choice(0, true);
    choice.addProbability(0, storm::utility::one<ValueType>());

    // No further exploration required
    storm::generator::StateBehavior<ValueType, StateType> result;
    result.addChoice(std::move(choice));
    result.setExpanded();
    return result;
}

template class DftNextStateGenerator<double>;
template class DftNextStateGenerator<storm::RationalFunction>;

}  // namespace generator
}  // namespace storm::dft
