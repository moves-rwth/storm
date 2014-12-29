#ifndef STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        template<typename MapType>
        class SubstitutionVisitor : public ExpressionVisitor {
        public:
            /*!
             * Creates a new substitution visitor that uses the given map to replace identifiers.
             *
             * @param identifierToExpressionMap A mapping from identifiers to expressions.
             */
            SubstitutionVisitor(MapType const& identifierToExpressionMap);
            
            /*!
             * Substitutes the identifiers in the given expression according to the previously given map and returns the
             * resulting expression.
             *
             * @param expression The expression in which to substitute the identifiers.
             * @return The expression in which all identifiers in the key set of the previously given mapping are
             * substituted with the mapped-to expressions.
             */
            Expression substitute(Expression const& expression);
            
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
            // A mapping of identifier names to expressions with which they shall be replaced.
            MapType const& identifierToExpressionMap;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_ */