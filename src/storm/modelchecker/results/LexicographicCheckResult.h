#pragma once

#include <vector>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/storage/sparse/StateType.h"

namespace storm {
namespace modelchecker {
template<typename ValueType>
class LexicographicCheckResult : public CheckResult {
   public:
    LexicographicCheckResult() = default;
    LexicographicCheckResult(std::vector<ValueType> const& values, storm::storage::sparse::state_type state);
    virtual ~LexicographicCheckResult() = default;

    std::vector<ValueType> const& getInitialStateValue() const;
    storm::storage::sparse::state_type const& getState() const;

    virtual bool isLexicographicCheckResult() const override;
    virtual std::unique_ptr<CheckResult> clone() const override;
    virtual bool isExplicit() const override;
    virtual void filter(QualitativeCheckResult const& filter) override;

    virtual std::ostream& writeToStream(std::ostream& out) const override;

   private:
    std::vector<ValueType> values;
    // The state of the checked model to which the result applies
    storm::storage::sparse::state_type state;
};
}  // namespace modelchecker
}  // namespace storm
