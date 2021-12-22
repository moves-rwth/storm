#pragma once

#include "storm/modelchecker/results/CheckResult.h"

namespace storm {
namespace modelchecker {
template<typename ValueType>
class QuantitativeCheckResult : public CheckResult {
   public:
    virtual ~QuantitativeCheckResult() = default;

    virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const;

    virtual void oneMinus() = 0;

    virtual ValueType getMin() const = 0;
    virtual ValueType getMax() const = 0;

    virtual ValueType average() const = 0;
    virtual ValueType sum() const = 0;

    virtual bool isQuantitative() const override;
};
}  // namespace modelchecker
}  // namespace storm
