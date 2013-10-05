/*
 * UnaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_UNARYFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_UNARYFUNCTIONEXPRESSION_H_

#include "UnaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a unary function expression of numerical type.
             */
            class UnaryNumericalFunctionExpression : public UnaryExpression {
            public:
                /*!
                 * An enum type specifying the different functions applicable.
                 */
                enum FunctionType {MINUS};
                
                /*!
                 * Creates a unary numerical function expression tree node with the given child and function type.
                 *
                 * @param child The child of the node.
                 * @param functionType The operator that is to be applied to the two children.
                 */
                UnaryNumericalFunctionExpression(ReturnType type, std::shared_ptr<BaseExpression> child, FunctionType functionType);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param unaryNumericalFunctionExpression The expression to copy.
                 */
                UnaryNumericalFunctionExpression(UnaryNumericalFunctionExpression const& unaryNumericalFunctionExpression);
                
                virtual std::shared_ptr<BaseExpression> clone() const override;
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                /*!
                 * Retrieves the operator that is associated with this node.
                 *
                 * @param The operator that is associated with this node.
                 */
                FunctionType getFunctionType() const;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The operator that is associated with this node.
                FunctionType functionType;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_UNARYFUNCTIONEXPRESSION_H_ */
