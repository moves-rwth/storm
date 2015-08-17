#include "src/storage/expressions/DoubleLiteralExpression.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        DoubleLiteralExpression::DoubleLiteralExpression(ExpressionManager const& manager, double value) : BaseExpression(manager, manager.getRationalType()), value(value) {
            // Intentionally left empty.
        }
        
        double DoubleLiteralExpression::evaluateAsDouble(Valuation const* valuation) const {
            return this->getValue();
        }
        
        bool DoubleLiteralExpression::isLiteral() const {
            return true;
        }
        
        void DoubleLiteralExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
            return;
		}
        
        std::shared_ptr<BaseExpression const> DoubleLiteralExpression::simplify() const {
            return this->shared_from_this();
        }
        
        boost::any DoubleLiteralExpression::accept(ExpressionVisitor& visitor) const {
            return visitor.visit(*this);
        }
        
        double DoubleLiteralExpression::getValue() const {
            return this->value;
        }
        
        void DoubleLiteralExpression::printToStream(std::ostream& stream) const {
            stream << this->getValue();
        }
    }
}