#include "src/logic/LongRunAverageRewardFormula.h"

namespace storm {
    namespace logic {
        LongRunAverageRewardFormula::LongRunAverageRewardFormula() {
            // Intentionally left empty.
        }
        
        bool LongRunAverageRewardFormula::isLongRunAverageRewardFormula() const {
            return true;
        }
        
        bool LongRunAverageRewardFormula::isValidRewardPathFormula() const {
            return true;
        }
        
        std::shared_ptr<Formula> LongRunAverageRewardFormula::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) const {
            return std::shared_ptr<Formula>(new LongRunAverageRewardFormula());
        }
        
        std::ostream& LongRunAverageRewardFormula::writeToStream(std::ostream& out) const {
            return out << "LRA";
        }
    }
}