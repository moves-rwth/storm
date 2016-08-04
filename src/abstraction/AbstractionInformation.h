#pragma once

#include <vector>
#include <set>

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace expressions {
        class ExpressionManager;
        class Expression;
        class Variable;
    }
    
    namespace abstraction {
        
        template<storm::dd::DdType DdType, typename ValueType>
        class AbstractionInformation {
        public:
            /*!
             * Creates a new abstraction information object.
             *
             * @param expressionManager The manager responsible for all variables and expressions during the abstraction process.
             */
            AbstractionInformation(storm::expressions::ExpressionManager& expressionManager);

            /*!
             * Adds the given variable.
             *
             * @param variable The variable to add.
             */
            void addVariable(storm::expressions::Variable const& variable);

            /*!
             * Adds the given variable whose range is restricted.
             *
             * @param variable The variable to add.
             * @param constraint An expression characterizing the legal values of the variable.
             */
            void addVariable(storm::expressions::Variable const& variable, storm::expressions::Expression const& constraint);

            /*!
             * Adds an expression that constrains the legal variable values.
             *
             * @param constraint The constraint.
             */
            void addConstraint(storm::expressions::Expression const& constraint);
            
            /*!
             * Adds the given predicate.
             *
             * @param predicate The predicate to add.
             */
            void addPredicate(storm::expressions::Expression const& predicate);
            
            /*!
             * Adds the given predicates.
             *
             * @param predicates The predicates to add.
             */
            void addPredicates(std::vector<storm::expressions::Expression> const& predicates);
            
            /*!
             * Retrieves the expression manager.
             *
             * @return The manager.
             */
            storm::expressions::ExpressionManager& getExpressionManager();
            
            /*!
             * Retrieves the expression manager.
             *
             * @return The manager.
             */
            storm::expressions::ExpressionManager const& getExpressionManager() const;
            
            /*!
             * Retrieves all currently known predicates.
             *
             * @return The list of known predicates.
             */
            std::vector<storm::expressions::Expression> const& getPredicates() const;

            /*!
             * Retrieves the predicate with the given index.
             *
             * @param index The index of the predicate.
             */
            storm::expressions::Expression const& getPredicateByIndex(uint_fast64_t index) const;
            
            /*!
             * Retrieves the number of predicates.
             *
             * @return The number of predicates.
             */
            std::size_t getNumberOfPredicates() const;
            
            /*!
             * Retrieves all currently known variables.
             *
             * @return The set of known variables.
             */
            std::set<storm::expressions::Variable> const& getVariables() const;
            
            /*!
             * Retrieves a list of expressions that constrain the valid variable values.
             *
             * @return The constraint expressions.
             */
            std::vector<storm::expressions::Expression> const& getConstraints() const;
            
        private:
            // The expression related data.
            
            /// The manager responsible for the expressions of the program and the SMT solvers.
            storm::expressions::ExpressionManager& expressionManager;
            
            /// The current set of predicates used in the abstraction.
            std::vector<storm::expressions::Expression> predicates;
            
            /// The set of all variables.
            std::set<storm::expressions::Variable> variables;
            
            /// The expressions characterizing legal variable values.
            std::vector<storm::expressions::Expression> constraints;
        };
        
    }
}