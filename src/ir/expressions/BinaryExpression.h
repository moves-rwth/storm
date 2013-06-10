/*
 * BinaryExpression.h
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a generic binary expression.
             */
            class BinaryExpression : public BaseExpression {
            public:
                /*!
                 * Constructs a binary expression with the given type and children.
                 * @param type The type of the binary expression.
                 * @param left The left child of the binary expression.
                 * @param right The right child of the binary expression.
                 */
                BinaryExpression(ReturnType type, std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param binaryExpression The expression to copy.
                 */
                BinaryExpression(BinaryExpression const& binaryExpression);
                
                /*!
                 * Retrieves the left child of the expression node.
                 *
                 * @return The left child of the expression node.
                 */
                std::shared_ptr<BaseExpression> const& getLeft() const;
                
                /*!
                 * Retrieves the right child of the expression node.
                 *
                 * @return The right child of the expression node.
                 */
                std::shared_ptr<BaseExpression> const& getRight() const;
                
            private:
                // The left child of the binary expression.
                std::shared_ptr<BaseExpression> left;
                
                // The right child of the binary expression.
                std::shared_ptr<BaseExpression> right;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BINARYEXPRESSION_H_ */
