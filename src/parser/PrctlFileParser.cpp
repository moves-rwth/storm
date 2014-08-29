/*
 * PrctlFileParser.cpp
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#include <sstream>

#include "PrctlFileParser.h"
#include "PrctlParser.h"
#include "src/exceptions/FileIoException.h"

namespace storm {
namespace parser {

std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> PrctlFileParser::parsePrctlFile(std::string filename) {
	// Open file
	std::ifstream inputFileStream;
	inputFileStream.open(filename, std::ios::in);

	if (!inputFileStream.is_open()) {
		std::string message = "Error while opening file ";
		throw storm::exceptions::FileIoException() << message << filename;
	}

	std::list<std::shared_ptr<storm::properties::prctl::PrctlFilter<double>>> result;

	std::string line;
	//The while loop reads the input file line by line
	while (std::getline(inputFileStream, line)) {
		std::shared_ptr<storm::properties::prctl::PrctlFilter<double>> formula = PrctlParser::parsePrctlFormula(line);
		if (formula != nullptr) {
			//lines containing comments will be skipped.
			LOG4CPLUS_INFO(logger, "Parsed formula \"" + line + "\" into \"" + formula->toString() + "\"");
			result.push_back(formula);
		}
	}

	return result;
}

} /* namespace parser */
} /* namespace storm */
