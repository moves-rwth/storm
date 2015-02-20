#include "src/storage/expressions/ExpressionEvaluator.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        ExpressionEvaluator<double>::ExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExprtkExpressionEvaluator(manager) {
            // Intentionally left empty.
        }
        
#ifdef STORM_HAVE_CARL
        ExpressionEvaluator<RationalFunction>::ExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExprtkExpressionEvaluatorBase<RationalFunction>(manager) {
            // Intentionally left empty.
        }
        
        void ExpressionEvaluator<RationalFunction>::setBooleanValue(storm::expressions::Variable const& variable, bool value) {
            ExprtkExpressionEvaluatorBase::setBooleanValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().boolean(value);
        }
        
        void ExpressionEvaluator<RationalFunction>::setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) {
            ExprtkExpressionEvaluatorBase::setIntegerValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().integer(value);
        }
        
        void ExpressionEvaluator<RationalFunction>::setRationalValue(storm::expressions::Variable const& variable, double value) {
            ExprtkExpressionEvaluatorBase::setRationalValue(variable, value);
            this->variableToExpressionMap[variable] = this->getManager().rational(value);
        }
        
        RationalFunction ExpressionEvaluator<RationalFunction>::asRational(Expression const& expression) const {
            Expression substitutedExpression = expression.substitute(variableToExpressionMap);
            return this->rationalFunctionVisitor.toRationalFunction(substitutedExpression);
        }
#endif
    }
}