#ifndef STORM_STORAGE_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_

#include "src/storage/expressions/BinaryExpression.h"

namespace storm {
    namespace expressions {
        class BinaryRelationExpression : public BinaryExpression {
        public:
            /*!
             * An enum type specifying the different relations applicable.
             */
            enum RelationType {EQUAL, NOT_EQUAL, LESS, LESS_OR_EQUAL, GREATER, GREATER_OR_EQUAL};
            
            /*!
             * Creates a binary relation expression with the given return type, operands and relation type.
             *
             * @param returnType The return type of the expression.
             * @param firstOperand The first operand of the expression.
             * @param secondOperand The second operand of the expression.
             * @param relationType The operator of the expression.
             */
            BinaryRelationExpression(ExpressionReturnType returnType, std::unique_ptr<BaseExpression>&& firstOperand, std::unique_ptr<BaseExpression>&& secondOperand, RelationType relationType);
            
            // Provide custom versions of copy construction and assignment.
            BinaryRelationExpression(BinaryRelationExpression const& other);
            BinaryRelationExpression& operator=(BinaryRelationExpression const& other);
            
            // Create default variants of move construction/assignment and virtual destructor.
            BinaryRelationExpression(BinaryRelationExpression&&) = default;
            BinaryRelationExpression& operator=(BinaryRelationExpression&&) = default;
            virtual ~BinaryRelationExpression() = default;
            
            // Override base class methods.
            virtual bool evaluateAsBool(Valuation const& valuation) const override;
            virtual std::unique_ptr<BaseExpression> simplify() const override;
            virtual void accept(ExpressionVisitor* visitor) const override;
            virtual std::unique_ptr<BaseExpression> clone() const override;
            
            /*!
             * Retrieves the relation associated with the expression.
             *
             * @return The relation associated with the expression.
             */
            RelationType getRelationType() const;
            
        private:
            // The relation type of the expression.
            RelationType relationType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BINARYRELATIONEXPRESSION_H_ */