#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_

#include <memory>

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class Expression {
        public:
            Expression() = default;
            
            /*!
             * Creates an expression with the given underlying base expression.
             *
             * @param expressionPtr A pointer to the underlying base expression.
             */
            Expression(std::shared_ptr<BaseExpression const> const& expressionPtr);
            
            // Instantiate constructors and assignments with their default implementations.
            Expression(Expression const& other) = default;
            Expression& operator=(Expression const& other) = default;
            Expression(Expression&&) = default;
            Expression& operator=(Expression&&) = default;
            
            // Static factory methods to create atomic expression parts.
            static Expression createBooleanLiteral(bool value);
            static Expression createTrue();
            static Expression createFalse();
            static Expression createIntegerLiteral(int_fast64_t value);
            static Expression createDoubleLiteral(double value);
            static Expression createBooleanVariable(std::string const& variableName);
            static Expression createIntegerVariable(std::string const& variableName);
            static Expression createDoubleVariable(std::string const& variableName);
            static Expression createUndefinedVariable(std::string const& variableName);
            static Expression createBooleanConstant(std::string const& constantName);
            static Expression createIntegerConstant(std::string const& constantName);
            static Expression createDoubleConstant(std::string const& constantName);
            
            // Provide operator overloads to conveniently construct new expressions from other expressions.
            Expression operator+(Expression const& other) const;
            Expression operator-(Expression const& other) const;
            Expression operator-() const;
            Expression operator*(Expression const& other) const;
            Expression operator/(Expression const& other) const;
            Expression operator&&(Expression const& other) const;
            Expression operator||(Expression const& other) const;
            Expression operator!() const;
            Expression operator==(Expression const& other) const;
            Expression operator!=(Expression const& other) const;
            Expression operator>(Expression const& other) const;
            Expression operator>=(Expression const& other) const;
            Expression operator<(Expression const& other) const;
            Expression operator<=(Expression const& other) const;
            
            Expression ite(Expression const& thenExpression, Expression const& elseExpression);
            Expression implies(Expression const& other) const;
            Expression iff(Expression const& other) const;
            
            Expression floor() const;
            Expression ceil() const;

            static Expression minimum(Expression const& lhs, Expression const& rhs);
            static Expression maximum(Expression const& lhs, Expression const& rhs);
            
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
             * Substitutes all occurrences of identifiers with different names given by a mapping.
             *
             * @param identifierToIdentifierMap A mapping from identifiers to identifiers they are substituted with.
             * @return An expression in which all identifiers in the key set of the mapping are replaced by the
             * identifiers they are mapped to.
             */
            template<template<typename... Arguments> class MapType>
            Expression substitute(MapType<std::string, std::string> const& identifierToIdentifierMap) const;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting boolean value. If the return type of the expression is not a boolean
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The boolean value of the expression under the given valuation.
             */
            bool evaluateAsBool(Valuation const* valuation = nullptr) const;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting integer value. If the return type of the expression is not an integer
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The integer value of the expression under the given valuation.
             */
            int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting double value. If the return type of the expression is not a double
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The double value of the expression under the given valuation.
             */
            double evaluateAsDouble(Valuation const* valuation = nullptr) const;
            
            /*!
             * Simplifies the expression according to some basic rules.
             *
             * @return The simplified expression.
             */
            Expression simplify();
            
            /*!
             * Retrieves whether the expression is constant, i.e., contains no variables or undefined constants.
             *
             * @return True iff the expression is constant.
             */
            bool isConstant() const;
            
            /*!
             * Checks if the expression is equal to the boolean literal true.
             *
             * @return True iff the expression is equal to the boolean literal true.
             */
            bool isTrue() const;
            
            /*!
             * Checks if the expression is equal to the boolean literal false.
             *
             * @return True iff the expression is equal to the boolean literal false.
             */
            bool isFalse() const;
            
            /*!
             * Retrieves the set of all variables that appear in the expression.
             *
             * @return The set of all variables that appear in the expression.
             */
            std::set<std::string> getVariables() const;
            
            /*!
             * Retrieves the set of all constants that appear in the expression.
             *
             * @return The set of all constants that appear in the expression.
             */
            std::set<std::string> getConstants() const;
            
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
            std::shared_ptr<BaseExpression const> const& getBaseExpressionPointer() const;

            /*!
             * Retrieves the return type of the expression.
             *
             * @return The return type of the expression.
             */
            ExpressionReturnType getReturnType() const;
            
            /*!
             * Retrieves whether the expression has a numerical return type, i.e., integer or double.
             *
             * @return True iff the expression has a numerical return type.
             */
            bool hasNumericalReturnType() const;
            
            /*!
             * Retrieves whether the expression has a boolean return type.
             *
             * @return True iff the expression has a boolean return type.
             */
            bool hasBooleanReturnType() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Expression const& expression);

        private:
            // A pointer to the underlying base expression.
            std::shared_ptr<BaseExpression const> expressionPtr;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_ */