/*
 * VariableExpression.h
 *
 *  Created on: 03.01.2013
 *      Author: Christian Dehnert
 */

#ifndef VARIABLEEXPRESSION_H_
#define VARIABLEEXPRESSION_H_

#include <memory>
#include <iostream>

#include "BaseExpression.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

namespace storm {
	
    // Forward-declare VariableState.
	namespace parser {
		namespace prismparser {
			class VariableState;
		} // namespace prismparser
	} // namespace parser
    
	namespace ir {
		namespace expressions {
			
			class VariableExpression : public BaseExpression {
			public:
				VariableExpression(ReturnType type, std::string variableName);
				
				VariableExpression(ReturnType type, uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string variableName);
				
				VariableExpression(VariableExpression const& oldExpression, std::string const& newName, uint_fast64_t newGlobalIndex);
				
				virtual ~VariableExpression();
				
				virtual std::shared_ptr<BaseExpression> clone(std::map<std::string, std::string> const& renaming, parser::prismparser::VariableState const& variableState) const;
				
				virtual void accept(ExpressionVisitor* visitor);
				
				virtual std::string toString() const;
				
				virtual int_fast64_t getValueAsInt(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
				
				virtual bool getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
				
				virtual double getValueAsDouble(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const;
				
				std::string const& getVariableName() const;
				
				uint_fast64_t getLocalVariableIndex() const;
				
				uint_fast64_t getGlobalVariableIndex() const;
				
			private:
				uint_fast64_t localIndex;
				uint_fast64_t globalIndex;
				std::string variableName;
			};
			
		} // namespace expressions
	} // namespace ir
} // namespace storm

#endif /* VARIABLEEXPRESSION_H_ */
