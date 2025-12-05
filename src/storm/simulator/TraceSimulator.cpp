#include "storm/simulator/TraceSimulator.h"

#include "storm/exceptions/UnsupportedModelException.h"
#include "storm/simulator/SparseModelSimulator.h"

namespace storm {
namespace simulator {

template<typename ValueType>
TraceSimulator<ValueType>::TraceSimulator(std::shared_ptr<storm::simulator::SparseModelSimulator<ValueType>> simulator) : simulator(simulator) {
    // Intentionally left empty.
}

template<typename ValueType>
SimulationTraceResult TraceSimulator<ValueType>::simulateStepBoundedTrace(size_t stepBound, std::optional<std::string> const& goalLabel) {
    simulator->resetToInitial();
    size_t steps = 0;
    while (steps <= stepBound) {
        if (reachedLabel(goalLabel)) {
            return SimulationTraceResult::GOAL_REACHED;
        }
        if (!simulator->randomStep()) {
            return SimulationTraceResult::DEADLOCK;
        }
        steps++;
    }
    return SimulationTraceResult::BOUND_REACHED;
}

template<typename ValueType>
SimulationTraceResult TraceSimulator<ValueType>::simulateTimeBoundedTrace(ValueType timeBound, std::optional<std::string> const& goalLabel) {
    simulator->resetToInitial();

    while (simulator->getCurrentTime() <= timeBound) {
        if (reachedLabel(goalLabel)) {
            return SimulationTraceResult::GOAL_REACHED;
        }
        if (!simulator->randomStep()) {
            return SimulationTraceResult::DEADLOCK;
        }
    }
    return SimulationTraceResult::BOUND_REACHED;
}

template<typename ValueType>
ValueType TraceSimulator<ValueType>::simulateStepBoundedReachability(std::string const& label, uint64_t stepBound, uint64_t numberRuns) {
    STORM_LOG_WARN_COND(!simulator->isContinuousTimeModel(), "Simulating step-bounded reachability on a continuous-time model.");
    uint64_t numberGoal = 0;
    storm::simulator::SimulationTraceResult res;
    for (size_t i = 0; i < numberRuns; ++i) {
        res = simulateStepBoundedTrace(stepBound, label);
        if (res == storm::simulator::SimulationTraceResult::GOAL_REACHED) {
            ++numberGoal;
        } else {
            if (res == storm::simulator::SimulationTraceResult::DEADLOCK) {
                STORM_LOG_WARN("Simulation reached a deadlock state.");
            }
        }
    }
    return storm::utility::convertNumber<ValueType>(numberGoal) / storm::utility::convertNumber<ValueType>(numberRuns);
}

template<typename ValueType>
ValueType TraceSimulator<ValueType>::simulateTimeBoundedReachability(std::string const& label, ValueType timebound, uint64_t numberRuns) {
    STORM_LOG_THROW(simulator->isContinuousTimeModel(), storm::exceptions::UnsupportedModelException,
                    "Time-bounded reachability can only be simulated on continuous-time models.");
    uint64_t numberGoal = 0;
    storm::simulator::SimulationTraceResult res;
    for (size_t i = 0; i < numberRuns; ++i) {
        res = simulateTimeBoundedTrace(timebound, label);
        if (res == storm::simulator::SimulationTraceResult::GOAL_REACHED) {
            ++numberGoal;
        } else {
            if (res == storm::simulator::SimulationTraceResult::DEADLOCK) {
                STORM_LOG_WARN("Simulation reached a deadlock state.");
            }
        }
    }
    return storm::utility::convertNumber<ValueType>(numberGoal) / storm::utility::convertNumber<ValueType>(numberRuns);
}

template<typename ValueType>
bool TraceSimulator<ValueType>::reachedLabel(std::optional<std::string> const& goalLabel) const {
    if (!goalLabel.has_value()) {
        return false;
    }
    return simulator->getCurrentStateLabelling().contains(goalLabel.value());
}

template<typename ValueType>
std::shared_ptr<storm::simulator::SparseModelSimulator<ValueType> const> TraceSimulator<ValueType>::getSimulator() const {
    return simulator;
}

template class TraceSimulator<double>;
template class TraceSimulator<storm::RationalNumber>;

}  // namespace simulator
}  // namespace storm
