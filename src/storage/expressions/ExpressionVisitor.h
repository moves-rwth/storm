#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_

#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/storage/expressions/BinaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BinaryRelationExpression.h"

namespace storm {
    namespace expressions {
        class ExpressionVisitor {
            virtual void visit(BinaryBooleanFunctionExpression const* expression) = 0;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) = 0;
            virtual void visit(BinaryRelationExpression const* expression) = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_ */