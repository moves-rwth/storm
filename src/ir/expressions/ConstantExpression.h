/*
 * ConstantExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_

#include "BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a generic constant expression.
             */
            class ConstantExpression : public BaseExpression {
            public:
                
                /*!
                 * Constructs a constant expression of the given type with the given constant name.
                 *
                 * @param type The type of the constant.
                 * @param constantName The name of the constant.
                 */
                ConstantExpression(ReturnType type, std::string const& constantName);
                
                /*!
                 * Retrieves the name of the constant.
                 *
                 * @return The name of the constant.
                 */
                std::string const& getConstantName() const;
                
                virtual std::string toString() const override;
                
            private:
                // The name of the constant.
                std::string constantName;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_CONSTANTEXPRESSION_H_ */
