/*
 * Update.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "Update.h"
#include "src/parser/prismparser/VariableState.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {
    namespace ir {
        
        Update::Update() : likelihoodExpression(), booleanAssignments(), integerAssignments(), globalIndex() {
            // Nothing to do here.
        }
        
        Update::Update(uint_fast64_t globalIndex, std::shared_ptr<storm::ir::expressions::BaseExpression> const& likelihoodExpression, std::map<std::string, storm::ir::Assignment> const& booleanAssignments, std::map<std::string, storm::ir::Assignment> const& integerAssignments)
        : likelihoodExpression(likelihoodExpression), booleanAssignments(booleanAssignments), integerAssignments(integerAssignments), globalIndex(globalIndex) {
            // Nothing to do here.
        }
        
        Update::Update(Update const& update, uint_fast64_t newGlobalIndex, std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) : globalIndex(newGlobalIndex) {
            for (auto const& variableAssignmentPair : update.booleanAssignments) {
                if (renaming.count(variableAssignmentPair.first) > 0) {
                    this->booleanAssignments[renaming.at(variableAssignmentPair.first)] = Assignment(variableAssignmentPair.second, renaming, variableState);
                } else {
                    this->booleanAssignments[variableAssignmentPair.first] = Assignment(variableAssignmentPair.second, renaming, variableState);
                }
            }
            for (auto const& variableAssignmentPair : update.integerAssignments) {
                if (renaming.count(variableAssignmentPair.first) > 0) {
                    this->integerAssignments[renaming.at(variableAssignmentPair.first)] = Assignment(variableAssignmentPair.second, renaming, variableState);
                } else {
                    this->integerAssignments[variableAssignmentPair.first] = Assignment(variableAssignmentPair.second, renaming, variableState);
                }
            }
            this->likelihoodExpression = update.likelihoodExpression->clone(renaming, variableState);
        }
        
        std::shared_ptr<storm::ir::expressions::BaseExpression> const& Update::getLikelihoodExpression() const {
            return likelihoodExpression;
        }
        
        uint_fast64_t Update::getNumberOfBooleanAssignments() const {
            return booleanAssignments.size();
        }
        
        uint_fast64_t Update::getNumberOfIntegerAssignments() const {
            return integerAssignments.size();
        }
        
        std::map<std::string, storm::ir::Assignment> const& Update::getBooleanAssignments() const {
            return booleanAssignments;
        }
        
        std::map<std::string, storm::ir::Assignment> const& Update::getIntegerAssignments() const {
            return integerAssignments;
        }
        
        storm::ir::Assignment const& Update::getBooleanAssignment(std::string const& variableName) const {
            auto variableAssignmentPair = booleanAssignments.find(variableName);
            if (variableAssignmentPair == booleanAssignments.end()) {
                throw storm::exceptions::OutOfRangeException() << "Cannot find boolean assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
            }
            
            return variableAssignmentPair->second;
        }
        
        storm::ir::Assignment const& Update::getIntegerAssignment(std::string const& variableName) const {
            auto variableAssignmentPair = integerAssignments.find(variableName);
            if (variableAssignmentPair == integerAssignments.end()) {
                throw storm::exceptions::OutOfRangeException() << "Cannot find integer assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
            }
            
            return variableAssignmentPair->second;
        }
        
        uint_fast64_t Update::getGlobalIndex() const {
            return this->globalIndex;
        }
        
        std::string Update::toString() const {
            std::stringstream result;
            result << likelihoodExpression->toString() << " : ";
            uint_fast64_t i = 0;
            for (auto const& assignment : booleanAssignments) {
                result << assignment.second.toString();
                if (i < booleanAssignments.size() - 1 || integerAssignments.size() > 0) {
                    result << " & ";
                }
                ++i;
            }
            i = 0;
            for (auto const& assignment : integerAssignments) {
                result << assignment.second.toString();
                if (i < integerAssignments.size() - 1) {
                    result << " & ";
                }
                ++i;
            }
            return result.str();
        }
        
    } // namespace ir
} // namespace storm
