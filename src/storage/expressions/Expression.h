#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_

#include <functional>

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class Expression {
        public:
            Expression() = default;
            
            // Static factory methods to create atomic expression parts.
            
            // Virtual operator overloading.
            Expression operator+(Expression const& other) const;
            Expression operator-(Expression const& other) const;
            Expression operator-() const;
            Expression operator*(Expression const& other) const;
            Expression operator/(Expression const& other) const;
            Expression operator&(Expression const& other) const;
            Expression operator|(Expression const& other) const;
            Expression operator~() const;
            
            Expression equals(Expression const& other) const;
            Expression notEquals(Expression const& other) const;
            Expression greater(Expression const& other) const;
            Expression greaterOrEqual(Expression const& other) const;
            Expression less(Expression const& other) const;
            Expression lessOrEqual(Expression const& other) const;
            Expression minimum(Expression const& other) const;
            Expression maximum(Expression const& other) const;
            Expression mod(Expression const& other) const;
            Expression floor() const;
            Expression ceil() const;
            
            /*!
             * Substitutes all occurrences of identifiers according to the given map. Note that this substitution is
             * done simultaneously, i.e., identifiers appearing in the expressions that were "plugged in" are not
             * substituted.
             *
             * @param identifierToExpressionMap A mapping from identifiers to the expression they are substituted with.
             * @return An expression in which all identifiers in the key set of the mapping are replaced by the
             * expression they are mapped to.
             */
            template<template<typename... Arguments> class MapType>
            Expression substitute(MapType<std::string, Expression> const& identifierToExpressionMap) const;
            
            /*!
             * Retrieves the return type of the expression.
             *
             * @return The return type of the expression.
             */
            ExpressionReturnType getReturnType() const;
            
        private:
            /*!
             * Creates an expression with the given underlying base expression.
             *
             * @param expressionPtr A pointer to the underlying base expression.
             */
            Expression(std::unique_ptr<BaseExpression>&& expressionPtr);
            
            /*!
             * Retrieves the base expression underlying this expression object. Note that prior to calling this, the
             * expression object must be properly initialized.
             *
             * @return A reference to the underlying base expression.
             */
            BaseExpression const& getBaseExpression() const;
            
            /*!
             * Retrieves a pointer to the base expression underlying this expression object.
             *
             * @return A pointer to the underlying base expression.
             */
            BaseExpression const* getBaseExpressionPointer() const;
            
            // A pointer to the underlying base expression.
            std::unique_ptr<BaseExpression> expressionPtr;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_ */