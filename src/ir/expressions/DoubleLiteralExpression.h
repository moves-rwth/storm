/*
 * DoubleLiteralExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_

#include "src/ir/expressions/BaseExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a double literal.
             */
            class DoubleLiteralExpression : public BaseExpression {
            public:
                /*!
                 * Creates a double literal expression with the given value.
                 *
                 * @param value The value for the double literal.
                 */
                DoubleLiteralExpression(double value);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param doubleLiteralExpression The expression to copy.
                 */
                DoubleLiteralExpression(DoubleLiteralExpression const& doubleLiteralExpression);
                
                virtual std::shared_ptr<BaseExpression> clone() const override;
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual void accept(ExpressionVisitor* visitor) override;
                
                virtual std::string toString() const override;
                
            private:
                // The value of the double literal.
                double value;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_DOUBLELITERALEXPRESSION_H_ */
