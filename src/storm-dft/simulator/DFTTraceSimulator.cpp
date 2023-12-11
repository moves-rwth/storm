#include "DFTTraceSimulator.h"

namespace storm::dft {
namespace simulator {

template<typename ValueType>
DFTTraceSimulator<ValueType>::DFTTraceSimulator(storm::dft::storage::DFT<ValueType> const& dft,
                                                storm::dft::storage::DFTStateGenerationInfo const& stateGenerationInfo, boost::mt19937& randomGenerator)
    : dft(dft), stateGenerationInfo(stateGenerationInfo), generator(dft, stateGenerationInfo), randomGenerator(randomGenerator) {
    // Set initial state
    state = generator.createInitialState();
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::setRandomNumberGenerator(boost::mt19937& randomNumberGenerator) {
    this->randomGenerator = randomNumberGenerator;
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::resetToInitial() {
    state = generator.createInitialState();
}

template<typename ValueType>
typename DFTTraceSimulator<ValueType>::DFTStatePointer DFTTraceSimulator<ValueType>::getCurrentState() const {
    return state;
}

template<typename ValueType>
std::tuple<storm::dft::storage::FailableElements::const_iterator, double, bool> DFTTraceSimulator<ValueType>::randomNextFailure() {
    auto iterFailable = state->getFailableElements().begin();

    // Check for absorbing state:
    // - either no relevant event remains (i.e., all relevant events have failed already), or
    // - no BE can fail
    if (!state->hasOperationalRelevantEvent() || iterFailable == state->getFailableElements().end()) {
        STORM_LOG_TRACE("No successor states available for " << state->getId());
        return std::make_tuple(iterFailable, -1, true);
    }

    // Get all failable elements
    if (iterFailable.isFailureDueToDependency()) {
        if (iterFailable.isConflictingDependency()) {
            // We take the first dependency to resolve the non-determinism
            STORM_LOG_WARN("Non-determinism present! We take the dependency with the lowest id");
        }

        auto dependency = iterFailable.asDependency(dft);
        bool successful = true;
        if (!dependency->isFDEP()) {
            // Flip a coin whether the PDEP is successful
            storm::utility::BernoulliDistributionGenerator probGenerator(dependency->probability());
            successful = probGenerator.random(randomGenerator);
        }
        STORM_LOG_TRACE("Let dependency " << *dependency << " " << (successful ? "successfully" : "unsuccessfully") << " fail");
        return std::make_tuple(iterFailable, 0, successful);
    } else {
        // Consider all "normal" BE failures
        // Initialize with first BE
        storm::dft::storage::FailableElements::const_iterator nextFail = iterFailable;
        double rate = state->getBERate(nextFail.asBE(dft)->id());
        storm::utility::ExponentialDistributionGenerator rateGenerator(rate);
        double smallestTimebound = rateGenerator.random(randomGenerator);
        ++iterFailable;

        // Consider all other BEs and find the one which fails first
        for (; iterFailable != state->getFailableElements().end(); ++iterFailable) {
            rate = state->getBERate(iterFailable.asBE(dft)->id());
            rateGenerator = storm::utility::ExponentialDistributionGenerator(rate);
            double timebound = rateGenerator.random(randomGenerator);
            if (timebound < smallestTimebound) {
                // BE fails earlier -> use as nextFail
                nextFail = iterFailable;
                smallestTimebound = timebound;
            }
        }
        STORM_LOG_TRACE("Let BE " << *nextFail.asBE(dft) << " fail after time " << smallestTimebound);
        return std::make_tuple(nextFail, smallestTimebound, true);
    }
}

template<>
std::tuple<storm::dft::storage::FailableElements::const_iterator, double, bool> DFTTraceSimulator<storm::RationalFunction>::randomNextFailure() {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
}

template<typename ValueType>
std::pair<SimulationResult, double> DFTTraceSimulator<ValueType>::randomStep() {
    auto [nextFailable, time, successful] = this->randomNextFailure();
    if (time < 0) {
        return std::make_pair(SimulationResult::UNSUCCESSFUL, -1);
    } else {
        // Apply next failure
        return std::make_pair(step(nextFailable, successful), time);
    }
}

template<typename ValueType>
SimulationResult DFTTraceSimulator<ValueType>::step(storm::dft::storage::FailableElements::const_iterator nextFailElement, bool dependencySuccessful) {
    if (nextFailElement == state->getFailableElements().end()) {
        // No next failure possible
        return SimulationResult::UNSUCCESSFUL;
    }

    DFTStatePointer newState;
    if (nextFailElement.isFailureDueToDependency()) {
        newState = generator.createSuccessorState(state, nextFailElement.asDependency(dft), dependencySuccessful);
    } else {
        newState = generator.createSuccessorState(state, nextFailElement.asBE(dft));
    }

    if (newState->isInvalid() || newState->isTransient()) {
        STORM_LOG_TRACE("Step is invalid because new state " << (newState->isInvalid() ? "is invalid." : "has transient fault."));
        return SimulationResult::INVALID;
    }

    state = newState;
    return SimulationResult::SUCCESSFUL;
}

template<typename ValueType>
SimulationResult DFTTraceSimulator<ValueType>::simulateCompleteTrace(double timebound) {
    resetToInitial();

    // Check whether DFT is initially already failed.
    if (state->hasFailed(dft.getTopLevelIndex())) {
        STORM_LOG_TRACE("DFT is initially failed");
        return SimulationResult::SUCCESSFUL;
    }

    double time = 0;
    while (time <= timebound) {
        // Generate next failure
        auto retTuple = randomNextFailure();
        storm::dft::storage::FailableElements::const_iterator nextFailable = std::get<0>(retTuple);
        double addTime = std::get<1>(retTuple);
        bool successfulDependency = std::get<2>(retTuple);
        if (addTime < 0) {
            // No next state can be reached, because no element can fail anymore.
            STORM_LOG_TRACE("No next state possible in state " << dft.getStateString(state) << " because no element can fail anymore");
            return SimulationResult::UNSUCCESSFUL;
        }

        // TODO: exit if time would be up after this failure
        // This is only correct if no invalid states are possible! (no restrictors and no transient failures)

        // Apply next failure
        auto stepResult = step(nextFailable, successfulDependency);
        STORM_LOG_TRACE("Current state: " << dft.getStateString(state));

        // Check whether state is invalid
        if (stepResult != SimulationResult::SUCCESSFUL) {
            STORM_LOG_ASSERT(stepResult == SimulationResult::INVALID, "Result of simulation step should be invalid.");
            // No next state can be reached, because the state is invalid.
            STORM_LOG_TRACE("No next state possible in state " << dft.getStateString(state) << " because simulation was invalid");
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Handling of invalid states is not supported for simulation");
            return SimulationResult::INVALID;
        }

        // Check whether time is up
        // Checking whether the time is up must be performed after checking if a state is invalid.
        // Otherwise we would erroneously mark invalid traces as unsuccessful.
        time += addTime;
        if (time > timebound) {
            STORM_LOG_TRACE("Time limit" << timebound << " exceeded: " << time);
            return SimulationResult::UNSUCCESSFUL;
        }

        // Check whether DFT is failed
        if (state->hasFailed(dft.getTopLevelIndex())) {
            STORM_LOG_TRACE("DFT has failed after " << time);
            return SimulationResult::SUCCESSFUL;
        }
    }
    STORM_LOG_ASSERT(false, "Should not be reachable");
    return SimulationResult::UNSUCCESSFUL;
}

template<>
SimulationResult DFTTraceSimulator<storm::RationalFunction>::simulateCompleteTrace(double timebound) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
}

template class DFTTraceSimulator<double>;
template class DFTTraceSimulator<storm::RationalFunction>;

}  // namespace simulator
}  // namespace storm::dft
