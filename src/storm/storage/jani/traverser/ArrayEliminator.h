#pragma once


#include <boost/any.hpp>

#include "storm/storage/jani/traverser/JaniTraverser.h"

namespace storm {
    namespace jani {
        class ArrayEliminator {
/*
        public:
            void eliminate(Model& model);

        private:
        
            class ArrayVariableReplacer : public JaniTraverser {
            public:

                ArrayVariableReplacer() = default;
                virtual ~ArrayVariableReplacer() = default;
                std::unordered_map<storm::expressions::Variable, std::vector<reference_wrapper<storm::jani::Variable>> replace();

                virtual void traverse(Assignment const& assignment, boost::any const& data) const override;
                
            private:
                void std::unordered_map<storm::expressions::Variable, std::size_t>::getMaxSizes(Model& model);
                
            };
            
            
            
        };
        
        : public JaniTraverser {
        
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
        */
        };
    }
}

