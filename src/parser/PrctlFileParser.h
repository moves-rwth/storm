/*
 * PrctlFileParser.h
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#ifndef STORM_PARSER_PRCTLFILEPARSER_H_
#define STORM_PARSER_PRCTLFILEPARSER_H_

#include "PrctlParser.h"

namespace storm {
namespace parser {

class PrctlFileParser: public storm::parser::PrctlParser {
public:
	PrctlFileParser(std::string filename);
	virtual ~PrctlFileParser();
};

} /* namespace parser */
} /* namespace storm */
#endif /* STORM_PARSER_PRCTLFILEPARSER_H_ */
