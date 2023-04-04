#ifndef STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <map>
#include <optional>
#include <vector>

#include "storm/modelchecker/results/QuantitativeCheckResult.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/sparse/StateType.h"
#include "storm/storage/sparse/StateValuations.h"

#include "storm/adapters/JsonAdapter.h"
#include "storm/utility/OsDetection.h"

namespace storm {
namespace modelchecker {
// fwd
class ExplicitQualitativeCheckResult;

template<typename ValueType>
class ExplicitQuantitativeCheckResult : public QuantitativeCheckResult<ValueType> {
   public:
    typedef std::vector<ValueType> vector_type;
    typedef std::map<storm::storage::sparse::state_type, ValueType> map_type;

    ExplicitQuantitativeCheckResult();
    ExplicitQuantitativeCheckResult(map_type const& values);
    ExplicitQuantitativeCheckResult(map_type&& values);
    ExplicitQuantitativeCheckResult(storm::storage::sparse::state_type const& state, ValueType const& value);
    ExplicitQuantitativeCheckResult(vector_type const& values);
    ExplicitQuantitativeCheckResult(vector_type&& values);
    ExplicitQuantitativeCheckResult(boost::variant<vector_type, map_type> const& values,
                                    boost::optional<std::shared_ptr<storm::storage::Scheduler<ValueType>>> scheduler = boost::none);
    ExplicitQuantitativeCheckResult(boost::variant<vector_type, map_type>&& values,
                                    boost::optional<std::shared_ptr<storm::storage::Scheduler<ValueType>>> scheduler = boost::none);

    ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult const& other) = default;
    ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult const& other) = default;
#ifndef WINDOWS
    ExplicitQuantitativeCheckResult(ExplicitQuantitativeCheckResult&& other) = default;
    ExplicitQuantitativeCheckResult& operator=(ExplicitQuantitativeCheckResult&& other) = default;
#endif
    explicit ExplicitQuantitativeCheckResult(ExplicitQualitativeCheckResult const& other);

    virtual ~ExplicitQuantitativeCheckResult() = default;

    virtual std::unique_ptr<CheckResult> clone() const override;

    ValueType& operator[](storm::storage::sparse::state_type state);
    ValueType const& operator[](storm::storage::sparse::state_type state) const;

    virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, ValueType const& bound) const override;

    virtual bool isExplicit() const override;
    virtual bool isResultForAllStates() const override;

    virtual bool isExplicitQuantitativeCheckResult() const override;

    vector_type const& getValueVector() const;
    vector_type& getValueVector();
    map_type const& getValueMap() const;

    virtual std::ostream& writeToStream(std::ostream& out) const override;

    virtual void filter(QualitativeCheckResult const& filter) override;

    virtual void oneMinus() override;

    virtual ValueType getMin() const override;
    virtual ValueType getMax() const override;
    virtual std::pair<ValueType, ValueType> getMinMax() const;
    virtual ValueType average() const override;
    virtual ValueType sum() const override;

    virtual bool hasScheduler() const override;
    void setScheduler(std::unique_ptr<storm::storage::Scheduler<ValueType>>&& scheduler);
    storm::storage::Scheduler<ValueType> const& getScheduler() const;
    storm::storage::Scheduler<ValueType>& getScheduler();

    storm::json<ValueType> toJson(std::optional<storm::storage::sparse::StateValuations> const& stateValuations = std::nullopt,
                                  std::optional<storm::models::sparse::StateLabeling> const& stateLabels = std::nullopt) const;

   private:
    // The values of the quantitative check result.
    boost::variant<vector_type, map_type> values;

    // An optional scheduler that accompanies the values.
    boost::optional<std::shared_ptr<storm::storage::Scheduler<ValueType>>> scheduler;
};
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_EXPLICITQUANTITATIVECHECKRESULT_H_ */
