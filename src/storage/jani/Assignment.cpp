#include "src/storage/jani/Assignment.h"

namespace storm  {
    namespace jani {
        
        Assignment::Assignment(storm::jani::Variable const& variable, storm::expressions::Expression const& expression) : variable(variable), expression(expression) {
            // Intentionally left empty.
        }
        
        bool Assignment::operator==(Assignment const& other) const {
            return this->isTransientAssignment() == other.isTransientAssignment() && this->getExpressionVariable() == other.getExpressionVariable() && this->getAssignedExpression().isSyntacticallyEqual(other.getAssignedExpression());
        }
        
        storm::jani::Variable const& Assignment::getVariable() const {
            return variable.get();
        }
        
        storm::expressions::Variable const& Assignment::getExpressionVariable() const {
            return variable.get().getExpressionVariable();
        }
        
        storm::expressions::Expression const& Assignment::getAssignedExpression() const {
            return expression;
        }
        
        void Assignment::setAssignedExpression(storm::expressions::Expression const& expression) {
            this->expression = expression;
        }
        
        bool Assignment::isTransientAssignment() const {
            return this->variable.get().isTransientVariable();
        }
        
        void Assignment::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            this->setAssignedExpression(this->getAssignedExpression().substitute(substitution));
        }
        
        std::ostream& operator<<(std::ostream& stream, Assignment const& assignment) {
            stream << assignment.getVariable().getName() << " := " << assignment.getAssignedExpression();
            return stream;
        }
        
    }
}