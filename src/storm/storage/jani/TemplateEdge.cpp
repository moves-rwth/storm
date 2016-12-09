#include "storm/storage/jani/TemplateEdge.h"

namespace storm {
    namespace jani {
        
        TemplateEdge::TemplateEdge(storm::expressions::Expression const& guard, std::vector<std::shared_ptr<TemplateEdgeDestination const>> destinations) : guard(guard), destinations(destinations) {
            // Intentionally left empty.
        }
        
        
    }
}
