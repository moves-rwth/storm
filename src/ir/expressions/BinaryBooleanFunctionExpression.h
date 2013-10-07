/*
 * BinaryBooleanFunctionExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_

#include "BinaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a binary function expression of boolean type.
             */
            class BinaryBooleanFunctionExpression : public BinaryExpression {
            public:
                /*!
                 * An enum type specifying the different functions applicable.
                 */
                enum FunctionType {AND, OR};
                
                /*!
                 * Creates a binary boolean function expression tree node with the given children and function type.
                 *
                 * @param left The left child of the node.
                 * @param right The right child of the node.
                 * @param functionType The operator that is to be applied to the two children.
                 */
                BinaryBooleanFunctionExpression(std::unique_ptr<BaseExpression>&& left, std::unique_ptr<BaseExpression>&& right, FunctionType functionType);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param binaryBooleanFunctionExpression The expression to copy.
                 */
                BinaryBooleanFunctionExpression(BinaryBooleanFunctionExpression const& binaryBooleanFunctionExpression);
                
                virtual std::unique_ptr<BaseExpression> clone() const override;
                
                virtual std::unique_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
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

#endif /* STORM_IR_EXPRESSIONS_BINARYBOOLEANFUNCTIONEXPRESSION_H_ */
