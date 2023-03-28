#ifndef STORM_PARSER_VALUEPARSER_H_
#define STORM_PARSER_VALUEPARSER_H_

#include <boost/lexical_cast.hpp>
#include "storm-parsers/parser/ExpressionParser.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/utility/constants.h"
namespace storm {
namespace parser {
/*!
 * Parser for values according to their ValueType.
 */
template<typename ValueType>
class ValueParser {
   public:
    /*!
     * Constructor.
     */
    ValueParser() : manager(new storm::expressions::ExpressionManager()), parser(*manager), evaluator(*manager) {
        // Set empty mapping to enable expression creation even without parameters
        parser.setIdentifierMapping(identifierMapping);
    }

    /*!
     * Parse ValueType from string.
     *
     * @param value String containing the value.
     *
     * @return ValueType
     */
    ValueType parseValue(std::string const& value) const;

    /*!
     * Add declaration of parameter.
     *
     * @param parameter New parameter.
     */
    void addParameter(std::string const& parameter);

   private:
    std::shared_ptr<storm::expressions::ExpressionManager> manager;
    storm::parser::ExpressionParser parser;
    storm::expressions::ExpressionEvaluator<ValueType> evaluator;
    std::unordered_map<std::string, storm::expressions::Expression> identifierMapping;
};

/*!
 * Parse number from string.
 *
 * @param value String containing the value.
 *
 * @return NumberType.
 */
template<typename NumberType>
inline NumberType parseNumber(std::string const& value) {
    try {
        return boost::lexical_cast<NumberType>(value);
    } catch (boost::bad_lexical_cast&) {
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Could not parse value '" << value << "' into " << typeid(NumberType).name() << ".");
    }
}

template<>
inline storm::RationalNumber parseNumber(std::string const& value) {
    return storm::utility::convertNumber<storm::RationalNumber>(value);
}

template<>
inline double parseNumber(std::string const& value) {
    try {
        return boost::lexical_cast<double>(value);
    } catch (boost::bad_lexical_cast&) {
        return storm::utility::convertNumber<double>(parseNumber<storm::RationalNumber>(value));
    }
}

}  // namespace parser
}  // namespace storm

#endif /* STORM_PARSER_VALUEPARSER_H_ */
