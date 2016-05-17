#include "src/storage/prism/ModuleComposition.h"

namespace storm {
    namespace prism {
        
        ModuleComposition::ModuleComposition(std::string const& moduleName) : moduleName(moduleName) {
            // Intentionally left empty.
        }
        
        void ModuleComposition::writeToStream(std::ostream& stream) const {
            stream << moduleName;
        }
    }
}