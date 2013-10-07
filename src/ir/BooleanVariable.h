/*
 * BooleanVariable.h
 *
 *  Created on: 08.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_BOOLEANVARIABLE_H_
#define STORM_IR_BOOLEANVARIABLE_H_

#include <memory>
#include <map>

#include "src/ir/Variable.h"

namespace storm {
    
    namespace parser {
        namespace prism {
            class VariableState;
        } // namespace prismparser
    } // namespace parser
    
    namespace ir {
        
        /*!
         * A class representing a boolean variable.
         */
        class BooleanVariable : public Variable {
        public:
            /*!
             * Default constructor. Creates a boolean variable without a name.
             */
            BooleanVariable();
            
            /*!
             * Creates a boolean variable with the given name and the given initial value.
             *
             * @param localIndex A module-local unique index for the variable.
             * @param globalIndex A globally unique index for the variable.
             * @param variableName The name of the variable.
             * @param initialValue The expression that defines the initial value of the variable.
             */
            BooleanVariable(uint_fast64_t localIndex, uint_fast64_t globalIndex, std::string const& variableName, std::unique_ptr<storm::ir::expressions::BaseExpression>&& initialValue = nullptr);
            
            /*!
             * Creates a copy of the given boolean variable and performs the provided renaming.
             *
             * @param oldVariable The variable to copy.
             * @param newName New name of this variable.
             * @param newGlobalIndex The new global index of the variable.
             * @param renaming A mapping from names that are to be renamed to the names they are to be
             * replaced with.
             * @param variableState An object knowing about the variables in the system.
             */
            BooleanVariable(BooleanVariable const& oldVariable, std::string const& newName, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState);
            
            BooleanVariable& operator=(BooleanVariable const& otherVariable);
            
            /*!
             * Retrieves a string representation of this variable.
             * @returns a string representation of this variable.
             */
            std::string toString() const;
        };
        
    } // namespace ir
} // namespace storm

#endif /* STORM_IR_BOOLEANVARIABLE_H_ */
