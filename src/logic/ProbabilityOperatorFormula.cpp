#include "src/logic/ProbabilityOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        ProbabilityOperatorFormula::ProbabilityOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation) : OperatorFormula(subformula, operatorInformation) {
            // Intentionally left empty.
        }
        
        bool ProbabilityOperatorFormula::isProbabilityOperatorFormula() const {
            return true;
        }
        
        boost::any ProbabilityOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
                
        std::ostream& ProbabilityOperatorFormula::writeToStream(std::ostream& out) const {
            out << "P";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}