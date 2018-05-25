#pragma once

#include "storm/storage/jani/Variable.h"
#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class BoundedIntegerVariable : public Variable {
        public:
            /*!
             * Creates a bounded integer variable with initial value.
             */
            BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound);
            /*!
             * Creates a bounded integer variable with transient set to false and an initial value.
             */
            BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound);
            /*!
             * Creates a bounded integer variable with transient set to false and no initial value.
             */
            BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound);

            virtual std::unique_ptr<Variable> clone() const override;
            
            /*!
             * Retrieves the expression defining the lower bound of the variable.
             */
            storm::expressions::Expression const& getLowerBound() const;

            /*!
             * Sets a new lower bound of the variable.
             */
            void setLowerBound(storm::expressions::Expression const& expression);
            
            /*!
             * Retrieves whether the variable has a lower bound.
             */
            bool hasLowerBound() const;
            
            /*!
             * Retrieves the expression defining the upper bound of the variable.
             */
            storm::expressions::Expression const& getUpperBound() const;

            /*!
             * Sets a new upper bound of the variable.
             */
            void setUpperBound(storm::expressions::Expression const& expression);

            /*!
             * Retrieves whether the variable has an upper bound.
             */
            bool hasUpperBound() const;
            
            /*!
             * Retrieves an expression characterizing the legal range of the bounded integer variable.
             */
            storm::expressions::Expression getRangeExpression() const;
            
            virtual bool isBoundedIntegerVariable() const override;

            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            virtual void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) override;
            
        private:
            // The expression defining the lower bound of the variable.
            storm::expressions::Expression lowerBound;
            
            // The expression defining the upper bound of the variable.
            storm::expressions::Expression upperBound;
        };
        
        /**
         * Convenience function to call the appropriate constructor and return a shared pointer to the variable.
         */
        std::shared_ptr<BoundedIntegerVariable> makeBoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient, boost::optional<storm::expressions::Expression> lowerBound, boost::optional<storm::expressions::Expression> upperBound);
    }
}
