#include "ProgramEdge.h"
#include "ProgramGraph.h"

namespace storm {
    namespace ppg {
        
        ProgramAction const& ProgramEdge::getAction() const {
            return group->getGraph().getAction(action);
        }
    }
}