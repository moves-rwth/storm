#pragma once

#include "storm/simulator/ModelSimulator.h"

namespace storm {
namespace simulator {

/*!
 * The result of simulating a trace can be either that
 * - the goal label was reached
 * - the step/time bound was reached
 * - or a deadlock was encountered.
 */
enum class SimulationTraceResult { GOAL_REACHED, BOUND_REACHED, DEADLOCK };

/*!
 * Perform simulation runs of a model.
 */
template<typename ValueType>
class TraceSimulator {
   public:
    /*!
     * Constructor.
     * @param simulator Underlying simulator.
     */
    TraceSimulator(std::shared_ptr<storm::simulator::ModelSimulator<ValueType>> simulator);

    /*!
     * Simulate a single (untimed) trace until either the goal label or the step bound is reached.
     * @param stepBound Step bound.
     * @param goalLabel Label to reach.
     * @return Result of trace (step bound reached, goal reached or deadlock).
     */
    SimulationTraceResult simulateStepBoundedTrace(size_t stepBound, std::optional<std::string> const& goalLabel);

    /*!
     * Simulate a single (timed) trace until either the goal label or the time bound is reached.
     * @param timeBound Time bound.
     * @param goalLabel Label to reach.
     * @return Result of trace (time bound reached, goal reached or deadlock).
     */
    SimulationTraceResult simulateTimeBoundedTrace(ValueType timeBound, std::optional<std::string> const& goalLabel);

    /*!
     * Perform Monte Carlo simulation of a step-bounded reachability property.
     * Approximates P=? [F<=stepBound label].
     * @param label Goal label to reach.
     * @param stepBound Step bound.
     * @param numberRuns Number of simulation runs to perform.
     * @return Probability of satisfying the property. Calculated as the ratio of runs reaching the label within stepBound / numberRuns.
     */
    ValueType simulateStepBoundedReachability(std::string const& label, uint64_t stepBound, uint64_t numberRuns);

    /*!
     * Perform Monte Carlo simulation of a time-bounded reachability property (for continuous-time models).
     * Approximates P=? [F<=timeBound label].
     * @param label Goal label to reach.
     * @param timebound Time bound.
     * @param numberRuns Number of simulation runs to perform.
     * @return Probability of satisfying the property. Calculated as the ratio of runs reaching the label within timeBound / numberRuns.
     */
    ValueType simulateTimeBoundedReachability(std::string const& label, ValueType timebound, uint64_t numberRuns);

    /*!
     * Return the underlying simulator.
     * @return Simulator.
     */
    std::shared_ptr<storm::simulator::ModelSimulator<ValueType> const> getSimulator() const;

   private:
    /*!
     * Check whether the current state of the simulator contains the given label.
     * @param goalLabel Label to reach.
     * @return True if current state contains goal label. Always returns false if no goal label was provided.
     */
    bool reachedLabel(std::optional<std::string> const& goalLabel) const;

    /// The underlying simulator
    std::shared_ptr<storm::simulator::ModelSimulator<ValueType>> simulator;
};
}  // namespace simulator
}  // namespace storm