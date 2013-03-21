/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "PrctlParser.h"
#include "modelchecker/AbstractModelChecker.h"

namespace storm {
namespace parser {

/*!
 * Reads a PRCTL formula from a file and return the formula tree.
 *
 * @note
 * This class creates a PctlFormula object which can be accessed through the getFormula() method (of base
 * class PrctlParser). However, it will not delete this object.
 */
class PrctlFileParser {
public:
	/*!
	 * Reads the formula from the given file and parses it into a formula tree, consisting of
	 * classes in the namespace storm::formula.
	 *
	 * If the contents of the file could not be parsed successfully, it will throw a wrongFormatException.
	 *
	 * @param filename The name of the file to parse
	 * @throw wrongFormatException If the input could not be parsed successfully
	 */
	PrctlFileParser(std::string filename, storm::modelChecker::AbstractModelChecker<double>* modelChecker);

	/*!
	 * Destructor.
	 * At this time, empty
	 *
	 * Will not delete the constructed formula!
	 */
	virtual ~PrctlFileParser();
};

} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
