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

    explicit AnnotatedRegion(Region const& region);

    void propagateAnnotationsToSubregions(bool allowDeleteAnnotationsOfThis);

    void splitAndPropagate(typename Region::Valuation const& splittingPoint, std::set<VariableType> const& consideredVariable,
                           bool allowDeleteAnnotationsOfThis);

    void splitLeafNodeAtCenter(std::set<VariableType> const& splittingVariables, bool allowDeleteAnnotationsOfThis);

    void postOrderTraverseSubRegions(std::function<void(AnnotatedRegion<ParametricType>&)> const& visitor);

    void preOrderTraverseSubRegions(std::function<void(AnnotatedRegion<ParametricType>&)> const& visitor);

    uint64_t getMaxDepthOfSubRegions() const;

    std::vector<AnnotatedRegion<ParametricType>> subRegions;  /// The subregions of this region

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

    storm::OptionalRef<DefaultMonotonicityAnnotation> getDefaultMonotonicityAnnotation();
    storm::OptionalRef<DefaultMonotonicityAnnotation const> getDefaultMonotonicityAnnotation() const;

    storm::OptionalRef<OrderBasedMonotonicityAnnotation> getOrderBasedMonotonicityAnnotation();
    storm::OptionalRef<OrderBasedMonotonicityAnnotation const> getOrderBasedMonotonicityAnnotation() const;

    storm::OptionalRef<storm::analysis::MonotonicityResult<VariableType> const> getGlobalMonotonicityResult() const;

    bool updateValueBound(CoefficientType const& newValue, storm::OptimizationDirection dir);

    storm::utility::Maximum<CoefficientType> knownLowerValueBound;  // Maximal known lower bound on the value of the region
    storm::utility::Minimum<CoefficientType> knownUpperValueBound;  // Minimal known upper bound on the value of the region
};
}  // namespace storm::modelchecker