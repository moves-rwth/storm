#ifndef STORM_PARSER_PRISMPARSER_H_
#define STORM_PARSER_PRISMPARSER_H_

// All classes of the intermediate representation are used.
#include "src/storage/prism/Program.h"

// Used for file input.
#include <istream>

namespace storm {
    namespace parser {
        class PrismParser {
        public:
            /*!
             * Parses the given file into the PRISM storage classes assuming it complies with the PRISM syntax.
             *
             * @param filename the name of the file to parse.
             * @param typeCheck Sets whether the expressions are generated and therefore typechecked.
             * @return The resulting PRISM program.
             */
            static storm::prism::Program parse(std::string const& filename, bool typeCheck = true);
            
            /*!
             * Parses the given input stream into the PRISM storage classes assuming it complies with the PRISM syntax.
             *
             * @param input The input string to parse.
             * @param filename The name of the file from which the input was read.
             * @param typeCheck Sets whether the expressions are generated and therefore typechecked.
             * @return The resulting PRISM program.
             */
            static storm::prism::Program parseFromString(std::string const& input, std::string const& filename, bool typeCheck = true);
        };
    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRISMPARSER_H_ */
