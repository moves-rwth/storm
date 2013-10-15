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
		BaseGrammar(std::shared_ptr<VariableState> const& state) : state(state) {}

		/*!
		 * Create and return a new instance of class T, usually the subclass.
		 * @param state VariableState to be given to the constructor.
		 * @returns Instance of class T.
		 */
		static T& instance(std::shared_ptr<VariableState> const& state = nullptr) {
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
		std::shared_ptr<BaseExpression> createBoolLiteral(bool value) {
			return std::shared_ptr<BaseExpression>(new BooleanLiteralExpression(value));
		}
		/*!
		 * Create a new double literal with the given value.
		 * @param value Value of the literal.
		 * @returns Double literal.
		 */
		std::shared_ptr<BaseExpression> createDoubleLiteral(double value) {
			return std::shared_ptr<BaseExpression>(new DoubleLiteralExpression(value));
		}
		/*!
		 * Create a new integer literal with the given value.
		 * @param value Value of the literal.
		 * @returns Integer literal.
		 */
		std::shared_ptr<BaseExpression> createIntLiteral(int_fast64_t value) {
			return std::shared_ptr<BaseExpression>(new IntegerLiteralExpression(value));
		}
		
		/*!
		 * Create a new plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @param type Return type.
		 * @returns Plus expression.
		 */
		std::shared_ptr<BaseExpression> createPlus(std::shared_ptr<BaseExpression> const& left, bool addition, std::shared_ptr<BaseExpression> const& right, BaseExpression::ReturnType type) {
			if (addition) {
				return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(type, left->clone(), right->clone(), BinaryNumericalFunctionExpression::PLUS));
			} else {
				return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(type, left->clone(), right->clone(), BinaryNumericalFunctionExpression::MINUS));
			}
		}
		/*!
		 * Create a new double plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @returns Double plus expression.
		 */
		std::shared_ptr<BaseExpression> createDoublePlus(std::shared_ptr<BaseExpression> const& left, bool addition, std::shared_ptr<BaseExpression> const& right) {
			return this->createPlus(left, addition, right, BaseExpression::double_);
		}
		/*!
		 * Create a new integer plus expression. If addition is true, it will be an addition, otherwise a subtraction.
		 * @param left Left operand.
		 * @param addition Flag for addition or subtraction.
		 * @param right Right operand.
		 * @returns Integer plus expression.
		 */
		std::shared_ptr<BaseExpression> createIntPlus(std::shared_ptr<BaseExpression> const& left, bool addition, std::shared_ptr<BaseExpression> const& right) {
			return this->createPlus(left, addition, right, BaseExpression::int_);
		}

		/*!
		 * Create a new integer multiplication expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns Integer multiplication expression.
		 */
		std::shared_ptr<BaseExpression> createIntMult(std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right) {
			return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(BaseExpression::int_, left->clone(), right->clone(), BinaryNumericalFunctionExpression::TIMES));
		}
        
		/*!
		 * Create a new integer multiplication expression. If multiplication is true, it will be an multiplication, otherwise a division.
		 * @param left Left operand.
		 * @param addition Flag for multiplication or division.
		 * @param right Right operand.
		 * @returns Integer multiplication expression.
		 */
		std::shared_ptr<BaseExpression> createDoubleMult(std::shared_ptr<BaseExpression> const& left, bool multiplication, std::shared_ptr<BaseExpression> const& right) {
			if (multiplication) {
				return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left->clone(), right->clone(), BinaryNumericalFunctionExpression::TIMES));
			} else {
				return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(BaseExpression::double_, left->clone(), right->clone(), BinaryNumericalFunctionExpression::DIVIDE));
			}
		}
        
        /*!
         * Creates an integer min/max expression.
         *
         * @param min Indicates whether the expression is min or max.
         * @param left The left operand.
         * @param right The right operand.
         * @return An integer min/max expression.
         */
        std::shared_ptr<BaseExpression> createIntMinMax(bool min, std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right) {
            if (min) {
                return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(BaseExpression::int_, left->clone(), right->clone(), BinaryNumericalFunctionExpression::MIN));
            } else {
                return std::shared_ptr<BaseExpression>(new BinaryNumericalFunctionExpression(BaseExpression::int_, left->clone(), right->clone(), BinaryNumericalFunctionExpression::MAX));
            }
        }
        
        /*!
         * Creates an integer floor/ceil expression.
         *
         * @param floor Indicates whether the expression is a floor expression.
         * @param operand The argument of the floor/ceil operation.
         * @return An integer floor/ceil expression.
         */
        std::shared_ptr<BaseExpression> createIntFloorCeil(bool floor, std::shared_ptr<BaseExpression> const& operand) {
            if (floor) {
                return std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(BaseExpression::int_, operand->clone(), UnaryNumericalFunctionExpression::FLOOR));
            } else {
                return std::shared_ptr<BaseExpression>(new UnaryNumericalFunctionExpression(BaseExpression::int_, operand->clone(), UnaryNumericalFunctionExpression::CEIL));
            }
        }
        
		/*!
		 * Create a new binary relation expression.
		 * @param left Left operand.
		 * @param relationType Type of binary relation.
		 * @param right Right operand.
		 * @returns Binary relation expression.
		 */
		std::shared_ptr<BaseExpression> createRelation(std::shared_ptr<BaseExpression> const& left, BinaryRelationExpression::RelationType relationType, std::shared_ptr<BaseExpression> const& right) {
			return std::shared_ptr<BaseExpression>(new BinaryRelationExpression(left->clone(), right->clone(), relationType));
		}
		/*!
		 * Create a new negation expression.
		 * @param child Expression to be negated.
		 * @returns Negation expression.
		 */
		std::shared_ptr<BaseExpression> createNot(std::shared_ptr<BaseExpression> const& child) {
			return std::shared_ptr<UnaryBooleanFunctionExpression>(new UnaryBooleanFunctionExpression(child->clone(), UnaryBooleanFunctionExpression::NOT));
		}
		/*!
		 * Create a new And expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns And expression.
		 */
		std::shared_ptr<BaseExpression> createAnd(std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right) {
			return std::shared_ptr<BaseExpression>(new BinaryBooleanFunctionExpression(left->clone(), right->clone(), BinaryBooleanFunctionExpression::AND));
		}
		/*!
		 * Create a new Or expression.
		 * @param left Left operand.
		 * @param right Right operand.
		 * @returns Or expression.
		 */
		std::shared_ptr<BaseExpression> createOr(std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right) {
			return std::shared_ptr<BinaryBooleanFunctionExpression>(new BinaryBooleanFunctionExpression(left->clone(), right->clone(), BinaryBooleanFunctionExpression::OR));
		}
		/*!
		 * Retrieve boolean variable by name.
		 * @param name Variable name.
		 * @returns Boolean variable.
		 */
		std::shared_ptr<BaseExpression> getBoolVariable(std::string const& name) {
			return std::shared_ptr<BaseExpression>(new VariableExpression(*state->getBooleanVariableExpression(name)));
		}
		/*!
		 * Retrieve integer variable by name.
		 * @param name Variable name.
		 * @returns Integer variable.
		 */
		std::shared_ptr<BaseExpression> getIntVariable(std::string const& name) {
			return std::shared_ptr<BaseExpression>(new VariableExpression(*state->getIntegerVariableExpression(name)));
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

