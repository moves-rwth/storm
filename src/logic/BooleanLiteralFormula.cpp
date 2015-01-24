#include "src/logic/BooleanLiteralFormula.h"

namespace storm {
    namespace logic {
        BooleanLiteralFormula::BooleanLiteralFormula(bool value) : value(value) {
            // Intenionally left empty.
        }
        
        bool BooleanLiteralFormula::isTrueFormula() const {
            return value;
        }
        
        bool BooleanLiteralFormula::isFalseFormula() const {
            return !value;
        }
        
        bool BooleanLiteralFormula::isPropositionalFormula() const {
            return true;
        }
        
        bool BooleanLiteralFormula::isBooleanLiteralFormula() const {
            return true;
        }
        
        std::ostream& BooleanLiteralFormula::writeToStream(std::ostream& out) const {
            if (value) {
                out << "true";
            } else {
                out << "false";
            }
            return out;
        }
    }
}