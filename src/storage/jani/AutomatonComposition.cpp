#include "src/storage/jani/AutomatonComposition.h"

namespace storm {
    namespace jani {
        
        AutomatonComposition::AutomatonComposition(std::string const& name) : name(name) {
            // Intentionally left empty.
        }
        
        boost::any AutomatonComposition::accept(CompositionVisitor& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::string const& AutomatonComposition::getAutomatonName() const {
            return name;
        }
        
        void AutomatonComposition::write(std::ostream& stream) const {
            stream << name;
        }
        
    }
}
