#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_

#include <boost/any.hpp>

namespace storm {
    namespace expressions {
        // Forward-declare all expression classes.
        class IfThenElseExpression;
        class BinaryBooleanFunctionExpression;
        class BinaryNumericalFunctionExpression;
        class BinaryRelationExpression;
        class VariableExpression;
        class UnaryBooleanFunctionExpression;
        class UnaryNumericalFunctionExpression;
        class BooleanLiteralExpression;
        class IntegerLiteralExpression;
        class DoubleLiteralExpression;
        
        class ExpressionVisitor {
        public:
            virtual boost::any visit(IfThenElseExpression const& expression) = 0;
            virtual boost::any visit(BinaryBooleanFunctionExpression const& expression) = 0;
            virtual boost::any visit(BinaryNumericalFunctionExpression const& expression) = 0;
            virtual boost::any visit(BinaryRelationExpression const& expression) = 0;
            virtual boost::any visit(VariableExpression const& expression) = 0;
            virtual boost::any visit(UnaryBooleanFunctionExpression const& expression) = 0;
            virtual boost::any visit(UnaryNumericalFunctionExpression const& expression) = 0;
            virtual boost::any visit(BooleanLiteralExpression const& expression) = 0;
            virtual boost::any visit(IntegerLiteralExpression const& expression) = 0;
            virtual boost::any visit(DoubleLiteralExpression const& expression) = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_ */