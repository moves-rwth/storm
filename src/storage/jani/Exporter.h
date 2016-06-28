#pragma once

#include <sstream>
#include <cstdint>

#include "src/storage/jani/Model.h"

namespace storm {
    namespace jani {
        
        class Exporter {
        public:
            Exporter() = default;
            
            std::string toJaniString(storm::jani::Model const& model) const;
            
        private:
            void appendVersion(std::stringstream& out, uint64_t janiVersion, uint64_t indent) const;
            
            void appendModelName(std::stringstream& out, std::string const& name, uint64_t indent) const;
            
            void appendModelType(std::stringstream& out, storm::jani::ModelType const& modelType, uint64_t indent) const;
            
            void appendAction(std::stringstream& out, storm::jani::Action const& action, uint64_t indent) const;
            
            void appendActions(std::stringstream& out, storm::jani::Model const& model, uint64_t indent) const;
            
            void appendVariables(std::stringstream& out, storm::jani::VariableSet const& variables, uint64_t indent) const;
            
            void appendVariable(std::stringstream& out, storm::jani::BooleanVariable const& variable, uint64_t indent) const;
            void appendVariable(std::stringstream& out, storm::jani::BoundedIntegerVariable const& variable, uint64_t indent) const;
            void appendBoundedIntegerVariableType(std::stringstream& out, storm::jani::BoundedIntegerVariable const& variable, uint64_t indent) const;
            void appendVariable(std::stringstream& out, storm::jani::UnboundedIntegerVariable const& variable, uint64_t indent) const;

            void appendAutomata(std::stringstream& out, storm::jani::Model const& model, uint64_t indent) const;
            void appendAutomaton(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, uint64_t indent) const;
            
            void appendLocation(std::stringstream& out, storm::jani::Location const& location, uint64_t indent) const;
            void appendLocations(std::stringstream& out, storm::jani::Automaton const& automaton, uint64_t indent) const;
            
            void appendAssignment(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::Assignment const& assignment, uint64_t indent) const;
            void appendEdgeDestination(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::EdgeDestination const& destination, uint64_t indent) const;
            void appendEdge(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, storm::jani::Edge const& edge, uint64_t indent) const;
            void appendEdges(std::stringstream& out, storm::jani::Model const& model, storm::jani::Automaton const& automaton, uint64_t indent) const;
        };
        
    }
}