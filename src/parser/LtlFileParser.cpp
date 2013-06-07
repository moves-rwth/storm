/*
 * LtlFileParser.cpp
 *
 *  Created on: 13.05.2013
 *      Author: thomas
 */

#include "LtlFileParser.h"
#include "LtlParser.h"

namespace storm {
namespace parser {

std::list<storm::property::ltl::AbstractLtlFormula<double>*> LtlFileParser(std::string filename) {
	// Open file
	std::ifstream inputFileStream(filename, std::ios::in);

	if (!inputFileStream.is_open()) {
		std::string message = "Error while opening file ";
		throw storm::exceptions::FileIoException() << message << filename;
	}

	std::list<storm::property::ltl::AbstractLtlFormula<double>*> result;

	while(!inputFileStream.eof()) {
		std::string line;
		//The while loop reads the input file line by line
		while (std::getline(inputFileStream, line)) {
			result.push_back(storm::parser::LtlParser(line));
		}
	}

	return result;
}

} //namespace parser
} //namespace storm


