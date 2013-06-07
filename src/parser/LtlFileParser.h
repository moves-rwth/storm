/*
 * LtlFileParser.h
 *
 *  Created on: 13.05.2013
 *      Author: Thonas Heinemann
 */

#ifndef LTLFILEPARSER_H_
#define LTLFILEPARSER_H_

#include "formula/Ltl.h"

#include <list>

namespace storm {
namespace parser {

/*!
 * Parses each line of a given file as prctl formula and returns a list containing the results of the parsing.
 *
 * @param filename
 * @return The list of parsed formulas
 */
std::list<storm::property::ltl::AbstractLtlFormula<double>*> LtlFileParser(std::string filename);

} //namespace parser
} //namespace storm

#endif /* LTLFILEPARSER_H_ */
