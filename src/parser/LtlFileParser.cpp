/*
 * LtlFileParser.cpp
 *
 *  Created on: 13.05.2013
 *      Author: thomas
 */

#include <fstream>

#include "LtlFileParser.h"
#include "LtlParser.h"

#include "src/exceptions/FileIoException.h"

namespace storm {
namespace parser {

std::list<std::shared_ptr<storm::property::ltl::LtlFilter<double>>> LtlFileParser::parseLtlFile(std::string filename) {
	// Open file
	std::ifstream inputFileStream(filename, std::ios::in);

	if (!inputFileStream.is_open()) {
		std::string message = "Error while opening file ";
		throw storm::exceptions::FileIoException() << message << filename;
	}

	std::list<std::shared_ptr<storm::property::ltl::LtlFilter<double>>> result;

	while(!inputFileStream.eof()) {
		std::string line;
		//The while loop reads the input file line by line
		while (std::getline(inputFileStream, line)) {
			result.push_back(storm::parser::LtlParser::parseLtlFormula(line));
		}
	}

	return result;
}

} //namespace parser
} //namespace storm


