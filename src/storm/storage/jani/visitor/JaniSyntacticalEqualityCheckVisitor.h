#pragma once

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/expressions/SyntacticalEqualityCheckVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"
#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"



namespace storm {
    namespace expressions {
        class JaniSyntacticalEqualityCheckVisitor: public SyntacticalEqualityCheckVisitor, public JaniExpressionVisitor  {
        public:
        /*!
         * Creates a new substitution visitor that uses the given map to replace variables.
         *
         * @param variableToExpressionMapping A mapping from variables to expressions.
         */
        JaniSyntacticalEqualityCheckVisitor();
        using SyntacticalEqualityCheckVisitor::visit;

        virtual boost::any visit(ValueArrayExpression const& expression, boost::any const& data) override;
        virtual boost::any visit(ValueArrayExpression::ValueArrayElements const& elements, boost::any const& data) override;
        virtual boost::any visit(ConstructorArrayExpression const& expression, boost::any const& data) override;
        virtual boost::any visit(ArrayAccessExpression const& expression, boost::any const& data) override;
        virtual boost::any visit(ArrayAccessIndexExpression const& expression, boost::any const& data) override;
        virtual boost::any visit(FunctionCallExpression const& expression, boost::any const& data) override;
        };
    }
}
