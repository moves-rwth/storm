#include "ProgramEdgeGroup.h"
#include "ProgramGraph.h"

namespace storm {
namespace ppg {
ProgramEdge* ProgramEdgeGroup::addEdge(ProgramActionIdentifier action, ProgramLocationIdentifier target) {
    return graph->addProgramEdge(*this, action, target);
}
ProgramEdge* ProgramEdgeGroup::addEdge(ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier target) {
    return graph->addProgramEdge(*this, action, condition, target);
}

}  // namespace ppg
}  // namespace storm
