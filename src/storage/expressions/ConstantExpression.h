#ifndef STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_

#include "src/storage/expressions/BaseExpression.h"

namespace storm {
    namespace expressions {
        class ConstantExpression : public BaseExpression {
            ConstantExpression(ExpressionReturnType returnType, std::string const& constantName);
            virtual ~ConstantExpression() = default;
            
            std::string const& getConstantName() const;
            
        private:
            std::string constantName;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_CONSTANTEXPRESSION_H_ */
