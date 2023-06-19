#include "storm/modelchecker/blackbox/blackbox_interface.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"



namespace storm {
namespace modelchecker {
namespace blackbox {

typename blackboxMDP::index_type blackboxMDP::get_suc_count(blackboxMDP::index_type state, blackboxMDP::index_type action) {
    STORM_LOG_THROW(is_greybox(), storm::exceptions::NotImplementedException, "get_suc_count is not implemented for this greybox MDP");
    STORM_LOG_THROW(!is_greybox(), storm::exceptions::NotSupportedException, "get_suc_count is not implemented for this greybox MDP");
}

double blackboxMDP::get_pmin() {
    throw storm::exceptions::NotImplementedException();
}

template <typename StateType, typename ValueType>
blackboxWrapperOnWhitebox<StateType, ValueType>::blackboxWrapperOnWhitebox(storm::prism::Program& program)
                                                : explorationInformation(storm::OptimizationDirection::Maximize),
                                                  stateGeneration(program, explorationInformation, 
                                                  (storm::expressions::Expression()), (storm::expressions::Expression())) {
    // intentionally left empty
}

template <typename StateType, typename ValueType>
typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type blackboxWrapperOnWhitebox<StateType, ValueType>::get_initial_state() {
    return 0;
}

template <typename StateType, typename ValueType>
storage::KeyIterator<typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type> blackboxWrapperOnWhitebox<StateType, ValueType>::get_avail_actions(index_type state) {
    return storage::KeyIterator<typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type>();
}

template <typename StateType, typename ValueType>
typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type blackboxWrapperOnWhitebox<StateType, ValueType>::sample_suc(index_type state, index_type action) {
 return 0;
}

template <typename StateType, typename ValueType>
bool blackboxWrapperOnWhitebox<StateType, ValueType>::is_greybox() {
    return false;
}

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
