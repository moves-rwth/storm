#ifndef STORM_PARSER_VALUEPARSER_H_
#define STORM_PARSER_VALUEPARSER_H_

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include "storm-parsers/parser/ExpressionParser.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"

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
inline NumberType parseNumber(std::string const& value, bool logError = true) {
    try {
        return boost::lexical_cast<NumberType>(value);
    } catch (boost::bad_lexical_cast&) {
        if (logError) {
            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException,
                            "Could not parse value '" << value << "' into " << typeid(NumberType).name() << ".");
        } else {
            throw storm::exceptions::WrongFormatException() << "Could not parse value '" << value << "' into " << typeid(NumberType).name() << ".";
        }
    }
}

template<>
inline storm::RationalNumber parseNumber(std::string const& value, bool logError) {
    storm::RationalNumber result;
    bool success = carl::try_parse(value, result);
    if (!success) {
        if(logError) {
            STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Cannot parse " << value << " as a rational number.");
        }
        else {
            STORM_LOG_TRACE("Cannot parse " << value << " as a rational number.");
            throw storm::exceptions::WrongFormatException() << "Cannot parse " << value << " as a rational number.";
        }
    }
    return result;
}

template<>
inline double parseNumber(std::string const& value, bool logError) {
    try {
        return boost::lexical_cast<double>(value);
    } catch (boost::bad_lexical_cast&) {
        return storm::utility::convertNumber<double>(parseNumber<storm::RationalNumber>(value, logError));
    }
}

template<>
inline storm::Interval parseNumber(std::string const& value, bool logError) {
    // Try whether it is a constant.
    STORM_LOG_ASSERT(logError, "Disabling logError is not supported for intervals.");
    try {
        return parseNumber<double>(value, false);
    } catch(storm::exceptions::WrongFormatException&) {
        // Okay, lets continue;
    }
    std::string intermediate = value;
    boost::trim(intermediate);
    carl::BoundType leftBound;
    carl::BoundType rightBound;
    if (intermediate.front() == '(') {
        leftBound = carl::BoundType::STRICT;
    } else {
        STORM_LOG_THROW(intermediate.front() == '[', storm::exceptions::WrongFormatException, "Could not parse value '" << value << "' into an interval. Expect start with '(' or '['.");
        leftBound = carl::BoundType::WEAK;
    }
    if (intermediate.back() == ')') {
        rightBound = carl::BoundType::STRICT;
    } else {
        STORM_LOG_THROW(intermediate.back() == ']', storm::exceptions::WrongFormatException, "Could not parse value '" << value << "' into an interval. Expect end with ']' or ')'");
        rightBound = carl::BoundType::WEAK;
    }
    intermediate = intermediate.substr(1,intermediate.size() - 2);

    std::vector<std::string> words;
    boost::split(words, intermediate, boost::is_any_of(","));
    STORM_LOG_THROW(words.size() == 2, storm::exceptions::WrongFormatException, "Could not parse value '" << value << "' into an interval. Did not find a comma.");
    double leftVal, rightVal;
    try {
        leftVal = parseNumber<double>(words[0]);
    } catch (storm::exceptions::WrongFormatException&) {
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Could not parse value '" << words[0] << "' as lower value of interval " << value << ".");
    }
    try {
        rightVal = parseNumber<double>(words[1]);
    } catch (storm::exceptions::WrongFormatException&) {
        STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Could not parse value '" << words[1] << "' as lower value of interval " << value << ".");
    }
    return storm::Interval(leftVal, leftBound, rightVal, rightBound);
}

}  // namespace parser
}  // namespace storm

#endif /* STORM_PARSER_VALUEPARSER_H_ */
