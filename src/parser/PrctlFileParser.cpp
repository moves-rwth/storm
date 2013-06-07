/*
 * PrctlFileParser.cpp
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#include <sstream>

#include "PrctlFileParser.h"
#include "PrctlParser.h"

namespace storm {
namespace parser {

std::list<storm::property::prctl::AbstractPrctlFormula<double>*> PrctlFileParser(std::string filename) {
	// Open file
	std::ifstream inputFileStream(filename, std::ios::in);

	if (!inputFileStream.is_open()) {
		std::string message = "Error while opening file ";
		throw storm::exceptions::FileIoException() << message << filename;
	}

	std::list<storm::property::prctl::AbstractPrctlFormula<double>*> result;

	while(!inputFileStream.eof()) {
		std::string line;
		//The while loop reads the input file line by line
		while (std::getline(inputFileStream, line)) {
			PrctlParser parser(line);
			if (!parser.parsedComment()) {
				//lines containing comments will be skipped.
				LOG4CPLUS_INFO(logger, "Parsed formula \"" + line + "\" into \"" + parser.getFormula()->toString() + "\"");
				result.push_back(parser.getFormula());
			}
		}
	}

	return result;
}

} /* namespace parser */
} /* namespace storm */
