/*
 * UnaryExpression.cpp
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#include "UnaryExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
    
            UnaryExpression::UnaryExpression(ReturnType type, std::unique_ptr<BaseExpression>&& child) : BaseExpression(type), child(std::move(child)) {
                // Nothing to do here.
            }
            
            UnaryExpression::UnaryExpression(UnaryExpression const& unaryExpression) : BaseExpression(unaryExpression), child(unaryExpression.child->clone()) {
                // Nothing to do here.
            }
            
            std::unique_ptr<BaseExpression> const& UnaryExpression::getChild() const {
                return child;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm
