#ifndef STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_

#include "src/storage/expressions/ConstantExpression.h"

namespace storm {
    namespace expressions {
        class BooleanConstantExpression : public ConstantExpression {
        public:
            BooleanConstantExpression(std::string const& constantName);
            virtual ~BooleanConstantExpression() = default;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_BOOLEANCONSTANTEXPRESSION_H_ */
