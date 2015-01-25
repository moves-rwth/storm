#include "src/logic/AtomicLabelFormula.h"

namespace storm {
    namespace logic {
        AtomicLabelFormula::AtomicLabelFormula(std::string const& label) : label(label) {
            // Intentionally left empty.
        }
        
        bool AtomicLabelFormula::isAtomicLabelFormula() const {
            return true;
        }
        
        bool AtomicLabelFormula::isPctlStateFormula() const {
            return true;
        }
    
        bool AtomicLabelFormula::isLtlFormula() const {
            return true;
        }
        
        bool AtomicLabelFormula::isPropositionalFormula() const {
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