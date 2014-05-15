#ifndef STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        class LinearityCheckVisitor : public ExpressionVisitor {
        public:
            /*!
             * Creates a linearity check visitor.
             */
            LinearityCheckVisitor();
            
            /*!
             * Checks that the given expression is linear.
             *
             * @param expression The expression to check for linearity.
             */
            bool check(Expression const& expression);
            
            virtual void visit(IfThenElseExpression const* expression) override;
            virtual void visit(BinaryBooleanFunctionExpression const* expression) override;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BinaryRelationExpression const* expression) override;
            virtual void visit(VariableExpression const* expression) override;
            virtual void visit(UnaryBooleanFunctionExpression const* expression) override;
            virtual void visit(UnaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BooleanLiteralExpression const* expression) override;
            virtual void visit(IntegerLiteralExpression const* expression) override;
            virtual void visit(DoubleLiteralExpression const* expression) override;
            
        private:
            enum class LinearityStatus { NonLinear, LinearContainsVariables, LinearWithoutVariables };
            
            // A stack for communicating the results of the subexpressions.
            std::stack<LinearityStatus> resultStack;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_ */