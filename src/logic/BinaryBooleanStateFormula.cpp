#include "src/logic/BinaryBooleanStateFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        BinaryBooleanStateFormula::BinaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : BinaryStateFormula(leftSubformula, rightSubformula), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        bool BinaryBooleanStateFormula::isBinaryBooleanStateFormula() const {
            return true;
        }
        
        boost::any BinaryBooleanStateFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        BinaryBooleanStateFormula::OperatorType BinaryBooleanStateFormula::getOperator() const {
            return operatorType;
        }
        
        bool BinaryBooleanStateFormula::isAnd() const {
            return this->getOperator() == OperatorType::And;
        }
        
        bool BinaryBooleanStateFormula::isOr() const {
            return this->getOperator() == OperatorType::Or;
        }
        
        std::shared_ptr<Formula> BinaryBooleanStateFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<BinaryBooleanStateFormula>(this->operatorType, this->getLeftSubformula().substitute(substitution), this->getRightSubformula().substitute(substitution));
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