/*
 * LtlFileParser.h
 *
 *  Created on: 13.05.2013
 *      Author: Thonas Heinemann
 */

#ifndef LTLFILEPARSER_H_
#define LTLFILEPARSER_H_

#include "properties/Ltl.h"
#include "src/properties/ltl/LtlFilter.h"

#include <list>

namespace storm {
namespace parser {

class LtlFileParser {
public:

	/*!
	 * Parses each line of a given file as prctl formula and returns a list containing the results of the parsing.
	 *
	 * @param filename Name and path to the file in which the formula strings can be found.
	 * @return The list of parsed formulas.
	 */
	static std::list<std::shared_ptr<storm::properties::ltl::LtlFilter<double>>> parseLtlFile(std::string filename);
};

} //namespace parser
} //namespace storm

#endif /* LTLFILEPARSER_H_ */
