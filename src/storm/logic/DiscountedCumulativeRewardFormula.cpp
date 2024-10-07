#include "storm/logic/DiscountedCumulativeRewardFormula.h"
#include <boost/any.hpp>
#include <ostream>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
namespace logic {
DiscountedCumulativeRewardFormula::DiscountedCumulativeRewardFormula(storm::expressions::Expression const discountFactor, TimeBound const& bound,
                                                                     TimeBoundReference const& timeBoundReference,
                                                                     boost::optional<RewardAccumulation> rewardAccumulation)
    : discountFactor(discountFactor), timeBoundReferences({timeBoundReference}), bounds({bound}), rewardAccumulation(rewardAccumulation) {
    // Intentionally left empty.
}

DiscountedCumulativeRewardFormula::DiscountedCumulativeRewardFormula(storm::expressions::Expression const discountFactor, std::vector<TimeBound> const& bounds,
                                                                     std::vector<TimeBoundReference> const& timeBoundReferences,
                                                                     boost::optional<RewardAccumulation> rewardAccumulation)
    : discountFactor(discountFactor), timeBoundReferences(timeBoundReferences), bounds(bounds), rewardAccumulation(rewardAccumulation) {
    STORM_LOG_ASSERT(this->timeBoundReferences.size() == this->bounds.size(), "Number of time bounds and number of time bound references does not match.");
    STORM_LOG_ASSERT(!this->timeBoundReferences.empty(), "No time bounds given.");
}

bool DiscountedCumulativeRewardFormula::isDiscountedCumulativeRewardFormula() const {
    return true;
}

bool DiscountedCumulativeRewardFormula::isCumulativeRewardFormula() const {
    return true;
}

bool DiscountedCumulativeRewardFormula::isRewardPathFormula() const {
    return true;
}

bool DiscountedCumulativeRewardFormula::isMultiDimensional() const {
    return getDimension() > 1;
}

unsigned DiscountedCumulativeRewardFormula::getDimension() const {
    return timeBoundReferences.size();
}

boost::any DiscountedCumulativeRewardFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
    return visitor.visit(*this, data);
}

void DiscountedCumulativeRewardFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
    for (unsigned i = 0; i < this->getDimension(); ++i) {
        if (getTimeBoundReference(i).isRewardBound()) {
            referencedRewardModels.insert(this->getTimeBoundReference(i).getRewardName());
        }
    }
}

void DiscountedCumulativeRewardFormula::gatherUsedVariables(std::set<storm::expressions::Variable>& usedVariables) const {
    for (unsigned i = 0; i < this->getDimension(); ++i) {
        this->getBound(i).gatherVariables(usedVariables);
    }
}

TimeBoundReference const& DiscountedCumulativeRewardFormula::getTimeBoundReference() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    return getTimeBoundReference(0);
}

TimeBoundReference const& DiscountedCumulativeRewardFormula::getTimeBoundReference(unsigned i) const {
    return timeBoundReferences.at(i);
}

bool DiscountedCumulativeRewardFormula::isBoundStrict() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    return isBoundStrict(0);
}

bool DiscountedCumulativeRewardFormula::isBoundStrict(unsigned i) const {
    return bounds.at(i).isStrict();
}

bool DiscountedCumulativeRewardFormula::hasIntegerBound() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    return hasIntegerBound(0);
}

bool DiscountedCumulativeRewardFormula::hasIntegerBound(unsigned i) const {
    return bounds.at(i).getBound().hasIntegerType();
}

storm::expressions::Expression const& DiscountedCumulativeRewardFormula::getBound() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    return getBound(0);
}

storm::expressions::Expression const& DiscountedCumulativeRewardFormula::getBound(unsigned i) const {
    return bounds.at(i).getBound();
}

template<typename ValueType>
ValueType DiscountedCumulativeRewardFormula::getBound() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    return getBound<ValueType>(0);
}

template<>
double DiscountedCumulativeRewardFormula::getBound(unsigned i) const {
    checkNoVariablesInBound(bounds.at(i).getBound());
    double value = bounds.at(i).getBound().evaluateAsDouble();
    return value;
}

template<>
storm::RationalNumber DiscountedCumulativeRewardFormula::getBound(unsigned i) const {
    checkNoVariablesInBound(bounds.at(i).getBound());
    storm::RationalNumber value = bounds.at(i).getBound().evaluateAsRational();
    return value;
}

template<>
uint64_t DiscountedCumulativeRewardFormula::getBound(unsigned i) const {
    checkNoVariablesInBound(bounds.at(i).getBound());
    uint64_t value = bounds.at(i).getBound().evaluateAsInt();
    return value;
}

template<>
double DiscountedCumulativeRewardFormula::getNonStrictBound() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    double bound = getBound<double>();
    STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
    return bound;
}

template<>
uint64_t DiscountedCumulativeRewardFormula::getNonStrictBound() const {
    STORM_LOG_ASSERT(!isMultiDimensional(), "Cumulative Reward Formula is multi-dimensional.");
    int_fast64_t bound = getBound<uint64_t>();
    if (isBoundStrict()) {
        STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
        return bound - 1;
    } else {
        return bound;
    }
}

std::vector<TimeBound> const& DiscountedCumulativeRewardFormula::getBounds() const {
    return bounds;
}

void DiscountedCumulativeRewardFormula::checkNoVariablesInBound(storm::expressions::Expression const& bound) {
    STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::InvalidOperationException,
                    "Cannot evaluate time-bound '" << bound << "' as it contains undefined constants.");
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

bool DiscountedCumulativeRewardFormula::hasRewardAccumulation() const {
    return rewardAccumulation.is_initialized();
}

RewardAccumulation const& DiscountedCumulativeRewardFormula::getRewardAccumulation() const {
    return rewardAccumulation.get();
}

std::shared_ptr<DiscountedCumulativeRewardFormula const> DiscountedCumulativeRewardFormula::stripRewardAccumulation() const {
    return std::make_shared<DiscountedCumulativeRewardFormula const>(discountFactor, bounds, timeBoundReferences);
}

std::shared_ptr<DiscountedCumulativeRewardFormula const> DiscountedCumulativeRewardFormula::restrictToDimension(unsigned i) const {
    return std::make_shared<DiscountedCumulativeRewardFormula const>(discountFactor, bounds.at(i), getTimeBoundReference(i), rewardAccumulation);
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

template uint64_t DiscountedCumulativeRewardFormula::getBound<uint64_t>() const;
template double DiscountedCumulativeRewardFormula::getBound<double>() const;
template storm::RationalNumber DiscountedCumulativeRewardFormula::getBound<storm::RationalNumber>() const;

}  // namespace logic
}  // namespace storm
