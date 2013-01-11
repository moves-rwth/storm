/*
 * PrismParser.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

#include "PrismParser.h"
#include "src/utility/OsDetection.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

// Needed for file IO.
#include <fstream>
#include <iomanip>

// Some typedefs and namespace definitions to reduce code size.
typedef std::istreambuf_iterator<char> base_iterator_type;
typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
typedef boost::spirit::classic::position_iterator2<forward_iterator_type> pos_iterator_type;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace storm {

namespace parser {

/*!
 * Opens the given file for parsing, invokes the helper function to parse the actual content and
 * closes the file properly, even if an exception is thrown in the parser. In this case, the
 * exception is passed on to the caller.
 */
std::shared_ptr<storm::ir::Program> PrismParser::parseFile(std::string const& filename) const {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);
	std::shared_ptr<storm::ir::Program> result(nullptr);

	// Now try to parse the contents of the file.
	try {
		result = std::shared_ptr<storm::ir::Program>(parse(inputFileStream, filename));
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
std::shared_ptr<storm::ir::Program> PrismParser::parse(std::istream& inputStream, std::string const& filename) const {
		// Prepare iterators to input.
		base_iterator_type in_begin(inputStream);
		forward_iterator_type fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
		forward_iterator_type fwd_end;
		pos_iterator_type position_begin(fwd_begin, fwd_end, filename);
		pos_iterator_type position_end;

		// Prepare resulting intermediate representation of input.
		std::shared_ptr<storm::ir::Program> result(new storm::ir::Program());

		// In order to instantiate the grammar, we have to pass the type of the skipping parser.
		// As this is more complex, we let Boost figure out the actual type for us.
		prismGrammar<pos_iterator_type,  BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol)> grammar;
		try {
			// Now parse the content using phrase_parse in order to be able to supply a skipping
			// parser.
			phrase_parse(position_begin, position_end, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, *result);
		} catch(const qi::expectation_failure<pos_iterator_type>& e) {
			// If the parser expected content different than the one provided, display information
			// about the location of the error.
			const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();

			// Construct the error message including a caret display of the position in the
			// erroneous line.
			std::stringstream msg;
			msg << pos.file << ", line " << pos.line << ", column " << pos.column
					<< ": parse error: expected " << e.what_ << std::endl << "\t"
					<< e.first.get_currentline() << std::endl << "\t";
			int i = 0;
			for (i = 0; i < pos.column; ++i) {
				msg << "-";
			}
			msg << "^";
			for (; i < 80; ++i) {
				msg << "-";
			}
			msg << std::endl;

// On Mac OS, exception messages are not displayed in case an exception is propagated to the
// operating system, so we need to display the message ourselves.
#if defined(MACOSX)
			std::cout << msg.str();
#endif
			// Now propagate exception.
			throw storm::exceptions::WrongFileFormatException() << msg.str();
		}

		return result;
	}

} // namespace parser

} // namespace storm
