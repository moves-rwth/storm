#pragma once

#include "src/adapters/CarlAdapter.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Expressions.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace expressions {
        
        template<typename RationalNumberType>
        class ToRationalNumberVisitor : public ExpressionVisitor {
        public:
            ToRationalNumberVisitor();
            
            RationalNumberType toRationalNumber(Expression const& expression);
            
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
