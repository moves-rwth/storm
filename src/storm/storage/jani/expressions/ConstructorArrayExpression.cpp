#include "storm/storage/jani/expressions/ConstructorArrayExpression.h"

#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"
#include "storm/storage/jani/visitor/JaniExpressionSubstitutionVisitor.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace expressions {
        
        ConstructorArrayExpression::ConstructorArrayExpression(ExpressionManager const& manager, Type const& type, std::vector<std::shared_ptr<BaseExpression const>> const& size, std::vector<std::shared_ptr<storm::expressions::Variable>> indexVars, std::shared_ptr<BaseExpression const> const& elementExpression) : ArrayExpression(manager, type), sizeExpressions(size), indexVars(indexVars), elementExpression(elementExpression) {
            STORM_LOG_ASSERT(size.size() == indexVars.size(), "Expecting the size of the vector of sizes (" << size.size() << ") and the size of the vector of indexVars (" << indexVars.size() << ") to be equal");
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
            for (auto const& indexVar : indexVars) {
                bool indexVarContained = variables.find(*indexVar) != variables.end();
                for (auto &expr : sizeExpressions) {
                    expr->gatherVariables(variables);
                }
                elementExpression->gatherVariables(variables);
                if (!indexVarContained) {
                    variables.erase(*indexVar);
                }
            }
        }
        
        bool ConstructorArrayExpression::containsVariables() const {
            bool containsVariables = false;
            for (auto const& indexVar : indexVars) {

                for (auto &expr : sizeExpressions) {
                    if (expr->containsVariables()) {
                        return true;
                    }
                }

                // The index variable should not count
                std::set<storm::expressions::Variable> variables;
                elementExpression->gatherVariables(variables);
                variables.erase(*indexVar);
                containsVariables |= !variables.empty();
                if (containsVariables) {
                    break;
                }
            }
            return containsVariables;
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::simplify() const {
            std::vector<std::shared_ptr<BaseExpression const>> simplifiedSizeExpressions;
            simplifiedSizeExpressions.reserve(sizeExpressions.size());
            for (auto & expr : sizeExpressions) {
                simplifiedSizeExpressions.push_back(expr->simplify());
            }
            return std::shared_ptr<BaseExpression const>(new ConstructorArrayExpression(getManager(), getType(), simplifiedSizeExpressions, indexVars, elementExpression->simplify()));
        }
        
        boost::any ConstructorArrayExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
            auto janiVisitor = dynamic_cast<JaniExpressionVisitor*>(&visitor);
            STORM_LOG_THROW(janiVisitor != nullptr, storm::exceptions::UnexpectedException, "Visitor of jani expression should be of type JaniVisitor.");
            return janiVisitor->visit(*this, data);
        }
        
        void ConstructorArrayExpression::printToStream(std::ostream& stream) const {
            stream << "array[elementValue: " << *elementExpression << " | index: ";
            for (auto i = 0; i < indexVars.size(); ++i) {
                assert (i < sizeExpressions.size());
                stream << indexVars.at(i)->getExpression() << " < bound: " << *sizeExpressions.at(i);
                if (i < (indexVars.size() - 1)) {
                    stream << ",";
                }
            }
            stream << " ]";
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::size() const {
            return sizeExpression;
        }

        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::size(uint_fast64_t index) const {
            return sizeExpressions.at(index);
        }

        std::vector<std::shared_ptr<const BaseExpression>> const& ConstructorArrayExpression::getSizes() const {
            return sizeExpressions;
        }
        
        std::shared_ptr<const BaseExpression> ConstructorArrayExpression::at(std::vector<uint64_t> &indices) const {
            assert (indices.size() == indexVars.size());
            std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
            for (auto i = 0; i < indices.size(); ++i) {
                substitution.emplace(*indexVars.at(i), this->getManager().integer(indices.at(i)));
            }
            return storm::jani::substituteJaniExpression(elementExpression->toExpression(), substitution).getBaseExpressionPointer();
        }

        std::shared_ptr<const BaseExpression> ConstructorArrayExpression::at(uint64_t i) const {
            std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
            std::vector<uint64_t> newI;
            STORM_LOG_THROW(sizeExpressions.size() > 2, storm::exceptions::NotImplementedException, "Getting entry at " << i << " in Constructor Array for more than two nested arrays is not yet implemented");
            if (sizeExpressions.size() == 2) {
                auto size = sizeExpressions.at(1).get()->evaluateAsInt();
                auto indexSecond = i % size;
                auto indexFirst = (i-indexSecond) / size;
                newI.push_back(indexFirst);
                newI.push_back(indexSecond);
            } else {
                newI.push_back(i);
            }
            return at(newI);
        }

        size_t ConstructorArrayExpression::getNumberOfArrays() const {
            return sizeExpressions.size();
        }

        std::shared_ptr<BaseExpression const> const& ConstructorArrayExpression::getElementExpression() const {
            return elementExpression;
        }

        std::shared_ptr<storm::expressions::Variable> ConstructorArrayExpression::getIndexVar(uint64_t i) const {
            return indexVars.at(i);
        }

        std::vector<std::shared_ptr<storm::expressions::Variable>> ConstructorArrayExpression::getIndexVars() const {
            return indexVars;
        }

        size_t ConstructorArrayExpression::getSizeIndexVar(uint64_t i) const {
            return sizeExpressions.at(i)->evaluateAsInt();
        }
        
    }
}