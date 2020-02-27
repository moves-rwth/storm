#pragma once

#include <vector>
#include <set>

#include "storm/adapters/DereferenceIteratorAdapter.h"

#include "storm/storage/jani/BooleanVariable.h"
#include "storm/storage/jani/UnboundedIntegerVariable.h"
#include "storm/storage/jani/BoundedIntegerVariable.h"
#include "storm/storage/jani/RealVariable.h"
#include "storm/storage/jani/ArrayVariable.h"
#include "storm/storage/jani/ClockVariable.h"

namespace storm {
    namespace jani {
                
        namespace detail {
            template <typename VariableType>
            using Variables = storm::adapters::DereferenceIteratorAdapter<std::vector<std::shared_ptr<VariableType>>>;

            template <typename VariableType>
            using ConstVariables = storm::adapters::DereferenceIteratorAdapter<std::vector<std::shared_ptr<VariableType>> const>;
        }
        
        class VariableSet {
        public:
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
             * Retrieves the real variables in this set.
             */
            detail::Variables<RealVariable> getRealVariables();
            
            /*!
             * Retrieves the real variables in this set.
             */
            detail::ConstVariables<RealVariable> getRealVariables() const;
            
            /*!
             * Retrieves the Array variables in this set.
             */
            detail::Variables<ArrayVariable> getArrayVariables();
            
            /*!
             * Retrieves the Array variables in this set.
             */
            detail::ConstVariables<ArrayVariable> getArrayVariables() const;
            
            /*!
             * Retrieves the clock variables in this set.
             */
            detail::Variables<ClockVariable> getClockVariables();
            
            /*!
             * Retrieves the clock variables in this set.
             */
            detail::ConstVariables<ClockVariable> getClockVariables() const;

            /*!
             * Adds the given variable to this set.
             */
            Variable const& addVariable(Variable const& variable);

            /*!
             * Adds the given boolean variable to this set.
             */
            BooleanVariable const& addVariable(BooleanVariable const& variable);

            /*!
             * Adds the given bounded integer variable to this set.
             */
            BoundedIntegerVariable const& addVariable(BoundedIntegerVariable const& variable);

            /*!
             * Adds the given unbounded integer variable to this set.
             */
            UnboundedIntegerVariable const& addVariable(UnboundedIntegerVariable const& variable);
            
            /*!
             * Adds the given real variable to this set.
             */
            RealVariable const& addVariable(RealVariable const& variable);

            /*!
             * Adds the given array variable to this set.
             */
            ArrayVariable const& addVariable(ArrayVariable const& variable);

            /*!
             * Removes all array variables in this set
             */
            std::vector<std::shared_ptr<ArrayVariable>> dropAllArrayVariables();
 
            /*!
             * Adds the given clock variable to this set.
             */
            ClockVariable const& addVariable(ClockVariable const& variable);
            
            /*!
             * Retrieves whether this variable set contains a variable with the given name.
             */
            bool hasVariable(std::string const& name) const;
            
            /*!
             * Retrieves the variable with the given name.
             */
            Variable const& getVariable(std::string const& name) const;

            /*!
             * Retrieves whether this variable set contains a given variable.
             */
            bool hasVariable(storm::jani::Variable const& variable) const;
            /*!
             * Retrieves whether this variable set contains a variable with the expression variable.
             */
            bool hasVariable(storm::expressions::Variable const& variable) const;
            
            /*!
             * Retrieves the variable object associated with the given expression variable (if any).
             */
            Variable const& getVariable(storm::expressions::Variable const& variable) const;
            
            /*!
             * Erases the given variable from this set.
             */
             std::shared_ptr<Variable> eraseVariable(storm::expressions::Variable const& variable);

            /*!
             * Retrieves whether this variable set contains a transient variable.
             */
            bool hasTransientVariable() const;
            
            /*!
             * Retrieves an iterator to the variables in this set.
             */
            typename detail::Variables<Variable>::iterator begin();

            /*!
             * Retrieves an iterator to the variables in this set.
             */
            typename detail::ConstVariables<Variable>::iterator begin() const;

            /*!
             * Retrieves the end iterator to the variables in this set.
             */
            typename detail::Variables<Variable>::iterator end();

            /*!
             * Retrieves the end iterator to the variables in this set.
             */
            typename detail::ConstVariables<Variable>::iterator end() const;

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
             * Retrieves whether the set of variables contains a real variable.
             */
            bool containsRealVariables() const;

            /*!
             * Retrieves whether the set of variables contains a Array variable.
             */
            bool containsArrayVariables() const;

            /*!
             * Retrieves whether the set of variables contains a clock variable.
             */
            bool containsClockVariables() const;

            /*!
             * Retrieves whether the set of variables contains a non-transient real variable.
             */
            bool containsNonTransientRealVariables() const;

            /*!
             * Retrieves whether the set of variables contains a non-transient unbounded integer variable.
             */
            bool containsNonTransientUnboundedIntegerVariables() const;

            /*!
             * Retrieves whether this variable set is empty.
             */
            bool empty() const;

            /*!
             * Total number of variables, including transient variables.
             */
            uint64_t getNumberOfVariables() const;

            /*
             * Total number of nontransient variables
             */
            uint64_t getNumberOfNontransientVariables() const;

            /*!
             * Retrieves the number of transient variables in this variable set.
             */
            uint64_t getNumberOfTransientVariables() const;
            
            /*!
             * Retrieves the number of real transient variables in this variable set.
             */
            uint64_t getNumberOfRealTransientVariables() const;

            /*!
             * Retrieves the number of unbounded integer transient variables in this variable set.
             */
            uint64_t getNumberOfUnboundedIntegerTransientVariables() const;

            /*!
             * Retrieves the number of numerical (i.e. real, or integer) transient variables in this variable set.
             */
            uint64_t getNumberOfNumericalTransientVariables() const;

            /*!
             * Retrieves the transient variables in this variable set.
             */
            typename detail::ConstVariables<Variable> getTransientVariables() const;
            
            /*!
             * Checks whether any of the provided variables appears in bound expressions or initial values of the
             * variables contained in this variable set.
             */
            bool containsVariablesInBoundExpressionsOrInitialValues(std::set<storm::expressions::Variable> const& variables) const;
            
            /*!
             * Retrieves a mapping from variable names to (references of) the variable objects.
             */
            std::map<std::string, std::reference_wrapper<Variable const>> getNameToVariableMap() const;
            
            /*!
             * Applies the given substitution to all variables in this set.
             * The substitution does not apply to the variables itself, but to initial expressions, variable bounds, ...
             * @param substitution
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            /*!
             * Substitutes the actual variables according to the given substitution.
             * @param substitution The substitution. Assumed to only map variables to VariableExpressions.
             * @note does not substitute variables in initial expressions, variable bounds, ...
             */
            void substituteExpressionVariables(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            
        private:
            /// The vector of all variables.
            std::vector<std::shared_ptr<Variable>> variables;
            
            /// The boolean variables in this set.
            std::vector<std::shared_ptr<BooleanVariable>> booleanVariables;

            /// The bounded integer variables in this set.
            std::vector<std::shared_ptr<BoundedIntegerVariable>> boundedIntegerVariables;

            /// The unbounded integer variables in this set.
            std::vector<std::shared_ptr<UnboundedIntegerVariable>> unboundedIntegerVariables;
            
            /// The real variables in this set.
            std::vector<std::shared_ptr<RealVariable>> realVariables;
            
            /// The array variables in this set.
            std::vector<std::shared_ptr<ArrayVariable>> arrayVariables;
            
            /// The clock variables in this set.
            std::vector<std::shared_ptr<ClockVariable>> clockVariables;
            
            /// The transient variables in this set.
            std::vector<std::shared_ptr<Variable>> transientVariables;
            
            /// A set of all variable names currently in use.
            std::map<std::string, storm::expressions::Variable> nameToVariable;
            
            /// A mapping from expression variables to their variable objects.
            std::map<storm::expressions::Variable, std::shared_ptr<Variable>> variableToVariable;
        };
        
    }
}
