#include "src/parser/ExpressionParser.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        ExpressionParser::ExpressionParser(storm::expressions::ExpressionManager const& manager, qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_, bool allowBacktracking) : ExpressionParser::base_type(expression), orOperator_(), andOperator_(), equalityOperator_(), relationalOperator_(), plusOperator_(), multiplicationOperator_(), powerOperator_(), unaryOperator_(), floorCeilOperator_(), minMaxOperator_(), trueFalse_(manager), manager(manager), createExpressions(false), acceptDoubleLiterals(true), identifiers_(nullptr), invalidIdentifiers_(invalidIdentifiers_) {
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&ExpressionParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            if (allowBacktracking) {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) >> plusExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createFloorCeilExpression, phoenix::ref(*this), qi::_1, qi::_2)];
            } else {
                floorCeilExpression = ((floorCeilOperator_ >> qi::lit("(")) > plusExpression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createFloorCeilExpression, phoenix::ref(*this), qi::_1, qi::_2)];
            }
            floorCeilExpression.name("floor/ceil expression");
            
            if (allowBacktracking) {
                minMaxExpression = ((minMaxOperator_ >> qi::lit("(")) >> plusExpression >> qi::lit(",") >> plusExpression >> qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createMinimumMaximumExpression, phoenix::ref(*this), qi::_2, qi::_1, qi::_3)];
            } else {
                minMaxExpression = ((minMaxOperator_ >> qi::lit("(")) > plusExpression > qi::lit(",") > plusExpression > qi::lit(")"))[qi::_val = phoenix::bind(&ExpressionParser::createMinimumMaximumExpression, phoenix::ref(*this), qi::_2, qi::_1, qi::_3)];
            }
            minMaxExpression.name("min/max expression");
            
            identifierExpression = identifier[qi::_val = phoenix::bind(&ExpressionParser::getIdentifierExpression, phoenix::ref(*this), qi::_1, allowBacktracking, phoenix::ref(qi::_pass))];
            identifierExpression.name("identifier expression");
            
            literalExpression = trueFalse_[qi::_val = qi::_1] | strict_double[qi::_val = phoenix::bind(&ExpressionParser::createDoubleLiteralExpression, phoenix::ref(*this), qi::_1, qi::_pass)] | qi::int_[qi::_val = phoenix::bind(&ExpressionParser::createIntegerLiteralExpression, phoenix::ref(*this), qi::_1)];
            literalExpression.name("literal expression");
            
            atomicExpression = floorCeilExpression | minMaxExpression | (qi::lit("(") >> expression >> qi::lit(")")) | literalExpression | identifierExpression;
            atomicExpression.name("atomic expression");
            
            unaryExpression = (-unaryOperator_ >> atomicExpression)[qi::_val = phoenix::bind(&ExpressionParser::createUnaryExpression, phoenix::ref(*this), qi::_1, qi::_2)];
            unaryExpression.name("unary expression");
            
            if (allowBacktracking) {
                powerExpression = unaryExpression[qi::_val = qi::_1] > -(powerOperator_ > expression)[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            } else {
                powerExpression = unaryExpression[qi::_val = qi::_1] > -(powerOperator_ >> expression)[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            }
            powerExpression.name("power expression");
            
            if (allowBacktracking) {
                multiplicationExpression = powerExpression[qi::_val = qi::_1] >> *(multiplicationOperator_ >> powerExpression)[qi::_val = phoenix::bind(&ExpressionParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            } else {
                multiplicationExpression = powerExpression[qi::_val = qi::_1] > *(multiplicationOperator_ > powerExpression)[qi::_val = phoenix::bind(&ExpressionParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            }
            multiplicationExpression.name("multiplication expression");
            
            plusExpression = multiplicationExpression[qi::_val = qi::_1] > *(plusOperator_ >> multiplicationExpression)[qi::_val = phoenix::bind(&ExpressionParser::createPlusExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            plusExpression.name("plus expression");
            
            if (allowBacktracking) {
                relativeExpression = plusExpression[qi::_val = qi::_1] >> -(relationalOperator_ >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createRelationalExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            } else {
                relativeExpression = plusExpression[qi::_val = qi::_1] > -(relationalOperator_ > plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createRelationalExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            }
            relativeExpression.name("relative expression");
                        
            equalityExpression = relativeExpression[qi::_val = qi::_1] >> *(equalityOperator_ >> relativeExpression)[qi::_val = phoenix::bind(&ExpressionParser::createEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            equalityExpression.name("equality expression");
            
            if (allowBacktracking) {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ >> equalityExpression)[qi::_val = phoenix::bind(&ExpressionParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            } else {
                andExpression = equalityExpression[qi::_val = qi::_1] >> *(andOperator_ > equalityExpression)[qi::_val = phoenix::bind(&ExpressionParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            }
            andExpression.name("and expression");
            
            if (allowBacktracking) {
                orExpression = andExpression[qi::_val = qi::_1] >> *(orOperator_ >> andExpression)[qi::_val = phoenix::bind(&ExpressionParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            } else {
                orExpression = andExpression[qi::_val = qi::_1] > *(orOperator_ > andExpression)[qi::_val = phoenix::bind(&ExpressionParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            }
            orExpression.name("or expression");
            
            iteExpression = orExpression[qi::_val = qi::_1] > -(qi::lit("?") > orExpression > qi::lit(":") > orExpression)[qi::_val = phoenix::bind(&ExpressionParser::createIteExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
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
            debug(powerExpression);
            debug(unaryExpression);
            debug(atomicExpression);
            debug(literalExpression);
            debug(identifierExpression);
             */
            
            
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
        
        void ExpressionParser::setIdentifierMapping(qi::symbols<char, storm::expressions::Expression> const* identifiers_) {
            if (identifiers_ != nullptr) {
                this->createExpressions = true;
                this->identifiers_ = identifiers_;
            } else {
                this->createExpressions = false;
                this->identifiers_ = nullptr;
            }
        }
        
        void ExpressionParser::unsetIdentifierMapping() {
            this->createExpressions = false;
            this->identifiers_ = nullptr;
        }
        
        void ExpressionParser::setAcceptDoubleLiterals(bool flag) {
            this->acceptDoubleLiterals = flag;
        }
        
        storm::expressions::Expression ExpressionParser::createIteExpression(storm::expressions::Expression e1, storm::expressions::Expression e2, storm::expressions::Expression e3) const {
            if (this->createExpressions) {
                try {
                    return storm::expressions::ite(e1, e2, e3);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createOrExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Or: return e1 || e2; break;
                        case storm::expressions::OperatorType::Implies: return storm::expressions::implies(e1, e2); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createAndExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::And: return e1 && e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createRelationalExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
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
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createEqualsExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Equal: return e1.hasBooleanType() && e2.hasBooleanType() ? storm::expressions::iff(e1, e2) : e1 == e2; break;
                        case storm::expressions::OperatorType::NotEqual: return e1 != e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createPlusExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Plus: return e1 + e2; break;
                        case storm::expressions::OperatorType::Minus: return e1 - e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createMultExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Times: return e1 * e2; break;
                        case storm::expressions::OperatorType::Divide: return e1 / e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::createPowerExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Power: return e1 ^ e2; break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createUnaryExpression(boost::optional<storm::expressions::OperatorType> const& operatorType, storm::expressions::Expression const& e1) const {
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
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
                
        storm::expressions::Expression ExpressionParser::createDoubleLiteralExpression(double value, bool& pass) const {
            // If we are not supposed to accept double expressions, we reject it by setting pass to false.
            if (!this->acceptDoubleLiterals) {
                pass = false;
            }
            
            if (this->createExpressions) {
                return manager.rational(value);
            } else {
                return manager.boolean(false);
            }
        }
        
        storm::expressions::Expression ExpressionParser::createIntegerLiteralExpression(int value) const {
            if (this->createExpressions) {
                return manager.integer(value);
            } else {
                return manager.boolean(false);
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMinimumMaximumExpression(storm::expressions::Expression const& e1, storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e2) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Min: return storm::expressions::minimum(e1, e2); break;
                        case storm::expressions::OperatorType::Max: return storm::expressions::maximum(e1, e2); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }

        storm::expressions::Expression ExpressionParser::createFloorCeilExpression(storm::expressions::OperatorType const& operatorType, storm::expressions::Expression const& e1) const {
            if (this->createExpressions) {
                try {
                    switch (operatorType) {
                        case storm::expressions::OperatorType::Floor: return storm::expressions::floor(e1); break;
                        case storm::expressions::OperatorType::Ceil: return storm::expressions::ceil(e1); break;
                        default: STORM_LOG_ASSERT(false, "Invalid operation."); break;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            }
            return manager.boolean(false);
        }
        
        storm::expressions::Expression ExpressionParser::getIdentifierExpression(std::string const& identifier, bool allowBacktracking, bool& pass) const {
            if (this->createExpressions) {
                STORM_LOG_THROW(this->identifiers_ != nullptr, storm::exceptions::WrongFormatException, "Unable to substitute identifier expressions without given mapping.");
                storm::expressions::Expression const* expression = this->identifiers_->find(identifier);
                if (expression == nullptr) {
                    if (allowBacktracking) {
                        pass = false;
                        return manager.boolean(false);
                    } else {
                        STORM_LOG_THROW(expression != nullptr, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": Undeclared identifier '" << identifier << "'.");
                    }
                }
                return *expression;
            } else {
                return manager.boolean(false);
            }
        }
        
        bool ExpressionParser::isValidIdentifier(std::string const& identifier) {
            if (this->invalidIdentifiers_.find(identifier) != nullptr) {
                return false;
            }
            return true;
        }
    }
}