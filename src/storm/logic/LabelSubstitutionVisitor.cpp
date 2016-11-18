#include "storm/logic/LabelSubstitutionVisitor.h"

#include "storm/logic/Formulas.h"

namespace storm {
    namespace logic {
        
        LabelSubstitutionVisitor::LabelSubstitutionVisitor(std::map<std::string, storm::expressions::Expression> const& labelToExpressionMapping) : labelToExpressionMapping(labelToExpressionMapping) {
            // Intentionally left empty.
        }

        std::shared_ptr<Formula> LabelSubstitutionVisitor::substitute(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<std::shared_ptr<Formula>>(result);
        }
        
        boost::any LabelSubstitutionVisitor::visit(AtomicLabelFormula const& f, boost::any const& data) const {
            auto it = labelToExpressionMapping.find(f.getLabel());
            if (it != labelToExpressionMapping.end()) {
                return std::static_pointer_cast<Formula>(std::make_shared<AtomicExpressionFormula>(it->second));
            } else {
                return std::static_pointer_cast<Formula>(std::make_shared<AtomicLabelFormula>(f));
            }
        }        
    }
}
