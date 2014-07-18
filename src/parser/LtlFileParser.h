/*
 * LtlFileParser.h
 *
 *  Created on: 13.05.2013
 *      Author: Thonas Heinemann
 */

#ifndef LTLFILEPARSER_H_
#define LTLFILEPARSER_H_

#include "formula/Ltl.h"
#include "src/formula/ltl/LtlFilter.h"

#include <list>

namespace storm {
namespace parser {

class LtlFileParser {
public:

	/*!
	 * Parses each line of a given file as prctl formula and returns a list containing the results of the parsing.
	 *
	 * @param filename
	 * @return The list of parsed formulas
	 */
	static std::list<std::shared_ptr<storm::property::ltl::LtlFilter<double>>> parseLtlFile(std::string filename);
};

} //namespace parser
} //namespace storm

#endif /* LTLFILEPARSER_H_ */
