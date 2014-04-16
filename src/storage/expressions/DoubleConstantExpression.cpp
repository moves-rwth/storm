#include "src/storage/expressions/DoubleConstantExpression.h"
#include "src/exceptions/ExceptionMacros.h"

namespace storm {
    namespace expressions {
        DoubleConstantExpression::DoubleConstantExpression(std::string const& constantName) : ConstantExpression(ExpressionReturnType::Double, constantName) {
            // Intentionally left empty.
        }
        
        double DoubleConstantExpression::evaluateAsDouble(Valuation const* valuation) const {
            LOG_ASSERT(valuation != nullptr, "Evaluating expressions with unknowns without valuation.");
            return valuation->getDoubleValue(this->getConstantName());
        }
        
        std::shared_ptr<BaseExpression const> DoubleConstantExpression::simplify() const {
            return this->shared_from_this();
        }
        
        void DoubleConstantExpression::accept(ExpressionVisitor* visitor) const {
            visitor->visit(this);
        }
    }
}