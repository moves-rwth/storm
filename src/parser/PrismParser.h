/* * PrismParser.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_PARSER_PRISMPARSER_H_
#define STORM_PARSER_PRISMPARSER_H_

// All classes of the intermediate representation are used.
#include "src/ir/IR.h"

// Used for file input.
#include <istream>
#include <memory>

namespace storm {

namespace parser {

using namespace storm::ir;
using namespace storm::ir::expressions;

/*
 * This functions parse the format of the PRISM model checker into an intermediate representation.
 */

/*!
 * Parses the given file into the intermediate representation assuming it complies with the
 * PRISM syntax.
 * @param filename the name of the file to parse.
 * @return a shared pointer to the intermediate representation of the PRISM file.
 */
storm::ir::Program PrismParserFromFile(std::string const& filename);

/*!
 * Parses the given input stream into the intermediate representation assuming it complies with
 * the PRISM syntax.
 * @param inputStream the input stream to parse.
 * @param filename the name of the file the input stream belongs to. Used for diagnostics.
 * @return a shared pointer to the intermediate representation of the PRISM file.
 */
storm::ir::Program PrismParser(std::istream& inputStream, std::string const& filename);

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_PRISMPARSER_H_ */
