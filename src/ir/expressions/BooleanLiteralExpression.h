/*
 * BooleanLiteralExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a boolean literal.
             */
            class BooleanLiteralExpression : public BaseExpression {
            public:
                /*!
                 * Creates a boolean literal expression with the given value.
                 *
                 * @param value The value for the boolean literal.
                 */
                BooleanLiteralExpression(bool value);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param booleanLiteralExpression The expression to copy.
                 */
                BooleanLiteralExpression(BooleanLiteralExpression const& booleanLiteralExpression);
                
                virtual std::unique_ptr<BaseExpression> clone() const override;
                
                virtual std::unique_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The value of the boolean literal.
                bool value;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BOOLEANLITERALEXPRESSION_H_ */
