#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_

namespace storm {
    namespace expressions {
        class Expression {
            Expression() = default;
            
            // Static factory methods to create atomic expression parts.
            
            // Virtual operator overloading.
            virtual Expression operator+(Expression const& other);
            
            /*!
             * Substitutes all occurrences of identifiers according to the given substitution. Note that this
             * substitution is done simultaneously, i.e., identifiers appearing in the expressions that were "plugged
             * in" are not substituted.
             *
             * @param substitutionFilter A function that returns true iff the given identifier is supposed to be
             * substituted.
             * @param substitution A substitution that returns for each identifier an expression that is supposed to
             * replace the identifier.
             * @return An expression in which all identifiers
             */
            Expression substitute(std::function<Expression (std::string const&)> const& substitution) const;
            
        private:
            /*!
             * Retrieves the base expression underlying this expression object. Note that prior to calling this, the
             * expression object must be properly initialized.
             *
             * @return A reference to the underlying base expression.
             */
            BaseExpression const& getBaseExpression() const;
            
            // A pointer to the underlying base expression.
            std::unique_ptr<BaseExpression> expressionPtr;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSION_H_ */