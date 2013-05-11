/*
 * PrismParser.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

#include "PrismParser.h"

#include "src/utility/OsDetection.h"

#include "src/parser/PrismParser/PrismGrammar.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <limits>

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;


namespace storm {
namespace parser {

/*!
 * Opens the given file for parsing, invokes the helper function to parse the actual content and
 * closes the file properly, even if an exception is thrown in the parser. In this case, the
 * exception is passed on to the caller.
 */
storm::ir::Program PrismParser::parseFile(std::string const& filename) const {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);
	storm::ir::Program result;

	// Now try to parse the contents of the file.
	try {
		result = parse(inputFileStream, filename);
	} catch(std::exception& e) {
		// In case of an exception properly close the file before passing exception.
		inputFileStream.close();
		throw e;
	}

	// Close the stream in case everything went smoothly and return result.
	inputFileStream.close();
	return result;
}

/*!
 * Passes iterators to the input stream to the Boost spirit parser and thereby parses the input.
 * If the parser throws an expectation failure exception, i.e. expected input different than the one
 * provided, this is caught and displayed properly before the exception is passed on.
 */
storm::ir::Program PrismParser::parse(std::istream& inputStream, std::string const& filename) const {
	// Prepare iterators to input.
	// TODO: Right now, this parses the whole contents of the file into a string first.
	// While this is usually not necessary, because there exist adapters that make an input stream
	// iterable in both directions without storing it into a string, using the corresponding
	// Boost classes gives an awful output under valgrind and is thus disabled for the time being.
	std::string fileContent((std::istreambuf_iterator<char>(inputStream)), (std::istreambuf_iterator<char>()));
	BaseIteratorType stringIteratorBegin = fileContent.begin();
	BaseIteratorType stringIteratorEnd = fileContent.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, filename);
	PositionIteratorType positionIteratorBegin2(stringIteratorBegin, stringIteratorEnd, filename);
	PositionIteratorType positionIteratorEnd;

	// Prepare resulting intermediate representation of input.
	storm::ir::Program result;

	// In order to instantiate the grammar, we have to pass the type of the skipping parser.
	// As this is more complex, we let Boost figure out the actual type for us.
	prism::PrismGrammar grammar;
	try {
		// Now parse the content using phrase_parse in order to be able to supply a skipping parser.
		// First run.
		LOG4CPLUS_INFO(logger, "Start parsing...");
		qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
		grammar.prepareForSecondRun();
		result = storm::ir::Program();
		LOG4CPLUS_INFO(logger, "Start second parsing run...");
		// Second run.
		qi::phrase_parse(positionIteratorBegin2, positionIteratorEnd, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
		LOG4CPLUS_INFO(logger, "Finished parsing, here is the parsed program:" << std::endl << result.toString());
		// Reset grammars.
		grammar.resetGrammars();
	} catch(const qi::expectation_failure<PositionIteratorType>& e) {
		// If the parser expected content different than the one provided, display information
		// about the location of the error.
		const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();

		// Construct the error message including a caret display of the position in the
		// erroneous line.
		std::stringstream msg;
		std::string line = e.first.get_currentline();
		while (line.find('\t') != std::string::npos) line.replace(line.find('\t'),1," ");
		msg << pos.file << ", line " << pos.line << ", column " << pos.column
				<< ": parse error: expected " << e.what_ << std::endl << "\t"
				<< line << std::endl << "\t";
		int i = 0;
		for (i = 1; i < pos.column; ++i) {
			msg << "-";
		}
		msg << "^";
		for (; i < 80; ++i) {
			msg << "-";
		}
		msg << std::endl;

		std::cerr << msg.str();
		
		// Reset grammars in any case.
		grammar.resetGrammars();

		// Now propagate exception.
		throw storm::exceptions::WrongFileFormatException() << msg.str();
	}

	return result;
}

} // namespace parser

} // namespace storm
