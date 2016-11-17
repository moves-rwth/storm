#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_

#include <unordered_map>

#include "src/adapters/CarlAdapter.h"
#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/expressions/ToRationalFunctionVisitor.h"
#include "src/storage/expressions/ToRationalNumberVisitor.h"

namespace storm {
    namespace expressions {
        template<typename RationalType>
        class ExpressionEvaluator;
        
        template<>
        class ExpressionEvaluator<double> : public ExprtkExpressionEvaluator  {
        public:
            ExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
        };
        
        template<typename RationalType>
        class ExpressionEvaluatorWithVariableToExpressionMap : public ExprtkExpressionEvaluatorBase<RationalType> {
        public:
            ExpressionEvaluatorWithVariableToExpressionMap(storm::expressions::ExpressionManager const& manager);

            void setBooleanValue(storm::expressions::Variable const& variable, bool value) override;
            void setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) override;
            void setRationalValue(storm::expressions::Variable const& variable, double value) override;

        protected:
            // A mapping of variables to their expressions.
            std::unordered_map<storm::expressions::Variable, storm::expressions::Expression> variableToExpressionMap;
        };
        
#ifdef STORM_HAVE_CARL
        template<>
        class ExpressionEvaluator<RationalNumber> : public ExpressionEvaluatorWithVariableToExpressionMap<RationalNumber> {
        public:
            ExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
            
            RationalNumber asRational(Expression const& expression) const override;
            
        private:
            // A visitor that can be used to translate expressions to rational numbers.
            mutable ToRationalNumberVisitor<RationalNumber> rationalNumberVisitor;
        };
        
        template<>
        class ExpressionEvaluator<RationalFunction> : public ExpressionEvaluatorWithVariableToExpressionMap<RationalFunction> {
        public:
            ExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
            
            RationalFunction asRational(Expression const& expression) const override;
            
        private:
            // A visitor that can be used to translate expressions to rational functions.
            mutable ToRationalFunctionVisitor<RationalFunction> rationalFunctionVisitor;
        };
#endif
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_ */
