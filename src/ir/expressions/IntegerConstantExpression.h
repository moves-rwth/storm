/*
 * IntegerConstantExpression.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_

#include "ConstantExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            /*!
             * A class representing a constant expression of type integer.
             */
            class IntegerConstantExpression : public ConstantExpression {
            public:
                /*!
                 * Creates an integer constant expression with the given constant name.
                 *
                 * @param constantName The name of the constant to use.
                 */
                IntegerConstantExpression(std::string const& constantName);
                
                /*!
                 * Copy-constructs from the given expression.
                 *
                 * @param integerConstantExpression The expression to copy.
                 */
                IntegerConstantExpression(IntegerConstantExpression const& integerConstantExpression);
                
                virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
                
                virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
                virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
                
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
                int_fast64_t getValue() const;
                
                /*!
                 * Defines the constant using the given value.
                 *
                 * @param value The value to use for defining the constant.
                 */
                void define(int_fast64_t value);
                
            private:
                // This member stores the value of the constant if it is defined.
                int_fast64_t value;
                
                // A flag indicating whether the member is defined or not.
                bool defined;
            };
            
        } // namespace expressions
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_INTEGERCONSTANTEXPRESSION_H_ */
