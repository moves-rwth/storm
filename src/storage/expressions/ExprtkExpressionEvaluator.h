#ifndef STORM_STORAGE_EXPRESSIONS_EXPRTKEXPRESSIONEVALUATOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRTKEXPRESSIONEVALUATOR_H_

#include <memory>
#include <unordered_map>
#include <vector>

#include "src/storage/expressions/ExpressionEvaluatorBase.h"

#include "exprtk.hpp"
#include "src/storage/expressions/ToExprtkStringVisitor.h"

namespace storm {
    namespace expressions {
        template <typename RationalType>
        class ExprtkExpressionEvaluatorBase : public ExpressionEvaluatorBase<RationalType> {
        public:
            ExprtkExpressionEvaluatorBase(storm::expressions::ExpressionManager const& manager);
            
            bool asBool(Expression const& expression) const override;
            int_fast64_t asInt(Expression const& expression) const override;

            void setBooleanValue(storm::expressions::Variable const& variable, bool value) override;
            void setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) override;
            void setRationalValue(storm::expressions::Variable const& variable, double value) override;

        protected:
            typedef double ValueType;
            typedef exprtk::expression<ValueType> CompiledExpressionType;
            typedef std::unordered_map<std::shared_ptr<BaseExpression const>, CompiledExpressionType> CacheType;
            
            /*!
             * Adds a compiled version of the given expression to the internal storage.
             *
             * @param expression The expression that is to be compiled.
             */
            CompiledExpressionType& getCompiledExpression(storm::expressions::Expression const& expression) const;
            
            // The parser used.
            mutable std::unique_ptr<exprtk::parser<ValueType>> parser;
            
            // The symbol table used.
            mutable std::unique_ptr<exprtk::symbol_table<ValueType>> symbolTable;
            
            // The actual data that is fed into the expression.
            std::vector<ValueType> booleanValues;
            std::vector<ValueType> integerValues;
            std::vector<ValueType> rationalValues;
            
            // A mapping of expressions to their compiled counterpart.
            mutable CacheType compiledExpressions;
        };
        
        class ExprtkExpressionEvaluator : public ExprtkExpressionEvaluatorBase<double> {
        public:
            /*!
             * Creates an expression evaluator that is capable of evaluating expressions managed by the given manager.
             *
             * @param manager The manager responsible for the expressions.
             */
            ExprtkExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
            
            double asRational(Expression const& expression) const override;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRTKEXPRESSIONEVALUATOR_H_ */