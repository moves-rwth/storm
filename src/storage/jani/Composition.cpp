#include "src/storage/jani/Composition.h"

namespace storm {
    namespace jani {
        
        std::ostream& operator<<(std::ostream& stream, Composition const& composition) {
            composition.write(stream);
            return stream;
        }
        
    }
}