#include "storm/logic/VariableSubstitutionVisitor.h"

#include "storm/logic/Formulas.h"

namespace storm {
    namespace logic {
        
        VariableSubstitutionVisitor::VariableSubstitutionVisitor(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) : substitution(substitution) {
            // Intentionally left empty.
        }
        
        std::shared_ptr<Formula> VariableSubstitutionVisitor::substitute(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }
        
        boost::any VariableSubstitutionVisitor::visit(AtomicExpressionFormula const& f, boost::any const& data) const {
            return std::static_pointer_cast<Formula>(std::make_shared<AtomicExpressionFormula>(f.getExpression().substitute(substitution)));
        }
    }
}
