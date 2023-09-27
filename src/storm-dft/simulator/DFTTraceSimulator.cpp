#include "DFTTraceSimulator.h"

namespace storm::dft {
namespace simulator {

template<typename ValueType>
DFTTraceSimulator<ValueType>::DFTTraceSimulator(storm::dft::storage::DFT<ValueType> const& dft,
                                                storm::dft::storage::DFTStateGenerationInfo const& stateGenerationInfo, boost::mt19937& randomGenerator)
    : dft(dft), stateGenerationInfo(stateGenerationInfo), generator(dft, stateGenerationInfo), randomGenerator(randomGenerator) {
    // Set initial state
    resetToInitial();
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::setRandomNumberGenerator(boost::mt19937& randomNumberGenerator) {
    this->randomGenerator = randomNumberGenerator;
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::resetToInitial() {
    resetToState(generator.createInitialState());
    setTime(0);
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::resetToState(DFTStatePointer state) {
    this->state = state;
}

template<typename ValueType>
void DFTTraceSimulator<ValueType>::setTime(double time) {
    this->time = time;
}

template<typename ValueType>
typename DFTTraceSimulator<ValueType>::DFTStatePointer DFTTraceSimulator<ValueType>::getCurrentState() const {
    return state;
}

template<typename ValueType>
double DFTTraceSimulator<ValueType>::getCurrentTime() const {
    return time;
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

        auto dependency = iterFailable.getFailBE(dft).second;
        if (!dependency->isFDEP()) {
            // Flip a coin whether the PDEP is successful
            storm::utility::BernoulliDistributionGenerator probGenerator(dependency->probability());
            bool successful = probGenerator.random(randomGenerator);
            return std::make_tuple(iterFailable, 0, successful);
        }
        STORM_LOG_TRACE("Let dependency " << *dependency << " fail");
        return std::make_tuple(iterFailable, 0, true);
    } else {
        // Consider all "normal" BE failures
        // Initialize with first BE
        storm::dft::storage::FailableElements::const_iterator nextFail = iterFailable;
        double rate = state->getBERate(iterFailable.getFailBE(dft).first->id());
        storm::utility::ExponentialDistributionGenerator rateGenerator(rate);
        double smallestTimebound = rateGenerator.random(randomGenerator);
        ++iterFailable;

        // Consider all other BEs and find the one which fails first
        for (; iterFailable != state->getFailableElements().end(); ++iterFailable) {
            auto nextBE = iterFailable.getFailBE(dft).first;
            rate = state->getBERate(nextBE->id());
            rateGenerator = storm::utility::ExponentialDistributionGenerator(rate);
            double timebound = rateGenerator.random(randomGenerator);
            if (timebound < smallestTimebound) {
                // BE fails earlier -> use as nextFail
                nextFail = iterFailable;
                smallestTimebound = timebound;
            }
        }
        STORM_LOG_TRACE("Let BE " << *nextFail.getFailBE(dft).first << " fail after time " << smallestTimebound);
        return std::make_tuple(nextFail, smallestTimebound, true);
    }
}

template<>
std::tuple<storm::dft::storage::FailableElements::const_iterator, double, bool> DFTTraceSimulator<storm::RationalFunction>::randomNextFailure() {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
}

template<typename ValueType>
SimulationStepResult DFTTraceSimulator<ValueType>::randomStep() {
    // Randomly generate next failure
    auto retTuple = randomNextFailure();
    storm::dft::storage::FailableElements::const_iterator nextFailable = std::get<0>(retTuple);
    double addTime = std::get<1>(retTuple);
    bool successfulDependency = std::get<2>(retTuple);
    if (addTime < 0) {
        // No next state can be reached, because no element can fail anymore.
        STORM_LOG_TRACE("No next state possible in state " << dft.getStateString(state) << " because no element can fail anymore");
        return SimulationStepResult::UNSUCCESSFUL;
    }

    // Apply next failure
    auto stepResult = step(nextFailable, successfulDependency);
    STORM_LOG_TRACE("Current state: " << dft.getStateString(state));

    // Update time
    this->time += addTime;
    return stepResult;
}

template<typename ValueType>
SimulationStepResult DFTTraceSimulator<ValueType>::step(storm::dft::storage::FailableElements::const_iterator nextFailElement, bool dependencySuccessful) {
    if (nextFailElement == state->getFailableElements().end()) {
        // No next failure possible
        return SimulationStepResult::UNSUCCESSFUL;
    }

    auto nextBEPair = nextFailElement.getFailBE(dft);
    state = generator.createSuccessorState(state, nextBEPair.first, nextBEPair.second, dependencySuccessful);

    if (state->isInvalid() || state->isTransient()) {
        STORM_LOG_TRACE("Step is invalid because new state " << (state->isInvalid() ? "it is invalid" : "the transient fault is ignored"));
        return SimulationStepResult::INVALID;
    }

    return SimulationStepResult::SUCCESSFUL;
}

template<typename ValueType>
SimulationTraceResult DFTTraceSimulator<ValueType>::simulateNextStep(double timebound) {
    // Perform random step
    SimulationStepResult stepResult = randomStep();

    // Check current state
    if (stepResult == SimulationStepResult::INVALID) {
        // No next state can be reached, because the state is invalid.
        STORM_LOG_TRACE("Invalid state " << dft.getStateString(state) << " was reached.");
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Handling of invalid states is not supported for simulation");
        return SimulationTraceResult::INVALID;
    } else if (stepResult == SimulationStepResult::UNSUCCESSFUL) {
        STORM_LOG_TRACE("No next state possible in state " << dft.getStateString(state) << " because no further failures are possible.");
        return SimulationTraceResult::UNSUCCESSFUL;
    }
    STORM_LOG_ASSERT(stepResult == SimulationStepResult::SUCCESSFUL, "Simulation step should be successful.");

    if (getCurrentTime() > timebound) {
        // Timebound was exceeded
        return SimulationTraceResult::UNSUCCESSFUL;
    } else if (state->hasFailed(dft.getTopLevelIndex())) {
        // DFT is failed
        STORM_LOG_TRACE("DFT has failed after " << getCurrentTime());
        return SimulationTraceResult::SUCCESSFUL;
    } else {
        // No conclusive outcome was reached yet
        return SimulationTraceResult::CONTINUE;
    }
}

template<typename ValueType>
SimulationTraceResult DFTTraceSimulator<ValueType>::simulateCompleteTrace(double timebound) {
    resetToInitial();

    // Check whether DFT is initially already failed.
    if (state->hasFailed(dft.getTopLevelIndex())) {
        STORM_LOG_TRACE("DFT is initially failed");
        return SimulationTraceResult::SUCCESSFUL;
    }

    SimulationTraceResult result = SimulationTraceResult::CONTINUE;
    do {
        result = simulateNextStep(timebound);
    } while (result == SimulationTraceResult::CONTINUE);
    return result;
}

template<>
SimulationTraceResult DFTTraceSimulator<storm::RationalFunction>::simulateCompleteTrace(double timebound) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Simulation not support for parametric DFTs.");
}

template class DFTTraceSimulator<double>;
template class DFTTraceSimulator<storm::RationalFunction>;

}  // namespace simulator
}  // namespace storm::dft
