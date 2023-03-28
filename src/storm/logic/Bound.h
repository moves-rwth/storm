#ifndef STORM_LOGIC_BOUND_H_
#define STORM_LOGIC_BOUND_H_

#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/logic/ComparisonType.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
/// TODO this inclusion is actually unpleasant, but cannot be avoided right now.
#include "storm/adapters/RationalNumberAdapter.h"

namespace storm {
namespace logic {
struct Bound {
    Bound(ComparisonType comparisonType, storm::expressions::Expression const& threshold) : comparisonType(comparisonType), threshold(threshold) {
        // Intentionally left empty.
    }

    ComparisonType comparisonType;
    storm::expressions::Expression threshold;

    template<typename ValueType>
    bool isSatisfied(ValueType const& compareValue) const {
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

    friend std::ostream& operator<<(std::ostream& out, Bound const& bound);
};

inline std::ostream& operator<<(std::ostream& out, Bound const& bound) {
    out << bound.comparisonType << bound.threshold;
    return out;
}
}  // namespace logic

}  // namespace storm

#endif /* STORM_LOGIC_BOUND_H_ */
