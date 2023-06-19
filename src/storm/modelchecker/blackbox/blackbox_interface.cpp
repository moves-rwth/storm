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

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
