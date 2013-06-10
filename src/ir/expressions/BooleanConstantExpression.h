/*
 * BooleanConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a boolean constant expression.
             */
            class BooleanConstantExpression : public ConstantExpression {
            public:
                /*!
                 * Creates a boolean constant expression with the given constant name.
                 *
                 * @param constantName The name of the constant to use.
                 */
                BooleanConstantExpression(std::string const& constantName);
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, parser::prismparser::VariableState const& variableState) const override;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;

                /*!
                 * Retrieves whether the constant is defined or not.
                 *
                 * @return True if the constant is defined.
                 */
                bool isDefined() const;
                
                /*!
                 * Retrieves the value of the constant if it is defined and false otherwise.
                 */
                bool getValue() const;
                
                /*!
                 * Defines the constant using the given value.
                 *
                 * @param value The value to use for defining the constant.
                 */
                void define(bool value);
                
            private:
                // This member stores the value of the constant if it is defined.
                bool value;
                
                // A flag indicating whether the member is defined or not.
                bool defined;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_ */
