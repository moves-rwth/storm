#include "storm-pars/modelchecker/region/RegionRefinementChecker.h"

#include <queue>

#include "storm-pars/modelchecker/region/AnnotatedRegion.h"
#include "storm/utility/ProgressMeasurement.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm::modelchecker {

template<typename ParametricType>
RegionRefinementChecker<ParametricType>::RegionRefinementChecker(std::unique_ptr<RegionModelChecker<ParametricType>>&& regionChecker) {
    STORM_LOG_ASSERT(regionChecker != nullptr, "The region model checker must not be null.");
    this->regionChecker = std::move(regionChecker);
}

template<typename ParametricType>
bool RegionRefinementChecker<ParametricType>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                        const CheckTask<storm::logic::Formula, ParametricType>& checkTask) const {
    return regionChecker->canHandle(parametricModel, checkTask);
}

template<typename ParametricType>
void RegionRefinementChecker<ParametricType>::specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                      CheckTask<storm::logic::Formula, ParametricType> const& checkTask,
                                                      RegionSplittingStrategy splittingStrategy,
                                                      std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend,
                                                      bool allowModelSimplifications) {
    this->monotonicityBackend = monotonicityBackend ? monotonicityBackend : std::make_shared<MonotonicityBackend<ParametricType>>();
    this->regionSplittingStrategy = std::move(splittingStrategy);
    // Potentially determine the kind of region split estimate to generate
    if (regionSplittingStrategy.heuristic == RegionSplittingStrategy::Heuristic::EstimateBased) {
        if (regionSplittingStrategy.estimateKind.has_value()) {
            STORM_LOG_THROW(regionChecker->isRegionSplitEstimateKindSupported(regionSplittingStrategy.estimateKind.value(), checkTask),
                            storm::exceptions::NotSupportedException, "The specified region split estimate kind is not supported by the region model checker.");
        } else {
            regionSplittingStrategy.estimateKind = regionChecker->getDefaultRegionSplitEstimateKind(checkTask);
            STORM_LOG_ASSERT(regionChecker->isRegionSplitEstimateKindSupported(regionSplittingStrategy.estimateKind.value(), checkTask),
                             "The region model checker does not support its default region split estimate kind.");
        }
    } else {
        regionSplittingStrategy.estimateKind = std::nullopt;  // do not compute estimates
    }

    regionChecker->specify(env, parametricModel, checkTask, regionSplittingStrategy.estimateKind, monotonicityBackend, allowModelSimplifications);
}

template<typename T>
class PartitioningProgress {
   public:
    PartitioningProgress(T const totalArea, T const coverageThreshold = storm::utility::zero<T>())
        : totalArea(totalArea),
          coverageThreshold(coverageThreshold),
          fractionOfUndiscoveredArea(storm::utility::one<T>()),
          fractionOfAllSatArea(storm::utility::zero<T>()),
          fractionOfAllViolatedArea(storm::utility::zero<T>()),
          progress("% covered area") {
        progress.setMaxCount(100 - asPercentage(coverageThreshold));
        progress.startNewMeasurement(0u);
    }

    uint64_t getUndiscoveredPercentage() const {
        return asPercentage(fractionOfUndiscoveredArea);
    }

    bool isCoverageThresholdReached() const {
        return fractionOfUndiscoveredArea <= coverageThreshold;
    }

    T addDiscoveredArea(T const& area) {
        auto addedFraction = area / totalArea;
        fractionOfUndiscoveredArea -= addedFraction;
        progress.updateProgress(100 - getUndiscoveredPercentage());
        return addedFraction;
    }

    void addAllSatArea(T const& area) {
        fractionOfAllSatArea += addDiscoveredArea(area);
    }

    void addAllViolatedArea(T const& area) {
        fractionOfAllViolatedArea += addDiscoveredArea(area);
    }

   private:
    static uint64_t asPercentage(T const& value) {
        return storm::utility::convertNumber<uint64_t>(storm::utility::round<T>(value * storm::utility::convertNumber<T, uint64_t>(100u)));
    }

    T const totalArea;
    T const coverageThreshold;
    T fractionOfUndiscoveredArea;
    T fractionOfAllSatArea;
    T fractionOfAllViolatedArea;
    storm::utility::ProgressMeasurement progress;
};

template<typename ParametricType>
std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> RegionRefinementChecker<ParametricType>::performRegionPartitioning(
    Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, std::optional<ParametricType> coverageThreshold,
    std::optional<uint64_t> depthThreshold, RegionResultHypothesis const& hypothesis, uint64_t monThresh) {
    STORM_LOG_INFO("Applying Region Partitioning on region: " << region.toString(true) << " .");

    auto progress = PartitioningProgress<CoefficientType>(
        region.area(), storm::utility::convertNumber<CoefficientType>(coverageThreshold.value_or(storm::utility::zero<ParametricType>())));

    // Holds the initial region as well as all considered (sub)-regions and their annotations as a tree
    AnnotatedRegion<ParametricType> rootRegion(region);

    // FIFO queue storing the current leafs of the region tree with neither allSat nor allViolated
    // As we currently split regions in the center, a FIFO queue will ensure that regions with a larger area are processed first.
    std::queue<std::reference_wrapper<AnnotatedRegion<ParametricType>>> unprocessedRegions;
    unprocessedRegions.emplace(rootRegion);

    uint64_t numOfAnalyzedRegions{0u};
    bool monotonicityInitialized{false};

    // Region Refinement Loop
    while (!progress.isCoverageThresholdReached() && !unprocessedRegions.empty()) {
        auto& currentRegion = unprocessedRegions.front().get();
        STORM_LOG_TRACE("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentRegion.refinementDepth << "; "
                                             << progress.getUndiscoveredPercentage() << "% still unknown; " << unprocessedRegions.size()
                                             << " regions unprocessed).");
        unprocessedRegions.pop();  // can pop already here, since the rootRegion has ownership.
        ++numOfAnalyzedRegions;

        if (!monotonicityInitialized && currentRegion.refinementDepth >= monThresh) {
            monotonicityInitialized = true;
            monotonicityBackend->initializeMonotonicity(env, currentRegion);
            rootRegion.propagateAnnotationsToSubregions(true);
        }
        monotonicityBackend->updateMonotonicity(env, currentRegion);

        currentRegion.result = regionChecker->analyzeRegion(env, currentRegion, hypothesis);

        if (currentRegion.result == RegionResult::AllSat) {
            progress.addAllSatArea(currentRegion.region.area());
        } else if (currentRegion.result == RegionResult::AllViolated) {
            progress.addAllViolatedArea(currentRegion.region.area());
        } else {
            // Split the region as long as the desired refinement depth is not reached.
            if (!depthThreshold || currentRegion.refinementDepth < depthThreshold.value()) {
                monotonicityBackend->updateMonotonicityBeforeSplitting(env, currentRegion);
                currentRegion.splitLeafNodeAtCenter(getSplittingVariables(currentRegion, Context::Partitioning), true);
                for (auto& child : currentRegion.subRegions) {
                    unprocessedRegions.emplace(child);
                }
            }
        }
    }

    // Prepare result
    uint64_t numberOfRegionsKnownThroughMonotonicity{0};
    std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> result;
    rootRegion.postOrderTraverseSubRegions([&result, &numberOfRegionsKnownThroughMonotonicity](auto& node) {
        if (node.subRegions.empty()) {
            if (node.resultKnownThroughMonotonicity) {
                ++numberOfRegionsKnownThroughMonotonicity;
            }
            result.emplace_back(node.region, node.result);
        }
    });
    auto const maxDepth = rootRegion.getMaxDepthOfSubRegions();
    STORM_LOG_INFO("Region partitioning terminated after analyzing " << numOfAnalyzedRegions << " regions.\n\t" << numberOfRegionsKnownThroughMonotonicity
                                                                     << " regions known through monotonicity.\n\tMaximum refinement depth: " << maxDepth
                                                                     << ".\n\t" << progress.getUndiscoveredPercentage()
                                                                     << "% of the parameter space are not covered.");

    auto regionCopyForResult = region;
    return std::make_unique<storm::modelchecker::RegionRefinementCheckResult<ParametricType>>(std::move(result), std::move(regionCopyForResult));
}

template<typename ParametricType>
std::pair<typename storm::storage::ParameterRegion<ParametricType>::CoefficientType, typename storm::storage::ParameterRegion<ParametricType>::Valuation>
RegionRefinementChecker<ParametricType>::computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region,
                                                              storm::solver::OptimizationDirection const& dir, ParametricType const& precision,
                                                              bool absolutePrecision, std::optional<storm::logic::Bound> const& boundInvariant) {
    auto progress = PartitioningProgress<CoefficientType>(region.area());

    // Holds the initial region as well as all considered (sub)-regions and their annotations as a tree
    AnnotatedRegion<ParametricType> rootRegion(region);

    // Priority Queue storing the regions that still need to be processed. Regions with a "good" bound are processed first
    auto cmp = storm::solver::minimize(dir) ? [](AnnotatedRegion<ParametricType> const& lhs,
                                                 AnnotatedRegion<ParametricType> const& rhs) { return *lhs.knownLowerValueBound > *rhs.knownLowerValueBound; }
                                            : [](AnnotatedRegion<ParametricType> const& lhs, AnnotatedRegion<ParametricType> const& rhs) {
                                                  return *lhs.knownUpperValueBound < *rhs.knownUpperValueBound;
                                              };
    std::priority_queue<std::reference_wrapper<AnnotatedRegion<ParametricType>>, std::vector<std::reference_wrapper<AnnotatedRegion<ParametricType>>>,
                        decltype(cmp)>
        unprocessedRegions(cmp);
    unprocessedRegions.push(rootRegion);

    // Initialize Monotonicity
    monotonicityBackend->initializeMonotonicity(env, rootRegion);
    // TODO: Catch and handle the easy case where parameters are already known to be monotone

    // Initialize result
    auto valueValuation = regionChecker->getAndEvaluateGoodPoint(env, rootRegion, dir);
    auto& value = valueValuation.first;
    if (boundInvariant && !boundInvariant->isSatisfied(value)) {
        return valueValuation;
    }

    // Handle input precision
    STORM_LOG_THROW(storm::utility::isConstant(precision), storm::exceptions::InvalidArgumentException,
                    "Precision must be a constant value. Got " << precision << " instead.");
    CoefficientType convertedPrecision = storm::utility::convertNumber<CoefficientType>(precision);

    // Helper functions to check if a given result is better than the currently known result
    auto isBetterThanValue = [&value, &dir](auto const& newValue) { return storm::solver::minimize(dir) ? newValue < value : newValue > value; };
    auto isStrictlyBetterThanValue = [&value, &dir, &convertedPrecision, &absolutePrecision](auto const& newValue) {
        CoefficientType const usedPrecision = convertedPrecision * (absolutePrecision ? storm::utility::one<CoefficientType>() : value);
        return storm::solver::minimize(dir) ? newValue < value - usedPrecision : newValue > value + usedPrecision;
    };

    // Region Refinement Loop
    uint64_t numOfAnalyzedRegions{0u};
    while (!unprocessedRegions.empty()) {
        auto& currentRegion = unprocessedRegions.top().get();
        auto currentBound =
            storm::solver::minimize(dir) ? currentRegion.knownLowerValueBound.getOptionalValue() : currentRegion.knownUpperValueBound.getOptionalValue();
        STORM_LOG_TRACE("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentRegion.refinementDepth << "; "
                                             << progress.getUndiscoveredPercentage() << "% still unknown; " << unprocessedRegions.size()
                                             << " regions unprocessed). Best known value: " << value << ".");
        unprocessedRegions.pop();  // can pop already here, since the rootRegion has ownership.
        ++numOfAnalyzedRegions;

        // Compute the bound for this region (unless the known bound is already too weak)
        if (!currentBound || isStrictlyBetterThanValue(currentBound.value())) {
            // Improve over-approximation of extremal value (within this region)
            currentBound = regionChecker->getBoundAtInitState(env, currentRegion, dir);
            if (storm::solver::minimize(dir)) {
                currentRegion.knownLowerValueBound &= *currentBound;
            } else {
                currentRegion.knownUpperValueBound &= *currentBound;
            }
            // TODO: Cache results as bounds for monotonicity
        }

        // Process the region if the bound is promising
        if (isStrictlyBetterThanValue(currentBound.value())) {
            // Improve (global) under-approximation of extremal value
            // Check whether this region contains a new 'good' value and set this value if that is the case
            monotonicityBackend->updateMonotonicity(env,
                                                    currentRegion);  // TODO: Why is this done after analysis here and before analysis in partitioning mode?
            auto [currValue, currValuation] = regionChecker->getAndEvaluateGoodPoint(env, currentRegion, dir);
            if (isBetterThanValue(currValue)) {
                valueValuation = {currValue, currValuation};
                if (boundInvariant && !boundInvariant->isSatisfied(value)) {
                    return valueValuation;
                }
            }
        }

        // Trigger region-splitting if over- and under-approximation are still too far apart
        if (isStrictlyBetterThanValue(currentBound.value())) {
            monotonicityBackend->updateMonotonicityBeforeSplitting(env, currentRegion);
            currentRegion.splitLeafNodeAtCenter(getSplittingVariables(currentRegion, Context::ExtremalValue), true);
            for (auto& child : currentRegion.subRegions) {
                unprocessedRegions.emplace(child);
            }
        } else {
            progress.addDiscoveredArea(currentRegion.region.area());
        }
    }

    STORM_LOG_INFO("Region partitioning for extremal value terminated after analyzing "
                   << numOfAnalyzedRegions << " regions.\n\t" << progress.getUndiscoveredPercentage() << "% of the parameter space are not covered.");
    return valueValuation;
}

template<typename ParametricType>
bool RegionRefinementChecker<ParametricType>::verifyRegion(const storm::Environment& env, const storm::storage::ParameterRegion<ParametricType>& region,
                                                           const storm::logic::Bound& bound) {
    // Use the bound from the formula.
    CoefficientType valueToCheck = storm::utility::convertNumber<CoefficientType>(bound.threshold.evaluateAsRational());
    // We will try to violate the bound.
    storm::solver::OptimizationDirection dir =
        isLowerBound(bound.comparisonType) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
    // We pass the bound as an invariant; as soon as it is obtained, we can stop the search.
    auto res = computeExtremalValue(env, region, dir, storm::utility::zero<ParametricType>(), false, bound).first;
    STORM_LOG_DEBUG("Reported extremal value " << res);
    // TODO use termination bound instead of initial value?
    return storm::solver::minimize(dir) ? res >= valueToCheck : res <= valueToCheck;
}

template<typename ParametricType>
std::set<typename RegionRefinementChecker<ParametricType>::VariableType> RegionRefinementChecker<ParametricType>::getSplittingVariables(
    AnnotatedRegion<ParametricType> const& region, Context context) const {
    // TODO: Use the splitting strategy, monotonicity, context, ... as in splitSmart
    return region.region.getVariables();
}

template class RegionRefinementChecker<storm::RationalFunction>;
}  // namespace storm::modelchecker
