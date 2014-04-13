#ifndef STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_

#include <cstdint>
#include <memory>
#include <set>
#include <iostream>

#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace expressions {
        /*!
         * Each node in an expression tree has a uniquely defined type from this enum.
         */
        enum class ExpressionReturnType {Undefined, Bool, Int, Double};
        
        std::ostream& operator<<(std::ostream& stream, ExpressionReturnType const& enumValue);
        
        /*!
         * The base class of all expression classes.
         */
        class BaseExpression : public std::enable_shared_from_this<BaseExpression> {
        public:
            /*!
             * Constructs a base expression with the given return type.
             *
             * @param returnType The return type of the expression.
             */
            BaseExpression(ExpressionReturnType returnType);
            
            // Create default versions of constructors and assignments.
            BaseExpression(BaseExpression const&) = default;
            BaseExpression(BaseExpression&&) = default;
            BaseExpression& operator=(BaseExpression const&) = default;
            BaseExpression& operator=(BaseExpression&&) = default;
            
            // Make the destructor virtual (to allow destruction via base class pointer) and default it.
            virtual ~BaseExpression() = default;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting boolean value. If the return type of the expression is not a boolean
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The boolean value of the expression under the given valuation.
             */
            virtual bool evaluateAsBool(Valuation const& valuation) const;

            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting integer value. If the return type of the expression is not an integer
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The integer value of the expression under the given valuation.
             */
            virtual int_fast64_t evaluateAsInt(Valuation const& valuation) const;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting double value. If the return type of the expression is not a double
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The double value of the expression under the given valuation.
             */
            virtual double evaluateAsDouble(Valuation const& valuation) const;

            /*!
             * Retrieves whether the expression is constant, i.e., contains no variables or undefined constants.
             *
             * @return True iff the expression is constant.
             */
            virtual bool isConstant() const;
            
            /*!
             * Checks if the expression is equal to the boolean literal true.
             *
             * @return True iff the expression is equal to the boolean literal true.
             */
            virtual bool isTrue() const;
            
            /*!
             * Checks if the expression is equal to the boolean literal false.
             *
             * @return True iff the expression is equal to the boolean literal false.
             */
            virtual bool isFalse() const;
            
            /*!
             * Retrieves the set of all variables that appear in the expression.
             *
             * @return The set of all variables that appear in the expression.
             */
            virtual std::set<std::string> getVariables() const = 0;
            
            /*!
             * Retrieves the set of all constants that appear in the expression.
             *
             * @return The set of all constants that appear in the expression.
             */
            virtual std::set<std::string> getConstants() const = 0;
            
            /*!
             * Simplifies the expression according to some simple rules.
             *
             * @return A pointer to the simplified expression.
             */
            virtual std::shared_ptr<BaseExpression const> simplify() const = 0;
            
            /*!
             * Accepts the given visitor by calling its visit method.
             *
             * @param visitor The visitor that is to be accepted.
             */
            virtual void accept(ExpressionVisitor* visitor) const = 0;
            
            /*!
             * Retrieves whether the expression has a numerical return type, i.e., integer or double.
             *
             * @return True iff the expression has a numerical return type.
             */
            bool hasNumericalReturnType() const;
            
            /*!
             * Retrieves whether the expression has an integral return type, i.e., integer.
             *
             * @return True iff the expression has an integral return type.
             */
            bool hasIntegralReturnType() const;
            
            /*!
             * Retrieves whether the expression has a boolean return type.
             *
             * @return True iff the expression has a boolean return type.
             */
            bool hasBooleanReturnType() const;
            
            /*!
             * Retrieves a shared pointer to this expression.
             *
             * @return A shared pointer to this expression.
             */
            std::shared_ptr<BaseExpression const> getSharedPointer() const;
            
            /*!
             * Retrieves the return type of the expression.
             *
             * @return The return type of the expression.
             */
            ExpressionReturnType getReturnType() const;
            
            friend std::ostream& operator<<(std::ostream& stream, BaseExpression const& expression);
        protected:
            /*!
             * Prints the expression to the given stream.
             *
             * @param stream The stream to which to write the expression.
             */
            virtual void printToStream(std::ostream& stream) const = 0;
            
        private:
            // The return type of this expression.
            ExpressionReturnType returnType;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_ */