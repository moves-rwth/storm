#pragma once

#include <memory>
#include <vector>

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/memorystructure/MemoryStructure.h"

namespace storm {
namespace storage {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class MemoryStructureBuilder {
   public:
    /*!
     * Initializes a new builder with the data from the provided memory structure
     * @param numberOfMemoryStates The number of states the resulting memory structure should have
     * @param onlyInitialStatesRelevant If true, assume that we only consider the fragment reachable from initial model states. If false, initial memory states
     * need to be provided for *all* model states.
     */
    MemoryStructureBuilder(uint_fast64_t numberOfMemoryStates, storm::models::sparse::Model<ValueType, RewardModelType> const& model,
                           bool onlyInitialStatesRelevant = true);

    /*!
     * Initializes a new builder with the data from the provided memory structure
     */
    MemoryStructureBuilder(MemoryStructure const& memoryStructure, storm::models::sparse::Model<ValueType, RewardModelType> const& model);

    /*!
     * Specifies for the given state of the model the corresponding initial memory state.
     *
     * @note The default initial memory state is 0.
     *
     * @param initialModelState the index of a state of the model. Has to be an initial state iff `onlyInitialStatesRelevant` is true.
     * @param initialMemoryState the initial memory state associated to the corresponding model state.
     */
    void setInitialMemoryState(uint_fast64_t initialModelState, uint_fast64_t initialMemoryState);

    /*!
     * Specifies a transition of the memory structure.
     * The interpretation is that we switch from startState to goalState upon entering one of the specified model states (via one of the specified choices).
     *
     * @note If it is not possible to move from startState to goalState, such a transition does not have to be set explicitly.
     *
     * @param startState the memorystate in which the transition starts
     * @param goalState the memorystate in which the transition ends
     * @param modelStates the model states that trigger this transition.
     * @param modelChoices if given, filers the choices of the model that trigger this transition.
     *
     */
    void setTransition(uint_fast64_t const& startState, uint_fast64_t const& goalState, storm::storage::BitVector const& modelStates,
                       boost::optional<storm::storage::BitVector> const& modelChoices = boost::none);

    /*!
     * Sets a label to the given memory state.
     */
    void setLabel(uint_fast64_t const& state, std::string const& label);

    /*!
     * Builds the memory structure.
     * @note Calling this invalidates this builder.
     * @note When calling this method, the specified transitions should be deterministic and complete, i.e., every triple
     * (memoryState, modelChoice, modelState) should uniquely specify a successor memory state.
     */
    MemoryStructure build();

    /*!
     * Builds a trivial memory structure for the given model (consisting of a single memory state)
     */
    static MemoryStructure buildTrivialMemoryStructure(storm::models::sparse::Model<ValueType, RewardModelType> const& model);

   private:
    storm::models::sparse::Model<ValueType, RewardModelType> const& model;
    MemoryStructure::TransitionMatrix transitions;
    storm::models::sparse::StateLabeling stateLabeling;
    std::vector<uint_fast64_t> initialMemoryStates;
    bool onlyInitialStatesRelevant;
};

}  // namespace storage
}  // namespace storm
