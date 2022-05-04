#pragma once

#include <memory>
#include <vector>

#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/BitVector.h"

namespace storm {
namespace transformer {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
struct SubsystemBuilderReturnType {
    // The resulting model
    std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model;
    // Gives for each state in the resulting model the corresponding state in the original model.
    std::vector<uint_fast64_t> newToOldStateIndexMapping;
    // Gives for each action in the resulting model the corresponding action in the original model.
    // If this action was introduced to fix a deadlock, it will get index std::numeric_limits<uint64_t>::max()
    std::vector<uint64_t> newToOldActionIndexMapping;
    // marks the actions of the original model that are still available in the subsystem
    storm::storage::BitVector keptActions;
    // If set, deadlock states have been introduced and have been assigned this label.
    boost::optional<std::string> deadlockLabel;
};

struct SubsystemBuilderOptions {
    bool checkTransitionsOutside = true;
    bool buildStateMapping = true;
    bool buildActionMapping = false;
    bool buildKeptActions = true;
    bool fixDeadlocks = false;
    bool makeRowGroupingTrivial = false;
};

/*
 * Removes all states and actions that are not part of the subsystem.
 * A state is part of the subsystem iff
 *    * it is selected in subsystemStates AND
 *    * keepUnreachableStates is true or the state is reachable from the initial states
 * An action is part of the subsystem iff
 *    * it is selected in subsystemActions AND
 *    * it originates from a state that is part of the subsystem AND
 *    * it does not contain a transition leading to a state outside of the subsystem.
 *
 * If this introduces a deadlock state (i.e., a state without an action) it is either
 *    * fixed by inserting a selfloop (if fixDeadlocks is true) or
 *    * an exception is thrown (otherwise).
 *
 * @param originalModel The original model.
 * @param subsystemStates The selected states.
 * @param subsystemActions The selected actions
 * @param keepUnreachableStates if true, states that are not reachable from the initial state are kept
 */
template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
SubsystemBuilderReturnType<ValueType, RewardModelType> buildSubsystem(storm::models::sparse::Model<ValueType, RewardModelType> const& originalModel,
                                                                      storm::storage::BitVector const& subsystemStates,
                                                                      storm::storage::BitVector const& subsystemActions, bool keepUnreachableStates = true,
                                                                      SubsystemBuilderOptions options = SubsystemBuilderOptions());

}  // namespace transformer
}  // namespace storm
