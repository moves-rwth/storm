#include "src/parser/ExpressionParser.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/WrongFormatException.h"

#include "src/utility/constants.h"

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
        ExpressionParser::ExpressionParser(storm::expressions::ExpressionManager const& manager, qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_, bool enableErrorHandling, bool allowBacktracking) : ExpressionParser::base_type(expression), orOperator_(), andOperator_(), equalityOperator_(), relationalOperator_(), plusOperator_(), multiplicationOperator_(), infixPowerOperator_(), unaryOperator_(), floorCeilOperator_(), minMaxOperator_(), prefixPowerOperator_(), trueFalse_(manager), manager(manager.getSharedPointer()), createExpressions(false), acceptDoubleLiterals(true), identifiers_(nullptr), invalidIdentifiers_(invalidIdentifiers_) {
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&ExpressionParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            if (allowBacktracking) {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) >> expression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createFloorCeilExpression, phoenix::ref(*this), qi::_1, qi::_2, qi::_pass)];
            } else {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) > expression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createFloorCeilExpression, phoenix::ref(*this), qi::_1, qi::_2, qi::_pass)];
            }
            floorCeilExpression.name("floor/ceil expression");
            
            if (allowBacktracking) {
                minMaxExpression = ((minMaxOperator_[qi::_a = qi::_1] >> qi::lit("(")) >> expression[qi::_val = qi::_1] >> +(qi::lit(",") >> expression)[qi::_val = phoenix::bind(&ExpressionParser::createMinimumMaximumExpression, phoenix::ref(*this), qi::_val, qi::_a, qi::_1, qi::_pass)]) >> qi::lit(")");
            } else {
                minMaxExpression = ((minMaxOperator_[qi::_a = qi::_1] >> qi::lit("(")) > expression[qi::_val = qi::_1] > +(qi::lit(",") > expression)[qi::_val = phoenix::bind(&ExpressionParser::createMinimumMaximumExpression, phoenix::ref(*this), qi::_val, qi::_a, qi::_1, qi::_pass)]) > qi::lit(")");
            }
            minMaxExpression.name("min/max expression");
            
            if (allowBacktracking) {
                prefixPowerExpression = ((prefixPowerOperator_ >> qi::lit("(")) >> expression >> qi::lit(",") >> expression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_2, qi::_1, qi::_3, qi::_pass)];
            } else {
                prefixPowerExpression = ((prefixPowerOperator_ >> qi::lit("(")) > expression > qi::lit(",") > expression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_2, qi::_1, qi::_3, qi::_pass)];
            }
            prefixPowerExpression.name("pow expression");
            
			identifierExpression = identifier[qi::_val = phoenix::bind(&ExpressionParser::getIdentifierExpression, phoenix::ref(*this), qi::_1, allowBacktracking, qi::_pass)];
            identifierExpression.name("identifier expression");
            
            literalExpression = trueFalse_[qi::_val = qi::_1] | rationalLiteral_[qi::_val = phoenix::bind(&ExpressionParser::createDoubleLiteralExpression, phoenix::ref(*this), qi::_1, qi::_pass)] | qi::int_[qi::_val = phoenix::bind(&ExpressionParser::createIntegerLiteralExpression, phoenix::ref(*this), qi::_1, qi::_pass)];
            literalExpression.name("literal expression");
            
            atomicExpression = floorCeilExpression | prefixPowerExpression | minMaxExpression | (qi::lit("(") >> expression >> qi::lit(")")) | literalExpression | identifierExpression;
            atomicExpression.name("atomic expression");
            
            unaryExpression = (-unaryOperator_ >> atomicExpression)[qi::_val = phoenix::bind(&ExpressionParser::createUnaryExpression, phoenix::ref(*this), qi::_1, qi::_2, qi::_pass)];
            unaryExpression.name("unary expression");
            
            if (allowBacktracking) {
                infixPowerExpression = unaryExpression[qi::_val = qi::_1] > -(infixPowerOperator_ > expression)[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                infixPowerExpression = unaryExpression[qi::_val = qi::_1] > -(infixPowerOperator_ >> expression)[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            infixPowerExpression.name("power expression");
            
            if (allowBacktracking) {
                multiplicationExpression = infixPowerExpression[qi::_val = qi::_1] >> *(multiplicationOperator_ >> infixPowerExpression)[qi::_val = phoenix::bind(&ExpressionParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                multiplicationExpression = infixPowerExpression[qi::_val = qi::_1] > *(multiplicationOperator_ > infixPowerExpression)[qi::_val = phoenix::bind(&ExpressionParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            multiplicationExpression.name("multiplication expression");
            
            plusExpression = multiplicationExpression[qi::_val = qi::_1] > *(plusOperator_ >> multiplicationExpression)[qi::_val = phoenix::bind(&ExpressionParser::createPlusExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            plusExpression.name("plus expression");
            
            if (allowBacktracking) {
                relativeExpression = plusExpression[qi::_val = qi::_1] >> -(relationalOperator_ >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createRelationalExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                relativeExpression = plusExpression[qi::_val = qi::_1] > -(relationalOperator_ > plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createRelationalExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            relativeExpression.name("relative expression");
                        
            equalityExpression = relativeExpression[qi::_val = qi::_1] >> *(equalityOperator_ >> relativeExpression)[qi::_val = phoenix::bind(&ExpressionParser::createEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            equalityExpression.name("equality expression");
            
            if (allowBacktracking) {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ >> equalityExpression)[qi::_val = phoenix::bind(&ExpressionParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ > equalityExpression)[qi::_val = phoenix::bind(&ExpressionParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            andExpression.name("and expression");
            
            if (allowBacktracking) {
                orExpression = andExpression[qi::_val = qi::_1] >> *(orOperator_ >> andExpression)[qi::_val = phoenix::bind(&ExpressionParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            } else {
                orExpression = andExpression[qi::_val = qi::_1] > *(orOperator_ > andExpression)[qi::_val = phoenix::bind(&ExpressionParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
            }
            orExpression.name("or expression");
            
            iteExpression = orExpression[qi::_val = qi::_1] > -(qi::lit("?") > orExpression > qi::lit(":") > orExpression)[qi::_val = phoenix::bind(&ExpressionParser::createIteExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2, qi::_pass)];
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
            if (identifiers_ != nullptr) {
                this->createExpressions = true;
                this->identifiers_ = identifiers_;
            } else {
                this->createExpressions = false;
                this->identifiers_ = nullptr;
            }
        }

        void ExpressionParser::setIdentifierMapping(std::unordered_map<std::string, storm::expressions::Expression> const& identifierMapping) {
            unsetIdentifierMapping();
            this->createExpressions = true;
            this->identifiers_ = new qi::symbols<char, storm::expressions::Expression>();
            for (auto const& identifierExpressionPair : identifierMapping) {
                this->identifiers_->add(identifierExpressionPair.first, identifierExpressionPair.second);
            }
            deleteIdentifierMapping = true;
        }

        void ExpressionParser::unsetIdentifierMapping() {
            this->createExpressions = false;
            if (deleteIdentifierMapping) {
                delete this->identifiers_;
                deleteIdentifierMapping = false;
            }
            this->identifiers_ = nullptr;
        }
        
        void ExpressionParser::setAcceptDoubleLiterals(bool flag) {
            this->acceptDoubleLiterals = flag;
        }
        
        storm::expressions::Expression ExpressionParser::createIteExpression(storm::expressions::Expression e1, storm::expressions::Expression e2, storm::expressions::Expression e3, bool& pass) const {
            if (this->createExpressions) {
                try {
                    return storm::expressions::ite(e1, e2, e3);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createOrExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Or: return e1 || e2; break;
                        case storm::expressions::OperatorType::Implies: return storm::expressions::implies(e1, e2); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createAndExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                storm::expressions::Expression result;
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::And: result = e1 && e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
                return result;
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createRelationalExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::GreaterOrEqual: return e1 >= e2; break;
                        case storm::expressions::OperatorType::Greater: return e1 > e2; break;
                        case storm::expressions::OperatorType::LessOrEqual: return e1 <= e2; break;
                        case storm::expressions::OperatorType::Less: return e1 < e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createEqualsExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Equal: return e1.hasBooleanType() && e2.hasBooleanType() ? storm::expressions::iff(e1, e2) : e1 == e2; break;
                        case storm::expressions::OperatorType::NotEqual: return e1 != e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createPlusExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Plus: return e1 + e2; break;
                        case storm::expressions::OperatorType::Minus: return e1 - e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createMultExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Times: return e1 * e2; break;
                        case storm::expressions::OperatorType::Divide: return e1 / e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createPowerExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Power: return e1 ^ e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createUnaryExpression(boost::optional<storm::expressions::OperatorType> const& operatorType, storm::expressions::Expression const& e1, bool& pass) const {
            if (this->createExpressions) {
                try {
                    if (operatorType) {
                        switch (operatorType.get()) {
                            case storm::expressions::OperatorType::Not: return !e1; break;
                            case storm::expressions::OperatorType::Minus: return -e1; break;
                            default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                        }
                    } else {
                        return e1;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createDoubleLiteralExpression(storm::RationalNumber const& value, bool& pass) const {
            // If we are not supposed to accept double expressions, we reject it by setting pass to false.
            if (!this->acceptDoubleLiterals) {
                pass = false;
            }
            
            if (this->createExpressions) {
                return manager->rational(value);
            } else {
                return manager->boolean(false);
            }
        }
        
        storm::expressions::Expression ExpressionParser::createIntegerLiteralExpression(int value, bool& pass) const {
            if (this->createExpressions) {
                return manager->integer(value);
            } else {
                return manager->boolean(false);
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMinimumMaximumExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Min: return storm::expressions::minimum(e1, e2); break;
                        case storm::expressions::OperatorType::Max: return storm::expressions::maximum(e1, e2); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }

        storm::expressions::Expression ExpressionParser::createFloorCeilExpression(storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e1, bool& pass) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Floor: return storm::expressions::floor(e1); break;
                        case storm::expressions::OperatorType::Ceil: return storm::expressions::ceil(e1); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    pass = false;
                }
            }
            return manager->boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::getIdentifierExpression(std::string const& identifier, bool allowBacktracking, bool& pass) const {
            if (this->createExpressions) {
                STORM_LOG_THROW(this->identifiers_ != nullptr, storm::exceptions::WrongFormatException, "Unable to substitute identifier expressions without given mapping.");
                storm::expressions::Expression const* expression = this->identifiers_->find(identifier);
                if (expression == nullptr) {
                    pass = false;
                    return manager->boolean(false);
                }
                return *expression;
            } else {
                return manager->boolean(false);
            }
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
