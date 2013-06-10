/*
 * IntegerLiteralExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing an integer literal.
             */
            class IntegerLiteralExpression : public BaseExpression {
            public:
                /*!
                 * Creates an integer literal expression with the given value.
                 *
                 * @param value The value for the integer literal.
                 */
                IntegerLiteralExpression(int_fast64_t value);
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The value of the double literal.
                int_fast64_t value;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_INTEGERLITERALEXPRESSION_H_ */
