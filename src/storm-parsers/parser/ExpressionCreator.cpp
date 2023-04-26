#include "ExpressionCreator.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidTypeException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/OperatorType.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
namespace parser {

ExpressionCreator::ExpressionCreator(storm::expressions::ExpressionManager const& manager) : manager(manager) {
    // Intenetionally left empty.
}

ExpressionCreator::~ExpressionCreator() {
    if (deleteIdentifierMapping) {
        delete this->identifiers;
    }
}

storm::expressions::Expression ExpressionCreator::createIteExpression(storm::expressions::Expression const& e1, storm::expressions::Expression const& e2,
                                                                      storm::expressions::Expression const& e3, bool& pass) const {
    if (this->createExpressions) {
        try {
            return storm::expressions::ite(e1, e2, e3);
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createOrExpression(storm::expressions::Expression const& e1,
                                                                     storm::expressions::OperatorType const& operatorType,
                                                                     storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Or:
                    return e1 || e2;
                    break;
                case storm::expressions::OperatorType::Implies:
                    return storm::expressions::implies(e1, e2);
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createAndExpression(storm::expressions::Expression const& e1,
                                                                      storm::expressions::OperatorType const& operatorType,
                                                                      storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        storm::expressions::Expression result;
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::And:
                    result = e1 && e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
        return result;
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createRelationalExpression(storm::expressions::Expression const& e1,
                                                                             storm::expressions::OperatorType const& operatorType,
                                                                             storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::GreaterOrEqual:
                    return e1 >= e2;
                    break;
                case storm::expressions::OperatorType::Greater:
                    return e1 > e2;
                    break;
                case storm::expressions::OperatorType::LessOrEqual:
                    return e1 <= e2;
                    break;
                case storm::expressions::OperatorType::Less:
                    return e1 < e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createEqualsExpression(storm::expressions::Expression const& e1,
                                                                         storm::expressions::OperatorType const& operatorType,
                                                                         storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Equal:
                    return e1.hasBooleanType() && e2.hasBooleanType() ? storm::expressions::iff(e1, e2) : e1 == e2;
                    break;
                case storm::expressions::OperatorType::NotEqual:
                    return e1 != e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createPlusExpression(storm::expressions::Expression const& e1,
                                                                       storm::expressions::OperatorType const& operatorType,
                                                                       storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Plus:
                    return e1 + e2;
                    break;
                case storm::expressions::OperatorType::Minus:
                    return e1 - e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createMultExpression(storm::expressions::Expression const& e1,
                                                                       storm::expressions::OperatorType const& operatorType,
                                                                       storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Times:
                    return e1 * e2;
                    break;
                case storm::expressions::OperatorType::Divide:
                    return e1 / e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createPowerModuloExpression(storm::expressions::Expression const& e1,
                                                                              storm::expressions::OperatorType const& operatorType,
                                                                              storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Power:
                    return storm::expressions::pow(e1, e2, true);
                    break;
                case storm::expressions::OperatorType::Modulo:
                    return e1 % e2;
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createUnaryExpression(std::vector<storm::expressions::OperatorType> const& operatorTypes,
                                                                        storm::expressions::Expression const& e1, bool& pass) const {
    if (this->createExpressions) {
        try {
            storm::expressions::Expression result = e1;
            for (auto const& op : operatorTypes) {
                switch (op) {
                    case storm::expressions::OperatorType::Not:
                        result = !result;
                        break;
                    case storm::expressions::OperatorType::Minus:
                        result = -result;
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "Invalid operation.");
                        break;
                }
            }
            return result;
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createRationalLiteralExpression(storm::RationalNumber const& value, bool& pass) const {
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

storm::expressions::Expression ExpressionCreator::createIntegerLiteralExpression(int64_t value, bool&) const {
    if (this->createExpressions) {
        return manager.integer(value);
    } else {
        return manager.boolean(false);
    }
}

storm::expressions::Expression ExpressionCreator::createBooleanLiteralExpression(bool value, bool&) const {
    if (this->createExpressions) {
        return manager.boolean(value);
    } else {
        return manager.boolean(false);
    }
}

storm::expressions::Expression ExpressionCreator::createMinimumMaximumExpression(storm::expressions::Expression const& e1,
                                                                                 storm::expressions::OperatorType const& operatorType,
                                                                                 storm::expressions::Expression const& e2, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Min:
                    return storm::expressions::minimum(e1, e2);
                    break;
                case storm::expressions::OperatorType::Max:
                    return storm::expressions::maximum(e1, e2);
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createFloorCeilExpression(storm::expressions::OperatorType const& operatorType,
                                                                            storm::expressions::Expression const& e1, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (operatorType) {
                case storm::expressions::OperatorType::Floor:
                    return storm::expressions::floor(e1);
                    break;
                case storm::expressions::OperatorType::Ceil:
                    return storm::expressions::ceil(e1);
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Invalid operation.");
                    break;
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createRoundExpression(storm::expressions::Expression const& e1, bool& pass) const {
    if (this->createExpressions) {
        try {
            return storm::expressions::round(e1);
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::createPredicateExpression(storm::expressions::OperatorType const& opTyp,
                                                                            std::vector<storm::expressions::Expression> const& operands, bool& pass) const {
    if (this->createExpressions) {
        try {
            switch (opTyp) {
                case storm::expressions::OperatorType::AtLeastOneOf:
                    return storm::expressions::atLeastOneOf(operands);
                case storm::expressions::OperatorType::AtMostOneOf:
                    return storm::expressions::atMostOneOf(operands);
                case storm::expressions::OperatorType::ExactlyOneOf:
                    return storm::expressions::exactlyOneOf(operands);
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Operator type " << opTyp << " invalid for predicate expression.");
            }
        } catch (storm::exceptions::InvalidTypeException const& e) {
            pass = false;
        }
    }
    return manager.boolean(false);
}

storm::expressions::Expression ExpressionCreator::getIdentifierExpression(std::string const& identifier, bool& pass) const {
    if (this->createExpressions) {
        STORM_LOG_THROW(this->identifiers != nullptr, storm::exceptions::WrongFormatException,
                        "Unable to substitute identifier expressions without given mapping.");
        storm::expressions::Expression const* expression = this->identifiers->find(identifier);
        if (expression == nullptr) {
            pass = false;
            return manager.boolean(false);
        }
        return *expression;
    } else {
        return manager.boolean(false);
    }
}

void ExpressionCreator::setIdentifierMapping(qi::symbols<char, storm::expressions::Expression> const* identifiers_) {
    if (identifiers_ != nullptr) {
        createExpressions = true;
        identifiers = identifiers_;
    } else {
        createExpressions = false;
        identifiers = nullptr;
    }
}

void ExpressionCreator::setIdentifierMapping(std::unordered_map<std::string, storm::expressions::Expression> const& identifierMapping) {
    unsetIdentifierMapping();

    createExpressions = true;
    identifiers = new qi::symbols<char, storm::expressions::Expression>();
    for (auto const& identifierExpressionPair : identifierMapping) {
        identifiers->add(identifierExpressionPair.first, identifierExpressionPair.second);
    }
    deleteIdentifierMapping = true;
}

void ExpressionCreator::unsetIdentifierMapping() {
    createExpressions = false;
    if (deleteIdentifierMapping) {
        delete this->identifiers;
        deleteIdentifierMapping = false;
    }
    this->identifiers = nullptr;
}

}  // namespace parser
}  // namespace storm
