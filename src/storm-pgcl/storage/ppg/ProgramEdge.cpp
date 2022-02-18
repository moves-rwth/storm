#include "ProgramEdge.h"
#include "ProgramGraph.h"

namespace storm {
namespace ppg {

ProgramLocationIdentifier ProgramEdge::getSourceId() const {
    return group->getSourceId();
}

ProgramAction const& ProgramEdge::getAction() const {
    return group->getGraph().getAction(action);
}

bool ProgramEdge::hasNoAction() const {
    return action == group->getGraph().getNoActionId();
}
}  // namespace ppg
}  // namespace storm
