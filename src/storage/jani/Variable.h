#pragma once

#include <cstdint>
#include <string>
#include <boost/optional.hpp>

#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class BooleanVariable;
        class BoundedIntegerVariable;
        class UnboundedIntegerVariable;
        
        class Variable {
        public:
            /*!
             * Creates a new variable with initial value construct
             */
            Variable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& init, bool transient = false);

            /*!
             * Creates a new variable without initial value construct.
             */
            Variable(std::string const& name, storm::expressions::Variable const& variable, bool transient = false);
            
            /*!
             * Retrieves the associated expression variable
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Retrieves the name of the variable.
             */
            std::string const& getName() const;

            /*!
             * Retrieves whether an initial expression is set.
             */
            bool hasInitExpression() const;

            /*!
             * Retrieves the initial expression
             * Should only be called if an initial expression is set for this variable.
             *
             * @see hasInitExpression()
             */
            storm::expressions::Expression const& getInitExpression() const;
            
            /*!
             * Sets the initial expression for this variable.
             */
            void setInitExpression(storm::expressions::Expression const& initialExpression);
            
            // Methods to determine the type of the variable.
            virtual bool isBooleanVariable() const;
            virtual bool isBoundedIntegerVariable() const;
            virtual bool isUnboundedIntegerVariable() const;

            virtual bool isTransientVariable() const;
            
            // Methods to get the variable as a different type.
            BooleanVariable& asBooleanVariable();
            BooleanVariable const& asBooleanVariable() const;
            BoundedIntegerVariable& asBoundedIntegerVariable();
            BoundedIntegerVariable const& asBoundedIntegerVariable() const;
            UnboundedIntegerVariable& asUnboundedIntegerVariable();
            UnboundedIntegerVariable const& asUnboundedIntegerVariable() const;
            
        private:
            // The name of the variable.
            std::string name;
            
            // The expression variable associated with this variable.
            storm::expressions::Variable variable;

            /// Whether this is a transient variable.
            bool transient;

            /// Expression for initial values
            boost::optional<storm::expressions::Expression> init;
        };
        
    }
}