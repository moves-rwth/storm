#include <map>
#include <unordered_map>

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/SubstitutionVisitor.h"

namespace storm {
    namespace expressions {
        Expression::Expression(std::unique_ptr<BaseExpression>&& expressionPtr) : expressionPtr(std::move(expressionPtr)) {
            // Intentionally left empty.
        }
        
        template<template<typename... Arguments> class MapType>
        Expression Expression::substitute(MapType<std::string, Expression> const& identifierToExpressionMap) const {
            SubstitutionVisitor visitor;
            return visitor.substitute<MapType>(this->getBaseExpressionPointer(), identifierToExpressionMap);
        }

        Expression Expression::operator+(Expression const& other) {
            return Expression(this->getBaseExpression() + other.getBaseExpression());
        }
        
        BaseExpression const& Expression::getBaseExpression() const {
            return *this->expressionPtr;
        }
        
        BaseExpression const* Expression::getBaseExpressionPointer() const {
            return this->expressionPtr.get();
        }
        
        template Expression Expression::substitute<std::map>(std::map<std::string, storm::expressions::Expression> const&) const;
        template Expression Expression::substitute<std::unordered_map>(std::unordered_map<std::string, storm::expressions::Expression> const&) const;
    }
}
