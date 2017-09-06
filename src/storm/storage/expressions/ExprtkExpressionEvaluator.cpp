#include <string>

#include "storm/storage/expressions/ExprtkExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace expressions {
        template<typename RationalType>
        ExprtkExpressionEvaluatorBase<RationalType>::ExprtkExpressionEvaluatorBase(storm::expressions::ExpressionManager const& manager) : ExpressionEvaluatorBase<RationalType>(manager), parser(std::make_unique<exprtk::parser<ValueType>>()), symbolTable(std::make_unique<exprtk::symbol_table<ValueType>>()), booleanValues(manager.getNumberOfBooleanVariables()), integerValues(manager.getNumberOfIntegerVariables()), rationalValues(manager.getNumberOfRationalVariables()) {

            for (auto const& variableTypePair : manager) {
                if (variableTypePair.second.isBooleanType()) {
                    symbolTable->add_variable("v" + std::to_string(variableTypePair.first.getIndex()), this->booleanValues[variableTypePair.first.getOffset()]);
                } else if (variableTypePair.second.isIntegerType()) {
                    symbolTable->add_variable("v" + std::to_string(variableTypePair.first.getIndex()), this->integerValues[variableTypePair.first.getOffset()]);
                } else if (variableTypePair.second.isRationalType()) {
                    symbolTable->add_variable("v" + std::to_string(variableTypePair.first.getIndex()), this->rationalValues[variableTypePair.first.getOffset()]);
                }
            }
        }
        
        template<typename RationalType>
        bool ExprtkExpressionEvaluatorBase<RationalType>::asBool(Expression const& expression) const {
            std::shared_ptr<BaseExpression const> expressionPtr = expression.getBaseExpressionPointer();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expression);
                return compiledExpression.value() == ValueType(1);
            }
            return expressionPair->second.value() == ValueType(1);
        }
        
        template<typename RationalType>
        int_fast64_t ExprtkExpressionEvaluatorBase<RationalType>::asInt(Expression const& expression) const {
            std::shared_ptr<BaseExpression const> expressionPtr = expression.getBaseExpressionPointer();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expression);
                return static_cast<int_fast64_t>(compiledExpression.value());
            }
            return static_cast<int_fast64_t>(expressionPair->second.value());
        }
        
        template<typename RationalType>
        typename ExprtkExpressionEvaluatorBase<RationalType>::CompiledExpressionType& ExprtkExpressionEvaluatorBase<RationalType>::getCompiledExpression(storm::expressions::Expression const& expression) const {
            std::pair<CacheType::iterator, bool> result = this->compiledExpressions.emplace(expression.getBaseExpressionPointer(), CompiledExpressionType());
            CompiledExpressionType& compiledExpression = result.first->second;
            compiledExpression.register_symbol_table(*symbolTable);
            bool parsingOk = parser->compile(ToExprtkStringVisitor().toString(expression), compiledExpression);
            STORM_LOG_THROW(parsingOk, storm::exceptions::UnexpectedException, "Expression was not properly parsed by ExprTk: " << expression << ". (Returned error: " << parser->error() << ")");
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
            std::shared_ptr<BaseExpression const> expressionPtr = expression.getBaseExpressionPointer();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expression);
                return static_cast<double>(compiledExpression.value());
            }
            return static_cast<double>(expressionPair->second.value());
        }
        
        template class ExprtkExpressionEvaluatorBase<double>;

#ifdef STORM_HAVE_CARL
        template class ExprtkExpressionEvaluatorBase<RationalNumber>;
        template class ExprtkExpressionEvaluatorBase<RationalFunction>;
#endif
    }
}
