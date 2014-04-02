#ifndef STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_

#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        class DoubleConstantExpression : public ConstantExpression {
        public:
            DoubleConstantExpression(std::string const& constantName);
            virtual ~DoubleConstantExpression() = default;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_DOUBLECONSTANTEXPRESSION_H_ */
