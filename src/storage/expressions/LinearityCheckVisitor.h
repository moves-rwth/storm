#ifndef STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_

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
            
            virtual boost::any visit(IfThenElseExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BinaryRelationExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(VariableExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(BooleanLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(IntegerLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(RationalLiteralExpression const& expression, boost::any const& data) override;
            
        private:
            enum class LinearityStatus { NonLinear, LinearContainsVariables, LinearWithoutVariables };
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_LINEARITYCHECKVISITOR_H_ */