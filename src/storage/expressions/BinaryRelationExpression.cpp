#include "src/storage/expressions/BinaryRelationExpression.h"

#include <boost/variant.hpp>

#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        BinaryRelationExpression::BinaryRelationExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& firstOperand, std::shared_ptr<BaseExpression const> const& secondOperand, RelationType relationType) : BinaryExpression(manager, type, firstOperand, secondOperand), relationType(relationType) {
            // Intentionally left empty.
        }
        
        storm::expressions::OperatorType BinaryRelationExpression::getOperator() const {
            storm::expressions::OperatorType result = storm::expressions::OperatorType::Equal;
            switch (this->getRelationType()) {
                case RelationType::Equal: result = storm::expressions::OperatorType::Equal; break;
                case RelationType::NotEqual: result = storm::expressions::OperatorType::NotEqual; break;
                case RelationType::Less: result = storm::expressions::OperatorType::Less; break;
                case RelationType::LessOrEqual: result = storm::expressions::OperatorType::LessOrEqual; break;
                case RelationType::Greater: result = storm::expressions::OperatorType::Greater; break;
                case RelationType::GreaterOrEqual: result = storm::expressions::OperatorType::GreaterOrEqual; break;
            }
            return result;
        }
        
        bool BinaryRelationExpression::evaluateAsBool(Valuation const* valuation) const {
            STORM_LOG_THROW(this->hasBooleanType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
            
            double firstOperandEvaluated = this->getFirstOperand()->evaluateAsDouble(valuation);
            double secondOperandEvaluated = this->getSecondOperand()->evaluateAsDouble(valuation);
            bool result = false;
            switch (this->getRelationType()) {
                case RelationType::Equal: result = firstOperandEvaluated == secondOperandEvaluated; break;
                case RelationType::NotEqual: result = firstOperandEvaluated != secondOperandEvaluated; break;
                case RelationType::Greater: result = firstOperandEvaluated > secondOperandEvaluated; break;
                case RelationType::GreaterOrEqual: result = firstOperandEvaluated >= secondOperandEvaluated; break;
                case RelationType::Less: result = firstOperandEvaluated < secondOperandEvaluated; break;
                case RelationType::LessOrEqual: result = firstOperandEvaluated <= secondOperandEvaluated; break;
            }
            return result;
        }
        
        std::shared_ptr<BaseExpression const> BinaryRelationExpression::simplify() const {
            std::shared_ptr<BaseExpression const> firstOperandSimplified = this->getFirstOperand()->simplify();
            std::shared_ptr<BaseExpression const> secondOperandSimplified = this->getSecondOperand()->simplify();
            
            if (firstOperandSimplified->isLiteral() && secondOperandSimplified->isLiteral()) {
                boost::variant<int_fast64_t, double> firstOperandEvaluation;
                boost::variant<int_fast64_t, double> secondOperandEvaluation;
                
                if (firstOperandSimplified->hasIntegerType()) {
                    firstOperandEvaluation = firstOperandSimplified->evaluateAsInt();
                } else {
                    firstOperandEvaluation = firstOperandSimplified->evaluateAsDouble();
                }
                if (secondOperandSimplified->hasIntegerType()) {
                    secondOperandEvaluation = secondOperandSimplified->evaluateAsInt();
                } else {
                    secondOperandEvaluation = secondOperandSimplified->evaluateAsDouble();
                }
                
                bool truthValue = false;
                switch (this->getRelationType()) {
                    case RelationType::Equal: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) == (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                    case RelationType::NotEqual: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) != (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                    case RelationType::Greater: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) > (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                    case RelationType::GreaterOrEqual: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) >= (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                    case RelationType::Less: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) < (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                    case RelationType::LessOrEqual: truthValue = (firstOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(firstOperandEvaluation) : boost::get<double>(firstOperandEvaluation)) <= (secondOperandSimplified->hasIntegerType() ? boost::get<int_fast64_t>(secondOperandEvaluation) : boost::get<double>(secondOperandEvaluation)); break;
                }
                return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(this->getManager(), truthValue));
            }
            
            if (firstOperandSimplified.get() == this->getFirstOperand().get() && secondOperandSimplified.get() == this->getSecondOperand().get()) {
                return this->shared_from_this();
            } else {
                return std::shared_ptr<BaseExpression>(new BinaryRelationExpression(this->getManager(), this->getType(), firstOperandSimplified, secondOperandSimplified, this->getRelationType()));
            }
        }
        
        boost::any BinaryRelationExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        BinaryRelationExpression::RelationType BinaryRelationExpression::getRelationType() const {
            return this->relationType;
        }
        
        void BinaryRelationExpression::printToStream(std::ostream& stream) const {
            stream << "(" << *this->getFirstOperand();
            switch (this->getRelationType()) {
                case RelationType::Equal: stream << " = "; break;
                case RelationType::NotEqual: stream << " != "; break;
                case RelationType::Greater: stream << " > "; break;
                case RelationType::GreaterOrEqual: stream << " >= "; break;
                case RelationType::Less: stream << " < "; break;
                case RelationType::LessOrEqual: stream << " <= "; break;
            }
            stream << *this->getSecondOperand() << ")";
        }
    }
}