/*
 * UnaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_

#include "UnaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a unary function expression of boolean type.
             */
            class UnaryBooleanFunctionExpression : public UnaryExpression {
            public:
                /*!
                 * An enum type specifying the different functions applicable.
                 */
                enum FunctionType {NOT};
                
                /*!
                 * Creates a unary boolean function expression tree node with the given child and function type.
                 *
                 * @param child The child of the node.
                 * @param functionType The operator that is to be applied to the two children.
                 */
                UnaryBooleanFunctionExpression(std::unique_ptr<BaseExpression>&& child, FunctionType functionType);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param unaryBooleanFunctionExpression The expression to copy.
                 */
                UnaryBooleanFunctionExpression(UnaryBooleanFunctionExpression const& unaryBooleanFunctionExpression);
                
                virtual std::unique_ptr<BaseExpression> clone() const override;
                
                virtual std::unique_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;

                /*!
                 * Retrieves the operator that is associated with this node.
                 *
                 * @param The operator that is associated with this node.
                 */
                FunctionType getFunctionType() const;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                                
            private:
                // The operator that is associated with this node.
                FunctionType functionType;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_UNARYBOOLEANFUNCTIONEXPRESSION_H_ */
