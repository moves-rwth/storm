#pragma once

#include <functional>

#include "src/storage/jani/Variable.h"
#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace jani {
        
        class Assignment {
        public:
            /*!
             * Creates an assignment of the given expression to the given variable.
             */
            Assignment(storm::jani::Variable const& variable, storm::expressions::Expression const& expression);
            
            /*!
             * Retrieves the expression variable that is written in this assignment.
             */
            storm::jani::Variable const& getVariable() const;
                
            /*!
             * Retrieves the expression variable that is written in this assignment.
             */
            storm::expressions::Variable const& getExpressionVariable() const;
            
            /*!
             * Retrieves the expression whose value is assigned to the target variable.
             */
            storm::expressions::Expression const& getAssignedExpression() const;
            
            /*!
             * Sets a new expression that is assigned to the target variable.
             */
            void setAssignedExpression(storm::expressions::Expression const& expression);

            /**
             * Retrieves whether the assignment assigns to a transient variable.
             */
            bool isTransientAssignment() const;
            
            friend std::ostream& operator<<(std::ostream& stream, Assignment const& assignment);
            
        private:
            // The variable being assigned.
            std::reference_wrapper<storm::jani::Variable const> variable;
            
            // The expression that is being assigned to the variable.
            storm::expressions::Expression expression;
        };
        
    }
}