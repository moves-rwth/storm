
#include "ProgramLocation.h"
#include "ProgramGraph.h"

namespace storm {
namespace ppg {

std::vector<ProgramEdge*> ProgramLocation::addProgramEdgeToAllGroups(ProgramActionIdentifier action, storm::expressions::Expression const& condition,
                                                                     ProgramLocationIdentifier targetId) {
    return graph->addProgramEdgeToAllGroups(*this, action, condition, targetId);
}

std::vector<ProgramEdge*> ProgramLocation::addProgramEdgeToAllGroups(ProgramActionIdentifier action, ProgramLocationIdentifier targetId) {
    return graph->addProgramEdgeToAllGroups(*this, action, targetId);
}

ProgramEdgeGroup* ProgramLocation::addProgramEdgeGroup(storm::expressions::Expression const& probability) {
    return graph->addProgramEdgeGroup(*this, probability);
}

}  // namespace ppg
}  // namespace storm
