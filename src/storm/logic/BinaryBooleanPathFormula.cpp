#include "storm/logic/BinaryBooleanPathFormula.h"

#include "storm/logic/FormulaVisitor.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        BinaryBooleanPathFormula::BinaryBooleanPathFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula) : BinaryPathFormula(leftSubformula, rightSubformula), operatorType(operatorType) {
            STORM_LOG_THROW(this->getLeftSubformula().isStateFormula() || this->getLeftSubformula().isPathFormula(), storm::exceptions::InvalidPropertyException, "Boolean path formula must have either path or state subformulas");
            STORM_LOG_THROW(this->getRightSubformula().isStateFormula() || this->getRightSubformula().isPathFormula(), storm::exceptions::InvalidPropertyException, "Boolean path formula must have either path or state subformulas");
        }
        
        bool BinaryBooleanPathFormula::isBinaryBooleanPathFormula() const {
            return true;
        }
        
        boost::any BinaryBooleanPathFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        BinaryBooleanPathFormula::OperatorType BinaryBooleanPathFormula::getOperator() const {
            return operatorType;
        }
        
        bool BinaryBooleanPathFormula::isAnd() const {
            return this->getOperator() == OperatorType::And;
        }
        
        bool BinaryBooleanPathFormula::isOr() const {
            return this->getOperator() == OperatorType::Or;
        }
                
        std::ostream& BinaryBooleanPathFormula::writeToStream(std::ostream& out) const {
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
