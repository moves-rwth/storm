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

            BaseExpression* BinaryExpression::performSubstitution(std::map<std::string, std::reference_wrapper<BaseExpression>> const& substitution) {
                // Get the new left successor recursively.
                BaseExpression* newLeftSuccessor = left->performSubstitution(substitution);
                
                // If the left successor changed, we need to update it. If it did not change, this must not be executed,
                // because assigning to the unique_ptr will destroy the current successor immediately.
                if (newLeftSuccessor != left.get()) {
                    left = std::unique_ptr<BaseExpression>(newLeftSuccessor);
                }
                
                // Now do the same thing for the right successor.
                BaseExpression* newRightSuccessor = right->performSubstitution(substitution);
                if (newRightSuccessor != right.get()) {
                    right = std::unique_ptr<BaseExpression>(newRightSuccessor);
                }
            
                return this;
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