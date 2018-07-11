#include "storm/storage/jani/Assignment.h"

#include "storm/storage/expressions/LinearityCheckVisitor.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm  {
    namespace jani {
        
        Assignment::Assignment(storm::jani::Variable const& variable, storm::expressions::Expression const& expression, uint64_t level) : variable(variable), expression(expression), level(level) {

        }
        
        bool Assignment::operator==(Assignment const& other) const {
            return this->isTransient() == other.isTransient() && this->getExpressionVariable() == other.getExpressionVariable() && this->getAssignedExpression().isSyntacticallyEqual(other.getAssignedExpression()) && this->getLevel() == other.getLevel();
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
        
        bool Assignment::isTransient() const {
            return this->variable.get().isTransient();
        }
        
        void Assignment::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            this->setAssignedExpression(this->getAssignedExpression().substitute(substitution).simplify());
        }
        
        int64_t Assignment::getLevel() const {
            return level;
        }

        void Assignment::setLevel(int64_t level)  {
            this->level = level;
        }

        bool Assignment::isLinear() const {
            storm::expressions::LinearityCheckVisitor linearityChecker;
            return linearityChecker.check(this->getAssignedExpression(), true);
        }
        
        std::ostream& operator<<(std::ostream& stream, Assignment const& assignment) {
            stream << assignment.getVariable().getName() << " := " << assignment.getAssignedExpression();
            return stream;
        }
        
        bool AssignmentPartialOrderByLevelAndVariable::operator()(Assignment const& left, Assignment const& right) const {
            return left.getLevel() < right.getLevel() || (left.getLevel() == right.getLevel() && left.getExpressionVariable() < right.getExpressionVariable());
        }

        bool AssignmentPartialOrderByLevelAndVariable::operator()(Assignment const& left, std::shared_ptr<Assignment> const& right) const {
            return left.getLevel() < right->getLevel() || (left.getLevel() == right->getLevel() && left.getExpressionVariable() < right->getExpressionVariable());
        }
        
        bool AssignmentPartialOrderByLevelAndVariable::operator()(std::shared_ptr<Assignment> const& left, std::shared_ptr<Assignment> const& right) const {
            return left->getLevel() < right->getLevel() || (left->getLevel() == right->getLevel() && left->getExpressionVariable() < right->getExpressionVariable());
        }
        
        bool AssignmentPartialOrderByLevelAndVariable::operator()(std::shared_ptr<Assignment> const& left, Assignment const& right) const {
            return left->getLevel() < right.getLevel() || (left->getLevel() == right.getLevel() && left->getExpressionVariable() < right.getExpressionVariable());
        }
    }
}
