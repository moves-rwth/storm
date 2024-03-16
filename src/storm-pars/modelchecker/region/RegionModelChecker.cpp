#include <queue>
#include <sstream>
#include <vector>

#include "storm-pars/analysis/OrderExtender.cpp"
#include "storm-pars/modelchecker/region/RegionModelChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/utility/Stopwatch.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm::modelchecker {

template<typename ParametricType>
std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> RegionModelChecker<ParametricType>::analyzeRegions(
    Environment const& env, std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, std::vector<RegionResultHypothesis> const& hypotheses,
    bool sampleVerticesOfRegion) {
    STORM_LOG_THROW(regions.size() == hypotheses.size(), storm::exceptions::InvalidArgumentException,
                    "The number of regions and the number of hypotheses do not match");
    std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, storm::modelchecker::RegionResult>> result;
    auto hypothesisIt = hypotheses.begin();
    for (auto const& region : regions) {
        detail::AnnotatedRegion<ParametricType> annotatedRegion(region);
        storm::modelchecker::RegionResult regionRes = analyzeRegion(env, annotatedRegion, *hypothesisIt, sampleVerticesOfRegion);
        result.emplace_back(annotatedRegion.region, regionRes);
        ++hypothesisIt;
    }
    return std::make_unique<storm::modelchecker::RegionCheckResult<ParametricType>>(std::move(result));
}

template<typename ParametricType>
detail::RegionSplitEstimateKind RegionModelChecker<ParametricType>::getDefaultRegionSplitEstimateKind(
    CheckTask<storm::logic::Formula, ParametricType> const&) const {
    return detail::RegionSplitEstimateKind::Distance;
}

template<typename ParametricType>
bool RegionModelChecker<ParametricType>::isRegionSplitEstimateKindSupported(detail::RegionSplitEstimateKind kind,
                                                                            CheckTask<storm::logic::Formula, ParametricType> const&) const {
    return kind == detail::RegionSplitEstimateKind::Distance;
}

template<typename ParametricType>
std::optional<detail::RegionSplitEstimateKind> RegionModelChecker<ParametricType>::getSpecifiedRegionSplitEstimateKind() const {
    return specifiedRegionSplitEstimateKind;
}

template<typename ParametricType>
std::vector<typename RegionModelChecker<ParametricType>::CoefficientType> RegionModelChecker<ParametricType>::obtainRegionSplitEstimates(
    std::set<VariableType> const& relevantParameters) const {
    STORM_LOG_ASSERT(specifiedRegionSplitEstimateKind.has_value(), "Unable to obtain region split estimates because they wre not requested.");
    STORM_LOG_ASSERT(specifiedRegionSplitEstimateKind.value() == detail::RegionSplitEstimateKind::Distance,
                     "requested region split estimate kind not supported");
    STORM_LOG_ASSERT(lastCheckedRegion.has_value(), "Unable to obtain region split estimates because no region was checked.");
    std::vector<CoefficientType> result;
    result.reserve(relevantParameters.size());
    for (auto const& p : relevantParameters) {
        result.push_back(lastCheckedRegion->getDifference(p));
    }
    return result;
}

template<typename ParametricType>
void RegionModelChecker<ParametricType>::specifySplitEstimates(std::optional<detail::RegionSplitEstimateKind> splitEstimates,
                                                               [[maybe_unused]] CheckTask<storm::logic::Formula, ParametricType> const& checkTask) {
    STORM_LOG_ASSERT(!splitEstimates.has_value() || isRegionSplitEstimateKindSupported(splitEstimates.value(), checkTask),
                     "specified region split estimate kind not supported");
    specifiedRegionSplitEstimateKind = splitEstimates;
}

template<typename ParametricType>
void RegionModelChecker<ParametricType>::specifyMonotonicity(std::shared_ptr<MonotonicityBackend<ParametricType>> backend,
                                                             CheckTask<storm::logic::Formula, ParametricType> const& checkTask) {
    STORM_LOG_ASSERT(!backend || isMonotonicitySupported(*backend, checkTask), "specified monotonicity backend not supported");
    monotonicityBackend = backend;
}

template class RegionModelChecker<storm::RationalFunction>;
}  // namespace storm::modelchecker
