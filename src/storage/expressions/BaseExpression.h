#ifndef STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_

#include "src/storage/expressions/Valuation.h"
#include "src/storage/expressions/ExpressionVisitor.h"

namespace storm {
    namespace expressions {
        class BaseExpression {
        public:
            /*!
             * Each node in an expression tree has a uniquely defined type from this enum.
             */
            enum ReturnType {undefined, bool_, int_, double_};
            
            virtual int_fast64_t evaluateAsInt(Valuation const& evaluation) const = 0;
            
            virtual bool evaluateAsBool(Valuation const& evaluation) const = 0;
            
            virtual double evaluateAsDouble(Valuation const& evaluation) const = 0;
            
            virtual std::unique_ptr<BaseExpression> operator+(BaseExpression const& other) const = 0;
            
            virtual void visit(ExpressionVisitor* visitor) const = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_ */