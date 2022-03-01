#ifndef STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_
#define STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_

#include "storm/logic/FormulaVisitor.h"
#include "storm/logic/OperatorFormula.h"
#include "storm/logic/RewardMeasureType.h"

namespace storm {
namespace logic {
class TimeOperatorFormula : public OperatorFormula {
   public:
    TimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation = OperatorInformation(),
                        RewardMeasureType rewardMeasureType = RewardMeasureType::Expectation);

    virtual ~TimeOperatorFormula() {
        // Intentionally left empty.
    }

    virtual bool isTimeOperatorFormula() const override;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

    /*!
     * Retrieves the measure type of the operator.
     *
     * @return The measure type of the operator.
     */
    RewardMeasureType getMeasureType() const;

   private:
    // The measure type of the operator.
    RewardMeasureType rewardMeasureType;
};
}  // namespace logic
}  // namespace storm

#endif /* STORM_LOGIC_EXPECTEDTIMEOPERATORFORMULA_H_ */
