/*
 * CuddUtility.cpp
 *
 *  Created on: 26.01.2013
 *      Author: Christian Dehnert
 */

#include "CuddUtility.h"
#include "src/exceptions/InvalidArgumentException.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

#include <stdio.h>
#include <iostream>

namespace storm {

namespace utility {

storm::utility::CuddUtility* storm::utility::CuddUtility::instance = nullptr;

ADD* CuddUtility::getNewAddVariable() {
	ADD* result = new ADD(manager.addVar());
	allDecisionDiagramVariables.push_back(result);
	return result;
}

ADD* CuddUtility::getAddVariable(int index) const {
	return new ADD(manager.addVar(index));
}

ADD* CuddUtility::getOne() const {
	return new ADD(manager.addOne());
}

ADD* CuddUtility::getZero() const {
	return new ADD(manager.addZero());
}

ADD* CuddUtility::getConstantEncoding(uint_fast64_t constant, std::vector<ADD*> const& variables) const {
	if ((constant >> variables.size()) != 0) {
		LOG4CPLUS_ERROR(logger, "Cannot create encoding for constant " << constant << " with "
				<< variables.size()	<< " variables.");
		throw storm::exceptions::InvalidArgumentException() << "Cannot create encoding"
				<< " for constant " << constant << " with " << variables.size()
				<< " variables.";
	}

	// Determine whether the new ADD will be rooted by the first variable or its complement.
	ADD initialNode;
	if ((constant & (1 << (variables.size() - 1))) != 0) {
		initialNode = *variables[0];
	} else {
		initialNode = ~(*variables[0]);
	}
	ADD* result = new ADD(initialNode);

	// Add (i.e. multiply) the other variables as well according to whether their bit is set or not.
	for (uint_fast64_t i = 1; i < variables.size(); ++i) {
		if ((constant & (1 << (variables.size() - i - 1))) != 0) {
			*result *= *variables[i];
		} else {
			*result *= ~(*variables[i]);
		}
	}

	return result;
}

void CuddUtility::setValueAtIndex(ADD* add, uint_fast64_t index, std::vector<ADD*> const& variables, double value) const {
	if ((index >> variables.size()) != 0) {
		LOG4CPLUS_ERROR(logger, "Cannot create encoding for index " << index << " with "
				<< variables.size()	<< " variables.");
		throw storm::exceptions::InvalidArgumentException() << "Cannot create encoding"
				<< " for index " << index << " with " << variables.size()
				<< " variables.";
	}

	// Determine whether the new ADD will be rooted by the first variable or its complement.
	ADD initialNode;
	if ((index & (1 << (variables.size() - 1))) != 0) {
		initialNode = *variables[0];
	} else {
		initialNode = ~(*variables[0]);
	}
	ADD* encoding = new ADD(initialNode);

	// Add (i.e. multiply) the other variables as well according to whether their bit is set or not.
	for (uint_fast64_t i = 1; i < variables.size(); ++i) {
		if ((index & (1 << (variables.size() - i - 1))) != 0) {
			*encoding *= *variables[i];
		} else {
			*encoding *= ~(*variables[i]);
		}
	}

	*add = encoding->Ite(manager.constant(value), *add);
}

void CuddUtility::setValueAtIndices(ADD* add, uint_fast64_t rowIndex, uint_fast64_t columnIndex, std::vector<ADD*> const& rowVariables, std::vector<ADD*> const& columnVariables, double value) const {
	if ((rowIndex >> rowVariables.size()) != 0) {
		LOG4CPLUS_ERROR(logger, "Cannot create encoding for index " << rowIndex << " with "
				<< rowVariables.size()	<< " variables.");
		throw storm::exceptions::InvalidArgumentException() << "Cannot create encoding"
				<< " for index " << rowIndex << " with " << rowVariables.size()
				<< " variables.";
	}
	if ((columnIndex >> columnVariables.size()) != 0) {
		LOG4CPLUS_ERROR(logger, "Cannot create encoding for index " << columnIndex << " with "
				<< columnVariables.size()	<< " variables.");
		throw storm::exceptions::InvalidArgumentException() << "Cannot create encoding"
				<< " for index " << columnIndex << " with " << columnVariables.size()
				<< " variables.";
	}
	if (rowVariables.size() != columnVariables.size()) {
		LOG4CPLUS_ERROR(logger, "Number of variables for indices encodings does not match.");
		throw storm::exceptions::InvalidArgumentException()
			<< "Number of variables for indices encodings does not match.";
	}

	ADD initialNode;
	if ((rowIndex & (1 << (rowVariables.size() - 1))) != 0) {
		initialNode = *rowVariables[0];
	} else {
		initialNode = ~(*rowVariables[0]);
	}
	ADD* encoding = new ADD(initialNode);
	if ((columnIndex & (1 << (rowVariables.size() - 1))) != 0) {
		*encoding *= *columnVariables[0];
	} else {
		*encoding *= ~(*columnVariables[0]);
	}

	for (uint_fast64_t i = 1; i < rowVariables.size(); ++i) {
		if ((rowIndex & (1 << (rowVariables.size() - i - 1))) != 0) {
			*encoding *= *rowVariables[i];
		} else {
			*encoding *= ~(*rowVariables[i]);
		}
		if ((columnIndex & (1 << (columnVariables.size() - i - 1))) != 0) {
			*encoding *= *columnVariables[i];
		} else {
			*encoding *= ~(*columnVariables[i]);
		}
	}

	*add = encoding->Ite(manager.constant(value), *add);
}


ADD* CuddUtility::getConstant(double value) const {
	return new ADD(manager.constant(value));
}

ADD* CuddUtility::permuteVariables(ADD* add, std::vector<ADD*> fromVariables, std::vector<ADD*> toVariables, uint_fast64_t totalNumberOfVariables) const {
	std::vector<int> permutation;
	permutation.resize(totalNumberOfVariables);
	for (uint_fast64_t i = 0; i < totalNumberOfVariables; ++i) {
		permutation[i] = i;
	}
	for (uint_fast64_t i = 0; i < fromVariables.size(); ++i) {
		permutation[fromVariables[i]->NodeReadIndex()] = toVariables[i]->NodeReadIndex();
	}
	return new ADD(add->Permute(&permutation[0]));
}

void CuddUtility::dumpDotToFile(ADD* add, std::string filename) const {
	std::vector<ADD> nodes;
	nodes.push_back(*add);

	FILE* filePtr;
	filePtr = fopen(filename.c_str(), "w");
	manager.DumpDot(nodes, 0, 0, filePtr);
	fclose(filePtr);
}

Cudd const& CuddUtility::getManager() const {
	return manager;
}

CuddUtility* cuddUtilityInstance() {
	if (CuddUtility::instance == nullptr) {
		CuddUtility::instance = new CuddUtility();
	}
	return CuddUtility::instance;
}

} // namespace utility

} // namespace storm


