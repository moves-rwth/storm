/*
 * BinaryFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYFUNCTIONEXPRESSION_H_

#include "src/ir/expressions/BinaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a binary function expression of numerical type.
             */
            class BinaryNumericalFunctionExpression : public BinaryExpression {
            public:
                /*!
                 * An enum type specifying the different functions applicable.
                 */
                enum FunctionType {PLUS, MINUS, TIMES, DIVIDE, MIN, MAX};
                
                /*!
                 * Creates a binary numerical function expression with the given type, children and function type.
                 *
                 * @param type The type of the expression tree node.
                 * @param left The left child of the expression tree node.
                 * @param right The right child of the  expression tree node.
                 * @param functionType The function that is applied to the children of this node.
                 */
                BinaryNumericalFunctionExpression(ReturnType type, std::unique_ptr<BaseExpression>&& left, std::unique_ptr<BaseExpression>&& right, FunctionType functionType);
                
                /*!
                 * Performs a deep-copy of the given expression.
                 *
                 * @param binaryNumericalFunctionExpression The expression to copy.
                 */
                BinaryNumericalFunctionExpression(BinaryNumericalFunctionExpression const& binaryNumericalFunctionExpression);

                virtual std::unique_ptr<BaseExpression> clone() const override;
                
                virtual std::unique_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                /*!
                 * Retrieves the operator that is associated with this node.
                 *
                 * @param The operator that is associated with this node.
                 */
                FunctionType getFunctionType() const;
                
                virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The operator that is associated with this node.
                FunctionType functionType;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYFUNCTIONEXPRESSION_H_ */
