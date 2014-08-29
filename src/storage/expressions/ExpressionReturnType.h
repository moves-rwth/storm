#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_

#include <iostream>

namespace storm {
    namespace expressions {
        /*!
         * Each node in an expression tree has a uniquely defined type from this enum.
         */
        enum class ExpressionReturnType {Undefined, Bool, Int, Double};
        
        std::ostream& operator<<(std::ostream& stream, ExpressionReturnType const& enumValue);
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONRETURNTYPE_H_ */