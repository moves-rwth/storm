#pragma once

#include <vector>

#include "storm/storage/expressions/Expression.h"

#include "storm/storage/jani/EdgeDestination.h"

namespace storm {
    namespace jani {
        
        class TemplateEdge {
        public:
            TemplateEdge() = default;
            TemplateEdge(storm::expressions::Expression const& guard, std::vector<EdgeDestination> destinations = {});
            
        private:
            // The guard of the template edge.
            storm::expressions::Expression guard;
            
            // The destinations of the template edge.
            std::vector<EdgeDestination> destinations;
        };
        
    }
}
