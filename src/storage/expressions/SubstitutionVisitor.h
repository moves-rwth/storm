#ifndef STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        template<template<typename... Arguments> class MapType>
        class SubstitutionVisitor : public ExpressionVisitor {
        public:
            /*!
             * Creates a new substitution visitor that uses the given map to replace identifiers.
             *
             * @param identifierToExpressionMap A mapping from identifiers to expressions.
             */
            SubstitutionVisitor(MapType<std::string, Expression> const& identifierToExpressionMap);
            
            /*!
             * Substitutes the identifiers in the given expression according to the previously given map and returns the
             * resulting expression.
             *
             * @param expression The expression in which to substitute the identifiers.
             * @return The expression in which all identifiers in the key set of the previously given mapping are
             * substituted with the mapped-to expressions.
             */
            Expression substitute(BaseExpression const* expression);
            
            virtual void visit(BinaryBooleanFunctionExpression const* expression) override;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BinaryRelationExpression const* expression) override;
            virtual void visit(BooleanConstantExpression const* expression) override;
            virtual void visit(DoubleConstantExpression const* expression) override;
            virtual void visit(IntegerConstantExpression const* expression) override;
            virtual void visit(VariableExpression const* expression) override;
            virtual void visit(UnaryBooleanFunctionExpression const* expression) override;
            virtual void visit(UnaryNumericalFunctionExpression const* expression) override;
            virtual void visit(BooleanLiteralExpression const* expression) override;
            virtual void visit(IntegerLiteralExpression const* expression) override;
            virtual void visit(DoubleLiteralExpression const* expression) override;
            
        private:
            // A stack of expression used to pass the results to the higher levels.
            std::stack<std::shared_ptr<BaseExpression const>> expressionStack;
            
            // A mapping of identifier names to expressions with which they shall be replaced.
            MapType<std::string, Expression> const& identifierToExpressionMap;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_ */