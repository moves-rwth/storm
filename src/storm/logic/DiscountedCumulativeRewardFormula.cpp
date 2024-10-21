#include "storm/logic/DiscountedCumulativeRewardFormula.h"
#include <boost/any.hpp>
#include <ostream>
#include <utility>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
namespace logic {
DiscountedCumulativeRewardFormula::DiscountedCumulativeRewardFormula(storm::expressions::Expression const& discountFactor, TimeBound const& bound,
                                                                     TimeBoundReference const& timeBoundReference,
                                                                     boost::optional<RewardAccumulation> rewardAccumulation)
    : CumulativeRewardFormula(bound, timeBoundReference, std::move(rewardAccumulation)), discountFactor(discountFactor) {
    // Intentionally left empty.
}

DiscountedCumulativeRewardFormula::DiscountedCumulativeRewardFormula(storm::expressions::Expression const& discountFactor, std::vector<TimeBound> const& bounds,
                                                                     std::vector<TimeBoundReference> const& timeBoundReferences,
                                                                     boost::optional<RewardAccumulation> rewardAccumulation)
    : CumulativeRewardFormula(bounds, timeBoundReferences, std::move(rewardAccumulation)), discountFactor(discountFactor) {
    // Intentionally left empty.
}

bool DiscountedCumulativeRewardFormula::isDiscountedCumulativeRewardFormula() const {
    return true;
}

void DiscountedCumulativeRewardFormula::checkNoVariablesInDiscountFactor(storm::expressions::Expression const& factor) {
    STORM_LOG_THROW(!factor.containsVariables(), storm::exceptions::InvalidOperationException,
                    "Cannot evaluate discount factor '" << factor << "' as it contains undefined constants.");
}

storm::expressions::Expression const& DiscountedCumulativeRewardFormula::getDiscountFactor() const {
    return discountFactor;
}

template<>
double DiscountedCumulativeRewardFormula::getDiscountFactor() const {
    checkNoVariablesInDiscountFactor(discountFactor);
    double value = discountFactor.evaluateAsDouble();
    return value;
}

template<>
storm::RationalNumber DiscountedCumulativeRewardFormula::getDiscountFactor() const {
    checkNoVariablesInDiscountFactor(discountFactor);
    storm::RationalNumber value = discountFactor.evaluateAsRational();
    return value;
}


std::ostream& DiscountedCumulativeRewardFormula::writeToStream(std::ostream& out, bool /*allowParentheses*/) const {
    // No parentheses necessary
    out << "Cdiscount=";
    out << discountFactor;
    if (hasRewardAccumulation()) {
        out << "[" << getRewardAccumulation() << "]";
    }
    if (this->isMultiDimensional()) {
        out << "^{";
    }
    for (unsigned i = 0; i < this->getDimension(); ++i) {
        if (i > 0) {
            out << ", ";
        }
        if (this->getTimeBoundReference(i).isRewardBound()) {
            out << "rew";
            if (this->getTimeBoundReference(i).hasRewardAccumulation()) {
                out << "[" << this->getTimeBoundReference(i).getRewardAccumulation() << "]";
            }
            out << "{\"" << this->getTimeBoundReference(i).getRewardName() << "\"}";
        } else if (this->getTimeBoundReference(i).isStepBound()) {
            out << "steps";
            //} else if (this->getTimeBoundReference(i).isStepBound())
            //  Note: the 'time' keyword is optional.
            //    out << "time";
        }
        if (this->isBoundStrict(i)) {
            out << "<";
        } else {
            out << "<=";
        }
        out << this->getBound(i);
    }
    if (this->isMultiDimensional()) {
        out << "}";
    }
    return out;
}

boost::any DiscountedCumulativeRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
    return visitor.visit(*this, data);
}

}  // namespace logic
}  // namespace storm
