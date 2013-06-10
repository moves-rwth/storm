/*
 * UnaryExpression.h
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_

#include "BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a generic unary expression.
             */
            class UnaryExpression : public BaseExpression {
            public:
                /*!
                 * Constructs a unary expression with the given type and child.
                 * @param type The type of the unary expression.
                 * @param right The child of the unary expression.
                 */
                UnaryExpression(ReturnType type, std::shared_ptr<BaseExpression> child);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param unaryExpression The expression to copy.
                 */
                UnaryExpression(UnaryExpression const& unaryExpression);
                
                /*!
                 * Retrieves the child of the expression node.
                 *
                 * @return The child of the expression node.
                 */
                std::shared_ptr<BaseExpression> const& getChild() const;
                
            private:
                // The left child of the unary expression.
                std::shared_ptr<BaseExpression> child;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_UNARYEXPRESSION_H_ */
