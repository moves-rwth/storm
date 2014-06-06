#include "src/parser/ExpressionParser.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidTypeException.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        ExpressionParser::ExpressionParser(qi::symbols<char, uint_fast64_t> const& invalidIdentifiers_) : ExpressionParser::base_type(expression), createExpressions(false), acceptDoubleLiterals(true), identifiers_(nullptr), invalidIdentifiers_(invalidIdentifiers_) {
            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]][qi::_pass = phoenix::bind(&ExpressionParser::isValidIdentifier, phoenix::ref(*this), qi::_1)];
            identifier.name("identifier");
            
            floorCeilExpression = ((qi::lit("floor")[qi::_a = true] | qi::lit("ceil")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&ExpressionParser::createFloorExpression, phoenix::ref(*this), qi::_1)] .else_ [qi::_val = phoenix::bind(&ExpressionParser::createCeilExpression, phoenix::ref(*this), qi::_1)]];
            floorCeilExpression.name("floor/ceil expression");
            
            minMaxExpression = ((qi::lit("min")[qi::_a = true] | qi::lit("max")[qi::_a = false]) >> qi::lit("(") >> plusExpression >> qi::lit(",") >> plusExpression >> qi::lit(")"))[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&ExpressionParser::createMinimumExpression, phoenix::ref(*this), qi::_1, qi::_2)] .else_ [qi::_val = phoenix::bind(&ExpressionParser::createMaximumExpression, phoenix::ref(*this), qi::_1, qi::_2)]];
            minMaxExpression.name("min/max expression");
            
            identifierExpression = identifier[qi::_val = phoenix::bind(&ExpressionParser::getIdentifierExpression, phoenix::ref(*this), qi::_1)];
            identifierExpression.name("identifier expression");
            
            literalExpression = qi::lit("true")[qi::_val = phoenix::bind(&ExpressionParser::createTrueExpression, phoenix::ref(*this))] | qi::lit("false")[qi::_val = phoenix::bind(&ExpressionParser::createFalseExpression, phoenix::ref(*this))] | strict_double[qi::_val = phoenix::bind(&ExpressionParser::createDoubleLiteralExpression, phoenix::ref(*this), qi::_1, qi::_pass)] | qi::int_[qi::_val = phoenix::bind(&ExpressionParser::createIntegerLiteralExpression, phoenix::ref(*this), qi::_1)];
            literalExpression.name("literal expression");
            
            atomicExpression = minMaxExpression | floorCeilExpression | qi::lit("(") >> expression >> qi::lit(")") | literalExpression | identifierExpression;
            atomicExpression.name("atomic expression");
            
            unaryExpression = atomicExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicExpression)[qi::_val = phoenix::bind(&ExpressionParser::createNotExpression, phoenix::ref(*this), qi::_1)] | (qi::lit("-") >> atomicExpression)[qi::_val = phoenix::bind(&ExpressionParser::createMinusExpression, phoenix::ref(*this), qi::_1)];
            unaryExpression.name("unary expression");
            
            powerExpression = unaryExpression[qi::_val = qi::_1] >> -(qi::lit("^") > expression)[qi::_val = phoenix::bind(&ExpressionParser::createPowerExpression, phoenix::ref(*this), qi::_val, qi::_1)];
            powerExpression.name("power expression");
            
            multiplicationExpression = powerExpression[qi::_val = qi::_1] >> *((qi::lit("*")[qi::_a = true] | qi::lit("/")[qi::_a = false]) >> powerExpression[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&ExpressionParser::createMultExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&ExpressionParser::createDivExpression, phoenix::ref(*this), qi::_val, qi::_1)]]);
            multiplicationExpression.name("multiplication expression");
            
            plusExpression = multiplicationExpression[qi::_val = qi::_1] >> *((qi::lit("+")[qi::_a = true] | qi::lit("-")[qi::_a = false]) >> multiplicationExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&ExpressionParser::createPlusExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&ExpressionParser::createMinusExpression, phoenix::ref(*this), qi::_val, qi::_1)]];
            plusExpression.name("plus expression");
            
            relativeExpression = (plusExpression >> qi::lit(">=") >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createGreaterOrEqualExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit(">") >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createGreaterExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit("<=") >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createLessOrEqualExpression, phoenix::ref(*this), qi::_1, qi::_2)] | (plusExpression >> qi::lit("<") >> plusExpression)[qi::_val = phoenix::bind(&ExpressionParser::createLessExpression, phoenix::ref(*this), qi::_1, qi::_2)] | plusExpression[qi::_val = qi::_1];
            relativeExpression.name("relative expression");
            
            equalityExpression = relativeExpression[qi::_val = qi::_1] >> *((qi::lit("=")[qi::_a = true] | qi::lit("!=")[qi::_a = false]) >> relativeExpression)[phoenix::if_(qi::_a) [ qi::_val = phoenix::bind(&ExpressionParser::createEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1) ] .else_ [ qi::_val = phoenix::bind(&ExpressionParser::createNotEqualsExpression, phoenix::ref(*this), qi::_val, qi::_1) ] ];
            equalityExpression.name("equality expression");
            
            andExpression = equalityExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> equalityExpression)[qi::_val = phoenix::bind(&ExpressionParser::createAndExpression, phoenix::ref(*this), qi::_val, qi::_1)];
            andExpression.name("and expression");
            
            orExpression = andExpression[qi::_val = qi::_1] >> *((qi::lit("|")[qi::_a = true] | qi::lit("=>")[qi::_a = false]) >> andExpression)[phoenix::if_(qi::_a) [qi::_val = phoenix::bind(&ExpressionParser::createOrExpression, phoenix::ref(*this), qi::_val, qi::_1)] .else_ [qi::_val = phoenix::bind(&ExpressionParser::createImpliesExpression, phoenix::ref(*this), qi::_val, qi::_1)] ];
            orExpression.name("or expression");
            
            iteExpression = orExpression[qi::_val = qi::_1] >> -(qi::lit("?") > orExpression > qi::lit(":") > orExpression)[qi::_val = phoenix::bind(&ExpressionParser::createIteExpression, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            iteExpression.name("if-then-else expression");
            
            expression %= iteExpression;
            expression.name("expression");
            
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
                    return e1.ite(e2, e3);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createImpliesExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1.implies(e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createOrExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 || e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createAndExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try{
                    return e1 && e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createGreaterExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 > e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createGreaterOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 >= e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createLessExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 < e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createLessOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 <= e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    if (e1.hasBooleanReturnType() && e2.hasBooleanReturnType()) {
                        return e1.iff(e2);
                    } else {
                        return e1 == e2;
                    }
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createNotEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 != e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createPlusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 + e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMinusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 - e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMultExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 * e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createPowerExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 ^ e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createDivExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return e1 / e2;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createNotExpression(storm::expressions::Expression e1) const {
            if (this->createExpressions) {
                try {
                    return !e1;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMinusExpression(storm::expressions::Expression e1) const {
            if (this->createExpressions) {
                try {
                    return -e1;
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createTrueExpression() const {
            if (this->createExpressions) {
                return storm::expressions::Expression::createFalse();
            } else {
                return storm::expressions::Expression::createTrue();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createFalseExpression() const {
            return storm::expressions::Expression::createFalse();
        }
        
        storm::expressions::Expression ExpressionParser::createDoubleLiteralExpression(double value, bool& pass) const {
            // If we are not supposed to accept double expressions, we reject it by setting pass to false.
            if (!this->acceptDoubleLiterals) {
                pass = false;
            }
            
            if (this->createExpressions) {
                return storm::expressions::Expression::createDoubleLiteral(value);
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createIntegerLiteralExpression(int value) const {
            if (this->createExpressions) {
                return storm::expressions::Expression::createIntegerLiteral(static_cast<int_fast64_t>(value));
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMinimumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return storm::expressions::Expression::minimum(e1, e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createMaximumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const {
            if (this->createExpressions) {
                try {
                    return storm::expressions::Expression::maximum(e1, e2);
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createFloorExpression(storm::expressions::Expression e1) const {
            if (this->createExpressions) {
                try {
                    return e1.floor();
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::createCeilExpression(storm::expressions::Expression e1) const {
            if (this->createExpressions) {
                try {
                    return e1.ceil();
                } catch (storm::exceptions::InvalidTypeException const& e) {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": " << e.what());
                }
            } else {
                return storm::expressions::Expression::createFalse();
            }
        }
        
        storm::expressions::Expression ExpressionParser::getIdentifierExpression(std::string const& identifier) const {
            if (this->createExpressions) {
                LOG_THROW(this->identifiers_ != nullptr, storm::exceptions::WrongFormatException, "Unable to substitute identifier expressions without given mapping.");
                storm::expressions::Expression const* expression = this->identifiers_->find(identifier);
                LOG_THROW(expression != nullptr, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(qi::_3) << ": Undeclared identifier '" << identifier << "'.");
                return *expression;
            } else {
                return storm::expressions::Expression::createFalse();
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