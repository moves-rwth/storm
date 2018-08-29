#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/Model.h"

namespace storm {
    namespace jani {
        class JaniTraverser {
        public:
            virtual ~JaniTraverser() = default;
            
            virtual void traverse(Model const& model, boost::any const& data) const;
            
            virtual void traverse(Action const& action, boost::any const& data) const;
            virtual void traverse(Automaton const& automaton, boost::any const& data) const;
            virtual void traverse(Constant const& constant, boost::any const& data) const;
            virtual void traverse(VariableSet const& variableSet, boost::any const& data) const;
            virtual void traverse(Location const& location, boost::any const& data) const;
            virtual void traverse(BooleanVariable const& variable, boost::any const& data) const;
            virtual void traverse(BoundedIntegerVariable const& variable, boost::any const& data) const;
            virtual void traverse(UnboundedIntegerVariable const& variable, boost::any const& data) const;
            virtual void traverse(RealVariable const& variable, boost::any const& data) const;
            virtual void traverse(EdgeContainer const& edgeContainer, boost::any const& data) const;
            virtual void traverse(TemplateEdge const& templateEdge, boost::any const& data) const;
            virtual void traverse(TemplateEdgeDestination const& templateEdgeDestination, boost::any const& data) const;
            virtual void traverse(Edge const& edge, boost::any const& data) const;
            virtual void traverse(EdgeDestination const& edgeDestination, boost::any const& data) const;
            virtual void traverse(OrderedAssignments const& orderedAssignments, boost::any const& data) const;
            virtual void traverse(Assignment const& assignment, boost::any const& data) const;
            virtual void traverse(storm::expressions::Expression const& expression, boost::any const& data) const;
        };
    }
}

