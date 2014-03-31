#ifndef STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_

#include "src/storage/expressions/Valuation.h"

namespace storm {
    namespace expressions {
        class BaseExpression {
        public:
            /*!
             * Each node in an expression tree has a uniquely defined type from this enum.
             */
            enum ReturnType {undefined, bool_, int_, double_};
            
            std::unique_ptr<BaseExpression> substitute() const = 0;
            
            virtual int_fast64_t evaluateAsInt(Evaluation const& evaluation) const = 0;
            
            virtual bool evaluateAsBool(Evaluation const& evaluation) const = 0;
            
            virtual double evaluateAsDouble(Evaluation const& evaluation) const = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BASEEXPRESSION_H_ */