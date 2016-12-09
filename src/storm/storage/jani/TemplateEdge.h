#pragma once

#include <vector>
#include <memory>

#include "storm/storage/expressions/Expression.h"

#include "storm/storage/jani/TemplateEdgeDestination.h"

namespace storm {
    namespace jani {
        
        class TemplateEdge {
        public:
            TemplateEdge() = default;
            TemplateEdge(storm::expressions::Expression const& guard, std::vector<std::shared_ptr<TemplateEdgeDestination const>> destinations = {});
            
        private:
            // The guard of the template edge.
            storm::expressions::Expression guard;
            
            // The destinations of the template edge.
            std::vector<std::shared_ptr<TemplateEdgeDestination const>> destinations;
        };
        
    }
}
