#pragma once

#include "storm/storage/expressions/SubstitutionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"


namespace storm {
    namespace expressions {
        template<typename MapType>
        class JaniExpressionVisitor{
        public:
            virtual boost::any visit(ValueArrayExpression const& expression, boost::any const& data) = 0;
            virtual boost::any visit(ConstructorArrayExpression const& expression, boost::any const& data) = 0;
            virtual boost::any visit(ArrayAccessExpression const& expression, boost::any const& data) = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_ */
