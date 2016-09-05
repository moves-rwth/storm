#include "src/storage/jani/Exporter.h"

#include <iostream>

namespace storm {
    namespace jani {
        
        void appendIndent(std::stringstream& out, uint64_t indent) {
            for (uint64_t i = 0; i < indent; ++i) {
                out << "\t";
            }
        }
        
        void appendField(std::stringstream& out, std::string const& name) {
            out << "\"" << name << "\": ";
        }
        
        void appendValue(std::stringstream& out, std::string const& value) {
            out << "\"" << value << "\"";
        }
        
        void clearLine(std::stringstream& out) {
            out << std::endl;
        }
        
        std::string expressionToString(storm::expressions::Expression const& expression) {
            std::stringstream s;
            s << expression;
            return s.str();
        }
        
        void Exporter::appendVersion(std::stringstream& out, uint64_t janiVersion, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "jani-version");
        }
        
        void Exporter::appendModelName(std::stringstream& out, std::string const& name, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "name");
            appendValue(out, name);
        }
        
        void Exporter::appendModelType(std::stringstream& out, storm::jani::ModelType const& modelType, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "type");
        }
        
        void Exporter::appendAction(std::stringstream& out, storm::jani::Action const& action, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{" << std::endl;
            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, action.getName());
            clearLine(out);
            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendActions(std::stringstream& out, storm::jani::Model const& model, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "actions");
            out << " [" << std::endl;
            
            for (uint64_t index = 0; index < model.actions.size(); ++index) {
                appendAction(out, model.actions[index], indent + 1);
                if (index < model.actions.size() - 1) {
                    out << ",";
                }
                clearLine(out);
            }
            
            appendIndent(out, indent);
            out << "]";
        }
        
        void Exporter::appendVariable(std::stringstream& out, storm::jani::BooleanVariable const& variable, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, variable.getName());
            out << ",";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "type");
            appendValue(out, "bool");
            out << ",";
            clearLine(out);
            appendIndent(out, indent);
            out << "}";
        }

        void Exporter::appendBoundedIntegerVariableType(std::stringstream& out, storm::jani::BoundedIntegerVariable const& variable, uint64_t indent) const {
            out << " {";
            clearLine(out);

            appendIndent(out, indent);
            appendField(out, "kind");
            appendValue(out, "bounded");
            clearLine(out);
            
            appendIndent(out, indent);
            appendField(out, "base");
            appendValue(out, "int");
            clearLine(out);
            
            appendIndent(out, indent);
            appendField(out, "lower-bound");
            appendValue(out, expressionToString(variable.getLowerBound()));
            clearLine(out);

            appendIndent(out, indent);
            appendField(out, "upper-bound");
            appendValue(out, expressionToString(variable.getLowerBound()));
            clearLine(out);
            
            appendIndent(out, indent - 1);
            out << "}";
        }
        
        void Exporter::appendVariable(std::stringstream& out, storm::jani::BoundedIntegerVariable const& variable, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, variable.getName());
            out << ",";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "type");
            appendBoundedIntegerVariableType(out, variable, indent + 2);
            out << ",";
            clearLine(out);
            appendIndent(out, indent);
            out << "}";
        }

        void Exporter::appendVariable(std::stringstream& out, storm::jani::UnboundedIntegerVariable const& variable, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, variable.getName());
            out << ",";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "type");
            appendValue(out, "int");
            out << ",";
            clearLine(out);
            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendVariables(std::stringstream& out, storm::jani::VariableSet const& variables, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "variables");
            out << " [";
            clearLine(out);
            
            for (auto const& variable : variables.getBooleanVariables()) {
                appendVariable(out, variable, indent + 1);
                clearLine(out);
            }
            for (auto const& variable : variables.getBoundedIntegerVariables()) {
                appendVariable(out, variable, indent + 1);
                clearLine(out);
            }
            for (auto const& variable : variables.getUnboundedIntegerVariables()) {
                appendVariable(out, variable, indent + 1);
                clearLine(out);
            }
            
            appendIndent(out, indent);
            out << "]";
        }

        void Exporter::appendLocation(std::stringstream& out, storm::jani::Location const& location, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, location.getName());
            clearLine(out);

            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendLocations(std::stringstream& out, storm::jani::Automaton const& automaton, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "locations");
            out << " [";
            clearLine(out);
            
            for (auto const& location : automaton.getLocations()) {
                appendLocation(out, location, indent + 1);
                out << ",";
                clearLine(out);
            }
            
            appendIndent(out, indent);
            out << "]";
        }

        void Exporter::appendAssignment(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::Assignment const& assignment, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "ref");
            storm::jani::Variable const& variable = model.getGlobalVariables().hasVariable(assignment.getExpressionVariable()) ? model.getGlobalVariables().getVariable(assignment.getExpressionVariable()) : automaton.getVariables().getVariable(assignment.getExpressionVariable());
            appendValue(out, variable.getName());
            out << ",";
            clearLine(out);

            appendIndent(out, indent + 1);
            appendField(out, "value");
            appendValue(out, expressionToString(assignment.getAssignedExpression()));
            clearLine(out);

            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendEdgeDestination(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::EdgeDestination const& destination, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "probability");
            appendValue(out, expressionToString(destination.getProbability()));
            out << ",";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "location");
            appendValue(out, automaton.getLocation(destination.getLocationIndex()).getName());
            out << ",";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "assignments");
            out << " [";
            clearLine(out);
            
            for (uint64_t index = 0; index < destination.getAssignments().size(); ++index) {
                appendAssignment(out, model, automaton, destination.getAssignments()[index], indent + 2);
                if (index < destination.getAssignments().size() - 1) {
                    out << ",";
                }
                clearLine(out);
            }
            
            appendIndent(out, indent + 1);
            out << "]";
            clearLine(out);
            
            appendIndent(out, indent);
            out << "}";
            clearLine(out);
        }
        
        void Exporter::appendEdge(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::Edge const& edge, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "location");
            appendValue(out, automaton.getLocation(edge.getSourceLocationIndex()).getName());
            out << ",";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "action");
            appendValue(out, model.getAction(edge.getActionIndex()).getName());
            out << ",";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "guard");
            appendValue(out, expressionToString(edge.getGuard()));
            out << ",";
            clearLine(out);
            
            appendIndent(out, indent + 1);
            appendField(out, "destinations");
            out << " [";
            clearLine(out);
            
            for (auto const& destination : edge.getDestinations()) {
                appendEdgeDestination(out, model, automaton, destination, indent + 2);
            }
            
            appendIndent(out, indent + 1);
            out << "]";
            clearLine(out);
            
            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendEdges(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "edges");
            out << " [";
            clearLine(out);
            
            for (uint64_t location = 0; location < automaton.getNumberOfLocations(); ++location) {
                for (auto const& edge : automaton.getEdgesFromLocation(location)) {
                    appendEdge(out, model, automaton, edge, indent + 1);
                    out << ",";
                    clearLine(out);
                }
            }
            
            appendIndent(out, indent);
            out << "]";
            clearLine(out);
        }
        
        void Exporter::appendAutomaton(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, uint64_t indent) const {
            appendIndent(out, indent);
            out << "{";
            clearLine(out);

            appendIndent(out, indent + 1);
            appendField(out, "name");
            appendValue(out, automaton.getName());
            clearLine(out);
            appendVariables(out, automaton.getVariables(), indent + 1);
            out << ",";
            clearLine(out);
            
            appendLocations(out, automaton, indent + 1);
            out << ",";
            clearLine(out);
            appendIndent(out, indent + 1);
            appendField(out, "initial-locations");
            out << " [";
            clearLine(out);
            for (auto const& index : automaton.getInitialLocationIndices()) {
                appendIndent(out, indent + 2);
                appendValue(out, automaton.getLocation(index).getName());
                clearLine(out);
            }
            appendIndent(out, indent + 1);
            out << "]";
            clearLine(out);
            if (automaton.hasInitialStatesRestriction()) {
                appendIndent(out, indent + 1);
                appendField(out, "initial-states");
                clearLine(out);
                appendIndent(out, indent + 2);
                out << "{";
                clearLine(out);
                appendIndent(out, indent + 3);
                appendField(out, "exp");
                appendValue(out, expressionToString(automaton.getInitialStatesExpression()));
                clearLine(out);
                appendIndent(out, indent + 2);
                out << "}";
                clearLine(out);
            }
            
            appendEdges(out, model, automaton, indent + 1);
            
            appendIndent(out, indent);
            out << "}";
        }
        
        void Exporter::appendAutomata(std::stringstream& out, storm::jani::Model const& model, uint64_t indent) const {
            appendIndent(out, indent);
            appendField(out, "automata");
            out << " [";
            clearLine(out);
            
            for (uint64_t index = 0; index < model.automata.size(); ++index) {
                appendAutomaton(out, model, model.automata[index], indent + 1);
                if (index < model.automata.size() - 1) {
                    out << ",";
                }
                clearLine(out);
            }
            
            appendIndent(out, indent);
            out << "]";
        }
        
        std::string Exporter::toJaniString(storm::jani::Model const& model) const {
            std::stringstream out;
            
            out << "{" << std::endl;
            appendVersion(out, model.getJaniVersion(), 1);
            out << ",";
            clearLine(out);
            appendModelName(out, model.getName(), 1);
            out << ",";
            clearLine(out);
            appendModelType(out, model.getModelType(), 1);
            out << ",";
            clearLine(out);
            appendActions(out, model, 1);
            clearLine(out);
            appendVariables(out, model.getGlobalVariables(), 1);
            clearLine(out);
            
            appendIndent(out, 1);
            appendField(out, "initial-states");
            clearLine(out);
            appendIndent(out, 2);
            out << "{";
            clearLine(out);
            appendIndent(out, 3);
            appendField(out, "exp");
            appendValue(out, expressionToString(model.getInitialStatesRestriction()));
            clearLine(out);
            appendIndent(out, 2);
            out << "}";
            clearLine(out);
            
            appendAutomata(out, model, 1);
            clearLine(out);
            out << "}" << std::endl;
            
            return out.str();
        }
        
    }
}