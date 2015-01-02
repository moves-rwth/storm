#ifndef STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/Valuation.h"

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
            std::pair<Valuation, double> getLinearCoefficients(Expression const& expression);
            
            virtual boost::any visit(IfThenElseExpression const& expression) override;
            virtual boost::any visit(BinaryBooleanFunctionExpression const& expression) override;
            virtual boost::any visit(BinaryNumericalFunctionExpression const& expression) override;
            virtual boost::any visit(BinaryRelationExpression const& expression) override;
            virtual boost::any visit(VariableExpression const& expression) override;
            virtual boost::any visit(UnaryBooleanFunctionExpression const& expression) override;
            virtual boost::any visit(UnaryNumericalFunctionExpression const& expression) override;
            virtual boost::any visit(BooleanLiteralExpression const& expression) override;
            virtual boost::any visit(IntegerLiteralExpression const& expression) override;
            virtual boost::any visit(DoubleLiteralExpression const& expression) override;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARCOEFFICIENTVISITOR_H_ */