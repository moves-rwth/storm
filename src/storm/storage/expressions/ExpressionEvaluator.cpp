#include "src/storage/expressions/ExpressionEvaluator.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        ExpressionEvaluator<double>::ExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExprtkExpressionEvaluator(manager) {
            // Intentionally left empty.
        }
        
        template<typename RationalType>
        ExpressionEvaluatorWithVariableToExpressionMap<RationalType>::ExpressionEvaluatorWithVariableToExpressionMap(storm::expressions::ExpressionManager const& manager) : ExprtkExpressionEvaluatorBase<RationalType>(manager) {
            // Intentionally left empty.
        }
        
        template<typename RationalType>
        void ExpressionEvaluatorWithVariableToExpressionMap<RationalType>::setBooleanValue(storm::expressions::Variable const& variable, bool value) {
            ExprtkExpressionEvaluatorBase<RationalType>::setBooleanValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().boolean(value);
        }
        
        template<typename RationalType>
        void ExpressionEvaluatorWithVariableToExpressionMap<RationalType>::setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) {
            ExprtkExpressionEvaluatorBase<RationalType>::setIntegerValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().integer(value);
        }
        
        template<typename RationalType>
        void ExpressionEvaluatorWithVariableToExpressionMap<RationalType>::setRationalValue(storm::expressions::Variable const& variable, double value) {
            ExprtkExpressionEvaluatorBase<RationalType>::setRationalValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().rational(value);
        }

#ifdef STORM_HAVE_CARL
        ExpressionEvaluator<RationalNumber>::ExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExpressionEvaluatorWithVariableToExpressionMap<RationalNumber>(manager) {
            // Intentionally left empty.
        }

        RationalNumber ExpressionEvaluator<RationalNumber>::asRational(Expression const& expression) const {
            Expression substitutedExpression = expression.substitute(this->variableToExpressionMap);
            return this->rationalNumberVisitor.toRationalNumber(substitutedExpression);
        }
        
        ExpressionEvaluator<RationalFunction>::ExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExpressionEvaluatorWithVariableToExpressionMap<RationalFunction>(manager) {
            // Intentionally left empty.
        }
        
        RationalFunction ExpressionEvaluator<RationalFunction>::asRational(Expression const& expression) const {
            Expression substitutedExpression = expression.substitute(this->variableToExpressionMap);
            return this->rationalFunctionVisitor.toRationalFunction(substitutedExpression);
        }
        
        template class ExpressionEvaluatorWithVariableToExpressionMap<RationalNumber>;
        template class ExpressionEvaluatorWithVariableToExpressionMap<RationalFunction>;
#endif
    }
}
