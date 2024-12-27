#include "storm/logic/DiscountedTotalRewardFormula.h"
#include <boost/any.hpp>

#include <ostream>

#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/logic/FormulaVisitor.h"

#include "storm/utility/macros.h"

namespace storm {
namespace logic {
DiscountedTotalRewardFormula::DiscountedTotalRewardFormula(storm::expressions::Expression const discountFactor,
                                                           boost::optional<RewardAccumulation> rewardAccumulation)
    : TotalRewardFormula(rewardAccumulation), discountFactor(discountFactor) {
    // Intentionally left empty.
}

bool DiscountedTotalRewardFormula::isDiscountedTotalRewardFormula() const {
    return true;
}

std::ostream& DiscountedTotalRewardFormula::writeToStream(std::ostream& out, bool /* allowParentheses */) const {
    // No parentheses necessary
    out << "Cdiscount=";
    out << discountFactor.toString();
    if (hasRewardAccumulation()) {
        out << "[" << getRewardAccumulation() << "]";
    }
    return out;
}

storm::expressions::Expression const& DiscountedTotalRewardFormula::getDiscountFactor() const {
    return discountFactor;
}

template<>
double DiscountedTotalRewardFormula::getDiscountFactor() const {
    checkNoVariablesInDiscountFactor(discountFactor);
    double value = discountFactor.evaluateAsDouble();
    return value;
}

template<>
storm::RationalNumber DiscountedTotalRewardFormula::getDiscountFactor() const {
    checkNoVariablesInDiscountFactor(discountFactor);
    storm::RationalNumber value = discountFactor.evaluateAsRational();
    return value;
}

void DiscountedTotalRewardFormula::checkNoVariablesInDiscountFactor(storm::expressions::Expression const& factor) {
    STORM_LOG_THROW(!factor.containsVariables(), storm::exceptions::InvalidOperationException,
                    "Cannot evaluate discount factor '" << factor << "' as it contains undefined constants.");
}

boost::any DiscountedTotalRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
    return visitor.visit(*this, data);
}

}  // namespace logic
}  // namespace storm
