/*
 * Update.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "Update.h"
#include "src/exceptions/OutOfRangeException.h"

namespace storm {

namespace ir {


Update::Update() : likelihoodExpression(), booleanAssignments(), integerAssignments() {
	// Nothing to do here.
}

Update::Update(std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression, std::map<std::string, storm::ir::Assignment> booleanAssignments, std::map<std::string, storm::ir::Assignment> integerAssignments)
	: likelihoodExpression(likelihoodExpression), booleanAssignments(booleanAssignments), integerAssignments(integerAssignments) {
	// Nothing to do here.
}

Update::Update(Update const& update, std::map<std::string, std::string> const& renaming, std::map<std::string, uint_fast64_t> const& booleanVariableToIndexMap, std::map<std::string, uint_fast64_t> const& integerVariableToIndexMap) {
	for (auto it : update.booleanAssignments) {
		if (renaming.count(it.first) > 0) {
			this->booleanAssignments[renaming.at(it.first)] = Assignment(it.second, renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
		} else {
			this->booleanAssignments[it.first] = Assignment(it.second, renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
		}
	}
	for (auto it : update.integerAssignments) {
		if (renaming.count(it.first) > 0) {
			this->integerAssignments[renaming.at(it.first)] = Assignment(it.second, renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
		} else {
			this->integerAssignments[it.first] = Assignment(it.second, renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
		}
	}
	this->likelihoodExpression = update.likelihoodExpression->clone(renaming, booleanVariableToIndexMap, integerVariableToIndexMap);
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

storm::ir::Assignment const& Update::getBooleanAssignment(std::string variableName) const {
	auto it = booleanAssignments.find(variableName);
	if (it == booleanAssignments.end()) {
		throw storm::exceptions::OutOfRangeException() << "Cannot find boolean assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
	}

	return (*it).second;
}

storm::ir::Assignment const& Update::getIntegerAssignment(std::string variableName) const {
	auto it = integerAssignments.find(variableName);
	if (it == integerAssignments.end()) {
		throw storm::exceptions::OutOfRangeException() << "Cannot find integer assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
	}

	return (*it).second;
}

std::string Update::toString() const {
	std::stringstream result;
	result << likelihoodExpression->toString() << " : ";
	uint_fast64_t i = 0;
	for (auto assignment : booleanAssignments) {
		result << assignment.second.toString();
		if (i < booleanAssignments.size() - 1 || integerAssignments.size() > 0) {
			result << " & ";
		}
		++i;
	}
	i = 0;
	for (auto assignment : integerAssignments) {
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
