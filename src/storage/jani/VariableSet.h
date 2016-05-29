#pragma once

#include <vector>
#include <set>

#include <boost/variant.hpp>

#include "src/storage/jani/BooleanVariable.h"
#include "src/storage/jani/UnboundedIntegerVariable.h"
#include "src/storage/jani/BoundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        class VariableSet;
        
        namespace detail {
            
            class VariableSetIterator {
            private:
                typedef std::vector<BooleanVariable>::iterator bool_iter;
                typedef std::vector<BoundedIntegerVariable>::iterator bint_iter;
                typedef std::vector<UnboundedIntegerVariable>::iterator int_iter;
                
            public:
                /*!
                 * Creates an iterator over all variables.
                 */
                VariableSetIterator(VariableSet& variableSet, boost::variant<bool_iter, bint_iter, int_iter> initialIterator);
                
                // Methods to advance the iterator.
                VariableSetIterator& operator++();
                VariableSetIterator& operator++(int);
                
                Variable& operator*();

                bool operator==(VariableSetIterator const& other) const;
                bool operator!=(VariableSetIterator const& other) const;
                
            private:
                // Moves the iterator to the next position.
                void incrementIterator();
                
                // The underlying variable set.
                VariableSet& variableSet;

                // The current iterator position.
                boost::variant<bool_iter, bint_iter, int_iter> it;
            };
            
            class ConstVariableSetIterator {
            private:
                typedef std::vector<BooleanVariable>::const_iterator bool_iter;
                typedef std::vector<BoundedIntegerVariable>::const_iterator bint_iter;
                typedef std::vector<UnboundedIntegerVariable>::const_iterator int_iter;
                
            public:
                /*!
                 * Creates an iterator over all variables.
                 */
                ConstVariableSetIterator(VariableSet const& variableSet, boost::variant<bool_iter, bint_iter, int_iter> initialIterator);
                
                // Methods to advance the iterator.
                ConstVariableSetIterator& operator++();
                ConstVariableSetIterator& operator++(int);
                
                Variable const& operator*();
                
                bool operator==(ConstVariableSetIterator const& other) const;
                bool operator!=(ConstVariableSetIterator const& other) const;
                
            private:
                // Moves the iterator to the next position.
                void incrementIterator();
                
                // The underlying variable set.
                VariableSet const& variableSet;
                
                // The current iterator position.
                boost::variant<bool_iter, bint_iter, int_iter> it;
            };
            
            class IntegerVariables {
            public:
                IntegerVariables(VariableSet& variableSet);
                
                /*!
                 * Retrieves an iterator to all integer variables (bounded and unbounded) in the variable set.
                 */
                VariableSetIterator begin();

                /*!
                 * Retrieves the end iterator to all integer variables (bounded and unbounded) in the variable set.
                 */
                VariableSetIterator end();
                
            private:
                // The underlying variable set.
                VariableSet& variableSet;
            };
            
            class ConstIntegerVariables {
            public:
                ConstIntegerVariables(VariableSet const& variableSet);
                
                /*!
                 * Retrieves an iterator to all integer variables (bounded and unbounded) in the variable set.
                 */
                ConstVariableSetIterator begin() const;
                
                /*!
                 * Retrieves the end iterator to all integer variables (bounded and unbounded) in the variable set.
                 */
                ConstVariableSetIterator end() const;
                
            private:
                // The underlying variable set.
                VariableSet const& variableSet;
            };
        }
        
        class VariableSet {
        public:
            friend class detail::VariableSetIterator;
            
            typedef detail::VariableSetIterator iterator;
            typedef detail::ConstVariableSetIterator const_iterator;
            typedef detail::IntegerVariables IntegerVariables;
            typedef detail::ConstIntegerVariables ConstIntegerVariables;
            
            /*!
             * Creates an empty variable set.
             */
            VariableSet();
            
            /*!
             * Retrieves the boolean variables in this set.
             */
            std::vector<BooleanVariable>& getBooleanVariables();

            /*!
             * Retrieves the boolean variables in this set.
             */
            std::vector<BooleanVariable> const& getBooleanVariables() const;

            /*!
             * Retrieves the bounded integer variables in this set.
             */
            std::vector<BoundedIntegerVariable>& getBoundedIntegerVariables();

            /*!
             * Retrieves the bounded integer variables in this set.
             */
            std::vector<BoundedIntegerVariable> const& getBoundedIntegerVariables() const;

            /*!
             * Retrieves the unbounded integer variables in this set.
             */
            std::vector<UnboundedIntegerVariable>& getUnboundedIntegerVariables();

            /*!
             * Retrieves the unbounded integer variables in this set.
             */
            std::vector<UnboundedIntegerVariable> const& getUnboundedIntegerVariables() const;

            /*!
             * Retrieves an iterable object to all integer (bounded and unbounded) variables in the variable set.
             */
            IntegerVariables getIntegerVariables();
            
            /*!
             * Retrieves an iterable object to all integer (bounded and unbounded) variables in the variable set.
             */
            ConstIntegerVariables getIntegerVariables() const;
            
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

        private:
            /// The boolean variables in this set.
            std::vector<BooleanVariable> booleanVariables;

            /// The bounded integer variables in this set.
            std::vector<BoundedIntegerVariable> boundedIntegerVariables;

            /// The unbounded integer variables in this set.
            std::vector<UnboundedIntegerVariable> unboundedIntegerVariables;
            
            /// A set of all variable names currently in use.
            std::map<std::string, storm::expressions::Variable> nameToVariable;
            
            /// A mapping from expression variables to their variable objects.
            std::map<storm::expressions::Variable, std::pair<uint8_t, uint64_t>> variableToVariable;
        };
        
    }
}