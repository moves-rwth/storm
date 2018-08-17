#include "storm/storage/jani/expressions/ConstructorArrayExpression.h"

#include "storm/storage/jani/expressions/JaniExpressionVisitor.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace expressions {
        
        ConstructorArrayExpression::ConstructorArrayExpression(ExpressionManager const& manager, Type const& type, std::shared_ptr<BaseExpression const> const& size, storm::expressions::Variable indexVar, std::shared_ptr<BaseExpression const> const& elementExpression) : ArrayExpression(manager, type), size(size), indexVar(indexVar), elementExpression(elementExpression) {
            // Intentionally left empty
        }
        
        void ConstructorArrayExpression::gatherVariables(std::set<storm::expressions::Variable>& variables) const {
            // The indexVar should not be gathered (unless it is already contained).
            bool indexVarContained = variables.find(indexVar) != variables.end();
            size->gatherVariables(variables);
            elementExpression->gatherVariables(variables);
            if (!indexVarContained) {
                variables.erase(indexVar);
            }
        }
        
        bool ConstructorArrayExpression::containsVariables() const {
            if (size->containsVariables()) {
                return true;
            }
            // The index variable should not count
            std::set<storm::expressions::Variable> variables;
            elementExpression->gatherVariables(variables);
            variables.erase(indexVar);
            return !variables.empty();
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::simplify() const {
            return std::shared_ptr<BaseExpression const>(new ConstructorArrayExpression(manager, type, size->simplify(), indexVar, elementExpression->simplify()));
        }
        
        boost::any ConstructorArrayExpression::accept(ExpressionVisitor& visitor, boost::any const& data) const {
            auto janiVisitor = dynamic_cast<JaniExpressionVisitor*>(&visitor);
            STORM_LOG_THROW(janiVisitor != nullptr, storm::exceptions::UnexpectedException, "Visitor of jani expression should be of type JaniVisitor.");
            return janiVisitor->visit(*this, data);
        }
        
        void ConstructorArrayExpression::printToStream(std::ostream& stream) const {
            stream << "array[ ";
            elementExpression->printToStream(stream);
            stream << " | " << indexVar << "<";
            size->printToStream(stream);
            stream << " ]";
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::size() const {
            return size;
        }
        
        std::shared_ptr<BaseExpression const> ConstructorArrayExpression::at(uint64_t i) const {
            STORM_LOG_THROW(i < elements.size(), storm::exceptions::InvalidArgumentException, "Tried to access the element with index " << i << " of an array of size " << elements.size() << ".");
            return elements[i];
        }
        
        std::shared_ptr<BaseExpression const> const& ConstructorArrayExpression::getElementExpression() const {
            return elementExpression;
        }
        
        storm::expressions::Variable const& ConstructorArrayExpression::getIndexVar() const {
            return indexVar;
        }
        
    }
}