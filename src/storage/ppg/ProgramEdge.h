#pragma once
#include "defines.h"

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace ppg {
        class ProgramEdge {
        public:
            ProgramEdge(ProgramEdgeGroup* group, ProgramEdgeIdentifier id, ProgramActionIdentifier action, storm::expressions::Expression const& condition, ProgramLocationIdentifier targetId)
            : group(group), edgeId(id), target(targetId), action(action), condition(condition)
            {
                // Intentionally left empty.
            }
            
            virtual ~ProgramEdge() {
                // Intentionally left empty.
            }
            
        private:
            /// Pointer to the group; not owned
            ProgramEdgeGroup* group;
            /// Edge identifier
            ProgramEdgeIdentifier edgeId;
            /// Target location identifier
            ProgramLocationIdentifier target;
            /// Action identifier
            ProgramActionIdentifier action;
            /// Condition
            storm::expressions::Expression condition;
            
        };
    }
}
