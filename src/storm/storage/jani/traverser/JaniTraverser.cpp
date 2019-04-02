#include "storm/storage/jani/traverser/JaniTraverser.h"


namespace storm {
    namespace jani {
        
        void JaniTraverser::traverse(Model& model, boost::any const& data) {
            for (auto& act : model.getActions()) {
                traverse(act, data);
            }
            for (auto& c : model.getConstants()) {
                traverse(c, data);
            }
            for (auto& f : model.getGlobalFunctionDefinitions()) {
                traverse(f.second, data);
            }
            traverse(model.getGlobalVariables(), data);
            for (auto& aut : model.getAutomata()) {
                traverse(aut, data);
            }
            if (model.hasInitialStatesRestriction()) {
                traverse(model.getInitialStatesRestriction(), data);
            }
            for (auto& nonTrivRew : model.getNonTrivialRewardExpressions()) {
                traverse(nonTrivRew.second, data);
            }
        }
        
        void JaniTraverser::traverse(Action const&, boost::any const&) {
            // Intentionally left empty.
        }
        
        void JaniTraverser::traverse(Automaton& automaton, boost::any const& data) {
            traverse(automaton.getVariables(), data);
            for (auto& f : automaton.getFunctionDefinitions()) {
                traverse(f.second, data);
            }
            for (auto& loc : automaton.getLocations()) {
                traverse(loc, data);
            }
            traverse(automaton.getEdgeContainer(), data);
            if (automaton.hasInitialStatesRestriction()) {
                traverse(automaton.getInitialStatesRestriction(), data);
            }
        }
        
        void JaniTraverser::traverse(Constant& constant, boost::any const& data) {
            if (constant.isDefined()) {
                traverse(constant.getExpression(), data);
            }
            if (constant.hasConstraint()) {
                traverse(constant.getConstraintExpression(), data);
            }
        }
        
        void JaniTraverser::traverse(FunctionDefinition& functionDefinition, boost::any const& data) {
            traverse(functionDefinition.getFunctionBody(), data);
        }
        
        void JaniTraverser::traverse(VariableSet& variableSet, boost::any const& data) {
            for (auto& v : variableSet.getBooleanVariables()) {
                traverse(v, data);
            }
            for (auto& v : variableSet.getBoundedIntegerVariables()) {
                traverse(v, data);
            }
            for (auto& v : variableSet.getUnboundedIntegerVariables()) {
                traverse(v, data);
            }
            for (auto& v : variableSet.getRealVariables()) {
                traverse(v, data);
            }
            for (auto& v : variableSet.getArrayVariables()) {
                traverse(v, data);
            }
            for (auto& v : variableSet.getClockVariables()) {
                traverse(v, data);
            }
        }
        
        void JaniTraverser::traverse(Location& location, boost::any const& data) {
            traverse(location.getAssignments(), data);
            if (location.hasTimeProgressInvariant()) {
                traverse(location.getTimeProgressInvariant(), data);
            }
        }
        
        void JaniTraverser::traverse(BooleanVariable& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void JaniTraverser::traverse(BoundedIntegerVariable& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
            traverse(variable.getLowerBound(), data);
            traverse(variable.getUpperBound(), data);
        }
        
        void JaniTraverser::traverse(UnboundedIntegerVariable& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void JaniTraverser::traverse(RealVariable& variable, boost::any const& data) {
             if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void JaniTraverser::traverse(ArrayVariable& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
            if (variable.hasLowerElementTypeBound()) {
                traverse(variable.getLowerElementTypeBound(), data);
            }
            if (variable.hasUpperElementTypeBound()) {
                traverse(variable.getUpperElementTypeBound(), data);
            }
        }
        
        void JaniTraverser::traverse(ClockVariable& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void JaniTraverser::traverse(EdgeContainer& edgeContainer, boost::any const& data) {
            for (auto& templateEdge : edgeContainer.getTemplateEdges()) {
                traverse(*templateEdge, data);
            }
            for (auto& concreteEdge : edgeContainer.getConcreteEdges()) {
                traverse(concreteEdge, data);
            }
        }
        
        void JaniTraverser::traverse(TemplateEdge& templateEdge, boost::any const& data) {
            traverse(templateEdge.getGuard(), data);
            for (auto& dest : templateEdge.getDestinations()) {
                traverse(dest, data);
            }
            traverse(templateEdge.getAssignments(), data);
        }
        
        void JaniTraverser::traverse(TemplateEdgeDestination& templateEdgeDestination, boost::any const& data) {
            traverse(templateEdgeDestination.getOrderedAssignments(), data);
        }
        
        void JaniTraverser::traverse(Edge& edge, boost::any const& data) {
            if (edge.hasRate()) {
                traverse(edge.getRate(), data);
            }
            for (auto& dest : edge.getDestinations()) {
                traverse(dest, data);
            }
        }
        
        void JaniTraverser::traverse(EdgeDestination& edgeDestination, boost::any const& data) {
            traverse(edgeDestination.getProbability(), data);
        }
        
        void JaniTraverser::traverse(OrderedAssignments& orderedAssignments, boost::any const& data) {
            for (auto& assignment : orderedAssignments) {
                traverse(assignment, data);
            }
            STORM_LOG_ASSERT(orderedAssignments.checkOrder(), "Order of ordered assignment has been violated.");
        }
        
        void JaniTraverser::traverse(Assignment& assignment, boost::any const& data) {
            traverse(assignment.getAssignedExpression(), data);
            traverse(assignment.getLValue(), data);
        }
        
        void JaniTraverser::traverse(LValue& lValue, boost::any const& data) {
            if (lValue.isArrayAccess()) {
                traverse(lValue.getArrayIndex(), data);
            }
        }
        
        void JaniTraverser::traverse(storm::expressions::Expression const&, boost::any const&) {
            // intentionally left empty.
        }
        
        void ConstJaniTraverser::traverse(Model const& model, boost::any const& data) {
            for (auto const& act : model.getActions()) {
                traverse(act, data);
            }
            for (auto const& c : model.getConstants()) {
                traverse(c, data);
            }
            for (auto const& f : model.getGlobalFunctionDefinitions()) {
                traverse(f.second, data);
            }
            traverse(model.getGlobalVariables(), data);
            for (auto const& aut : model.getAutomata()) {
                traverse(aut, data);
            }
            if (model.hasInitialStatesRestriction()) {
                traverse(model.getInitialStatesRestriction(), data);
            }
            for (auto const& nonTrivRew : model.getNonTrivialRewardExpressions()) {
                traverse(nonTrivRew.second, data);
            }
        }
        
        void ConstJaniTraverser::traverse(Action const&, boost::any const&) {
            // Intentionally left empty.
        }
        
        void ConstJaniTraverser::traverse(Automaton const& automaton, boost::any const& data) {
            traverse(automaton.getVariables(), data);
            for (auto const& f : automaton.getFunctionDefinitions()) {
                traverse(f.second, data);
            }
            for (auto const& loc : automaton.getLocations()) {
                traverse(loc, data);
            }
            traverse(automaton.getEdgeContainer(), data);
            if (automaton.hasInitialStatesRestriction()) {
                traverse(automaton.getInitialStatesRestriction(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(Constant const& constant, boost::any const& data) {
            if (constant.isDefined()) {
                traverse(constant.getExpression(), data);
            }
            if (constant.hasConstraint()) {
                traverse(constant.getConstraintExpression(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(FunctionDefinition const& functionDefinition, boost::any const& data) {
            traverse(functionDefinition.getFunctionBody(), data);
        }
        
        void ConstJaniTraverser::traverse(VariableSet const& variableSet, boost::any const& data) {
            for (auto const& v : variableSet.getBooleanVariables()) {
                traverse(v, data);
            }
            for (auto const& v : variableSet.getBoundedIntegerVariables()) {
                traverse(v, data);
            }
            for (auto const& v : variableSet.getUnboundedIntegerVariables()) {
                traverse(v, data);
            }
            for (auto const& v : variableSet.getRealVariables()) {
                traverse(v, data);
            }
            for (auto const& v : variableSet.getArrayVariables()) {
                traverse(v, data);
            }
            for (auto const& v : variableSet.getClockVariables()) {
                traverse(v, data);
            }
        }
        
        void ConstJaniTraverser::traverse(Location const& location, boost::any const& data) {
            traverse(location.getAssignments(), data);
            if (location.hasTimeProgressInvariant()) {
                traverse(location.getTimeProgressInvariant(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(BooleanVariable const& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(BoundedIntegerVariable const& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
            traverse(variable.getLowerBound(), data);
            traverse(variable.getUpperBound(), data);
        }
        
        void ConstJaniTraverser::traverse(UnboundedIntegerVariable const& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(RealVariable const& variable, boost::any const& data) {
             if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(ArrayVariable const& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
            if (variable.hasLowerElementTypeBound()) {
                traverse(variable.getLowerElementTypeBound(), data);
            }
            if (variable.hasUpperElementTypeBound()) {
                traverse(variable.getUpperElementTypeBound(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(ClockVariable const& variable, boost::any const& data) {
            if (variable.hasInitExpression()) {
                traverse(variable.getInitExpression(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(EdgeContainer const& edgeContainer, boost::any const& data) {
            for (auto const& templateEdge : edgeContainer.getTemplateEdges()) {
                traverse(*templateEdge, data);
            }
            for (auto const& concreteEdge : edgeContainer.getConcreteEdges()) {
                traverse(concreteEdge, data);
            }
        }
        
        void ConstJaniTraverser::traverse(TemplateEdge const& templateEdge, boost::any const& data) {
            traverse(templateEdge.getGuard(), data);
            for (auto const& dest : templateEdge.getDestinations()) {
                traverse(dest, data);
            }
            traverse(templateEdge.getAssignments(), data);
        }
        
        void ConstJaniTraverser::traverse(TemplateEdgeDestination const& templateEdgeDestination, boost::any const& data) {
            traverse(templateEdgeDestination.getOrderedAssignments(), data);
        }
        
        void ConstJaniTraverser::traverse(Edge const& edge, boost::any const& data) {
            if (edge.hasRate()) {
                traverse(edge.getRate(), data);
            }
            for (auto const& dest : edge.getDestinations()) {
                traverse(dest, data);
            }
        }
        
        void ConstJaniTraverser::traverse(EdgeDestination const& edgeDestination, boost::any const& data) {
            traverse(edgeDestination.getProbability(), data);
        }
        
        void ConstJaniTraverser::traverse(OrderedAssignments const& orderedAssignments, boost::any const& data) {
            for (auto const& assignment : orderedAssignments) {
                traverse(assignment, data);
            }
        }
        
        void ConstJaniTraverser::traverse(Assignment const& assignment, boost::any const& data) {
            traverse(assignment.getAssignedExpression(), data);
            traverse(assignment.getLValue(), data);
        }
        
        void ConstJaniTraverser::traverse(LValue const& lValue, boost::any const& data) {
            if (lValue.isArrayAccess()) {
                traverse(lValue.getArrayIndex(), data);
            }
        }
        
        void ConstJaniTraverser::traverse(storm::expressions::Expression const&, boost::any const&) {
            // intentionally left empty.
        }
        
    }
}

