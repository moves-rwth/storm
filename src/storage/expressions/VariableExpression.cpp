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
        
        bool VariableExpression::evaluateAsBool(Valuation const* valuation) const {
            LOG_ASSERT(valuation != nullptr, "Evaluating expressions with unknowns without valuation.");
            LOG_THROW(this->hasBooleanReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as boolean: return type is not a boolean.");
            
            return valuation->getBooleanValue(this->getVariableName());
        }

        int_fast64_t VariableExpression::evaluateAsInt(Valuation const* valuation) const {
            LOG_ASSERT(valuation != nullptr, "Evaluating expressions with unknowns without valuation.");
            LOG_THROW(this->hasIntegralReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as integer: return type is not an integer.");
            
            return valuation->getIntegerValue(this->getVariableName());
        }
        
        double VariableExpression::evaluateAsDouble(Valuation const* valuation) const {
            LOG_ASSERT(valuation != nullptr, "Evaluating expressions with unknowns without valuation.");
            LOG_THROW(this->hasNumericalReturnType(), storm::exceptions::InvalidTypeException, "Cannot evaluate expression as double: return type is not a double.");
            
            switch (this->getReturnType()) {
                case ExpressionReturnType::Int: return static_cast<double>(valuation->getIntegerValue(this->getVariableName())); break;
                case ExpressionReturnType::Double: valuation->getDoubleValue(this->getVariableName()); break;
                default: break;
            }
            LOG_ASSERT(false, "Type of variable is required to be numeric.");
            
            // Silence warning. This point can never be reached.
            return 0;
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