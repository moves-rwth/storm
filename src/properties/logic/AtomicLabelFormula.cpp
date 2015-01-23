#include "src/properties/logic/AtomicLabelFormula.h"

namespace storm {
    namespace logic {
        bool AtomicLabelFormula::isAtomicLabelFormula() const {
            return true;
        }
        
        std::string const& AtomicLabelFormula::getLabel() const {
            return label;
        }
        
        std::ostream& AtomicLabelFormula::writeToStream(std::ostream& out) const {
            out << "\"" << label << "\"";
            return out;
        }
    }
}