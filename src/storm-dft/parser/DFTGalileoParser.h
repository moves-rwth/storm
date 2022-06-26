#pragma once

#include <map>

#include "storm-parsers/parser/ExpressionParser.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/storage/DFT.h"
#include "storm-parsers/parser/ValueParser.h"

namespace storm::dft {
namespace parser {

/*!
 * Parser for DFT in the Galileo format.
 */
template<typename ValueType>
class DFTGalileoParser {
   public:
    /*!
     * Parse DFT in Galileo format and build DFT.
     *
     * @param filename File.
     *
     * @return DFT.
     */
    static storm::dft::storage::DFT<ValueType> parseDFT(std::string const& filename);

   private:
    /*!
     * Parse element name (strip quotation marks, etc.).
     *
     * @param name Element name.
     *
     * @return Name.
     */
    static std::string parseName(std::string const& name);

    /*!
     * Parse basic element and add it to builder.
     *
     * @param name Name of BE.
     * @param input Input line.
     * @param lineNo Line number.
     * @param builder DFTBuilder.
     * @param valueParser ValueParser.
     *
     * @return True iff the parsing and creation was successful.
     */
    static bool parseBasicElement(std::string const& name, std::string const& input, size_t lineNo, storm::dft::builder::DFTBuilder<ValueType>& builder,
                                  storm::parser::ValueParser<ValueType>& valueParser);

    /*!
     * Parse argument of basic element of the form "name=value".
     *
     * @param name Name of BE.
     * @param input Input line. The parsed argument will be removed from the line.
     * @param valueParser ValueParser.
     *
     * @return Pair (success, value). Success is true iff the parsing was succesful. Then value contains the parsed value.
     */
    static std::pair<bool, ValueType> parseValue(std::string name, std::string& line, storm::parser::ValueParser<ValueType>& valueParser);

    /*!
     * Parse argument of basic element of type number: "name=number".
     *
     * @param name Name of BE.
     * @param input Input line. The parsed argument will be removed from the line.
     *
     * @return Pair (success, value). Success is true iff the parsing was successful. Then value contains the parsed value.
     */
    static std::pair<bool, size_t> parseNumber(std::string name, std::string& line);

    enum Distribution { None, Constant, Exponential, Erlang, Weibull, LogNormal };
};

}  // namespace parser
}  // namespace storm::dft
