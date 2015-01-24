#include "src/logic/UnaryBooleanStateFormula.h"

namespace storm {
    namespace logic {
        UnaryBooleanStateFormula::UnaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& subformula) : UnaryStateFormula(subformula), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        bool UnaryBooleanStateFormula::isUnaryBooleanStateFormula() const {
            return true;
        }
        
        std::ostream& UnaryBooleanStateFormula::writeToStream(std::ostream& out) const {
            switch (operatorType) {
                case OperatorType::Not: out << "!("; break;
            }
            this->getSubformula().writeToStream(out);
            out << ")";
            return out;
        }
    }
}