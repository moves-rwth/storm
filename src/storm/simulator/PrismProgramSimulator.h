#pragma once

#include "storm/simulator/ModelSimulator.h"

#include "storm/generator/PrismNextStateGenerator.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "storm/storage/prism/Program.h"

namespace storm {
namespace simulator {

/*!
 * This class provides a simulator interface on the prism program,
 * and uses the next state generator. While the next state generator has been tuned,
 * it is not targeted for simulation purposes. In particular, we (as of now)
 * always extend all actions as soon as we arrive in a state.
 * This may cause significant overhead, especially with a larger branching factor.
 *
 * On the other hand, this simulator is convenient for stepping through the model
 * as it potentially allows considering the next states.
 * Thus, while a performant alternative would be great, this simulator has its own merits.
 */
template<typename ValueType>
class PrismProgramSimulator : public ModelSimulator<ValueType> {
   public:
    /*!
     * Initialize the simulator for a given prism program.
     *
     * @param program The prism program. Should have a unique initial state.
     * @param options The generator options that are used to generate successor states.
     */
    PrismProgramSimulator(storm::prism::Program program, storm::generator::NextStateGeneratorOptions const& options);

    void resetToInitial() override;

    /*!
     * Reset the current state to the given state.
     * @param compressedState State to reset to.
     * @param time Time to reset to.
     */
    void resetToState(generator::CompressedState const& compressedState, ValueType time);

    /*!
     * Reset the current state to the given state.
     * @param valuationState State to reset to.
     * @param time Time to reset to.
     */
    void resetToState(expressions::SimpleValuation const& valuationState, ValueType time);

    bool step(uint64_t actionNumber) override;

    /*!
     * Get choices in current state.
     * @return A list of choices that encode the possibilities in the current state.
     * @note Successor states are encoded using state indices that will potentially be invalidated as soon as the internal state of the simulator changes.
     */
    std::vector<generator::Choice<ValueType, uint32_t>> const& getChoices() const;

    uint64_t getCurrentNumberOfChoices() const override;

    /*!
     * Get current state.
     * @return Current state.
     */
    generator::CompressedState const& getCurrentState() const;

    /*!
     * Get valuation of current state.
     * @return Valuation.
     */
    expressions::SimpleValuation getCurrentStateAsValuation() const;

    /*!
     * Get string representation of current state.
     * @return String representation of current state.
     */
    std::string getCurrentStateString() const;

    /*!
     * Get json representation of current state.
     * @return Json representation of current state.
     */
    storm::json<ValueType> getStateAsJson() const;

    std::set<std::string> getCurrentStateLabelling() const override;

    /*!
     * Get json representation of observations.
     * @return Json representation of observations.
     */
    storm::json<ValueType> getObservationAsJson() const;

    std::vector<std::string> getRewardNames() const override;

    ValueType getCurrentExitRate() const override;

    bool isCurrentStateDeadlock() const override;

    /*!
     * Whether the current state is an absorbing state.
     * @return True if current state is absorbing.
     */
    bool isSinkState() const;

    bool isContinuousTimeModel() const override;

   private:
    /*!
     * Explore the current state and create choices and distribution over successor states.
     */
    void explore();

    /*!
     * Clear all state caches.
     */
    void clearStateCaches();

    /*!
     * Helper function for (temp) storing states.
     *
     * @param state State.
     * @return Index of state.
     */
    uint32_t getOrAddStateIndex(generator::CompressedState const& state);

    /// The program that we are simulating.
    storm::prism::Program program;
    /// The current state in the program, in its compressed form.
    generator::CompressedState currentState;
    /// Generator for the next states
    std::shared_ptr<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>> stateGenerator;
    /// Obtained behavior of a state
    generator::StateBehavior<ValueType> behavior;
    /// Data structure to temporarily store states.
    storm::storage::BitVectorHashMap<uint32_t> stateToId;
    /// Data structure to temporarily store ids to states.
    std::unordered_map<uint32_t, generator::CompressedState> idToState;

    /// Create a callback for the next-state generator to enable it to request the index of states.
    std::function<uint32_t(generator::CompressedState const&)> stateToIdCallback =
        std::bind(&PrismProgramSimulator<ValueType>::getOrAddStateIndex, this, std::placeholders::_1);
};
}  // namespace simulator
}  // namespace storm
