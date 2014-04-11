#include "src/parser/prismparser/PrismParser.h"
#include "src/parser/prismparser/PrismGrammar.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/WrongFormatException.h"

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <limits>

namespace storm {
    namespace parser {
        storm::prism::Program PrismParser::parse(std::string const& filename, bool typeCheck) {
            // Open file and initialize result.
            std::ifstream inputFileStream(filename, std::ios::in);
            LOG_THROW(inputFileStream.good(), storm::exceptions::WrongFormatException, "Unable to read from file " << filename << ".");
            
            storm::prism::Program result;
            
            // Now try to parse the contents of the file.
            try {
                std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
                result = parseFromString(fileContent, filename, typeCheck);
            } catch(std::exception& e) {
                // In case of an exception properly close the file before passing exception.
                inputFileStream.close();
                throw e;
            }
            
            // Close the stream in case everything went smoothly and return result.
            inputFileStream.close();
            return result;
        }
        
        storm::prism::Program PrismParser::parseFromString(std::string const& input, std::string const& filename, bool typeCheck) {
            PositionIteratorType first(input.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(input.end());
            
            // Create empty result;
            storm::prism::Program result;
            
            // Create grammar.
            storm::parser::prism::PrismGrammar grammar(filename, first);
            try {
                // Now parse the content using phrase_parse in order to be able to supply a skipping parser.
                bool succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
                LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in first pass.");
                if (typeCheck) {
                    first = PositionIteratorType(input.begin());
                    iter = first;
                    last = PositionIteratorType(input.end());
                    grammar.toggleExpressionGeneration();
                    succeeded = qi::phrase_parse(iter, last, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, result);
                    LOG_THROW(succeeded,  storm::exceptions::WrongFormatException, "Parsing failed in second pass.");
                }
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                // If the parser expected content different than the one provided, display information about the location of the error.
                std::size_t lineNumber = boost::spirit::get_line(e.first);
                
                // Now propagate exception.
                LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << lineNumber << " of file " << filename << ".");
            }
            
            return result;
        }
    } // namespace parser
} // namespace storm
