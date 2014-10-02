#ifndef STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONVISITORBASE_H_
#define STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONVISITORBASE_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        class IdentifierSubstitutionVisitorBase : public ExpressionVisitor {
        public:
            /*!
             * Substitutes the identifiers in the given expression according to the previously given map and returns the
             * resulting expression.
             *
             * @param expression The expression in which to substitute the identifiers.
             * @return The expression in which all identifiers in the key set of the previously given mapping are
             * substituted with the mapped-to expressions.
             */
            Expression substitute(Expression const& expression);
            
            virtual void visit(IfThenElseExpression const* expression) override;
            virtual void visit(BinaryBooleanFunctionExpression const* expression) override;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BinaryRelationExpression const* expression) override;
            virtual void visit(UnaryBooleanFunctionExpression const* expression) override;
            virtual void visit(UnaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BooleanLiteralExpression const* expression) override;
            virtual void visit(IntegerLiteralExpression const* expression) override;
            virtual void visit(DoubleLiteralExpression const* expression) override;
            
        protected:
            // A stack of expression used to pass the results to the higher levels.
            std::stack<std::shared_ptr<BaseExpression const>> expressionStack;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONVISITORBASE_H_ */