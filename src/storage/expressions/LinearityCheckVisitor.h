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
            
        private:
            enum class LinearityStatus { NonLinear, LinearContainsVariables, LinearWithoutVariables };
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_ */