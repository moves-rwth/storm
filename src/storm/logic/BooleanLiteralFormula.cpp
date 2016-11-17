#include "storm/logic/BooleanLiteralFormula.h"

#include "storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        BooleanLiteralFormula::BooleanLiteralFormula(bool value) : value(value) {
            // Intenionally left empty.
        }
        
        bool BooleanLiteralFormula::isBooleanLiteralFormula() const {
            return true;
        }

        bool BooleanLiteralFormula::isTrueFormula() const {
            return value;
        }
        
        bool BooleanLiteralFormula::isFalseFormula() const {
            return !value;
        }
        
        boost::any BooleanLiteralFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
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
