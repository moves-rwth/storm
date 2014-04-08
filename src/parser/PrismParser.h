#ifndef STORM_PARSER_PRISMPARSER_H_
#define STORM_PARSER_PRISMPARSER_H_

// All classes of the intermediate representation are used.
#include "src/storage/prism/Program.h"

// Used for file input.
#include <istream>

namespace storm {
    namespace parser {
        using namespace storm::prism;
        using namespace storm::expressions;
        
        /*!
         * Parses the given file into the PRISM storage classes assuming it complies with the PRISM syntax.
         *
         * @param filename the name of the file to parse.
         * @return The resulting PRISM program.
         */
        storm::prism::Program PrismParserFromFile(std::string const& filename);
        
        /*!
         * Parses the given input stream into the PRISM storage classes assuming it complies with the PRISM syntax.
         *
         * @param inputStream The input stream to parse.
         * @param filename The name of the file the input stream belongs to.
         * @return The resulting PRISM program.
         */
        storm::prism::Program PrismParser(std::istream& inputStream, std::string const& filename);
        
    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRISMPARSER_H_ */
