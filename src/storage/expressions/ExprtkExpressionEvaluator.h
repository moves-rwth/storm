#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_

#include <unordered_map>
#include <vector>

#include "exprtk.hpp"

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExpressionVisitor.h"
#include "src/storage/expressions/ToExprtkStringVisitor.h"

namespace storm {
    namespace expressions {
        class ExprtkExpressionEvaluator {
        public:
            /*!
             * Creates an expression evaluator that is capable of evaluating expressions managed by the given manager.
             *
             * @param manager The manager responsible for the expressions.
             */
            ExprtkExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
            
            bool asBool(Expression const& expression);
            int_fast64_t asInt(Expression const& expression);
            double asDouble(Expression const& expression);

            void setBooleanValue(storm::expressions::Variable const& variable, bool value);
            void setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value);
            void setRationalValue(storm::expressions::Variable const& variable, double value);
            
        private:
            typedef double ValueType;
            typedef exprtk::expression<ValueType> CompiledExpressionType;
            typedef std::unordered_map<BaseExpression const*, CompiledExpressionType> CacheType;
            
            /*!
             * Adds a compiled version of the given expression to the internal storage.
             *
             * @param expression The expression that is to be compiled.
             */
            CompiledExpressionType& getCompiledExpression(BaseExpression const* expression);
            
            // The expression manager that is used by this evaluator.
            std::shared_ptr<storm::expressions::ExpressionManager const> manager;
            
            // The parser used.
            exprtk::parser<ValueType> parser;
            
            // The symbol table used.
            exprtk::symbol_table<ValueType> symbolTable;
            
            // The actual data that is fed into the expression.
            std::vector<ValueType> booleanValues;
            std::vector<ValueType> integerValues;
            std::vector<ValueType> rationalValues;
            
            // A mapping of expressions to their compiled counterpart.
            CacheType compiledExpressions;

            // A translator that can be used for transforming an expression into the correct string format.
            ToExprtkStringVisitor stringTranslator;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_ */