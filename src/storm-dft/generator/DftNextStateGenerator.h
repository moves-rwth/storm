#pragma once

#include "storm/generator/NextStateGenerator.h"
#include "storm/utility/ConstantsComparator.h"

#include "storm-dft/storage/DFT.h"

namespace storm::dft {
namespace generator {

/*!
 * Next state generator for DFTs.
 */
template<typename ValueType, typename StateType = uint32_t>
class DftNextStateGenerator {
    // TODO: inherit from NextStateGenerator

    using DFTStatePointer = std::shared_ptr<storm::dft::storage::DFTState<ValueType>>;
    using DFTElementPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>;
    using DFTGatePointer = std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>;
    using DFTRestrictionPointer = std::shared_ptr<storm::dft::storage::elements::DFTRestriction<ValueType>>;

   public:
    typedef std::function<StateType(DFTStatePointer const&)> StateToIdCallback;

    DftNextStateGenerator(storm::dft::storage::DFT<ValueType> const& dft, storm::dft::storage::DFTStateGenerationInfo const& stateGenerationInfo);

    bool isDeterministicModel() const;
    std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback);

    void load(storm::storage::BitVector const& state);
    void load(DFTStatePointer const& state);

    /*!
     * Expand and explore current state.
     * @param stateToIdCallback  Callback function which adds new state and returns the corresponding id.
     * @return StateBehavior containing successor choices and distributions.
     */
    storm::generator::StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback);

    /*!
     * Create unique failed state.
     *
     * @param stateToIdCallback Callback for state. The callback should just return the id and not use the state.
     *
     * @return Behavior of state.
     */
    storm::generator::StateBehavior<ValueType, StateType> createMergeFailedState(StateToIdCallback const& stateToIdCallback);

    /*!
     * Create initial state.
     *
     * @return Initial state.
     */
    DFTStatePointer createInitialState() const;

    /*!
     * Create successor state from given state by letting the given BE fail next.
     *
     * @param state Current state.
     * @param failedBE BE which fails next.
     * @param triggeringDependency Dependency which triggered the failure (or nullptr if BE failed on its own).
     * @param dependencySuccessful Whether the triggering dependency was successful.
     *              If the dependency is unsuccessful, failedBE does not fail and only the depedendy is marked as failed.
     *
     * @return Successor state.
     */
    DFTStatePointer createSuccessorState(DFTStatePointer const state, std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& failedBE,
                                         std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const>& triggeringDependency,
                                         bool dependencySuccessful = true) const;

    /**
     * Propagate the failures in a given state if the given BE fails
     *
     * @param newState starting state of the propagation
     * @param nextBE BE whose failure is propagated
     */
    void propagateFailure(DFTStatePointer newState, std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& nextBE,
                          storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType>& queues) const;

    /**
     * Propagate the failsafe state in a given state if the given BE fails
     *
     * @param newState starting state of the propagation
     * @param nextBE BE whose failure is propagated
     */
    void propagateFailsafe(DFTStatePointer newState, std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType> const>& nextBE,
                           storm::dft::storage::DFTStateSpaceGenerationQueues<ValueType>& queues) const;

   private:
    /*!
     * Explore current state and generate all successor states.
     * @param stateToIdCallback Callback function which adds new state and returns the corresponding id.
     * @param exploreDependencies Flag indicating whether failures due to dependencies or due to BEs should be explored.
     * @param takeFirstDependency If true, instead of exploring all possible orders of dependency failures, a fixed order is explored where always the first
     * dependency is considered.
     * @return StateBehavior containing successor choices and distributions.
     */
    storm::generator::StateBehavior<ValueType, StateType> exploreState(StateToIdCallback const& stateToIdCallback, bool exploreDependencies,
                                                                       bool takeFirstDependency);

    // The dft used for the generation of next states.
    storm::dft::storage::DFT<ValueType> const& mDft;

    // General information for the state generation.
    storm::dft::storage::DFTStateGenerationInfo const& mStateGenerationInfo;

    // Current state
    DFTStatePointer state;

    // Flag indicating whether all failed states should be merged into one unique failed state.
    bool uniqueFailedState;

    // Flag indicating whether the model is deterministic.
    bool deterministicModel = false;

    // Flag indicating whether only the first dependency (instead of all) should be explored.
    bool mTakeFirstDependency = false;
};

}  // namespace generator
}  // namespace storm::dft
