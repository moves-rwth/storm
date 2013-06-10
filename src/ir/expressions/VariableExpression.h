/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_EXPRESSIONS_VARIABLEEXPRESSION_H_
#define STORM_IR_EXPRESSIONS_VARIABLEEXPRESSION_H_

#include "BaseExpression.h"

namespace storm {
	
    // Forward-declare VariableState.
	namespace parser {
		namespace prismparser {
			class VariableState;
		} // namespace prismparser
	} // namespace parser
    
	namespace ir {
		namespace expressions {
			
            /*!
             * A class representing a variable in the expression tree.
             */
			class VariableExpression : public BaseExpression {
			public:
                /*!
                 * Creates a variable expression of the given type with the given name. As this variable has no indices
                 * it is only meant as a dummy and needs to be replaced with a "full" variable expression.
                 *
                 * @param type The type of the variable.
                 * @param variableName The name of the variable.
                 */
				VariableExpression(ReturnType type, std::string const& variableName);
				
                /*!
                 * Creates a variable expression of the given type with the given name and indices.
                 *
                 * @param type The type of the variable.
                 * @param globalIndex The global (i.e. program-wide) index of the variable.
                 * @param variableName The name of the variable.
                 */
				VariableExpression(ReturnType type, uint_fast64_t globalIndex, std::string const& variableName);
                
				virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const override;
				
				virtual void accept(ExpressionVisitor* visitor) override;
				
				virtual std::string toString() const override;
				
				virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
				
				virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
				
				virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const override;
				
                /*!
                 * Retrieves the name of the variable.
                 *
                 * @return The name of the variable.
                 */
				std::string const& getVariableName() const;
				
                /*!
                 * Retrieves the global (i.e. program-wide) index of the variable.
                 *
                 * @return The global index of the variable.
                 */
				uint_fast64_t getGlobalVariableIndex() const;
				
			private:
                // The global index of the variable.
				uint_fast64_t globalIndex;
                
                // The name of the variable.
				std::string variableName;
			};
			
		} // namespace expressions
	} // namespace ir
} // namespace storm

#endif /* STORM_IR_EXPRESSIONS_VARIABLEEXPRESSION_H_ */
