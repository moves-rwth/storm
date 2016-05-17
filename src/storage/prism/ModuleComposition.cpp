#include "src/storage/prism/ModuleComposition.h"

namespace storm {
    namespace prism {
        
        ModuleComposition::ModuleComposition(std::string const& moduleName) : moduleName(moduleName) {
            // Intentionally left empty.
        }
        
        boost::any ModuleComposition::accept(CompositionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        std::string const& ModuleComposition::getModuleName() const {
            return moduleName;
        }
        
        void ModuleComposition::writeToStream(std::ostream& stream) const {
            stream << moduleName;
        }
    }
}