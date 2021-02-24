#include "storm/storage/jani/expressions/ArrayAccessIndexExpression.h"
#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/UnexpectedException.h"
namespace storm {
    namespace expressions {
        
        ArrayAccessIndexExpression::ArrayAccessIndexExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& lhs, std::shared_ptr<BaseExpression const> const& rhs) : BinaryExpression(manager, type, lhs, rhs) {
            // Assert correct types
            STORM_LOG_ASSERT(getFirstOperand()->getType().isIntegerType(), "The ArrayAccessIndexExpression should be integer type for first operand, instead of " << getFirstOperand()->getType() << ".");
            STORM_LOG_ASSERT(getFirstOperand()->isIntegerLiteralExpression(), "The ArrayAccessIndexExpression should be integer literal for first operand.");
            STORM_LOG_ASSERT(getSecondOperand()->getType().isIntegerType(), "The ArrayAccessIndexExpression should be integer type for second operand, instead of " << getSecondOperand()->getType() << ".");
//TODO            STORM_LOG_ASSERT(rhs->isBinaryRelationExpression(), "The ArrayAccessIndexExpression should have an ArrayAccessIndexExpression for the rhs operand");

        }

        ArrayAccessIndexExpression::ArrayAccessIndexExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& onehs) : BinaryExpression(manager, type, onehs, onehs) {
            // Assert correct types
            STORM_LOG_ASSERT(getFirstOperand()->getType().isIntegerType(), "The ArrayAccessIndexExpression should be integer type for first operand, instead of " << getFirstOperand()->getType() << ".");
            STORM_LOG_ASSERT(getFirstOperand()->isIntegerLiteralExpression(), "The ArrayAccessIndexExpression should be integer literal for first operand.");
        }

        std::shared_ptr<BaseExpression const> ArrayAccessIndexExpression::simplify() const {
            if (getFirstOperand() == getSecondOperand()) {
                return std::shared_ptr<BaseExpression const>(new ArrayAccessIndexExpression(getManager(), getType(), getFirstOperand()->simplify()));
            } else {
                return std::shared_ptr<BaseExpression const>(new ArrayAccessIndexExpression(getManager(), getType(), getFirstOperand()->simplify(), getSecondOperand()->simplify()));
            }
        }
        
        boost::any ArrayAccessIndexExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
            auto janiVisitor = dynamic_cast<JaniExpressionVisitor*>(&visitor);
            STORM_LOG_ASSERT(janiVisitor != nullptr, "Visitor of jani expression should be of type JaniVisitor.");
            STORM_LOG_THROW(janiVisitor != nullptr, storm::exceptions::UnexpectedException, "Visitor of jani expression should be of type JaniVisitor.");
            return janiVisitor->visit(*this, data);
        }
        
        void ArrayAccessIndexExpression::printToStream(std::ostream& stream) const {
            stream << "[" << *getFirstOperand() << "]";

            if (getFirstOperand() != getSecondOperand()) {
                stream << *getSecondOperand();
            }
        }
    }
}