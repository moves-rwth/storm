/*
 * PrctlFileParser.cpp
 *
 *  Created on: 06.02.2013
 *      Author: Thomas Heinemann
 */

#include "PrctlFileParser.h"

#define LINELENGTH 100

namespace storm {
namespace parser {

PrctlFileParser::PrctlFileParser(std::string filename, storm::modelChecker::AbstractModelChecker<double>* modelChecker) {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);

	// TODO: Right now, this parses the whole contents of the file into a string first.
	// While this is usually not necessary, because there exist adapters that make an input stream
	// iterable in both directions without storing it into a string, using the corresponding
	// Boost classes gives an awful output under valgrind and is thus disabled for the time being.
	while(!inputFileStream.eof()) {
		char line[LINELENGTH];
		inputFileStream.getline(line, LINELENGTH);

	}

}

PrctlFileParser::~PrctlFileParser() {
	//intentionally left empty
	//formulas are not deleted with the parser!
}

} /* namespace parser */
} /* namespace storm */
