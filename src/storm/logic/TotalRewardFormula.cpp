#include "src/storm/logic/TotalRewardFormula.h"

#include "src/storm/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        TotalRewardFormula::TotalRewardFormula() {
            // Intentionally left empty.
        }
        
        bool TotalRewardFormula::isTotalRewardFormula() const {
            return true;
        }
        
        bool TotalRewardFormula::isRewardPathFormula() const {
            return true;
        }
        
        boost::any TotalRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }

        std::ostream& TotalRewardFormula::writeToStream(std::ostream& out) const {
            out << "C";
            return out;
        }
    }
}
