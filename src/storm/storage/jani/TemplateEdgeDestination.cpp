#include "storm/storage/jani/TemplateEdgeDestination.h"

namespace storm {
    namespace jani {
        
        TemplateEdgeDestination::TemplateEdgeDestination(OrderedAssignments const& assignments) : assignments(assignments) {
            // Intentionally left empty.
        }
        
        TemplateEdgeDestination::TemplateEdgeDestination(Assignment const& assignment) : assignments(assignment) {
            // Intentionally left empty.
        }
        
        TemplateEdgeDestination::TemplateEdgeDestination(std::vector<Assignment> const& assignments) : assignments(assignments) {
            // Intentionally left empty.
        }
        
        void TemplateEdgeDestination::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            assignments.substitute(substitution);
        }
        
        OrderedAssignments const& TemplateEdgeDestination::getOrderedAssignments() const {
            return assignments;
        }
        
        bool TemplateEdgeDestination::removeAssignment(Assignment const& assignment) {
            return assignments.remove(assignment);
        }
        
    }
}
