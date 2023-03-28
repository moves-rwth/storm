#include "storm/logic/TimeOperatorFormula.h"
#include <boost/any.hpp>

#include <ostream>
#include "storm/logic/EventuallyFormula.h"
#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace logic {
TimeOperatorFormula::TimeOperatorFormula(std::shared_ptr<Formula const> const& subformula, OperatorInformation const& operatorInformation,
                                         RewardMeasureType rewardMeasureType)
    : OperatorFormula(subformula, operatorInformation), rewardMeasureType(rewardMeasureType) {
    assert(subformula->isTimePathFormula());
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

std::ostream& TimeOperatorFormula::writeToStream(std::ostream& out, bool /* allowParentheses */) const {
    // No parentheses necessary
    out << "T";
    out << "[" << rewardMeasureType << "]";
    OperatorFormula::writeToStream(out);
    return out;
}
}  // namespace logic
}  // namespace storm
