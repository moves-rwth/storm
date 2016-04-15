#include "src/logic/UnaryBooleanStateFormula.h"

#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        UnaryBooleanStateFormula::UnaryBooleanStateFormula(OperatorType operatorType, std::shared_ptr<Formula const> const& subformula) : UnaryStateFormula(subformula), operatorType(operatorType) {
            STORM_LOG_THROW(this->getSubformula().hasQualitativeResult(), storm::exceptions::InvalidPropertyException, "Boolean formula must have subformulas with qualitative result.");
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