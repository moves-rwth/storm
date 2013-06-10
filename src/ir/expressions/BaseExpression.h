/*
 * BaseExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "src/exceptions/ExpressionEvaluationException.h"
#include "src/exceptions/NotImplementedException.h"
#include "ExpressionVisitor.h"

namespace storm {
    
    // Forward-declare VariableState.
	namespace parser {
		namespace prism {
			class VariableState;
		} // namespace prismparser
	} // namespace parser
    
    namespace ir {
        namespace expressions {
            
            /*!
             * The base class for all expressions.
             */
            class BaseExpression {
                
            public:
                /*!
                 * Each node in an expression tree has a uniquely defined type from this enum.
                 */
                enum ReturnType {undefined, bool_, int_, double_};
                
                /*!
                 * Creates an expression with undefined type.
                 */
                BaseExpression();
                
                /*!
                 * Creates an expression with the given type.
                 *
                 * @param type The type of the expression.
                 */
                BaseExpression(ReturnType type);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param baseExpression The expression to copy.
                 */
                BaseExpression(BaseExpression const& baseExpression);
                
                /*!
                 * Destructor.
                 */
                virtual ~BaseExpression();
                
                /*!
                 * Copies the expression tree underneath (including) the current node and performs the provided renaming.
                 *
                 * @param renaming A mapping from identifier names to strings they are to be replaced with.
                 * @param variableState An object knowing about the global variable state.
                 */
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const = 0;
                
                /*!
                 * Retrieves the value of the expression as an integer given the provided variable valuation.
                 *
                 * @param variableValues The variable valuation under which to evaluate the expression. If set to null,
                 * constant expressions can be evaluated without variable values. However, upon encountering a variable
                 * expression an expression is thrown, because evaluation is impossible without the variable values then.
                 * @return The value of the expression as an integer.
                 */
                virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
                
                /*!
                 * Retrieves the value of the expression as a boolean given the provided variable valuation.
                 *
                 * @param variableValues The variable valuation under which to evaluate the expression. If set to null,
                 * constant expressions can be evaluated without variable values. However, upon encountering a variable
                 * expression an expression is thrown, because evaluation is impossible without the variable values then.
                 * @return The value of the expression as a boolean.
                 */
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
                
                /*!
                 * Retrieves the value of the expression as a double given the provided variable valuation.
                 *
                 * @param variableValues The variable valuation under which to evaluate the expression. If set to null,
                 * constant expressions can be evaluated without variable values. However, upon encountering a variable
                 * expression an expression is thrown, because evaluation is impossible without the variable values then.
                 * @return The value of the expression as a double.
                 */
                virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
                
                /*!
                 * Acceptor method for visitor pattern.
                 *
                 * @param visitor The visitor that is supposed to visit each node of the expression tree.
                 */
                virtual void accept(ExpressionVisitor* visitor);
                
                /*!
                 * Retrieves a string representation of the expression tree underneath the current node.
                 *
                 * @return A string representation of the expression tree underneath the current node.
                 */
                virtual std::string toString() const = 0;
                
                /*!
                 * Retrieves a string representation of the type to which this node evaluates.
                 *
                 * @return A string representation of the type to which this node evaluates.
                 */
                std::string getTypeName() const;
                
                /*!
                 * Retrieves the type to which the node evaluates.
                 *
                 * @return The type to which the node evaluates.
                 */
                ReturnType getType() const;
                
            private:
                // The type to which this node evaluates.
                ReturnType type;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BASEEXPRESSION_H_ */
