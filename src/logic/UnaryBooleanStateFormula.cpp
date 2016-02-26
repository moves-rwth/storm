#include "src/logic/UnaryBooleanStateFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        UnaryBooleanStateFormula::UnaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& subformula) : UnaryStateFormula(subformula), operatorType(operatorType) {
            // Intentionally left empty.
        }
        
        bool UnaryBooleanStateFormula::isUnaryBooleanStateFormula() const {
            return true;
        }

        boost::any UnaryBooleanStateFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        UnaryBooleanStateFormula::OperatorType UnaryBooleanStateFormula::getOperator() const {
            return operatorType;
        }
        
        bool UnaryBooleanStateFormula::isNot() const {
            return this->getOperator() == OperatorType::Not;
        }
        
        std::shared_ptr<Formula> UnaryBooleanStateFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::make_shared<UnaryBooleanStateFormula>(this->operatorType, this->getSubformula().substitute(substitution));
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