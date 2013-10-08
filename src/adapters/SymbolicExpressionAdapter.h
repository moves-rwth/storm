/*
 * SymbolicExpressionAdapter.h
 *
 *  Created on: 27.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_SYMBOLICEXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_SYMBOLICEXPRESSIONADAPTER_H_

#include "src/ir/expressions/ExpressionVisitor.h"
#include "src/exceptions/ExpressionEvaluationException.h"

#include "cuddObj.hh"

#include <stack>
#include <iostream>

namespace storm {

namespace adapters {

class SymbolicExpressionAdapter : public storm::ir::expressions::ExpressionVisitor {
public:
	SymbolicExpressionAdapter(storm::ir::Program const& program, std::unordered_map<std::string, std::vector<ADD*>>& variableToDecisionDiagramVariableMap) : program(program), stack(), variableToDecisionDiagramVariableMap(variableToDecisionDiagramVariableMap) {

	}

	ADD* translateExpression(std::unique_ptr<storm::ir::expressions::BaseExpression> const& expression) {
		expression->accept(this);
		return stack.top();
	}

	virtual void visit(storm::ir::expressions::BinaryBooleanFunctionExpression* expression) {
		expression->getLeft()->accept(this);
		expression->getRight()->accept(this);

		ADD* rightResult = stack.top();
		stack.pop();
		ADD* leftResult = stack.top();
		stack.pop();

		switch(expression->getFunctionType()) {
		case storm::ir::expressions::BinaryBooleanFunctionExpression::AND:
			stack.push(new ADD(leftResult->Times(*rightResult)));
			break;
		case storm::ir::expressions::BinaryBooleanFunctionExpression::OR:
			stack.push(new ADD(leftResult->Plus(*rightResult)));
			break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << expression->getFunctionType() << "'.";
		}

		// delete leftResult;
		// delete rightResult;
	}

	virtual void visit(storm::ir::expressions::BinaryNumericalFunctionExpression* expression) {
		expression->getLeft()->accept(this);
		expression->getRight()->accept(this);

		ADD* rightResult = stack.top();
		stack.pop();
		ADD* leftResult = stack.top();
		stack.pop();

		switch(expression->getFunctionType()) {
		case storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS:
			stack.push(new ADD(leftResult->Plus(*rightResult)));
			break;
		case storm::ir::expressions::BinaryNumericalFunctionExpression::MINUS:
			stack.push(new ADD(leftResult->Minus(*rightResult)));
			break;
		case storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES:
			stack.push(new ADD(leftResult->Times(*rightResult)));
			break;
		case storm::ir::expressions::BinaryNumericalFunctionExpression::DIVIDE:
			stack.push(new ADD(leftResult->Divide(*rightResult)));
			break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << expression->getFunctionType() << "'.";
		}
	}

	virtual void visit(storm::ir::expressions::BinaryRelationExpression* expression) {
		expression->getLeft()->accept(this);
		expression->getRight()->accept(this);

		ADD* rightResult = stack.top();
		stack.pop();
		ADD* leftResult = stack.top();
		stack.pop();

		switch(expression->getRelationType()) {
		case storm::ir::expressions::BinaryRelationExpression::EQUAL:
			stack.push(new ADD(leftResult->Equals(*rightResult)));
			break;
		case storm::ir::expressions::BinaryRelationExpression::NOT_EQUAL:
			stack.push(new ADD(leftResult->NotEquals(*rightResult)));
			break;
		case storm::ir::expressions::BinaryRelationExpression::LESS:
			stack.push(new ADD(leftResult->LessThan(*rightResult)));
			break;
		case storm::ir::expressions::BinaryRelationExpression::LESS_OR_EQUAL:
			stack.push(new ADD(leftResult->LessThanOrEqual(*rightResult)));
			break;
		case storm::ir::expressions::BinaryRelationExpression::GREATER:
			stack.push(new ADD(leftResult->GreaterThan(*rightResult)));
			break;
		case storm::ir::expressions::BinaryRelationExpression::GREATER_OR_EQUAL:
			stack.push(new ADD(leftResult->GreaterThanOrEqual(*rightResult)));
			break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean binary operator: '" << expression->getRelationType() << "'.";
		}
	}

	virtual void visit(storm::ir::expressions::BooleanConstantExpression* expression) {
		if (!expression->isDefined()) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Boolean constant '" << expression->getConstantName() << "' is undefined.";
		}

		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(expression->getValue() ? 1 : 0)));
	}

	virtual void visit(storm::ir::expressions::BooleanLiteralExpression* expression) {
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(expression->getValueAsBool(nullptr) ? 1 : 0)));
	}

	virtual void visit(storm::ir::expressions::DoubleConstantExpression* expression) {
		if (expression->isDefined()) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Double constant '" << expression->getConstantName() << "' is undefined.";
		}

		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(expression->getValue())));
	}

	virtual void visit(storm::ir::expressions::DoubleLiteralExpression* expression) {
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(expression->getValueAsDouble(nullptr))));
	}

	virtual void visit(storm::ir::expressions::IntegerConstantExpression* expression) {
		if (!expression->isDefined()) {
			throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
					<< "Integer constant '" << expression->getConstantName() << "' is undefined.";
		}

		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(static_cast<double>(expression->getValue()))));
	}

	virtual void visit(storm::ir::expressions::IntegerLiteralExpression* expression) {
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		stack.push(new ADD(*cuddUtility->getConstant(static_cast<double>(expression->getValueAsInt(nullptr)))));
	}

	virtual void visit(storm::ir::expressions::UnaryBooleanFunctionExpression* expression) {
		expression->getChild()->accept(this);

		ADD* childResult = stack.top();
		stack.pop();

		switch (expression->getFunctionType()) {
		case storm::ir::expressions::UnaryBooleanFunctionExpression::NOT:
			stack.push(new ADD(~(*childResult)));
			break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown boolean unary operator: '" << expression->getFunctionType() << "'.";
		}
	}

	virtual void visit(storm::ir::expressions::UnaryNumericalFunctionExpression* expression) {
		expression->getChild()->accept(this);

		ADD* childResult = stack.top();
		stack.pop();

		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();
		ADD* result = cuddUtility->getConstant(0);
		switch(expression->getFunctionType()) {
		case storm::ir::expressions::UnaryNumericalFunctionExpression::MINUS:
			stack.push(new ADD(result->Minus(*childResult)));
			break;
		default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
				<< "Unknown numerical unary operator: '" << expression->getFunctionType() << "'.";
		}

	}

	virtual void visit(storm::ir::expressions::VariableExpression* expression) {
		storm::utility::CuddUtility* cuddUtility = storm::utility::cuddUtilityInstance();

		std::vector<ADD*> const& variables = variableToDecisionDiagramVariableMap[expression->getVariableName()];

		ADD* result = cuddUtility->getConstant(0);
		if (expression->getType() == storm::ir::expressions::BaseExpression::bool_) {
			cuddUtility->setValueAtIndex(result, 1, variables, 1);
		} else {
            storm::ir::Module const& module = program.getModule(program.getModuleIndexForVariable(expression->getVariableName()));
            storm::ir::IntegerVariable const& integerVariable = module.getIntegerVariable(expression->getVariableName());
			int64_t low = integerVariable.getLowerBound()->getValueAsInt(nullptr);
			int64_t high = integerVariable.getUpperBound()->getValueAsInt(nullptr);

			for (int_fast64_t i = low; i <= high; ++i) {
				cuddUtility->setValueAtIndex(result, i - low, variables, static_cast<double>(i));
			}
		}

		stack.push(result);
	}

private:
    storm::ir::Program const& program;
	std::stack<ADD*> stack;
	std::unordered_map<std::string, std::vector<ADD*>>& variableToDecisionDiagramVariableMap;
};

} // namespace adapters

} // namespace storm

#endif /* STORM_ADAPTERS_SYMBOLICEXPRESSIONADAPTER_H_ */
