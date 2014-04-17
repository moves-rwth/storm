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
#include "IfThenElseExpression.h"
#include "DoubleConstantExpression.h"
#include "DoubleLiteralExpression.h"

#include "src/storage/parameters.h"

namespace storm {
namespace expressions {
	
	template<typename T>
	struct StateType
	{
		typedef int type;
	};
	
	template<>
	struct StateType<Polynomial>
	{
		typedef carl::Variable type;
	};
		
	template<typename T, typename S>
	class ExpressionEvaluationVisitor : public ExpressionVisitor
	{
		public:
			ExpressionEvaluationVisitor(S* sharedState)
			: mSharedState(sharedState)
			{
				
			}
			
			virtual ~ExpressionEvaluationVisitor() {}
			
			virtual void visit(IfThenElseExpression const* expression) 
			{
				bool condititionValue = expression->getCondition()->evaluateAsBool();
				
			}
		
            virtual void visit(BinaryBooleanFunctionExpression const* expression) 
			{
				
			}
            virtual void visit(BinaryNumericalFunctionExpression const* expression) 
			{
				
			}
            virtual void visit(BinaryRelationExpression const* expression) 
			{
				
			}
            virtual void visit(BooleanConstantExpression const* expression) 
			{
				
			}
            virtual void visit(DoubleConstantExpression const* expression) 
			{
				
			}
            virtual void visit(IntegerConstantExpression const* expression) 
			{
				
			}
            virtual void visit(VariableExpression const* expression) 
			{
				
			}
            virtual void visit(UnaryBooleanFunctionExpression const* expression) 
			{
				
			}
            virtual void visit(UnaryNumericalFunctionExpression const* expression) 
			{
				
			}
            virtual void visit(BooleanLiteralExpression const* expression) 
			{
				
			}
            virtual void visit(IntegerLiteralExpression const* expression) 
			{
				
			}
            virtual void visit(DoubleLiteralExpression const* expression) 
			{
				
			}

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
		
		
		T evaluate(Expression const& expr, storm::expressions::SimpleValuation const* val)
		{
			ExpressionEvaluationVisitor<T, typename StateType<T>::type>*  visitor = new ExpressionEvaluationVisitor<T, typename StateType<T>::type>(&mState);
			//expr.getBaseExpression().accept(visitor);
			T result = T(mpq_class(expr.evaluateAsDouble(val)));
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