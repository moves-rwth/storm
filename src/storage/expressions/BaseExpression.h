#ifndef STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_

#include <cstdint>
#include <memory>
#include <set>
#include <map>
#include <iostream>

#include "src/storage/expressions/ExpressionReturnType.h"
#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/OperatorType.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace expressions {        
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
            BaseExpression& operator=(BaseExpression const&) = default;
#ifndef WINDOWS
            BaseExpression(BaseExpression&&) = default;
            BaseExpression& operator=(BaseExpression&&) = default;
#endif
            
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
            virtual bool evaluateAsBool(Valuation const* valuation = nullptr) const;

            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting integer value. If the return type of the expression is not an integer
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The integer value of the expression under the given valuation.
             */
            virtual int_fast64_t evaluateAsInt(Valuation const* valuation = nullptr) const;
            
            /*!
             * Evaluates the expression under the valuation of unknowns (variables and constants) given by the
             * valuation and returns the resulting double value. If the return type of the expression is not a double
             * an exception is thrown.
             *
             * @param valuation The valuation of unknowns under which to evaluate the expression.
             * @return The double value of the expression under the given valuation.
             */
            virtual double evaluateAsDouble(Valuation const* valuation = nullptr) const;

            /*!
             * Returns the arity of the expression.
             *
             * @return The arity of the expression.
             */
            virtual uint_fast64_t getArity() const;
            
            /*!
             * Retrieves the given operand from the expression.
             *
             * @param operandIndex The index of the operand to retrieve. This must be lower than the arity of the expression.
             * @return The operand at the given index.
             */
            virtual std::shared_ptr<BaseExpression const> getOperand(uint_fast64_t operandIndex) const;
            
            /*!
             * Retrieves the identifier associated with this expression. This is only legal to call if the expression
             * is a variable.
             *
             * @return The identifier associated with this expression.
             */
            virtual std::string const& getIdentifier() const;
            
            /*!
             * Retrieves the operator of a function application. This is only legal to call if the expression is
             * function application.
             *
             * @return The operator associated with the function application.
             */
            virtual OperatorType getOperator() const;
            
            /*!
             * Retrieves whether the expression contains a variable.
             *
             * @return True iff the expression contains a variable.
             */
            virtual bool containsVariables() const;
            
            /*!
             * Retrieves whether the expression is a literal.
             *
             * @return True iff the expression is a literal.
             */
            virtual bool isLiteral() const;
            
            /*!
             * Retrieves whether the expression is a variable.
             *
             * @return True iff the expression is a variable.
             */
            virtual bool isVariable() const;
            
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
             * Checks if the expression is a function application (of any sort).
             *
             * @return True iff the expression is a function application.
             */
            virtual bool isFunctionApplication() const;
            
            /*!
             * Retrieves the set of all variables that appear in the expression.
             *
             * @return The set of all variables that appear in the expression.
             */
            virtual std::set<std::string> getVariables() const = 0;

			/*!
			* Retrieves the mapping of all variables that appear in the expression to their return type.
			*
			* @return The mapping of all variables that appear in the expression to their return type.
			*/
			virtual std::map<std::string, ExpressionReturnType> getVariablesAndTypes() const = 0;

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
            virtual boost::any accept(ExpressionVisitor& visitor) const = 0;
            
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