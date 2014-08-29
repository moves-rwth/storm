#ifndef STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/SimpleValuation.h"

namespace storm {
    namespace expressions {
        class LinearCoefficientVisitor : public ExpressionVisitor {
        public:
            /*!
             * Creates a linear coefficient visitor.
             */
            LinearCoefficientVisitor() = default;
            
            /*!
             * Computes the (double) coefficients of all identifiers appearing in the expression if the expression
             * was rewritten as a sum of atoms.. If the expression is not linear, an exception is thrown.
             *
             * @param expression The expression for which to compute the coefficients.
             * @return A pair consisting of a mapping from identifiers to their coefficients and the coefficient of
             * the constant atom.
             */
            std::pair<SimpleValuation, double> getLinearCoefficients(Expression const& expression);
            
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
            std::stack<std::pair<SimpleValuation, double>> resultStack;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_ */