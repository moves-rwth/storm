#include "storm-pars/modelchecker/region/RegionRefinementChecker.h"

#include <queue>

#include "storm-pars/modelchecker/region/detail/AnnotatedRegion.h"
#include "storm/utility/ProgressMeasurement.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm::modelchecker {

template<typename ParametricType>
struct RegionTree {
    using Region = storm::storage::ParameterRegion<ParametricType>;
    using AnnotatedRegion = detail::AnnotatedRegion<ParametricType>;
    using VariableType = typename Region::VariableType;

    RegionTree(Region const& region) : annotatedRegion(region) {
        // Intentionally left empty
    }

    AnnotatedRegion annotatedRegion;                   /// The (annotated) region at this node of the tree
    std::vector<RegionTree<ParametricType>> children;  /// The children of this node

    void postOrderTraverse(std::function<void(RegionTree<ParametricType>&)> const& visitor) {
        for (auto& child : children) {
            child.postOrderTraverse(visitor);
        }
        visitor(*this);
    }

    void preOrderTraverse(std::function<void(RegionTree<ParametricType>&)> const& visitor) {
        visitor(*this);
        for (auto& child : children) {
            child.preOrderTraverse(visitor);
        }
    }

    void propagateAnnotations(bool allowDeleteAnnotationsFromInnerNodes) {
        preOrderTraverse([allowDeleteAnnotationsFromInnerNodes](auto& node) {
            node.annotatedRegion.propagateAnnotationsToSubregions(node.children, allowDeleteAnnotationsFromInnerNodes);
        });
    }

    void splitLeafNodeAtCenter(std::set<VariableType> const& splittingVariables, bool allowDeleteAnnotationsOfThis) {
        STORM_LOG_ASSERT(children.empty(), "Cannot split a non-leaf node.");
        std::vector<AnnotatedRegion const&> splitResult;
        annotatedRegion.splitAndPropagate(annotatedRegion.region.getCenterPoint(), splittingVariables, allowDeleteAnnotationsOfThis, splitResult);
        for (auto const& region : splitResult) {
            children.emplace_back(region);
        }
    }

    uint64_t getDepth() const {
        if (children.empty()) {
            return 0u;
        } else {
            uint64_t max = 0;
            for (auto const& child : children) {
                max = std::max(max, child.getDepth());
            }
            return 1 + max;
        }
    }
};

template<typename ParametricType>
RegionRefinementChecker<ParametricType>::RegionRefinementChecker(RegionModelChecker<ParametricType>&& regionChecker) {
    this->regionChecker = std::make_unique<RegionModelChecker<ParametricType>>(std::move(regionChecker));
}

template<typename ParametricType>
bool RegionRefinementChecker<ParametricType>::canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                        const CheckTask<storm::logic::Formula, ParametricType>& checkTask) const {
    return regionChecker->canHandle(parametricModel, checkTask);
}

template<typename ParametricType>
void RegionRefinementChecker<ParametricType>::specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel,
                                                      CheckTask<storm::logic::Formula, ParametricType> const& checkTask,
                                                      detail::RegionSplittingStrategy splittingStrategy,
                                                      std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend,
                                                      bool allowModelSimplifications) {
    this->monotonicityBackend = monotonicityBackend ? monotonicityBackend : std::make_shared<MonotonicityBackend<ParametricType>>();
    this->regionSplittingStrategy = std::move(splittingStrategy);
    // Potentially determine the kind of region split estimate to generate
    if (regionSplittingStrategy.heuristic == detail::RegionSplittingStrategy::Heuristic::EstimateBased) {
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
        fractionOfAllSatArea += addDiscoveredArea(area, true);
    }

    void addAllViolatedArea(T const& area) {
        fractionOfAllViolatedArea += addDiscoveredArea(area, false);
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

    // All considered (sub)-regions
    RegionTree<ParametricType> regionTreeRoot(region);

    // FIFO queue storing the current leafs of the region tree with neither allSat nor allViolated
    // As we currently split regions in the center, a FIFO queue will ensure that regions with a larger area are processed first.
    std::queue<std::reference_wrapper<RegionTree<ParametricType>>> unprocessedRegions;
    unprocessedRegions.emplace(regionTreeRoot);

    uint64_t numOfAnalyzedRegions{0u};
    bool monotonicityInitialized{false};

    // Region Refinement Loop
    while (!progress.isCoverageThresholdReached() && !unprocessedRegions.empty()) {
        auto& currentRegionTreeNode = unprocessedRegions.front().get();
        auto& currentRegion = currentRegionTreeNode.region;
        STORM_LOG_TRACE("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentRegion.refinementDepth << "; "
                                             << progress.getUndiscoveredPercentage() << "% still unknown; " << unprocessedRegions.size()
                                             << " regions unprocessed).");
        ++numOfAnalyzedRegions;

        if (!monotonicityInitialized && currentRegion.refinementDepth >= monThresh) {
            monotonicityInitialized = true;
            monotonicityBackend.initializeMonotonicity(regionTreeRoot.region);
            regionTreeRoot.propagateAnnotations(true);
        }
        monotonicityBackend.updateMonotonicity(currentRegion.annotatedRegion);

        currentRegion.result = regionChecker->analyzeRegion(env, currentRegion, hypothesis);

        if (currentRegion.result == RegionResult::AllSat) {
            progress.addAllSatArea(currentRegion.area());
        } else if (currentRegion.result == RegionResult::AllViolated) {
            progress.addAllViolatedArea(currentRegion.area());
        } else {
            // Split the region as long as the desired refinement depth is not reached.
            if (!depthThreshold || currentRegion.refinementDepth < depthThreshold.value()) {
                currentRegionTreeNode.splitLeafNodeAtCenter(getSplittingVariables(currentRegion, Context::Partitioning), true);
                for (auto& child : currentRegionTreeNode.children) {
                    unprocessedRegions.emplace(child);
                }
            }
        }
        unprocessedRegions.pop();
    }

    // Prepare result
    uint64_t numberOfRegionsKnownThroughMonotonicity{0};
    std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> result;
    regionTreeRoot.postOrderTraverse([&result, &numberOfRegionsKnownThroughMonotonicity](auto& node) {
        if (node.children.empty()) {
            if (node.annotatedRegion.resultKnownThroughMonotonicity) {
                ++numberOfRegionsKnownThroughMonotonicity;
            }
            result.emplace_back(node.annotatedRegion.region, node.annotatedRegion.result);
        }
    });
    auto const maxDepth = regionTreeRoot.getDepth();
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
                                                              storm::solver::OptimizationDirection const& dir, CoefficientType const& precision,
                                                              bool absolutePrecision, std::optional<storm::logic::Bound> const& boundInvariant) {
    auto progress = PartitioningProgress<CoefficientType>(region.area());

    // Priority Queue storing the regions that still need to be processed. Regions with a "good" bound are processed first
    auto cmp =
        storm::solver::minimize(dir)
            ? [](detail::AnnotatedRegion<ParametricType> const& lhs,
                 detail::AnnotatedRegion<ParametricType> const& rhs) { return *lhs.knownLowerValueBound > rhs.knownLowerValueBound; }
            : [](detail::AnnotatedRegion<ParametricType> const& lhs, detail::AnnotatedRegion<ParametricType> const& rhs) { return lhs.bound < rhs.bound; };
    std::priority_queue<detail::AnnotatedRegion<ParametricType>, std::vector<detail::AnnotatedRegion<ParametricType>>, decltype(cmp)> unprocessedRegions(cmp);
    unprocessedRegions.emplace(region);

    monotonicityBackend.initializeMonotonicity(
        unprocessedRegions.top());  // TODO might need information on whether we min or max. Maybe use monDepth parameter?

    auto valueValuation = regionChecker->getAndEvaluateGoodPoint(env, unprocessedRegions.top(), dir);
    auto& value = valueValuation.first;
    if (boundInvariant && !boundInvariant.value().isSatisfied(value.get())) {
        return valueValuation;
    }

    // Helper functions to check if a given result is better than the currently known result
    auto isBetterValue = [&value, &dir](auto const& newValue) { return storm::solver::minimize(dir) ? newValue < value : newValue > value; };
    auto isStrictlyBetterValue = [&value, &dir, &precision, &absolutePrecision](auto const& newValue) {
        if (storm::solver::minimize(dir)) {
            return newValue < (absolutePrecision ? value - precision : value * (1 - precision));
        } else {
            return newValue > (absolutePrecision ? value + precision : value * (1 + precision));
        }
    };

    // TODO: maybe we can skip the loop if things are already known to be fully monotonic

    // Region Refinement Loop
    uint64_t numOfAnalyzedRegions{0u};
    while (!unprocessedRegions.empty()) {
        auto& currentRegion = unprocessedRegions.top();
        auto& currentBound = storm::solver::minimize(dir) ? currentRegion.knownLowerValueBound : currentRegion.knownUpperValueBound;
        STORM_LOG_TRACE("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentRegion.refinementDepth << "; "
                                             << progress.getUndiscoveredPercentage() << "% still unknown; " << unprocessedRegions.size()
                                             << " regions unprocessed). Best known value: " << value << ".");
        ++numOfAnalyzedRegions;

        // Compute the bound for this region (unless the known bound is already too weak)
        if (!currentBound || isStrictlyBetterValue(currentBound.value())) {
            // Improve over-approximation of extremal value (within this region)
            currentBound = regionChecker->getBoundAtInitState(env, currentRegion, dir);
            // TODO: Cache results as bounds for monotonicity
        }

        // Process the region if the bound is promising
        if (isStrictlyBetterValue(currentBound.value())) {
            // Improve (global) under-approximation of extremal value
            // Check whether this region contains a new 'good' value and set this value if that is the case
            monotonicityBackend.updateMonotonicity(currentRegion);  // TODO: Why is this done after analysis here and before analysis in partitioning mode?
            auto [currValue, currValuation] = regionChecker->getAndEvaluateGoodPoint(env, currentRegion, dir);
            if (isBetterValue(currValue)) {
                valueValuation = {currValue, currValuation};
                if (boundInvariant && !boundInvariant.value().isSatisfied(value.get())) {
                    return valueValuation;
                }
            }
        }

        // Trigger region-splitting if over- and under-approximation are still too far apart
        if (isStrictlyBetterValue(currentBound.value())) {
            monotonicityBackend.updateMonotonicityForSplitting(currentRegion);  // TODO: not sure if it is necessary like this?

            std::vector<detail::AnnotatedRegion<ParametricType> const&> splitResult;
            currentRegion.splitAndPropagate(currentRegion.region.getCenterPoint(), getSplittingVariables(currentRegion, Context::ExtremalValue), true,
                                            splitResult);
            for (auto const& region : splitResult) {
                unprocessedRegions.emplace(region);
            }
        } else {
            progress.addDiscoveredArea(currentRegion.region.area());
        }
        unprocessedRegions.pop();  // Pop at the end of the loop to ensure that references remain valid
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
    return storm::solver::minimize(dir) ? storm::utility::convertNumber<CoefficientType>(res) >= valueToCheck
                                        : storm::utility::convertNumber<CoefficientType>(res) <= valueToCheck;
}

template<typename ParametricType>
std::set<typename RegionRefinementChecker<ParametricType>::VariableType> RegionRefinementChecker<ParametricType>::getSplittingVariables(
    detail::AnnotatedRegion<ParametricType> const& region, Context context) const {
    // TODO: Use the splitting strategy, monotonicity, context, ... as in splitSmart
    return region.getVariables();
}

}  // namespace storm::modelchecker
