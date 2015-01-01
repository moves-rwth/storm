#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_

#include <iostream>

namespace storm {
    namespace expressions {
        /*!
         * Each node in an expression tree has a uniquely defined type from this enum.
         */
        enum class ExpressionReturnType { Undefined = 0, Bool = 1, Int = 2, Double = 3};
        
        std::ostream& operator<<(std::ostream& stream, ExpressionReturnType const& enumValue);
    }
}

namespace std {
    // Provide a hashing operator, so we can put variables in unordered collections.
    template <>
    struct hash<storm::expressions::ExpressionReturnType> {
        std::size_t operator()(storm::expressions::ExpressionReturnType const& type) const {
            return static_cast<std::size_t>(type);
        }
    };
    
    // Provide a less operator, so we can put variables in ordered collections.
    template <>
    struct less<storm::expressions::ExpressionReturnType> {
        std::size_t operator()(storm::expressions::ExpressionReturnType const& type1, storm::expressions::ExpressionReturnType const& type2) const {
            return static_cast<std::size_t>(type1) < static_cast<std::size_t>(type2);
        }
    };
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_ */