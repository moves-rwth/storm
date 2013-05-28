/*
 * Update.cpp
 *
 *  Created on: 12.01.2013
 *      Author: Christian Dehnert
 */

#include "Update.h"

#include "src/exceptions/OutOfRangeException.h"

#include <sstream>

namespace storm {

namespace ir {

// Initializes all members with their default constructors.
Update::Update() : likelihoodExpression(), booleanAssignments(), integerAssignments() {
	// Nothing to do here.
}

// Initializes all members according to the given values.
Update::Update(std::shared_ptr<storm::ir::expressions::BaseExpression> likelihoodExpression, std::map<std::string, storm::ir::Assignment> booleanAssignments, std::map<std::string, storm::ir::Assignment> integerAssignments)
	: likelihoodExpression(likelihoodExpression), booleanAssignments(booleanAssignments), integerAssignments(integerAssignments) {
	// Nothing to do here.
}

Update::Update(const Update& update, const std::map<std::string, std::string>& renaming, const std::map<std::string,uint_fast64_t>& bools, const std::map<std::string,uint_fast64_t>& ints) {
	for (auto it : update.booleanAssignments) {
		if (renaming.count(it.first) > 0) {
			this->booleanAssignments[renaming.at(it.first)] = Assignment(it.second, renaming, bools, ints);
		} else {
			this->booleanAssignments[it.first] = Assignment(it.second, renaming, bools, ints);
		}
	}
	for (auto it : update.integerAssignments) {
		if (renaming.count(it.first) > 0) {
			this->integerAssignments[renaming.at(it.first)] = Assignment(it.second, renaming, bools, ints);
		} else {
			this->integerAssignments[it.first] = Assignment(it.second, renaming, bools, ints);
		}
	}
	this->likelihoodExpression = update.likelihoodExpression->clone(renaming, bools, ints);
}

// Return the expression for the likelihood of the update.
std::shared_ptr<storm::ir::expressions::BaseExpression> const& Update::getLikelihoodExpression() const {
	return likelihoodExpression;
}

// Return the number of assignments.
uint_fast64_t Update::getNumberOfBooleanAssignments() const {
	return booleanAssignments.size();
}

uint_fast64_t Update::getNumberOfIntegerAssignments() const {
	return integerAssignments.size();
}

// Return the boolean variable name to assignment map.
std::map<std::string, storm::ir::Assignment> const& Update::getBooleanAssignments() const {
	return booleanAssignments;
}

// Return the integer variable name to assignment map.
std::map<std::string, storm::ir::Assignment> const& Update::getIntegerAssignments() const {
	return integerAssignments;
}

// Return the assignment for the boolean variable if it exists and throw an exception otherwise.
storm::ir::Assignment const& Update::getBooleanAssignment(std::string variableName) const {
	auto it = booleanAssignments.find(variableName);
	if (it == booleanAssignments.end()) {
		throw storm::exceptions::OutOfRangeException() << "Cannot find boolean assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
	}

	return (*it).second;
}

// Return the assignment for the boolean variable if it exists and throw an exception otherwise.
storm::ir::Assignment const& Update::getIntegerAssignment(std::string variableName) const {
	auto it = integerAssignments.find(variableName);
	if (it == integerAssignments.end()) {
		throw storm::exceptions::OutOfRangeException() << "Cannot find integer assignment for variable '"
				<< variableName << "' in update " << this->toString() << ".";
	}

	return (*it).second;
}

// Build a string representation of the update.
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
