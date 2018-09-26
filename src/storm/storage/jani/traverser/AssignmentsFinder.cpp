#include "storm/storage/jani/traverser/AssignmentsFinder.h"

#include "storm/storage/expressions/Variable.h"

namespace storm {
    namespace jani {
        
        AssignmentsFinder::ResultType AssignmentsFinder::find(Model const& model, storm::jani::Variable const& variable) {
            return find(model, variable.getExpressionVariable());
        }
        
        AssignmentsFinder::ResultType AssignmentsFinder::find(Model const& model, storm::expressions::Variable const& variable) {
            ResultType res;
            res.hasLocationAssignment = false;
            res.hasEdgeAssignment = false;
            res.hasEdgeDestinationAssignment = false;
            ConstJaniTraverser::traverse(model, std::make_pair(&variable, &res));
            return res;
        }
        
        void AssignmentsFinder::traverse(Location const& location, boost::any const& data) {
            auto resVar = boost::any_cast<std::pair<storm::expressions::Variable const*, ResultType*>>(data);
            for (auto const& assignment : location.getAssignments()) {
                storm::jani::Variable const& assignedVariable = assignment.lValueIsArrayAccess() ? assignment.getLValue().getArray() : assignment.getVariable();
                if (assignedVariable.getExpressionVariable() == *resVar.first) {
                    resVar.second->hasLocationAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(location, data);
        }
        
        void AssignmentsFinder::traverse(TemplateEdge const& templateEdge, boost::any const& data) {
            auto resVar = boost::any_cast<std::pair<storm::expressions::Variable const*, ResultType*>>(data);
            for (auto const& assignment : templateEdge.getAssignments()) {
                storm::jani::Variable const& assignedVariable = assignment.lValueIsArrayAccess() ? assignment.getLValue().getArray() : assignment.getVariable();
                if (assignedVariable.getExpressionVariable() == *resVar.first) {
                    resVar.second->hasEdgeAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(templateEdge, data);
        }
        
        void AssignmentsFinder::traverse(TemplateEdgeDestination const& templateEdgeDestination, boost::any const& data) {
            auto resVar = boost::any_cast<std::pair<storm::expressions::Variable const*, ResultType*>>(data);
            for (auto const& assignment : templateEdgeDestination.getOrderedAssignments()) {
                storm::jani::Variable const& assignedVariable = assignment.lValueIsArrayAccess() ? assignment.getLValue().getArray() : assignment.getVariable();
                if (assignedVariable.getExpressionVariable() == *resVar.first) {
                    resVar.second->hasEdgeDestinationAssignment = true;
                }
            }
            ConstJaniTraverser::traverse(templateEdgeDestination, data);
        }
    }
}

