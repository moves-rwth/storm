/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "properties/Prctl.h"
#include "src/properties/prctl/PrctlFilter.h"

#include <list>

namespace storm {
namespace parser {

class PrctlFileParser {
public:

	/*!
	 * Parses each line of a given file as Prctl formula and returns a list containing the results of the parsing.
	 *
	 * @param filename Name and path to the file in which the formula strings can be found.
	 * @return The list of parsed formulas
	 */
	static std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> parsePrctlFile(std::string filename);

};

} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
