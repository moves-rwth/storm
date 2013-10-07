/*
 * Assignment.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_ASSIGNMENT_H_
#define STORM_IR_ASSIGNMENT_H_

#include <memory>

#include "expressions/BaseExpression.h"

namespace storm {
    
    namespace parser {
        namespace prism {
            class VariableState;
        } // namespace prismparser
    } // namespace parser
    
    namespace ir {
        
        /*!
         * A class representing the assignment of an expression to a variable.
         */
        class Assignment {
        public:
            /*!
             * Default constructor. Creates an empty assignment.
             */
            Assignment();
            
            /*!
             * Constructs an assignment using the given variable name and expression.
             *
             * @param variableName The variable that this assignment targets.
             * @param expression The expression to assign to the variable.
             */
            Assignment(std::string const& variableName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& expression);
            
            /*!
             * Creates a copy of the given assignment and performs the provided renaming.
             *
             * @param oldAssignment The assignment to copy.
             * @param renaming A mapping from names that are to be renamed to the names they are to be
             * replaced with.
             * @param variableState An object knowing about the variables in the system.
             */
            Assignment(Assignment const& oldAssignment, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState);
            
            /*!
             * Performs a deep-copy of the given assignment.
             *
             * @param otherAssignment The assignment to copy.
             */
            Assignment(Assignment const& otherAssignment);
            
            /*!
             * Performs a deep-copy of the given assignment and assigns it to the current one.
             *
             * @param otherAssignment The assignment to assign.
             */
            Assignment& operator=(Assignment const& otherAssignment);
            
            /*!
             * Retrieves the name of the variable that this assignment targets.
             *
             * @return The name of the variable that this assignment targets.
             */
            std::string const& getVariableName() const;
            
            /*!
             * Retrieves the expression that is assigned to the variable.
             *
             * @return The expression that is assigned to the variable.
             */
            std::unique_ptr<storm::ir::expressions::BaseExpression> const& getExpression() const;
            
            /*!
             * Retrieves a string representation of this assignment.
             * @returns a string representation of this assignment.
             */
            std::string toString() const;
            
        private:
            // The name of the variable that this assignment targets.
            std::string variableName;
            
            // The expression that is assigned to the variable.
            std::unique_ptr<storm::ir::expressions::BaseExpression> expression;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_ASSIGNMENT_H_ */
