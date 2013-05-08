/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "formula/Prctl.h"

#include <list>

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
	 * Constructor
	 */
	PrctlFileParser();

	/*!
	 * Destructor.
	 * At this time, empty
	 *
	 */
	virtual ~PrctlFileParser();

	/*!
	 * Parses each line of a given file as prctl formula and returns a list containing the results of the parsing.
	 *
	 * @param filename
	 * @return The list of parsed formulas
	 */
	std::list<storm::property::prctl::AbstractPrctlFormula<double>*> parseFormulas(std::string filename);
};

} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
