#pragma once

#include "storm/utility/random.h"

namespace storm {
namespace simulator {

/*!
 * Abstract class for simulator of model.
 */
template<typename ValueType>
class ModelSimulator {
   public:
    /*!
     * Constructor.
     */
    ModelSimulator();

    /*!
     * Destructor.
     */
    virtual ~ModelSimulator() = default;

    /*!
     * Set specific seed for random number generator.
     * Useful to obtain deterministic behaviour of the simulator.
     *
     * @param seed Seed.
     */
    void setSeed(uint64_t seed);

    /*!
     * Reset the simulator to the initial state.
     */
    virtual void resetToInitial() = 0;

    /*!
     * Perform a random step from the current state.
     * For probabilistic states: Randomly select an action and then randomly select a transition.
     * For Markovian states (only in continuous-time models): Increase the time according to exit rate and then randomly select a transition.
     *
     * @return Whether the step was successful or not (e.g. deadlock).
     */
    bool randomStep();

    /*
     * Increase the current time according to the exit rate in the current state (if continuous-time model).
     * Nothing happens if for discrete-time models.
     */
    void randomTime();

    /*!
     * Perform the given action and then randomly select a transition.
     *
     * @param action Action index.
     * @return True iff step was successful.
     */
    virtual bool step(uint64_t action) = 0;

    /*!
     * Time progressed so far.
     * Is 0 for discrete-time models.
     *
     * @return Progressed time.
     */
    ValueType getCurrentTime() const;

    /*!
     * Return the number of choices for the current state.
     *
     * @return Number of current choices.
     */
    virtual uint64_t getCurrentNumberOfChoices() const = 0;

    /*!
     * Return all labels for the current state.
     * @return State labels.
     */
    virtual std::set<std::string> getCurrentStateLabelling() const = 0;

    /*!
     * Get the rewards for the current state.
     *
     * @return The current rewards (in the order from getRewardNames()).
     */
    std::vector<ValueType> const& getCurrentRewards() const;

    /*!
     * The names of the rewards that are returned.
     * @return Names of reward models.
     */
    virtual std::vector<std::string> getRewardNames() const = 0;

    /*!
     * Get the current exit rate of the state.
     * @return State exit rate (if Markovian state in a continuous-time model) or 0 (if probabilistic state or a discrete-time model).
     */
    virtual ValueType getCurrentExitRate() const = 0;

    /*!
     * Whether the current state is a deadlock.
     *
     * @return True if the current state has no outgoing transitions.
     */
    virtual bool isCurrentStateDeadlock() const = 0;

    /*!
     * Whether the model is a continuous-time model.
     *
     * @return True if the model is continuous-time.
     */
    virtual bool isContinuousTimeModel() const = 0;

   protected:
    /// Time which has progressed so far. Only relevant for continuous-time models
    ValueType currentTime;

    /// The current rewards
    std::vector<ValueType> currentRewards;

    /// Used to reinitialize the reward vector to all 0.
    std::vector<ValueType> zeroRewards;

    /// Random number generator
    storm::utility::RandomProbabilityGenerator<ValueType> randomGenerator;
};
}  // namespace simulator
}  // namespace storm