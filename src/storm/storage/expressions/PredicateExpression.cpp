
#include "storm/storage/expressions/PredicateExpression.h"

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/utility/macros.h"
#include "storm/storage/BitVector.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        OperatorType toOperatorType(PredicateExpression::PredicateType tp) {
            switch (tp) {
                case PredicateExpression::PredicateType::AtMostOneOf: return OperatorType::AtMostOneOf;
                case PredicateExpression::PredicateType::AtLeastOneOf: return OperatorType::AtLeastOneOf;
                case PredicateExpression::PredicateType::ExactlyOneOf: return OperatorType::ExactlyOneOf;
            }
            STORM_LOG_ASSERT(false, "Predicate type not supported");
        }

        PredicateExpression::PredicateExpression(ExpressionManager const &manager, Type const& type,  std::vector <std::shared_ptr<BaseExpression const>> const &operands, PredicateType predicateType) : BaseExpression(manager, type), predicate(predicateType), operands(operands) {}

        // Override base class methods.
        storm::expressions::OperatorType PredicateExpression::getOperator() const {
            return toOperatorType(predicate);
        }

        bool PredicateExpression::evaluateAsBool(Valuation const *valuation) const {
            STORM_LOG_THROW(this->hasBooleanType(), storm::exceptions::InvalidTypeException, "Unable to evaluate expression as boolean.");
            storm::storage::BitVector results(operands.size());
            uint64_t i = 0;
            for(auto const& operand : operands) {
                results.set(i, operand->evaluateAsBool(valuation));
                ++i;
            }
            switch(predicate) {
                case PredicateType::ExactlyOneOf: return results.getNumberOfSetBits() == 1;
                case PredicateType::AtMostOneOf: return results.getNumberOfSetBits() <= 1;
                case PredicateType::AtLeastOneOf: return results.getNumberOfSetBits() >= 1;
            }
            STORM_LOG_ASSERT(false, "Unknown predicate type");
        }

        std::shared_ptr<BaseExpression const> PredicateExpression::simplify() const {
            std::vector<std::shared_ptr<BaseExpression const>> simplifiedOperands;
            for (auto const& operand : operands) {
                simplifiedOperands.push_back(operand->simplify());
            }
            return std::shared_ptr<BaseExpression>(new PredicateExpression(this->getManager(), this->getType(), simplifiedOperands, predicate));
        }

        boost::any PredicateExpression::accept(ExpressionVisitor &visitor, boost::any const &data) const {
            return visitor.visit(*this, data);
        }

        bool PredicateExpression::isPredicateExpression() const {
            return true;
        }

        bool PredicateExpression::isFunctionApplication() const {
            return true;
        }

        bool PredicateExpression::containsVariables() const {
            for(auto const& operand : operands) {
                if(operand->containsVariables()) {
                    return true;
                }
            }
            return false;
        }

        uint_fast64_t PredicateExpression::getArity() const {
            return operands.size();
        }

        std::shared_ptr<BaseExpression const> PredicateExpression::getOperand(uint_fast64_t operandIndex) const {
            STORM_LOG_ASSERT(operandIndex < this->getArity(), "Invalid operand access");
            return operands[operandIndex];
        }

        void PredicateExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
            for(auto const& operand : operands) {
                operand->gatherVariables(variables);
            }
        }

        /*!
         * Retrieves the relation associated with the expression.
         *
         * @return The relation associated with the expression.
         */
        PredicateExpression::PredicateType PredicateExpression::getPredicateType() const {
            return predicate;
        }

        void PredicateExpression::printToStream(std::ostream& stream) const {

        }
    }
}