#include <map>
#include <unordered_map>

#include "src/storage/expressions/SubstitutionVisitor.h"

namespace storm {
    namespace expressions  {
        template<template<typename... Arguments> class MapType>
        Expression SubstitutionVisitor::substitute(BaseExpression const* expression, MapType<std::string, Expression> const& identifierToExpressionMap) {
            return Expression();
        }
        
        // Explicitly instantiate substitute with map and unordered_map.
        template Expression SubstitutionVisitor::substitute<std::map>(BaseExpression const* expression, std::map<std::string, Expression> const& identifierToExpressionMap);
        template Expression SubstitutionVisitor::substitute<std::unordered_map>(BaseExpression const* expression, std::unordered_map<std::string, Expression> const& identifierToExpressionMap);
    }
}
