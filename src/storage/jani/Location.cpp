#include "src/storage/jani/Location.h"

namespace storm {
    namespace jani {
        
        Location::Location(std::string const& name) : name(name) {
            // Intentionally left empty.
        }
        
        std::string const& Location::getName() const {
            return name;
        }
        
    }
}