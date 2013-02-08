/*
 * PrctlFileParser.cpp
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#include "PrctlFileParser.h"

namespace storm {
namespace parser {

PrctlFileParser::PrctlFileParser(std::string filename) {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);

	// Prepare iterators to input.
	// TODO: Right now, this parses the whole contents of the file into a string first.
	// While this is usually not necessary, because there exist adapters that make an input stream
	// iterable in both directions without storing it into a string, using the corresponding
	// Boost classes gives an awful output under valgrind and is thus disabled for the time being.
	std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
	parse(fileContent);
}

PrctlFileParser::~PrctlFileParser() {
	//intentionally left empty
	//formulas are not deleted with the parser!
}

} /* namespace parser */
} /* namespace storm */
