/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "formula/Prctl.h"
#include "src/formula/prctl/PrctlFilter.h"

#include <list>

namespace storm {
namespace parser {

class PrctlFileParser {
public:

	/*!
	 * Parses each line of a given file as prctl formula and returns a list containing the results of the parsing.
	 *
	 * @param filename
	 * @return The list of parsed formulas
	 */
	static std::list<storm::property::prctl::PrctlFilter<double>*> parsePrctlFile(std::string filename);

};

} /* namespace parser */
} /* namespace storm */

#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
