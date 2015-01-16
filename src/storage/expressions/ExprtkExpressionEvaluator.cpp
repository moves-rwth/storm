#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/expressions/ExpressionManager.h"

#include "src/utility/macros.h"

namespace storm {
    namespace expressions {
        ExprtkExpressionEvaluator::ExprtkExpressionEvaluator(storm::expressions::ExpressionManager const& manager) : ExpressionEvaluatorBase(manager), booleanValues(manager.getNumberOfBooleanVariables()), integerValues(manager.getNumberOfIntegerVariables()), rationalValues(manager.getNumberOfRationalVariables()) {

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
        
        bool ExprtkExpressionEvaluator::asBool(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return compiledExpression.value() == ValueType(1);
            }
            return expressionPair->second.value() == ValueType(1);
        }
        
        int_fast64_t ExprtkExpressionEvaluator::asInt(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return static_cast<int_fast64_t>(compiledExpression.value());
            }
            return static_cast<int_fast64_t>(expressionPair->second.value());
        }
        
        double ExprtkExpressionEvaluator::asDouble(Expression const& expression) const {
            BaseExpression const* expressionPtr = expression.getBaseExpressionPointer().get();
            auto const& expressionPair = this->compiledExpressions.find(expression.getBaseExpressionPointer().get());
            if (expressionPair == this->compiledExpressions.end()) {
                CompiledExpressionType const& compiledExpression = this->getCompiledExpression(expressionPtr);
                return static_cast<double>(compiledExpression.value());
            }
            return static_cast<double>(expressionPair->second.value());
        }
        
        ExprtkExpressionEvaluator::CompiledExpressionType& ExprtkExpressionEvaluator::getCompiledExpression(BaseExpression const* expression) const {
            std::pair<CacheType::iterator, bool> result = this->compiledExpressions.emplace(expression, CompiledExpressionType());
            CompiledExpressionType& compiledExpression = result.first->second;
            compiledExpression.register_symbol_table(symbolTable);
            bool parsingOk = parser.compile(ToExprtkStringVisitor().toString(expression), compiledExpression);
            STORM_LOG_ASSERT(parsingOk, "Expression was not properly parsed by ExprTk.");
            return compiledExpression;
        }
        
        void ExprtkExpressionEvaluator::setBooleanValue(storm::expressions::Variable const& variable, bool value) {
            this->booleanValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
        
        void ExprtkExpressionEvaluator::setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) {
            this->integerValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
        
        void ExprtkExpressionEvaluator::setRationalValue(storm::expressions::Variable const& variable, double value) {
            this->rationalValues[variable.getOffset()] = static_cast<ValueType>(value);
        }
    }
}