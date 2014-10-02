#ifndef STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHVALUATIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHVALUATIONVISITOR_H_

#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/IdentifierSubstitutionVisitorBase.h"

namespace storm {
    namespace expressions {
        class IdentifierSubstitutionWithValuationVisitor : public IdentifierSubstitutionVisitorBase {
        public:
            /*!
             * Creates a new substitution visitor that uses the given map to replace identifiers.
             *
             * @param identifierToExpressionMap A mapping from identifiers to expressions.
             */
            IdentifierSubstitutionWithValuationVisitor(Valuation const& valuation);
            
            virtual void visit(VariableExpression const* expression) override;
            
        private:
            // A mapping of identifier names to the values with which they shall be replaced.
            Valuation const& valuation;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_IDENTIFIERSUBSTITUTIONWITHVALUATIONVISITOR_H_ */