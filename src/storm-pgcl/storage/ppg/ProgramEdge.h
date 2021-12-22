#pragma once
#include "defines.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
namespace ppg {
class ProgramAction;

class ProgramEdge {
   public:
    ProgramEdge(ProgramEdgeGroup* group, ProgramEdgeIdentifier id, ProgramActionIdentifier action, storm::expressions::Expression const& condition,
                ProgramLocationIdentifier targetId)
        : group(group), edgeId(id), target(targetId), action(action), condition(condition) {
        // Intentionally left empty.
    }

    ProgramLocationIdentifier getSourceId() const;
    ProgramLocationIdentifier getTargetId() const {
        return target;
    }
    ProgramEdgeIdentifier getEdgeId() const {
        return edgeId;
    }

    storm::expressions::Expression const& getCondition() const {
        return condition;
    }

    bool hasNoAction() const;
    ProgramAction const& getAction() const;
    ProgramActionIdentifier getActionId() const {
        return action;
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
}  // namespace ppg
}  // namespace storm
