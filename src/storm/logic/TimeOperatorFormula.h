#pragma once

#include "storm/logic/FormulaVisitor.h"
#include "storm/logic/OperatorFormula.h"

namespace storm {
namespace logic {
class TimeOperatorFormula : public OperatorFormula {
   public:
    TimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation = OperatorInformation());

    virtual ~TimeOperatorFormula() {
        // Intentionally left empty.
    }

    virtual bool isTimeOperatorFormula() const override;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;
};
}  // namespace logic
}  // namespace storm
