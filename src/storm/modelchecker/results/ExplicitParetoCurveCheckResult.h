#ifndef STORM_MODELCHECKER_EXPLICITPARETOCURVECHECKRESULT_H_
#define STORM_MODELCHECKER_EXPLICITPARETOCURVECHECKRESULT_H_

#include <map>
#include <vector>

#include "storm/modelchecker/results/ParetoCurveCheckResult.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/sparse/StateType.h"

namespace storm {
namespace modelchecker {
template<typename ValueType>
class ExplicitParetoCurveCheckResult : public ParetoCurveCheckResult<ValueType> {
   public:
    ExplicitParetoCurveCheckResult();
    ExplicitParetoCurveCheckResult(storm::storage::sparse::state_type const& state,
                                   std::vector<typename ParetoCurveCheckResult<ValueType>::point_type> const& points,
                                   typename ParetoCurveCheckResult<ValueType>::polytope_type const& underApproximation = nullptr,
                                   typename ParetoCurveCheckResult<ValueType>::polytope_type const& overApproximation = nullptr);
    ExplicitParetoCurveCheckResult(storm::storage::sparse::state_type const& state,
                                   std::vector<typename ParetoCurveCheckResult<ValueType>::point_type>&& points,
                                   typename ParetoCurveCheckResult<ValueType>::polytope_type&& underApproximation = nullptr,
                                   typename ParetoCurveCheckResult<ValueType>::polytope_type&& overApproximation = nullptr);

    ExplicitParetoCurveCheckResult(ExplicitParetoCurveCheckResult const& other) = default;
    ExplicitParetoCurveCheckResult& operator=(ExplicitParetoCurveCheckResult const& other) = default;
    ExplicitParetoCurveCheckResult(ExplicitParetoCurveCheckResult&& other) = default;
    ExplicitParetoCurveCheckResult& operator=(ExplicitParetoCurveCheckResult&& other) = default;
    virtual ~ExplicitParetoCurveCheckResult() = default;

    virtual std::unique_ptr<CheckResult> clone() const override;

    virtual bool isExplicitParetoCurveCheckResult() const override;
    virtual bool isExplicit() const override;

    virtual void filter(QualitativeCheckResult const& filter) override;

    storm::storage::sparse::state_type const& getState() const;

    virtual bool hasScheduler() const override;
    // void addScheduler(const std::shared_ptr<storm::storage::Scheduler<ValueType>>& scheduler);
    void setSchedulers(std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> schedulers);

    std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> const& getSchedulers() const;

    std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>>& getSchedulers();

   private:
    // The state of the checked model to which the result applies
    storm::storage::sparse::state_type state;
    // The corresponding strategies to reach each point in the pareto curve
    std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> schedulers;
};
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_EXPLICITPARETOCURVECHECKRESULT_H_ */
