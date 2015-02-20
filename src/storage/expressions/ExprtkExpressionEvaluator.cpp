#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/expressions/ExpressionManager.h"

#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"

namespace storm {
    namespace expressions {
        template<typename RationalType>
        ExprtkExpressionEvaluatorBase<RationalType>::ExprtkExpressionEvaluatorBase(storm::expressions::ExpressionManager const& manager) : ExpressionEvaluatorBase<RationalType>(manager), booleanValues(manager.getNumberOfBooleanVariables()), integerValues(manager.getNumberOfIntegerVariables()), rationalValues(manager.getNumberOfRationalVariables()) {

            for (auto const& variableTypePair : manager) {
                if (variableTypePair.second.isBooleanType()) {
                    symbolTable.add_variable(variableTypePair.first.getName(), this->booleanValues[variableTypePair.first.getOffset()]);
                } else if (variableTypePair.second.isIntegerType()) {
                    symbolTable.add_variable(variableTypePair.first.getName(), this->integerValues[variableTypePair.first.getOffset()]);
                } else if (variableTypePair.second.isRationalType()) {
                    symbolTable.add_variable(variableTypePair.first.getName(), this->rationalValues[variableTypePair.first.getOffset()]);
                }
            }
            symbolTable.add_constants();
        }
        
        template<typename RationalType>
        bool ExprtkExpressionEvaluatorBase<RationalType>::asBool(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return compiledExpression.value() == ValueType(1);
            }
            return expressionPair->second.value() == ValueType(1);
        }
        
        template<typename RationalType>
        int_fast64_t ExprtkExpressionEvaluatorBase<RationalType>::asInt(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return static_cast<int_fast64_t>(compiledExpression.value());
            }
            return static_cast<int_fast64_t>(expressionPair->second.value());
        }
        
        template<typename RationalType>
        typename ExprtkExpressionEvaluatorBase<RationalType>::CompiledExpressionType& ExprtkExpressionEvaluatorBase<RationalType>::getCompiledExpression(BaseExpression const* expression) const {
            std::pair<CacheType::iterator, bool> result = this->compiledExpressions.emplace(expression, CompiledExpressionType());
            CompiledExpressionType& compiledExpression = result.first->second;
            compiledExpression.register_symbol_table(symbolTable);
            bool parsingOk = parser.compile(ToExprtkStringVisitor().toString(expression), compiledExpression);
            STORM_LOG_ASSERT(parsingOk, "Expression was not properly parsed by ExprTk.");
            return compiledExpression;
        }
        
        template<typename RationalType>
        void ExprtkExpressionEvaluatorBase<RationalType>::setBooleanValue(storm::expressions::Variable const& variable, bool value) {
            this->booleanValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
        
        template<typename RationalType>
        void ExprtkExpressionEvaluatorBase<RationalType>::setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) {
            this->integerValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
        
        template<typename RationalType>
        void ExprtkExpressionEvaluatorBase<RationalType>::setRationalValue(storm::expressions::Variable const& variable, double value) {
            this->rationalValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
        
        ExprtkExpressionEvaluator::ExprtkExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExprtkExpressionEvaluatorBase<double>(manager) {
            // Intentionally left empty.
        }
        
        double ExprtkExpressionEvaluator::asRational(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return static_cast<double>(compiledExpression.value());
            }
            return static_cast<double>(expressionPair->second.value());
        }
        
        template class ExprtkExpressionEvaluatorBase<double>;
#ifdef STORM_HAVE_CARL
        template class ExprtkExpressionEvaluatorBase<RationalFunction>;
#endif
    }
}