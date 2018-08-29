#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/traverser/JaniTraverser.h"

namespace storm {
    namespace jani {
        class AssignmentsFinder : public JaniTraverser {
        public:
            
            struct ResultType {
                bool hasLocationAssignment, hasEdgeAssignment, hasEdgeDestinationAssignment;
            };
            
            AssignmentsFinder() = default;
            
            ResultType find(Model const& model, Variable const& variable);
            
            virtual ~AssignmentsFinder() = default;
            
            virtual void traverse(Location const& location, boost::any const& data) const override;
            virtual void traverse(TemplateEdge const& templateEdge, boost::any const& data) const override;
            virtual void traverse(TemplateEdgeDestination const& templateEdgeDestination, boost::any const& data) const override;
        };
    }
}

