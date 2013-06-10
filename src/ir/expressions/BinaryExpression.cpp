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
            
            BinaryExpression::BinaryExpression(ReturnType type, std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right)
            : BaseExpression(type), left(left), right(right) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> const& BinaryExpression::getLeft() const {
                return left;
            }
            
            std::shared_ptr<BaseExpression> const& BinaryExpression::getRight() const {
                return right;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm