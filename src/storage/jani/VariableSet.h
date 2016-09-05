#pragma once

#include <vector>
#include <set>

#include <boost/iterator/transform_iterator.hpp>

#include "src/storage/jani/BooleanVariable.h"
#include "src/storage/jani/UnboundedIntegerVariable.h"
#include "src/storage/jani/BoundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        class VariableSet;
        
        namespace detail {
            
            template<typename VariableType>
            class Dereferencer {
            public:
                VariableType& operator()(std::shared_ptr<VariableType> const& d) const;
            };
            
            template<typename VariableType>
            class Variables {
            public:
                typedef typename std::vector<std::shared_ptr<VariableType>>::iterator input_iterator;
                typedef boost::transform_iterator<Dereferencer<VariableType>, input_iterator> iterator;
                
                Variables(input_iterator it, input_iterator ite);
                
                iterator begin();
                iterator end();
                
            private:
                input_iterator it;
                input_iterator ite;
            };

            template<typename VariableType>
            class ConstVariables {
            public:
                typedef typename std::vector<std::shared_ptr<VariableType>>::const_iterator const_input_iterator;
                typedef boost::transform_iterator<Dereferencer<VariableType const>, const_input_iterator> const_iterator;
                
                ConstVariables(const_input_iterator it, const_input_iterator ite);
                
                const_iterator begin();
                const_iterator end();
                
            private:
                const_input_iterator it;
                const_input_iterator ite;
            };
        }
        
        class VariableSet {
        public:
            typedef typename std::vector<std::shared_ptr<Variable>>::iterator input_iterator;
            typedef typename std::vector<std::shared_ptr<Variable>>::const_iterator const_input_iterator;
            typedef boost::transform_iterator<detail::Dereferencer<Variable>, input_iterator> iterator;
            typedef boost::transform_iterator<detail::Dereferencer<Variable const>, const_input_iterator> const_iterator;
            
            /*!
             * Creates an empty variable set.
             */
            VariableSet();
            
            /*!
             * Retrieves the boolean variables in this set.
             */
            detail::Variables<BooleanVariable> getBooleanVariables();

            /*!
             * Retrieves the boolean variables in this set.
             */
            detail::ConstVariables<BooleanVariable> getBooleanVariables() const;

            /*!
             * Retrieves the bounded integer variables in this set.
             */
            detail::Variables<BoundedIntegerVariable> getBoundedIntegerVariables();

            /*!
             * Retrieves the bounded integer variables in this set.
             */
            detail::ConstVariables<BoundedIntegerVariable> getBoundedIntegerVariables() const;

            /*!
             * Retrieves the unbounded integer variables in this set.
             */
            detail::Variables<UnboundedIntegerVariable> getUnboundedIntegerVariables();

            /*!
             * Retrieves the unbounded integer variables in this set.
             */
            detail::ConstVariables<UnboundedIntegerVariable> getUnboundedIntegerVariables() const;
            
            /*!
             * Adds the given boolean variable to this set.
             */
            BooleanVariable const& addBooleanVariable(BooleanVariable const& variable);

            /*!
             * Adds the given bounded integer variable to this set.
             */
            BoundedIntegerVariable const& addBoundedIntegerVariable(BoundedIntegerVariable const& variable);

            /*!
             * Adds the given unbounded integer variable to this set.
             */
            UnboundedIntegerVariable const& addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable);

            /*!
             * Retrieves whether this variable set contains a variable with the given name.
             */
            bool hasVariable(std::string const& name) const;
            
            /*!
             * Retrieves the variable with the given name.
             */
            Variable const& getVariable(std::string const& name) const;

            /*!
             * Retrieves whether this variable set contains a variable with the expression variable.
             */
            bool hasVariable(storm::expressions::Variable const& variable) const;
            
            /*!
             * Retrieves the variable object associated with the given expression variable (if any).
             */
            Variable const& getVariable(storm::expressions::Variable const& variable) const;

            /*!
             * Retrieves an iterator to the variables in this set.
             */
            iterator begin();

            /*!
             * Retrieves an iterator to the variables in this set.
             */
            const_iterator begin() const;

            /*!
             * Retrieves the end iterator to the variables in this set.
             */
            iterator end();

            /*!
             * Retrieves the end iterator to the variables in this set.
             */
            const_iterator end() const;

            /*!
             * Retrieves whether the set of variables contains a boolean variable.
             */
            bool containsBooleanVariable() const;

            /*!
             * Retrieves whether the set of variables contains a bounded integer variable.
             */
            bool containsBoundedIntegerVariable() const;

            /*!
             * Retrieves whether the set of variables contains an unbounded integer variable.
             */
            bool containsUnboundedIntegerVariables() const;

            /*!
             * Retrieves whether this variable set is empty.
             */
            bool empty() const;
            
        private:
            /// The vector of all variables.
            std::vector<std::shared_ptr<Variable>> variables;
            
            /// The boolean variables in this set.
            std::vector<std::shared_ptr<BooleanVariable>> booleanVariables;

            /// The bounded integer variables in this set.
            std::vector<std::shared_ptr<BoundedIntegerVariable>> boundedIntegerVariables;

            /// The unbounded integer variables in this set.
            std::vector<std::shared_ptr<UnboundedIntegerVariable>> unboundedIntegerVariables;
            
            /// A set of all variable names currently in use.
            std::map<std::string, storm::expressions::Variable> nameToVariable;
            
            /// A mapping from expression variables to their variable objects.
            std::map<storm::expressions::Variable, std::shared_ptr<Variable>> variableToVariable;
        };
        
    }
}
