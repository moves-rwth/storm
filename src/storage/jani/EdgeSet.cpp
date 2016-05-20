#include "src/storage/jani/EdgeSet.h"

namespace storm {
    namespace jani {
        
        EdgeSet::EdgeSet(std::vector<Edge> const& edges) : edges(edges) {
            // Intentionally left empty.
        }
        
        EdgeSet::iterator EdgeSet::begin() {
            return edges.begin();
        }
        
        EdgeSet::iterator EdgeSet::end() {
            return edges.end();
        }
        
        EdgeSet::const_iterator EdgeSet::begin() const {
            return edges.cbegin();
        }
        
        EdgeSet::const_iterator EdgeSet::end() const {
            return edges.cend();
        }
        
    }
}