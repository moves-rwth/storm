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
        };
    }
}
