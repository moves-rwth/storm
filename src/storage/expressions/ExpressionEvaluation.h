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
#include "DoubleLiteralExpression.h"
#include "BinaryNumericalFunctionExpression.h"

#include "src/storage/parameters.h"
#include "IntegerLiteralExpression.h"
#include "BinaryExpression.h"

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
		typedef std::map<std::string, carl::Variable> type;
	};
	
	template<>
	struct StateType<RationalFunction>
	{
		typedef std::map<std::string, carl::Variable> type;
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
				std::cout << "ite" << std::endl;
				
			}
		
            virtual void visit(BinaryBooleanFunctionExpression const* expression) 
			{
				std::cout << "bbf" << std::endl;
			}
            virtual void visit(BinaryNumericalFunctionExpression const* expression) 
			{
				ExpressionEvaluationVisitor* visitor = new ExpressionEvaluationVisitor(mSharedState);
				expression->getFirstOperand()->accept(visitor);
				mValue = visitor->value();
				expression->getSecondOperand()->accept(visitor);				
				switch(expression->getOperatorType())
				{
					case BinaryNumericalFunctionExpression::OperatorType::Plus:
						mValue += visitor->value();
						break;
					case BinaryNumericalFunctionExpression::OperatorType::Minus:
						mValue -= visitor->value();
						break;
					case BinaryNumericalFunctionExpression::OperatorType::Times:
						mValue *= visitor->value();
						break;
					case BinaryNumericalFunctionExpression::OperatorType::Divide:
						mValue /= visitor->value();
						break;
					default:
						// TODO exception.
						assert(false);
				}
				
				delete visitor;
			}
            virtual void visit(BinaryRelationExpression const* expression) 
			{
				std::cout << "br" << std::endl;
			}
           virtual void visit(VariableExpression const* expression) 
			{
			   std::string const& varName= expression->getVariableName();
				auto it =  mSharedState->find(varName);
				if(it != mSharedState->end())
				{
					mValue = T(it->second);
				}
				else
				{
					carl::Variable nVar = carl::VariablePool::getInstance().getFreshVariable(varName);
					mSharedState->emplace(varName,nVar);
					mValue = T(nVar);
				}
			}
            virtual void visit(UnaryBooleanFunctionExpression const* expression) 
			{
				std::cout << "ubf" << std::endl;
			}
            virtual void visit(UnaryNumericalFunctionExpression const* expression) 
			{
				std::cout << "unf" << std::endl;
			}
            virtual void visit(BooleanLiteralExpression const* expression) 
			{
				std::cout << "bl" << std::endl;
			}
            virtual void visit(IntegerLiteralExpression const* expression) 
			{
				mValue = T(expression->getValue());
			}
            virtual void visit(DoubleLiteralExpression const* expression) 
			{
				std::stringstream str;
				str << std::fixed << std::setprecision( 3 ) << expression->getValue();
				carl::DecimalStringToRational<typename T::CoeffType> transform;
				mValue = T(transform(str.str()));
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
			std::cout << expr;
			std::cout.flush();
			expr.getBaseExpression().accept(visitor);
			T result = visitor->value();
			result.simplify();
			std::cout << " -> " << result << std::endl;
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