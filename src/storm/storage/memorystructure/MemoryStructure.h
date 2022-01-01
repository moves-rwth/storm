#pragma once

#include <memory>
#include <vector>

#include "storm/logic/Formula.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"

namespace storm {
namespace storage {

template<typename ValueType, typename RewardModelType>
class SparseModelMemoryProduct;

/*!
 * This class represents a (deterministic) memory structure that can be used to encode certain events
 * (such as reaching a set of goal states) into the state space of the model.
 */
class MemoryStructure {
   public:
    typedef std::vector<std::vector<boost::optional<storm::storage::BitVector>>> TransitionMatrix;

    /*!
     * Creates a memory structure with the given transition matrix, the given memory state labeling, and
     * the given initial states.
     * The entry transitionMatrix[m][n] specifies the set of model transitions which trigger a transition
     * from memory state m to memory state n.
     * Transitions are assumed to be deterministic and complete, i.e., the sets in in
     * transitionMatrix[m] form a partition of the transitions of the considered model.
     *
     * @param transitionMatrix The transition matrix
     * @param memoryStateLabeling A labeling of the memory states to specify, e.g., accepting states
     * @param initialMemoryStates assigns an initial memory state to each (initial?) state of the model.
     * @param onlyInitialStatesRelevant if true, initial memory states are only provided for each initial model state. Otherwise, an initial memory state is
     * provided for *every* model state.
     */
    MemoryStructure(TransitionMatrix const& transitionMatrix, storm::models::sparse::StateLabeling const& memoryStateLabeling,
                    std::vector<uint_fast64_t> const& initialMemoryStates, bool onlyInitialStatesRelevant = true);
    MemoryStructure(TransitionMatrix&& transitionMatrix, storm::models::sparse::StateLabeling&& memoryStateLabeling,
                    std::vector<uint_fast64_t>&& initialMemoryStates, bool onlyInitialStatesRelevant = true);

    bool isOnlyInitialStatesRelevantSet() const;
    TransitionMatrix const& getTransitionMatrix() const;
    storm::models::sparse::StateLabeling const& getStateLabeling() const;
    std::vector<uint_fast64_t> const& getInitialMemoryStates() const;
    uint_fast64_t getNumberOfStates() const;

    uint_fast64_t getSuccessorMemoryState(uint_fast64_t const& currentMemoryState, uint_fast64_t const& modelTransitionIndex) const;

    /*!
     * Builds the product of this memory structure and the given memory structure.
     * The resulting memory structure will have the state labels of both given structures.
     * Throws an exception if the state labelings are not disjoint.
     */
    MemoryStructure product(MemoryStructure const& rhs) const;

    /*!
     * Builds the product of this memory structure and the given sparse model.
     * An exception is thrown if the state labelings of this memory structure and the given model are not disjoint.
     */
    template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
    SparseModelMemoryProduct<ValueType, RewardModelType> product(storm::models::sparse::Model<ValueType, RewardModelType> const& sparseModel) const;

    std::string toString() const;

   private:
    TransitionMatrix transitions;
    storm::models::sparse::StateLabeling stateLabeling;
    std::vector<uint_fast64_t> initialMemoryStates;
    bool onlyInitialStatesRelevant;  // Whether initial memory states are only defined for initial model states or for all model states
};

}  // namespace storage
}  // namespace storm
