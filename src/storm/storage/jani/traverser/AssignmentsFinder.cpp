#include "storm/storage/jani/traverser/AssignmentsFinder.h"


namespace storm {
    namespace jani {
        
        AssignmentsFinder::ResultType AssignmentsFinder::find(Model const& model, Variable const& variable) {
            ResultType res;
            res.hasLocationAssignment = false;
            res.hasEdgeAssignment = false;
            res.hasEdgeDestinationAssignment = false;
            ConstJaniTraverser::traverse(model, std::make_pair(&variable, &res));
            return res;
        }
        
        void AssignmentsFinder::traverse(Location const& location, boost::any const& data) const {
            auto resVar = boost::any_cast<std::pair<Variable const*, ResultType*>>(data);
            for (auto const& assignment : location.getAssignments()) {
                if (assignment.getVariable() == *resVar.first) {
                    resVar.second->hasLocationAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(location, data);
        }
        
        void AssignmentsFinder::traverse(TemplateEdge const& templateEdge, boost::any const& data) const {
            auto resVar = boost::any_cast<std::pair<Variable const*, ResultType*>>(data);
            for (auto const& assignment : templateEdge.getAssignments()) {
                if (assignment.getVariable() == *resVar.first) {
                    resVar.second->hasEdgeAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(templateEdge, data);
        }
        
        void AssignmentsFinder::traverse(TemplateEdgeDestination const& templateEdgeDestination, boost::any const& data) const {
            auto resVar = boost::any_cast<std::pair<Variable const*, ResultType*>>(data);
            for (auto const& assignment : templateEdgeDestination.getOrderedAssignments()) {
                if (assignment.getVariable() == *resVar.first) {
                    resVar.second->hasEdgeDestinationAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(templateEdgeDestination, data);
        }
    }
}

