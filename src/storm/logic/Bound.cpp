#include "storm/logic/Bound.h"
#include "storm/adapters/RationalNumberAdapter.h"

namespace storm::logic {

template<typename ValueType>
bool Bound::isSatisfied(ValueType const& compareValue) const {
    ValueType thresholdAsValueType = storm::utility::convertNumber<ValueType>(threshold.evaluateAsRational());
    switch (comparisonType) {
        case ComparisonType::Greater:
            return compareValue > thresholdAsValueType;
        case ComparisonType::GreaterEqual:
            return compareValue >= thresholdAsValueType;
        case ComparisonType::Less:
            return compareValue < thresholdAsValueType;
        case ComparisonType::LessEqual:
            return compareValue <= thresholdAsValueType;
    }
    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Unknown ComparisonType");
}

template bool Bound::isSatisfied(double const& compareValue) const;
template bool Bound::isSatisfied(storm::RationalNumber const& compareValue) const;
}  // namespace storm::logic