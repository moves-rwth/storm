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
            class BooleanConstantExpression : public ConstantExpression<bool> {
            public:
                /*!
                 * Creates a boolean constant expression with the given constant name.
                 *
                 * @param constantName The name of the constant to use.
                 */
                BooleanConstantExpression(std::string const& constantName);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param booleanConstantExpression The expression to copy.
                 */
                BooleanConstantExpression(BooleanConstantExpression const& booleanConstantExpression);
                
                virtual std::shared_ptr<BaseExpression> clone() const override;

                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_ */
