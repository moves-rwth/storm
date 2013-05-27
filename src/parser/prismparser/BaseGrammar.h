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

	/*!
	 * This is the base class for all expression grammars.
	 * It takes care of implementing a singleton, stores a VariableState and implements some common helper routines.
	 */
	template <typename T>
	class BaseGrammar {
	public:
		/*!
		 * Constructor.
		 */
		BaseGrammar(std::shared_ptr<VariableState>& state) : state(state) {}

		/*!
		 * Create and return a new instance of class T, usually the subclass.
		 * @param state VariableState to be given to the constructor.
		 * @returns Instance of class T.
		 */
		static T& instance(std::shared_ptr<VariableState> state = nullptr) {
			if (BaseGrammar::instanceObject == nullptr) {
				BaseGrammar::instanceObject = std::shared_ptr<T>(new T(state));
				if (!state->firstRun) BaseGrammar::instanceObject->secondRun();
			}
			return *BaseGrammar::instanceObject;
		}

		/*!
		 * Clear the cached instance.
		 */
		static void resetInstance() {
			BaseGrammar::instanceObject = nullptr;
		}

		/*!
		 * Notify the cached object, that we will begin with the second parsing run.
		 */
		static void secondRun() {
			if (BaseGrammar::instanceObject != nullptr) {
				BaseGrammar::instanceObject->prepareSecondRun();
			}
		}

		/*!
		 * Create a new boolean literal with the given value.
		 * @param value Value of the literal.
		 * @returns Boolean literal.
		 */
		std::shared_ptr<BaseExpression> createBoolLiteral(const bool value) {
			return std::shared_ptr<BooleanLiteral>(new BooleanLiteral(value));
		}
		/*!
		 * Create a new double literal with the given value.
		 * @param value Value of the literal.
		 * @returns Double literal.
		 */
		std::shared_ptr<BaseExpression> createDoubleLiteral(const double value) {
			return std::shared_ptr<DoubleLiteral>(new DoubleLiteral(value));
		}
		/*!
		 * Create a new integer literal with the given value.
		 * @param value Value of the literal.
		 * @returns Integer literal.
		 */
		std::shared_ptr<BaseExpression> createIntLiteral(const int_fast64_t value) {
			return std::shared_ptr<IntegerLiteral>(new IntegerLiteral(value));
		}
		
		/*!
		 * Create a new plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @param type Return type.
		 * @returns Plus expression.
		 */
		std::shared_ptr<BaseExpression> createPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right, BaseExpression::ReturnType type) {
			if (addition) {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(type, left, right, BinaryNumericalFunctionExpression::PLUS));
			} else {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(type, left, right, BinaryNumericalFunctionExpression::MINUS));
			}
		}
		/*!
		 * Create a new double plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @returns Double plus expression.
		 */
		std::shared_ptr<BaseExpression> createDoublePlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right) {
			return this->createPlus(left, addition, right, BaseExpression::double_);
		}
		/*!
		 * Create a new integer plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @returns Integer plus expression.
		 */
		std::shared_ptr<BaseExpression> createIntPlus(const std::shared_ptr<BaseExpression> left, bool addition, const std::shared_ptr<BaseExpression> right) {
			return this->createPlus(left, addition, right, BaseExpression::int_);
		}

		/*!
		 * Create a new integer multiplication expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns Integer multiplication expression.
		 */
		std::shared_ptr<BaseExpression> createIntMult(const std::shared_ptr<BaseExpression> left, const std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::int_, left, right, BinaryNumericalFunctionExpression::TIMES));
		}
		/*!
		 * Create a new integer multiplication expression. If multiplication is true, it will be an multiplication, otherwise a division.
		 * @param left Left operand.
		 * @param addition Flag for multiplication or division.
		 * @param right Right operand.
		 * @returns Integer multiplication expression.
		 */
		std::shared_ptr<BaseExpression> createDoubleMult(const std::shared_ptr<BaseExpression> left, bool multiplication, const std::shared_ptr<BaseExpression> right) {
			if (multiplication) {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::TIMES));
			} else {
				return std::shared_ptr<BinaryNumericalFunctionExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left, right, BinaryNumericalFunctionExpression::DIVIDE));
			}
		}
		/*!
		 * Create a new binary relation expression.
		 * @param left Left operand.
		 * @param relationType Type of binary relation.
		 * @param right Right operand.
		 * @returns Binary relation expression.
		 */
		std::shared_ptr<BaseExpression> createRelation(std::shared_ptr<BaseExpression> left, BinaryRelationExpression::RelationType relationType, std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryRelationExpression>(new BinaryRelationExpression(left, right, relationType));
		}
		/*!
		 * Create a new negation expression.
		 * @param child Expression to be negated.
		 * @returns Negation expression.
		 */
		std::shared_ptr<BaseExpression> createNot(std::shared_ptr<BaseExpression> child) {
			return std::shared_ptr<UnaryBooleanFunctionExpression>(new UnaryBooleanFunctionExpression(child, UnaryBooleanFunctionExpression::NOT));
		}
		/*!
		 * Create a new And expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns And expression.
		 */
		std::shared_ptr<BaseExpression> createAnd(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right) {
			//std::cerr << "Creating " << left->toString() << " & " << right->toString() << std::endl;
			return std::shared_ptr<BinaryBooleanFunctionExpression>(new BinaryBooleanFunctionExpression(left, right, BinaryBooleanFunctionExpression::AND));
		}
		/*!
		 * Create a new Or expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns Or expression.
		 */
		std::shared_ptr<BaseExpression> createOr(std::shared_ptr<BaseExpression> left, std::shared_ptr<BaseExpression> right) {
			return std::shared_ptr<BinaryBooleanFunctionExpression>(new BinaryBooleanFunctionExpression(left, right, BinaryBooleanFunctionExpression::OR));
		}
		/*!
		 * Retrieve boolean variable by name.
		 * @param name Variable name.
		 * @returns Boolean variable.
		 */
		std::shared_ptr<BaseExpression> getBoolVariable(const std::string name) {
			return state->getBooleanVariable(name);
		}
		/*!
		 * Retrieve integer variable by name.
		 * @param name Variable name.
		 * @returns Integer variable.
		 */
		std::shared_ptr<BaseExpression> getIntVariable(const std::string name) {
			return state->getIntegerVariable(name);
		}

		/*!
		 * Base method to switch to second run. This does nothing.
		 * Any subclass that needs to do something in order to proceed to the second run should override this method.
		 */
		virtual void prepareSecondRun() {}
	protected:
		/*!
		 * Pointer to variable state.
		 */
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

