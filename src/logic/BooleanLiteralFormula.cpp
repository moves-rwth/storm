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
        
        bool BooleanLiteralFormula::isPctlStateFormula() const {
            return true;
        }
        
        bool BooleanLiteralFormula::isPctlWithConditionalStateFormula() const {
            return true;
        }
        
        bool BooleanLiteralFormula::isLtlFormula() const {
            return true;
        }
        
        bool BooleanLiteralFormula::isPropositionalFormula() const {
            return true;
        }
        
        bool BooleanLiteralFormula::isBooleanLiteralFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> BooleanLiteralFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<BooleanLiteralFormula>(*this);
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