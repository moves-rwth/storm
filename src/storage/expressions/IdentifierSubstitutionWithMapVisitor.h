#ifndef STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHMAPVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHMAPVISITOR_H_

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/IdentifierSubstitutionVisitorBase.h"

namespace storm {
    namespace expressions {
        template<typename MapType>
        class IdentifierSubstitutionWithMapVisitor : public IdentifierSubstitutionVisitorBase {
        public:
            /*!
             * Creates a new substitution visitor that uses the given map to replace identifiers.
             *
             * @param identifierToExpressionMap A mapping from identifiers to expressions.
             */
            IdentifierSubstitutionWithMapVisitor(MapType const& identifierToExpressionMap);
            
            virtual void visit(VariableExpression const* expression) override;
            
        private:
            // A mapping of identifier names to expressions with which they shall be replaced.
            MapType const& identifierToIdentifierMap;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHMAPVISITOR_H_ */