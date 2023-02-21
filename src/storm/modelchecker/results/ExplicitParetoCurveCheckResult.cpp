
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
namespace modelchecker {
template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType>::ExplicitParetoCurveCheckResult() {
    // Intentionally left empty.
}

template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType>::ExplicitParetoCurveCheckResult(storm::storage::sparse::state_type const& state,
                                                                          std::vector<typename ParetoCurveCheckResult<ValueType>::point_type> const& points,
                                                                          typename ParetoCurveCheckResult<ValueType>::polytope_type const& underApproximation,
                                                                          typename ParetoCurveCheckResult<ValueType>::polytope_type const& overApproximation)
    : ParetoCurveCheckResult<ValueType>(points, underApproximation, overApproximation), state(state) {
    // Intentionally left empty.
}

template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType>::ExplicitParetoCurveCheckResult(storm::storage::sparse::state_type const& state,
                                                                          std::vector<typename ParetoCurveCheckResult<ValueType>::point_type>&& points,
                                                                          typename ParetoCurveCheckResult<ValueType>::polytope_type&& underApproximation,
                                                                          typename ParetoCurveCheckResult<ValueType>::polytope_type&& overApproximation)
    : ParetoCurveCheckResult<ValueType>(points, underApproximation, overApproximation), state(state) {
    // Intentionally left empty.
}

template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType>::ExplicitParetoCurveCheckResult(
    storm::storage::sparse::state_type const& state, std::vector<typename ParetoCurveCheckResult<ValueType>::point_type>&& points,
    std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>>&& schedulers,
    typename ParetoCurveCheckResult<ValueType>::polytope_type&& underApproximation,
    typename ParetoCurveCheckResult<ValueType>::polytope_type&& overApproximation)
    : ParetoCurveCheckResult<ValueType>(points, underApproximation, overApproximation), state(state), schedulers(schedulers) {
    // Intentionally left empty.
}

template<typename ValueType>
std::unique_ptr<CheckResult> ExplicitParetoCurveCheckResult<ValueType>::clone() const {
    return std::make_unique<ExplicitParetoCurveCheckResult<ValueType>>(this->state, this->points, this->underApproximation, this->overApproximation);
}

template<typename ValueType>
bool ExplicitParetoCurveCheckResult<ValueType>::isExplicitParetoCurveCheckResult() const {
    return true;
}

template<typename ValueType>
bool ExplicitParetoCurveCheckResult<ValueType>::isExplicit() const {
    return true;
}

template<typename ValueType>
void ExplicitParetoCurveCheckResult<ValueType>::filter(QualitativeCheckResult const& filter) {
    STORM_LOG_THROW(filter.isExplicitQualitativeCheckResult(), storm::exceptions::InvalidOperationException,
                    "Cannot filter explicit check result with non-explicit filter.");
    STORM_LOG_THROW(filter.isResultForAllStates(), storm::exceptions::InvalidOperationException, "Cannot filter check result with non-complete filter.");
    ExplicitQualitativeCheckResult const& explicitFilter = filter.asExplicitQualitativeCheckResult();
    ExplicitQualitativeCheckResult::vector_type const& filterTruthValues = explicitFilter.getTruthValuesVector();

    STORM_LOG_THROW(filterTruthValues.getNumberOfSetBits() == 1 && filterTruthValues.get(state), storm::exceptions::InvalidOperationException,
                    "The check result fails to contain some results referred to by the filter.");
}

template<typename ValueType>
storm::storage::sparse::state_type const& ExplicitParetoCurveCheckResult<ValueType>::getState() const {
    return state;
}

template<typename ValueType>
bool ExplicitParetoCurveCheckResult<ValueType>::hasScheduler() const {
    return schedulers.size() > 0;
}

template<typename ValueType>
void ExplicitParetoCurveCheckResult<ValueType>::setSchedulers(
    const std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> newSchedulers) {
    schedulers = newSchedulers;
}

template<typename ValueType>
std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>> const& ExplicitParetoCurveCheckResult<ValueType>::getSchedulers()
    const {
    STORM_LOG_THROW(this->hasScheduler(), storm::exceptions::InvalidOperationException, "Unable to retrieve non-existing scheduler.");
    return schedulers;
}

template<typename ValueType>
std::map<std::vector<ValueType>, std::shared_ptr<storm::storage::Scheduler<ValueType>>>& ExplicitParetoCurveCheckResult<ValueType>::getSchedulers() {
    STORM_LOG_THROW(this->hasScheduler(), storm::exceptions::InvalidOperationException, "Unable to retrieve non-existing scheduler.");
    return schedulers;
}

template class ExplicitParetoCurveCheckResult<double>;
#ifdef STORM_HAVE_CARL
template class ExplicitParetoCurveCheckResult<storm::RationalNumber>;
template class ExplicitParetoCurveCheckResult<storm::RationalFunction>;
#endif
}  // namespace modelchecker
}  // namespace storm
