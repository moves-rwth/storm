/*
 * Variable.h
 *
 *  Created on: 06.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_VARIABLE_H_
#define STORM_IR_VARIABLE_H_

#include <string>
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
         * A class representing a untyped variable.
         */
        class Variable {
        public:
            /*!
             * Default constructor. Creates an unnamed, untyped variable without initial value.
             */
            Variable();
            
            /*!
             * Creates an untyped variable with the given name and initial value.
             *
             * @param localIndex A module-local index for the variable.
             * @param globalIndex A globally unique (among the variables of equal type) index for the variable.
             * @param variableName the name of the variable.
             * @param initialValue the expression that defines the initial value of the variable.
             */
            Variable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue = nullptr);
            
            /*!
             * Creates a copy of the given variable and performs the provided renaming.
             *
             * @param oldVariable The variable to copy.
             * @param newName New name of this variable.
             * @param newGlobalIndex The new global index of the variable.
             * @param renaming A mapping from names that are to be renamed to the names they are to be
             * replaced with.
             * @param variableState An object knowing about the variables in the system.
             */
            Variable(Variable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState);
            
            /*!
             * Creates a deep-copy of the given variable.
             *
             * @param otherVariable The variable to copy.
             */
            Variable(Variable const& otherVariable);
            
            /*!
             * Creates a deep-copy of the given variable and assigns it to the current one.
             */
            Variable& operator=(Variable const& otherVariable);
            
            /*!
             * Retrieves the name of the variable.
             *
             * @return The name of the variable.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the global index of the variable, i.e. the index in all variables of equal type
             * of all modules.
             *
             * @return The global index of the variable.
             */
            uint_fast64_t getGlobalIndex() const;
            
            /*!
             * Retrieves the global index of the variable, i.e. the index in all variables of equal type in
             * the same module.
             *
             * @return The local index of the variable.
             */
            uint_fast64_t getLocalIndex() const;
            
            /*!
             * Retrieves the expression defining the initial value of the variable.
             *
             * @return The expression defining the initial value of the variable.
             */
            std::unique_ptr<storm::ir::expressions::BaseExpression> const& getInitialValue() const;
            
            /*!
             * Sets the initial value to the given expression.
             *
             * @param initialValue The new initial value.
             */
            void setInitialValue(std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue);
            
        private:
            // A unique (among the variables of equal type) index for the variable inside its module.
            uint_fast64_t localIndex;

            // A unique (among the variables of equal type) index for the variable over all modules.
            uint_fast64_t globalIndex;
            
            // The name of the variable.
            std::string variableName;
            
            // The expression defining the initial value of the variable.
            std::unique_ptr<storm::ir::expressions::BaseExpression> initialValue;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_VARIABLE_H_ */
