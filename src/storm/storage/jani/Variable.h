#pragma once

#include <cstdint>
#include <string>
#include <boost/optional.hpp>

#include "storm/storage/expressions/Variable.h"
#include "storm/storage/expressions/Type.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/jani/types/AllJaniTypes.h"

namespace storm {
    namespace jani {
        class Variable {
        public:
            /*!
             * Creates a new variable with initial value construct
             */
            Variable(std::string const& name, JaniType* type, storm::expressions::Variable const& variable, storm::expressions::Expression const& init, bool transient = false);

            /*!
             * Creates a new variable without initial value construct.
             */
            Variable(std::string const& name, JaniType* type, storm::expressions::Variable const& variable);
            
            /*!
             * Clones the variable.
             */
            std::unique_ptr<Variable> clone() const;
            
            /*!
             * Retrieves the associated expression variable
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Sets the associated expression variable.
             */
            void setExpressionVariable(storm::expressions::Variable const& newVariable);
            
            /*!
             * Retrieves the name of the variable.
             */
            std::string const& getName() const;

            /*!
             * Sets the name of the variable.
             */
            void setName(std::string const& newName);
            
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
            
            // Methods to determine the type of the variable.
            bool isBooleanVariable() const;
            bool isIntegerVariable() const;
            bool isBoundedVariable() const;
            bool isRealVariable() const;
            bool isArrayVariable() const;
            bool isClockVariable() const;
            bool isContinuousVariable() const;

            bool isTransient() const;

            JaniType* getType() const;
            JaniType* getArrayType() const;

            ~Variable();

            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);

            /**
         * Convenience function to call the appropriate constructor and return a shared pointer to the variable.
         */
            static std::shared_ptr<Variable> makeBoundedVariable(std::string const& name, JaniType::ElementType type, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient, boost::optional<storm::expressions::Expression> lowerBound, boost::optional<storm::expressions::Expression> upperBound);
            static std::shared_ptr<Variable> makeArrayVariable(std::string const& name, JaniType* type, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);
            static std::shared_ptr<Variable> makeBasicVariable(std::string const& name, JaniType::ElementType type, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);
            static std::shared_ptr<Variable> makeClockVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient);

        private:
            // The name of the variable.
            std::string name;
            
            // The expression variable associated with this variable.
            storm::expressions::Variable variable;

            JaniType* type;

            JaniType* arrayType;

            /// Whether this is a transient variable.
            bool transient;

            /// Expression for initial values
            boost::optional<storm::expressions::Expression> init;

            // The expression defining the lower bound of the variable.
            storm::expressions::Expression lowerBound;

            // The expression defining the upper bound of the variable.
            storm::expressions::Expression upperBound;
        };

        bool operator==(Variable const& lhs, Variable const& rhs);
        bool operator!=(Variable const& lhs, Variable const& rhs);
        
    }
}
