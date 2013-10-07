/*
 * BinaryBooleanFunctionExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include "BinaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BinaryExpression::BinaryExpression(ReturnType type, std::unique_ptr<BaseExpression>&& left, std::unique_ptr<BaseExpression>&& right)
            : BaseExpression(type), left(std::move(left)), right(std::move(right)) {
                // Nothing to do here.
            }
            
            BinaryExpression::BinaryExpression(BinaryExpression const& binaryExpression) : BaseExpression(binaryExpression.getType()), left(binaryExpression.left->clone()), right(binaryExpression.right->clone()) {
                // Nothing to do here.
            }

            std::unique_ptr<BaseExpression> const& BinaryExpression::getLeft() const {
                return left;
            }
            
            std::unique_ptr<BaseExpression> const& BinaryExpression::getRight() const {
                return right;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm