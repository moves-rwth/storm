#include "src/logic/LongRunAverageRewardFormula.h"

#include "src/logic/FormulaVisitor.h"

namespace storm {
    namespace logic {
        LongRunAverageRewardFormula::LongRunAverageRewardFormula() {
            // Intentionally left empty.
        }
        
        bool LongRunAverageRewardFormula::isLongRunAverageRewardFormula() const {
            return true;
        }
        
        bool LongRunAverageRewardFormula::isRewardPathFormula() const {
            return true;
        }
        
        boost::any LongRunAverageRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        std::shared_ptr<Formula> LongRunAverageRewardFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::shared_ptr<Formula>(new LongRunAverageRewardFormula());
        }
        
        std::ostream& LongRunAverageRewardFormula::writeToStream(std::ostream& out) const {
            return out << "LRA";
        }
    }
}