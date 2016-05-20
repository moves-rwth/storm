#pragma once

#include <vector>
#include <set>

#include "src/storage/jani/BooleanVariable.h"
#include "src/storage/jani/UnboundedIntegerVariable.h"
#include "src/storage/jani/BoundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        class VariableSet {
        public:
            /*!
             * Creates an empty variable set.
             */
            VariableSet();
            
            /*!
             * Retrieves the boolean variables in this set.
             */
            std::vector<BooleanVariable> const& getBooleanVariables() const;

            /*!
             * Retrieves the bounded integer variables in this set.
             */
            std::vector<BoundedIntegerVariable> const& getBoundedIntegerVariables() const;

            /*!
             * Retrieves the unbounded integer variables in this set.
             */
            std::vector<UnboundedIntegerVariable> const& getUnboundedIntegerVariables() const;
            
            /*!
             * Adds the given boolean variable to this set.
             */
            void addBooleanVariable(BooleanVariable const& variable);

            /*!
             * Adds the given bounded integer variable to this set.
             */
            void addBoundedIntegerVariable(BoundedIntegerVariable const& variable);

            /*!
             * Adds the given unbounded integer variable to this set.
             */
            void addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable);

            /*!
             * Retrieves whether this variable set contains a variable with the given name.
             */
            bool hasVariable(std::string const& name) const;
            
            /*!
             * Retrieves the variable with the given name.
             */
            Variable const& getVariable(std::string const& name) const;
            
        private:
            // The boolean variables in this set.
            std::vector<BooleanVariable> booleanVariables;

            // The bounded integer variables in this set.
            std::vector<BoundedIntegerVariable> boundedIntegerVariables;

            // The unbounded integer variables in this set.
            std::vector<UnboundedIntegerVariable> unboundedIntegerVariables;
            
            // A set of all variable names currently in use.
            std::map<std::string, std::reference_wrapper<Variable>> variables;
        };
        
    }
}