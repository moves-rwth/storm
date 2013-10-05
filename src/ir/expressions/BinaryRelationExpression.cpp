/*
 * BinaryBooleanFunctionExpression.cpp
 *
 *  Created on: 10.06.2013
 *      Author: Christian Dehnert
 */

#include <sstream>

#include "BinaryRelationExpression.h"

namespace storm {
    namespace ir {
        namespace expressions {
            
            BinaryRelationExpression::BinaryRelationExpression(std::shared_ptr<BaseExpression> const& left, std::shared_ptr<BaseExpression> const& right, RelationType relationType)
            : BinaryExpression(bool_, left, right), relationType(relationType) {
                // Nothing to do here.
            }
            
            BinaryRelationExpression::BinaryRelationExpression(BinaryRelationExpression const& binaryRelationExpression)
            : BinaryExpression(binaryRelationExpression), relationType(binaryRelationExpression.relationType) {
                // Nothing to do here.
            }
            
            std::shared_ptr<BaseExpression> BinaryRelationExpression::clone() const {
                return std::shared_ptr<BaseExpression>(new BinaryRelationExpression(this->getLeft()->clone(), this->getRight()->clone(), relationType));
            }

            std::shared_ptr<BaseExpression> BinaryRelationExpression::clone(std::map<std::string, std::string> const& renaming, storm::parser::prism::VariableState const& variableState) const {
                return std::shared_ptr<BaseExpression>(new BinaryRelationExpression(this->getLeft()->clone(renaming, variableState), this->getRight()->clone(renaming, variableState), this->relationType));
            }
            
            bool BinaryRelationExpression::getValueAsBool(std::pair<std::vector<bool>, std::vector<int_fast64_t>> const* variableValues) const {
                int_fast64_t resultLeft = this->getLeft()->getValueAsInt(variableValues);
                int_fast64_t resultRight = this->getRight()->getValueAsInt(variableValues);
                switch(relationType) {
                    case EQUAL: return resultLeft == resultRight; break;
                    case NOT_EQUAL: return resultLeft != resultRight; break;
                    case LESS: return resultLeft < resultRight; break;
                    case LESS_OR_EQUAL: return resultLeft <= resultRight; break;
                    case GREATER: return resultLeft > resultRight; break;
                    case GREATER_OR_EQUAL: return resultLeft >= resultRight; break;
                    default: throw storm::exceptions::ExpressionEvaluationException() << "Cannot evaluate expression: "
                        << "Unknown boolean binary relation: '" << relationType << "'.";
                }
            }
            
            BinaryRelationExpression::RelationType BinaryRelationExpression::getRelationType() const {
                return relationType;
            }
            
            void BinaryRelationExpression::accept(ExpressionVisitor* visitor) {
                visitor->visit(this);
            }
            
            std::string BinaryRelationExpression::toString() const {
                std::stringstream result;
                result << this->getLeft()->toString();
                switch (relationType) {
                    case EQUAL: result << " = "; break;
                    case NOT_EQUAL: result << " != "; break;
                    case LESS: result << " < "; break;
                    case LESS_OR_EQUAL: result << " <= "; break;
                    case GREATER: result << " > "; break;
                    case GREATER_OR_EQUAL: result << " >= "; break;
                }
                result << this->getRight()->toString();
                
                return result.str();
            }
            
        } // namespace expressions
    } // namespace ir
} // namespace storm