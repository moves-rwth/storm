#include "storm/parser/ExpressionParser.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidTypeException.h"
#include "storm/exceptions/WrongFormatException.h"

#include "storm/utility/constants.h"
#include "storm/parser/ExpressionCreator.h"
#include "storm/storage/expressions/Expression.h"


namespace boost {
    namespace spirit {
        namespace traits {
            template<>
            bool scale(int exp, storm::RationalNumber& r, storm::RationalNumber acc) {
                if (exp >= 0) {
                    r = acc * storm::utility::pow(storm::RationalNumber(10), static_cast<uint_fast64_t>(exp));
                } else {
                    r = acc / storm::utility::pow(storm::RationalNumber(10), static_cast<uint_fast64_t>(-exp));
                }
                return true;
            }
            
            template<>
            bool is_equal_to_one(storm::RationalNumber const& value) {
                return storm::utility::isOne(value);
            }
            
            template<>
            storm::RationalNumber negate(bool neg, storm::RationalNumber const& number) {
                return neg ? storm::RationalNumber(-number) : number;
            }
        }
    }
}

namespace storm {
    namespace parser {
        ExpressionParser::ExpressionParser(storm::expressions::ExpressionManager const& manager, qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_, bool enableErrorHandling, bool allowBacktracking) : ExpressionParser::base_type(expression), orOperator_(), andOperator_(), equalityOperator_(), relationalOperator_(), plusOperator_(), multiplicationOperator_(), infixPowerOperator_(), unaryOperator_(), floorCeilOperator_(), minMaxOperator_(), prefixPowerOperator_(), invalidIdentifiers_(invalidIdentifiers_) {
            
            expressionCreator = new ExpressionCreator(manager);
            
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_') | qi::char_('.')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&ExpressionParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            if (allowBacktracking) {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) >> expression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionCreator::createFloorCeilExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_2, qi::_pass)];
            } else {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) > expression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionCreator::createFloorCeilExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_2, qi::_pass)];
            }
            floorCeilExpression.name("floor/ceil expression");
            
            if (allowBacktracking) {
                minMaxExpression = ((minMaxOperator_[qi::_a = qi::_1] >> qi::lit("(")) >> expression[qi::_val = qi::_1] >> +(qi::lit(",") >> expression)[qi::_val = phoenix::bind(&ExpressionCreator::createMinimumMaximumExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_a, qi::_1, qi::_pass)]) >> qi::lit(")");
            } else {
                minMaxExpression = ((minMaxOperator_[qi::_a = qi::_1] >> qi::lit("(")) > expression[qi::_val = qi::_1] > +(qi::lit(",") > expression)[qi::_val = phoenix::bind(&ExpressionCreator::createMinimumMaximumExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_a, qi::_1, qi::_pass)]) > qi::lit(")");
            }
            minMaxExpression.name("min/max expression");
            
            if (allowBacktracking) {
                prefixPowerExpression = ((prefixPowerOperator_ >> qi::lit("(")) >> expression >> qi::lit(",") >> expression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionCreator::createPowerExpression, phoenix::ref(*expressionCreator), qi::_2, qi::_1, qi::_3, qi::_pass)];
            } else {
                prefixPowerExpression = ((prefixPowerOperator_ >> qi::lit("(")) > expression > qi::lit(",") > expression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionCreator::createPowerExpression, phoenix::ref(*expressionCreator), qi::_2, qi::_1, qi::_3, qi::_pass)];
            }
            prefixPowerExpression.name("pow expression");
            
			identifierExpression = identifier[qi::_val = phoenix::bind(&ExpressionCreator::getIdentifierExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_pass)];
            identifierExpression.name("identifier expression");
            
            literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&ExpressionCreator::createBooleanLiteralExpression, phoenix::ref(*expressionCreator), true, qi::_pass)]
                | qi::lit("false")[qi::_val = phoenix::bind(&ExpressionCreator::createBooleanLiteralExpression, phoenix::ref(*expressionCreator), false, qi::_pass)]
                | rationalLiteral_[qi::_val = phoenix::bind(&ExpressionCreator::createRationalLiteralExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_pass)]
                | qi::int_[qi::_val = phoenix::bind(&ExpressionCreator::createIntegerLiteralExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_pass)];
            literalExpression.name("literal expression");
            
            atomicExpression = floorCeilExpression | prefixPowerExpression | minMaxExpression | (qi::lit("(") >> expression >> qi::lit(")")) | literalExpression | identifierExpression;
            atomicExpression.name("atomic expression");
            
            unaryExpression = (-unaryOperator_ >> atomicExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createUnaryExpression, phoenix::ref(*expressionCreator), qi::_1, qi::_2, qi::_pass)];
            unaryExpression.name("unary expression");
            
            if (allowBacktracking) {
                infixPowerExpression = unaryExpression[qi::_val = qi::_1] > -(infixPowerOperator_ > expression)[qi::_val = phoenix::bind(&ExpressionCreator::createPowerExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                infixPowerExpression = unaryExpression[qi::_val = qi::_1] > -(infixPowerOperator_ >> expression)[qi::_val = phoenix::bind(&ExpressionCreator::createPowerExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            infixPowerExpression.name("power expression");
            
            if (allowBacktracking) {
                multiplicationExpression = infixPowerExpression[qi::_val = qi::_1] >> *(multiplicationOperator_ >> infixPowerExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createMultExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                multiplicationExpression = infixPowerExpression[qi::_val = qi::_1] > *(multiplicationOperator_ > infixPowerExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createMultExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            multiplicationExpression.name("multiplication expression");
            
            plusExpression = multiplicationExpression[qi::_val = qi::_1] > *(plusOperator_ >> multiplicationExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createPlusExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            plusExpression.name("plus expression");
            
            if (allowBacktracking) {
                relativeExpression = plusExpression[qi::_val = qi::_1] >> -(relationalOperator_ >> plusExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createRelationalExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                relativeExpression = plusExpression[qi::_val = qi::_1] > -(relationalOperator_ > plusExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createRelationalExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            relativeExpression.name("relative expression");
                        
            equalityExpression = relativeExpression[qi::_val = qi::_1] >> *(equalityOperator_ >> relativeExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createEqualsExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            equalityExpression.name("equality expression");
            
            if (allowBacktracking) {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ >> equalityExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createAndExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ > equalityExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createAndExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            andExpression.name("and expression");
            
            if (allowBacktracking) {
                orExpression = andExpression[qi::_val = qi::_1] >> *(orOperator_ >> andExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createOrExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                orExpression = andExpression[qi::_val = qi::_1] > *(orOperator_ > andExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createOrExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            orExpression.name("or expression");
            
            iteExpression = orExpression[qi::_val = qi::_1] > -(qi::lit("?") > iteExpression > qi::lit(":") > iteExpression)[qi::_val = phoenix::bind(&ExpressionCreator::createIteExpression, phoenix::ref(*expressionCreator), qi::_val, qi::_1, qi::_2, qi::_pass)];
            iteExpression.name("if-then-else expression");
            
            expression %= iteExpression;
            expression.name("expression");
            
            /*!
             * Enable this for debugging purposes.
            debug(expression);
            debug(iteExpression);
            debug(orExpression);
            debug(andExpression);
            debug(equalityExpression);
            debug(relativeExpression);
            debug(plusExpression);
            debug(multiplicationExpression);
            debug(infixPowerExpression);
            debug(unaryExpression);
            debug(atomicExpression);
            debug(literalExpression);
            debug(identifierExpression);
             */
            
            if (enableErrorHandling) {
                // Enable error reporting.
                qi::on_error<qi::fail>(expression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(iteExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(orExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(andExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(equalityExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(relativeExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(plusExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(multiplicationExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(unaryExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(atomicExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(literalExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(identifierExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(minMaxExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
                qi::on_error<qi::fail>(floorCeilExpression, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            }
        }
        
        void ExpressionParser::setIdentifierMapping(qi::symbols<char, storm::expressions::Expression> const* identifiers_) {
            expressionCreator->setIdentifierMapping(identifiers_);
        }

        void ExpressionParser::setIdentifierMapping(std::unordered_map<std::string, storm::expressions::Expression> const& identifierMapping) {
            expressionCreator->setIdentifierMapping(identifierMapping);
        }

        void ExpressionParser::unsetIdentifierMapping() {
            expressionCreator->unsetIdentifierMapping();
        }
        
        void ExpressionParser::setAcceptDoubleLiterals(bool flag) {
            expressionCreator->setAcceptDoubleLiterals(flag);
        }
                        
                
        bool ExpressionParser::isValidIdentifier(std::string const& identifier) {
            if (this->invalidIdentifiers_.find(identifier) != nullptr) {
                return false;
            }
            return true;
        }

        storm::expressions::Expression ExpressionParser::parseFromString(std::string const& expressionString) const {
            PositionIteratorType first(expressionString.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(expressionString.end());

            // Create empty result;
            storm::expressions::Expression result;

            try {
                // Start parsing.
                bool succeeded = qi::phrase_parse(iter, last, *this, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded, storm::exceptions::WrongFormatException, "Could not parse expression.");
                STORM_LOG_DEBUG("Parsed expression successfully.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, e.what_);
            }

            return result;
        }
    }
}
