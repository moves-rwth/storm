#include "src/logic/BinaryBooleanStateFormula.h"

namespace storm {
    namespace logic {
        BinaryBooleanStateFormula::BinaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula> const& leftSubformula, std::shared_ptr<Formula> const& rightSubformula) : BinaryStateFormula(leftSubformula, rightSubformula), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        bool BinaryBooleanStateFormula::isBinaryBooleanStateFormula() const {
            return true;
        }
        
        std::ostream& BinaryBooleanStateFormula::writeToStream(std::ostream& out) const {
            out << "(";
            this->getLeftSubformula().writeToStream(out);
            switch (operatorType) {
                case OperatorType::And: out << " & "; break;
                case OperatorType::Or: out << " | "; break;
            }
            this->getRightSubformula().writeToStream(out);
            out << ")";
            return out;
        }
    }
}