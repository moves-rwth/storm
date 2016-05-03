#include "src/logic/LongRunAverageOperatorFormula.h"

#include "src/logic/FormulaVisitor.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        LongRunAverageOperatorFormula::LongRunAverageOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation) : OperatorFormula(subformula, operatorInformation) {
            // Intentionally left empty.
        }
        
        bool LongRunAverageOperatorFormula::isLongRunAverageOperatorFormula() const {
            return true;
        }
        
        boost::any LongRunAverageOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
                
        std::ostream& LongRunAverageOperatorFormula::writeToStream(std::ostream& out) const {
            out << "LRA";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}