#ifndef STORM_STORAGE_EXPRESSIONS_TYPECHECKVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_TYPECHECKVISITOR_H_

#include <stack>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        template<typename MapType>
        class TypeCheckVisitor : public ExpressionVisitor {
        public:
            /*!
             * Creates a new type check visitor that uses the given map to check the types of variables and constants.
             *
             * @param identifierToTypeMap A mapping from identifiers to expressions.
             */
            TypeCheckVisitor(MapType const& identifierToTypeMap);
            
            /*!
             * Checks that the types of the identifiers in the given expression match the ones in the previously given
             * map.
             *
             * @param expression The expression in which to check the types.
             */
            void check(BaseExpression const* expression);
            
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
            // A mapping of identifier names to expressions with which they shall be replaced.
            MapType const& identifierToTypeMap;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_TYPECHECKVISITOR_H_ */