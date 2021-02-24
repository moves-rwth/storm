#include "storm/storage/jani/expressions/ConstructorArrayExpression.h"

#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"
#include "storm/storage/jani/visitor/JaniExpressionSubstitutionVisitor.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace expressions {
        
        ConstructorArrayExpression::ConstructorArrayExpression(ExpressionManager const& manager, Type const& type, std::vector<std::shared_ptr<BaseExpression const>> const& size, std::shared_ptr<storm::expressions::Variable> indexVar, std::shared_ptr<BaseExpression const> const& elementExpression) : ArrayExpression(manager, type), sizeExpressions(size), indexVar(indexVar), elementExpression(elementExpression) {
            int64_t exprSize = 0;
            for (auto & expr : sizeExpressions) {
                if (exprSize == 0) {
                    exprSize = expr->evaluateAsInt();
                } else {
                    exprSize = exprSize * expr->evaluateAsInt();
                }
            }
            sizeExpression = manager.integer(exprSize).getBaseExpressionPointer();
        }

        void ConstructorArrayExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
            // The indexVar should not be gathered (unless it is already contained).
            bool indexVarContained = variables.find(*indexVar) != variables.end();
            for (auto & expr : sizeExpressions) {
                expr->gatherVariables(variables);
            }
            elementExpression->gatherVariables(variables);
            if (!indexVarContained) {
                variables.erase(*indexVar);
            }
        }
        
        bool ConstructorArrayExpression::containsVariables() const {
            for (auto & expr : sizeExpressions) {
                if (expr->containsVariables()) {
                    return true;
                }
            }

            // The index variable should not count
            std::set<storm::expressions::Variable> variables;
            elementExpression->gatherVariables(variables);
            variables.erase(*indexVar);
            return !variables.empty();
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::simplify() const {
            std::vector<std::shared_ptr<BaseExpression const>> simplifiedSizeExpressions;
            simplifiedSizeExpressions.reserve(sizeExpressions.size());
            for (auto & expr : sizeExpressions) {
                simplifiedSizeExpressions.push_back(expr->simplify());
            }
            return std::shared_ptr<BaseExpression const>(new ConstructorArrayExpression(getManager(), getType(), simplifiedSizeExpressions, indexVar, elementExpression->simplify()));
        }
        
        boost::any ConstructorArrayExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
            auto janiVisitor = dynamic_cast<JaniExpressionVisitor*>(&visitor);
            STORM_LOG_THROW(janiVisitor != nullptr, storm::exceptions::UnexpectedException, "Visitor of jani expression should be of type JaniVisitor.");
            return janiVisitor->visit(*this, data);
        }
        
        void ConstructorArrayExpression::printToStream(std::ostream& stream) const {
            stream << "array[elementValue: " << *elementExpression << " | index: " << indexVar->getExpression() << " < bound: " << *sizeExpression << " ]";
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::size() const {
            return sizeExpression;
        }

        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::size(uint_fast64_t index) const {
            return sizeExpressions.at(index);
        }
        
        std::shared_ptr<const BaseExpression> ConstructorArrayExpression::at(std::vector<uint64_t> &i) const {
            assert (false);
//            std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
//            substitution.emplace(indexVar, this->getManager().integer(i));
//
//            return storm::jani::substituteJaniExpression(elementExpression->toExpression(), substitution).getBaseExpressionPointer();
        }

        std::shared_ptr<const BaseExpression> ConstructorArrayExpression::at(uint64_t i) const {
            std::cout << indexVar->getName() << std::endl;

            assert (false);
            std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
            substitution.emplace(*indexVar, this->getManager().integer(i));
            return storm::jani::substituteJaniExpression(elementExpression->toExpression(), substitution).getBaseExpressionPointer();
        }

        size_t ConstructorArrayExpression::getNumberOfArrays() const {
            return sizeExpressions.size();
        }

        std::shared_ptr<BaseExpression const> const& ConstructorArrayExpression::getElementExpression() const {
            return elementExpression;
        }

        std::shared_ptr<storm::expressions::Variable> ConstructorArrayExpression::getIndexVar() const {
            return indexVar;
        }
        
    }
}