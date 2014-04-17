/** 
 * @file:   ExpressionEvaluation.h
 * @author: Sebastian Junges
 *
 * @since April 4, 2014
 */

#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATION_H_

#include "ExpressionVisitor.h"
#include "BaseExpression.h"

namespace storm {
namespace expressions {
	
	template<typename T>
	struct StateType
	{
		typedef int type;
	};
		
	template<typename T, typename S>
	class ExpressionEvaluationVisitor : public ExpressionVisitor
	{
		public:
			ExpressionEvaluationVisitor(S* sharedState)
			: mSharedState(sharedState)
			{
				
			}
			
			virtual void visit(IfThenElseExpression const* expression) = 0;
            virtual void visit(BinaryBooleanFunctionExpression const* expression) = 0;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) = 0;
            virtual void visit(BinaryRelationExpression const* expression) = 0;
            virtual void visit(BooleanConstantExpression const* expression) = 0;
            virtual void visit(DoubleConstantExpression const* expression) = 0;
            virtual void visit(IntegerConstantExpression const* expression) = 0;
            virtual void visit(VariableExpression const* expression) = 0;
            virtual void visit(UnaryBooleanFunctionExpression const* expression) = 0;
            virtual void visit(UnaryNumericalFunctionExpression const* expression) = 0;
            virtual void visit(BooleanLiteralExpression const* expression) = 0;
            virtual void visit(IntegerLiteralExpression const* expression) = 0;
            virtual void visit(DoubleLiteralExpression const* expression) = 0;

		const T& value() const
		{
			return mValue;
		}
		
		private:
		S* mSharedState;
		T mValue;
	};
	
	template<typename T>
	class ExpressionEvaluation
	{
		public:
		ExpressionEvaluation() : mState()
		{
			
		}
		
		
		T evaluate(Expression const& expr, storm::expressions::SimpleValuation const*)
		{
			ExpressionEvaluationVisitor<T, typename StateType<T>::type>*  visitor = new ExpressionEvaluationVisitor<T, typename StateType<T>::type>(&mState);
			expr.getBaseExpression().accept(visitor);
			T result =  visitor->value();
			delete visitor;
			return result;
		}
		
		protected:
			typename StateType<T>::type mState;
	};
	
	/**
	 * For doubles, we keep using the getValueAs from the expressions, as this should be more efficient.
	 */
	template<>
	class ExpressionEvaluation<double>
	{
		public:
		double evaluate(Expression const& expr, storm::expressions::SimpleValuation const* val) const
		{
			return expr.evaluateAsDouble(val);
		}
	};


	
	
}
}

#endif