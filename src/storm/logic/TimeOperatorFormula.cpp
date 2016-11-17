#include "src/storm/logic/TimeOperatorFormula.h"

#include "src/storm/logic/FormulaVisitor.h"

#include "src/storm/utility/macros.h"
#include "src/storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace logic {
        TimeOperatorFormula::TimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation, RewardMeasureType rewardMeasureType) : OperatorFormula(subformula, operatorInformation), rewardMeasureType(rewardMeasureType) {
            // Intentionally left empty.
        }
        
        bool TimeOperatorFormula::isTimeOperatorFormula() const {
            return true;
        }
        
        boost::any TimeOperatorFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
                
        RewardMeasureType TimeOperatorFormula::getMeasureType() const {
            return rewardMeasureType;
        }
        
        std::ostream& TimeOperatorFormula::writeToStream(std::ostream& out) const {
            out << "T";
            out << "[" << rewardMeasureType << "]";
            OperatorFormula::writeToStream(out);
            return out;
        }
    }
}
