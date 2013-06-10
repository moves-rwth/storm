/*
 * Assignment.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "Assignment.h"
#include "src/parser/prismparser/VariableState.h"

namespace storm {
    namespace ir {
        
        Assignment::Assignment() : variableName(), expression() {
            // Nothing to do here.
        }
        
        Assignment::Assignment(std::string const& variableName, std::shared_ptr<storm::ir::expressions::BaseExpression> const& expression)
        : variableName(variableName), expression(expression) {
            // Nothing to do here.
        }
        
        Assignment::Assignment(Assignment const& oldAssignment, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState)
        : variableName(oldAssignment.variableName), expression(oldAssignment.expression->clone(renaming, variableState)) {
            auto renamingPair = renaming.find(oldAssignment.variableName);
            if (renamingPair != renaming.end()) {
                this->variableName = renamingPair->second;
            }
        }
        
        std::string const& Assignment::getVariableName() const {
            return variableName;
        }
        
        std::shared_ptr<storm::ir::expressions::BaseExpression> const& Assignment::getExpression() const {
            return expression;
        }
        
        std::string Assignment::toString() const {
            std::stringstream result;
            result << "(" << variableName << "' = " << expression->toString() << ")";
            return result.str();
        }
        
    } // namespace ir
} // namespace storm
