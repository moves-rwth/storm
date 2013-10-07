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
            
            BaseExpression* UnaryExpression::performSubstitution(std::map<std::string, std::reference_wrapper<BaseExpression>> const& substitution) {
                BaseExpression* newChild = child->performSubstitution(substitution);
                
                // Only update the child if it changed, because otherwise the child gets destroyed.
                if (newChild != child.get()) {
                    child = std::unique_ptr<BaseExpression>(newChild);
                }
                
                return this;
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm
