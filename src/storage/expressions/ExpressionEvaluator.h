#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_

#include <unordered_map>

#include "src/storage/parameters.h"
#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/expressions/ToRationalFunctionVisitor.h"

namespace storm {
    namespace expressions {
        template<typename RationalType>
        class ExpressionEvaluator;
        
        template<>
        class ExpressionEvaluator<double> : public ExprtkExpressionEvaluator  {
        public:
            ExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
        };
        
#ifdef STORM_HAVE_CARL
        template<>
        class ExpressionEvaluator<RationalFunction> : public ExprtkExpressionEvaluatorBase<RationalFunction> {
        public:
            ExpressionEvaluator(storm::expressions::ExpressionManager const& manager);
            
            void setBooleanValue(storm::expressions::Variable const& variable, bool value) override;
            void setIntegerValue(storm::expressions::Variable const& variable, int_fast64_t value) override;
            void setRationalValue(storm::expressions::Variable const& variable, double value) override;
            
            RationalFunction asRational(Expression const& expression) const override;
            
        private:
            // A mapping of variables to their expressions.
            std::unordered_map<storm::expressions::Variable, storm::expressions::Expression> variableToExpressionMap;
            
            // A visitor that can be used to translate expressions to rational functions.
            mutable ToRationalFunctionVisitor<RationalFunction> rationalFunctionVisitor;
        };
#endif
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONEVALUATOR_H_ */