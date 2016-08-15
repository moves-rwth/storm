#include "src/storage/expressions/DoubleLiteralExpression.h"
#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/ExpressionVisitor.h"

#include "src/utility/constants.h"

namespace storm {
    namespace expressions {
        DoubleLiteralExpression::DoubleLiteralExpression(ExpressionManager const& manager, double value) : BaseExpression(manager, manager.getRationalType()), value(storm::utility::convertNumber<storm::RationalNumber>(value)) {
            // Intentionally left empty.
        }
        
        DoubleLiteralExpression::DoubleLiteralExpression(ExpressionManager const& manager, std::string const& valueAsString) : BaseExpression(manager, manager.getRationalType()), value(storm::utility::convertNumber<storm::RationalNumber>(valueAsString)) {
            // Intentionally left empty.
        }
        
        double DoubleLiteralExpression::evaluateAsDouble(Valuation const* valuation) const {
            return this->getValueAsDouble();
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
        
        double DoubleLiteralExpression::getValueAsDouble() const {
            return storm::utility::convertNumber<double>(this->value);
        }
        
        storm::RationalNumber DoubleLiteralExpression::getValue() const {
            return this->value;
        }
        
        void DoubleLiteralExpression::printToStream(std::ostream& stream) const {
            stream << this->getValue();
        }
    }
}