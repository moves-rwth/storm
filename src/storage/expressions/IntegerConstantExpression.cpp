#include "src/storage/expressions/IntegerConstantExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {
        IntegerConstantExpression::IntegerConstantExpression(std::string const& constantName) : ConstantExpression(ExpressionReturnType::Int, constantName) {
            // Intentionally left empty.
        }
        
        int_fast64_t IntegerConstantExpression::evaluateAsInt(Valuation const* valuation) const {
            LOG_ASSERT(valuation != nullptr, "Evaluating expressions with unknowns without valuation.");
            return valuation->getIntegerValue(this->getConstantName());
        }
        
        double IntegerConstantExpression::evaluateAsDouble(Valuation const* valuation) const {
            return static_cast<double>(this->evaluateAsInt(valuation));
        }
        
        std::shared_ptr<BaseExpression const> IntegerConstantExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void IntegerConstantExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
    }
}