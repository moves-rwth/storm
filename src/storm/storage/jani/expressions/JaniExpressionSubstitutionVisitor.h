#pragma once

#include "storm/storage/expressions/SubstitutionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"


namespace storm {
    namespace expressions {
        template<typename MapType>
        class JaniExpressionSubstitutionVisitor : public SubstitutionVisitor<MapType>, public ExpressionManager {
        public:
            /*!
             * Creates a new substitution visitor that uses the given map to replace variables.
             *
             * @param variableToExpressionMapping A mapping from variables to expressions.
             */
            JaniExpressionSubstitutionVisitor(MapType const& variableToExpressionMapping);
            
            virtual boost::any visit(ValueArrayExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(ConstructorArrayExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(ArrayAccessExpression const& expression, boost::any const& data) override;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SUBSTITUTIONVISITOR_H_ */
