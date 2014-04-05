#include "src/storage/expressions/VariableExpression.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        VariableExpression::VariableExpression(ExpressionReturnType returnType, std::string const& variableName) : BaseExpression(returnType), variableName(variableName) {
            // Intentionally left empty.
        }
        
        std::string const& VariableExpression::getVariableName() const {
            return this->variableName;
        }
        
        bool VariableExpression::evaluateAsBool(Valuation const& evaluation) const {
            LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as boolean: return type is not a boolean.");
            
            return evaluation.getBooleanValue(this->getVariableName());
        }

        int_fast64_t VariableExpression::evaluateAsInt(Valuation const& evaluation) const {
            LOG_THROW(this->hasIntegralReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as integer: return type is not an integer.");
            
            return evaluation.getIntegerValue(this->getVariableName());
        }
        
        double VariableExpression::evaluateAsDouble(Valuation const& evaluation) const {
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as double: return type is not a double.");
            
            switch (this->getReturnType()) {
                case ExpressionReturnType::Int: return static_cast<double>(evaluation.getIntegerValue(this->getVariableName())); break;
                case ExpressionReturnType::Double: evaluation.getDoubleValue(this->getVariableName()); break;
                default: break;
            }
            LOG_THROW(false, storm::exceptions::InvalidTypeException, "Type of variable is required to be numeric.");
        }
        
        std::set<std::string> VariableExpression::getVariables() const {
            return {this->getVariableName()};
        }
        
        std::set<std::string> VariableExpression::getConstants() const {
            return std::set<std::string>();
        }
        
        std::shared_ptr<BaseExpression const> VariableExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void VariableExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
        
        void VariableExpression::printToStream(std::ostream& stream) const {
            stream << this->getVariableName();
        }
    }
}