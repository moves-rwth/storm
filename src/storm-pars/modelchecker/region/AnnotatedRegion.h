#pragma once

#include <memory>
#include <variant>

#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/analysis/Order.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/utility/Extremum.h"
#include "storm/utility/OptionalRef.h"

namespace storm::modelchecker {
template<typename ParametricType>
struct AnnotatedRegion {
    using Region = storm::storage::ParameterRegion<ParametricType>;
    using VariableType = typename Region::VariableType;
    using CoefficientType = typename Region::CoefficientType;

    explicit AnnotatedRegion(Region const& region) : region(region) {
        // Intentionally left empty
    }

    void propagateAnnotationsToSubregions(bool allowDeleteAnnotationsOfThis) {
        for (auto& r : subRegions) {
            if (result == storm::modelchecker::RegionResult::AllSat || result == storm::modelchecker::RegionResult::AllViolated) {
                r.result = result;
            } else if ((result == storm::modelchecker::RegionResult::CenterSat || result == storm::modelchecker::RegionResult::CenterViolated) &&
                       r.result == storm::modelchecker::RegionResult::Unknown && r.region.contains(region.getCenterPoint())) {
                r.result = result == storm::modelchecker::RegionResult::CenterSat ? storm::modelchecker::RegionResult::ExistsSat
                                                                                  : storm::modelchecker::RegionResult::ExistsViolated;
            }
            if (r.refinementDepth == 0) {
                r.refinementDepth = refinementDepth + 1;
            }
            r.monotonicityAnnotation =
                monotonicityAnnotation;  // Potentially shared for all subregions! Creating actual copies is handled via the monotonicity backend
            r.knownLowerValueBound &= knownLowerValueBound;
            r.knownUpperValueBound &= knownUpperValueBound;
        }
        if (allowDeleteAnnotationsOfThis) {
            // Delete annotations that are memory intensive
            monotonicityAnnotation.template emplace<0>();
        }
    }

    void splitAndPropagate(typename Region::Valuation const& splittingPoint, std::set<VariableType> const& consideredVariable,
                           bool allowDeleteAnnotationsOfThis) {
        std::vector<storm::storage::ParameterRegion<ParametricType>> subRegionsWithoutAnnotations;
        region.split(splittingPoint, subRegionsWithoutAnnotations, consideredVariable);
        subRegions.reserve(subRegionsWithoutAnnotations.size());
        for (auto& newRegion : subRegionsWithoutAnnotations) {
            subRegions.emplace_back(newRegion);
        }
        propagateAnnotationsToSubregions(allowDeleteAnnotationsOfThis);
    }

    void splitLeafNodeAtCenter(std::set<VariableType> const& splittingVariables, bool allowDeleteAnnotationsOfThis) {
        STORM_LOG_ASSERT(subRegions.empty(), "Region assumed to be a leaf.");
        splitAndPropagate(region.getCenterPoint(), splittingVariables, allowDeleteAnnotationsOfThis);
    }

    std::vector<AnnotatedRegion<ParametricType>> subRegions;  /// The subRegions of this region

    void postOrderTraverseSubRegions(std::function<void(AnnotatedRegion<ParametricType>&)> const& visitor) {
        for (auto& child : subRegions) {
            child.postOrderTraverseSubRegions(visitor);
        }
        visitor(*this);
    }

    void preOrderTraverseSubRegions(std::function<void(AnnotatedRegion<ParametricType>&)> const& visitor) {
        visitor(*this);
        for (auto& child : subRegions) {
            child.preOrderTraverseSubRegions(visitor);
        }
    }

    uint64_t getMaxDepthOfSubRegions() const {
        uint64_t max{0u};
        for (auto const& child : subRegions) {
            max = std::max(max, child.getMaxDepthOfSubRegions() + 1);
        }
        return max;
    }

    Region const region;  /// The region this is an annotation for

    uint64_t refinementDepth{0};  /// The depth of the refinement tree this region is in

    storm::modelchecker::RegionResult result{storm::modelchecker::RegionResult::Unknown};  /// The result of the analysis of this region
    bool resultKnownThroughMonotonicity{false};                                            /// Whether the result is known through monotonicity

    struct DefaultMonotonicityAnnotation {
        std::shared_ptr<storm::analysis::MonotonicityResult<VariableType>> globalMonotonicity{nullptr};
    };
    struct OrderBasedMonotonicityAnnotation {
        std::shared_ptr<storm::analysis::Order> stateOrder{nullptr};
        std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult{nullptr};
    };
    std::variant<DefaultMonotonicityAnnotation, OrderBasedMonotonicityAnnotation> monotonicityAnnotation;

    storm::OptionalRef<DefaultMonotonicityAnnotation> getDefaultMonotonicityAnnotation() {
        if (auto* mono = std::get_if<DefaultMonotonicityAnnotation>(&monotonicityAnnotation)) {
            return *mono;
        }
        return storm::NullRef;
    }

    storm::OptionalRef<OrderBasedMonotonicityAnnotation> getOrderBasedMonotonicityAnnotation() {
        if (auto* mono = std::get_if<OrderBasedMonotonicityAnnotation>(&monotonicityAnnotation)) {
            return *mono;
        }
        return storm::NullRef;
    }

    storm::OptionalRef<DefaultMonotonicityAnnotation const> getDefaultMonotonicityAnnotation() const {
        if (auto const* mono = std::get_if<DefaultMonotonicityAnnotation>(&monotonicityAnnotation)) {
            return *mono;
        }
        return storm::NullRef;
    }

    storm::OptionalRef<OrderBasedMonotonicityAnnotation const> getOrderBasedMonotonicityAnnotation() const {
        if (auto const* mono = std::get_if<OrderBasedMonotonicityAnnotation>(&monotonicityAnnotation)) {
            return *mono;
        }
        return storm::NullRef;
    }

    storm::OptionalRef<storm::analysis::MonotonicityResult<VariableType> const> getGlobalMonotonicityResult() const {
        storm::OptionalRef<storm::analysis::MonotonicityResult<VariableType> const> result;
        if (auto defaultMono = getDefaultMonotonicityAnnotation(); defaultMono.has_value() && defaultMono->globalMonotonicity) {
            result.reset(*defaultMono->globalMonotonicity);
        } else if (auto orderMono = getOrderBasedMonotonicityAnnotation(); orderMono.has_value() && orderMono->localMonotonicityResult) {
            if (auto globalRes = orderMono->localMonotonicityResult->getGlobalMonotonicityResult()) {
                result.reset(*globalRes);
            }
        } else {
            STORM_LOG_ASSERT(false, "Unknown monotonicity annotation type.");
        }
        return result;
    }

    bool updateValueBound(CoefficientType const& newValue, storm::OptimizationDirection dir) {
        if (minimize(dir)) {
            return knownLowerValueBound &= newValue;
        } else {
            return knownUpperValueBound &= newValue;
        }
    }

    storm::utility::Maximum<CoefficientType> knownLowerValueBound;  // Maximal known lower bound on the value of the region
    storm::utility::Minimum<CoefficientType> knownUpperValueBound;  // Minimal known upper bound on the value of the region

   private:
    bool monotonicityAnnotationsInitialized() const {
        if (auto defaultMono = getDefaultMonotonicityAnnotation(); defaultMono.has_value()) {
            return defaultMono->globalMonotonicity != nullptr;
        } else if (auto orderMono = getOrderBasedMonotonicityAnnotation(); orderMono.has_value()) {
            return orderMono->stateOrder != nullptr || orderMono->localMonotonicityResult != nullptr;
        } else {
            STORM_LOG_ASSERT(false, "Unknown monotonicity annotation type.");
            return false;
        }
    }
};
}  // namespace storm::modelchecker