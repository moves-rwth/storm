/* 
 * File:   Keywords.h
 * Author: nafur
 *
 * Created on April 10, 2013, 6:03 PM
 */

#ifndef BASEGRAMMAR_H
#define	BASEGRAMMAR_H

#include "Includes.h"

#include "VariableState.h"

namespace storm {
namespace parser {
namespace prism {

	template <typename T>
	class BaseGrammar {
	public:
		BaseGrammar(std::shared_ptr<VariableState>& state) : state(state) {}

		static T& instance(std::shared_ptr<VariableState> state = nullptr) {
			if (BaseGrammar::instanceObject == nullptr) {
				BaseGrammar::instanceObject = std::shared_ptr<T>(new T(state));
				if (!state->firstRun) BaseGrammar::instanceObject->secondRun();
			}
			return *BaseGrammar::instanceObject;
		}

		static void resetInstance() {
			BaseGrammar::instanceObject = nullptr;
		}

		static void secondRun() {
			if (BaseGrammar::instanceObject != nullptr) {
				BaseGrammar::instanceObject->prepareSecondRun();
			}
		}

		std::shared_ptr<BaseExpression> createBoolLiteral(const bool value) {
			return std::shared_ptr<BooleanLiteral>(new BooleanLiteral(value));
		}
		std::shared_ptr<BaseExpression> createDoubleLiteral(const double value) {
			return std::shared_ptr<DoubleLiteral>(new DoubleLiteral(value));
		}
		std::shared_ptr<BaseExpression> createIntLiteral(const int_fast64_t value) {
			return std::shared_ptr<IntegerLiteral>(new IntegerLiteral(value));
		}
		
		std::shared_ptr<BaseExpression> createPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right, BaseExpression::ReturnType type) {
			if (addition) {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(type, left, right, BinaryNumericalFunctionExpression::PLUS));
			} else {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(type, left, right, BinaryNumericalFunctionExpression::MINUS));
			}
		}
		std::shared_ptr<BaseExpression> createDoublePlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right) {
			return this->createPlus(left, addition, right, BaseExpression::double_);
		}
		std::shared_ptr<BaseExpression> createIntPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right) {
			return this->createPlus(left, addition, right, BaseExpression::int_);
		}

		std::shared_ptr<BaseExpression> createIntMult(const std::shared_ptr<BaseExpression> left, const std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::int_, left, right, BinaryNumericalFunctionExpression::TIMES));
		}
		std::shared_ptr<BaseExpression> createDoubleMult(const std::shared_ptr<BaseExpression> left, bool multiplication, const std::shared_ptr<BaseExpression> right) {
			if (multiplication) {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::TIMES));
			} else {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::DIVIDE));
			}
		}

		std::shared_ptr<BaseExpression> createRelation(std::shared_ptr<BaseExpression> left, BinaryRelationExpression::RelationType relationType, std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryRelationExpression>(new BinaryRelationExpression(left, right, relationType));
		}
		std::shared_ptr<BaseExpression> createNot(std::shared_ptr<BaseExpression> child) {
			return std::shared_ptr<UnaryBooleanFunctionExpression>(new UnaryBooleanFunctionExpression(child, UnaryBooleanFunctionExpression::NOT));
		}
		std::shared_ptr<BaseExpression> createAnd(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right) {
			//std::cerr << "Creating " << left->toString() << " & " << right->toString() << std::endl;
			return std::shared_ptr<BinaryBooleanFunctionExpression>(new BinaryBooleanFunctionExpression(left, right, BinaryBooleanFunctionExpression::AND));
		}
		std::shared_ptr<BaseExpression> createOr(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryBooleanFunctionExpression>(new BinaryBooleanFunctionExpression(left, right, BinaryBooleanFunctionExpression::OR));
		}
		std::shared_ptr<BaseExpression> getBoolVariable(const std::string name) {
			return state->getBooleanVariable(name);
		}
		std::shared_ptr<BaseExpression> getIntVariable(const std::string name) {
			return state->getIntegerVariable(name);
		}

		virtual void prepareSecondRun() {}
	protected:
		std::shared_ptr<VariableState> state;

	private:
		static std::shared_ptr<T> instanceObject;
		static bool inSecondRun;
	};

	template <typename T>
	std::shared_ptr<T> BaseGrammar<T>::instanceObject;
}
}
}
#endif	/* BASEGRAMMAR_H */

