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

ADD* CuddUtility::getConstantEncoding(uint_fast64_t constant, std::vector<ADD*>& variables) const {
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

ADD* CuddUtility::addValueForEncodingOfConstant(ADD* add, uint_fast64_t constant, std::vector<ADD*>& variables, double value) const {
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
	ADD* encoding = new ADD(initialNode);

	// Add (i.e. multiply) the other variables as well according to whether their bit is set or not.
	for (uint_fast64_t i = 1; i < variables.size(); ++i) {
		if ((constant & (1 << (variables.size() - i - 1))) != 0) {
			*encoding *= *variables[i];
		} else {
			*encoding *= ~(*variables[i]);
		}
	}

	ADD* result = new ADD(add->Ite(manager.constant(value), *add));
	return result;
}

ADD* CuddUtility::getConstant(double value) const {
	return new ADD(manager.constant(value));
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
	if (CuddUtility::instance != nullptr) {
		CuddUtility::instance = new CuddUtility();
	}
	return CuddUtility::instance;
}

} // namespace utility

} // namespace storm


