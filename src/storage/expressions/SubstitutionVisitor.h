#ifndef STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_

#include "src/storage/expressions/BaseExpression.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        class SubstitutionVisitor : public ExpressionVisitor {
        public:
            template<template<typename... Arguments> class MapType>
            Expression substitute(BaseExpression const* expression, MapType<std::string, Expression> const& identifierToExpressionMap);
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_ */