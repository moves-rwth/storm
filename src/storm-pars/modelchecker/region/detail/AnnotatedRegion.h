#pragma once

#include <memory>
#include <variant>

#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/analysis/Order.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/utility/Extremum.h"
#include "storm/utility/OptionalRef.h"

namespace storm::modelchecker::detail {
template<typename ParametricType>
struct AnnotatedRegion {
    using Region = storm::storage::ParameterRegion<ParametricType>;
    using VariableType = typename Region::VariableType;
    using CoefficientType = typename Region::CoefficientType;

    AnnotatedRegion(Region const& region) : region(region) {
        // Intentionally left empty
    }

    void propagateAnnotationsToSubregions(std::vector<AnnotatedRegion<ParametricType>>& subRegions, bool allowDeleteAnnotationsOfThis) {
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

            if (auto defaultMono = getDefaultMonotonicityAnnotation(); defaultMono.has_value()) {
                propagateMonotonicityAnnotationsToSubregions(subRegions, *defaultMono, allowDeleteAnnotationsOfThis);
            } else if (auto orderMono = getOrderBasedMonotonicityAnnotation(); orderMono.has_value()) {
                propagateMonotonicityAnnotationsToSubregions(subRegions, *orderMono, allowDeleteAnnotationsOfThis);
            } else {
                STORM_LOG_ASSERT(false, "Unknown monotonicity annotation type.");
            }

            if (knownLowerValueBound && (!r.knownLowerValueBound || *knownLowerValueBound > *r.knownLowerValueBound)) {
                r.knownLowerValueBound = knownLowerValueBound;
            }
            if (knownUpperValueBound && (!r.knownUpperValueBound || *knownUpperValueBound < *r.knownUpperValueBound)) {
                r.knownUpperValueBound = knownUpperValueBound;
            }
        }
    }

    void splitAndPropagate(typename Region::Valuation const& splittingPoint, std::set<VariableType> const& consideredVariable,
                           bool allowDeleteAnnotationsOfThis, std::vector<AnnotatedRegion<ParametricType>>& outputVector) const {
        std::vector<storm::storage::ParameterRegion<ParametricType>> subRegions;
        region.split(splittingPoint, subRegions, consideredVariable);
        outputVector.reserve(subRegions.size());
        for (auto& newRegion : subRegions) {
            outputVector.emplace_back(newRegion);
        }
        propagateAnnotationsToSubregions(outputVector, allowDeleteAnnotationsOfThis);
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
        }
    }

    static void propagateMonotonicityAnnotationsToSubregions(std::vector<AnnotatedRegion<ParametricType>>& subRegions,
                                                             DefaultMonotonicityAnnotation& parentAnnotation, bool allowDeleteAnnotationsOfThis) {
        if (parentAnnotation.globalMonotonicity) {
            bool const parentDone = parentAnnotation.globalMonotonicity->isDone();
            for (auto& r : subRegions) {
                if (!r.monotonicityAnnotationsInitialized()) {
                    // Share ownership with parent if done or Transfer ownership from parent if possible. Otherwise copy
                    if (parentDone || (allowDeleteAnnotationsOfThis && parentAnnotation.globalMonotonicity.use_count() == 1)) {
                        r.monotonicityAnnotation = parentAnnotation;
                    } else {
                        r.monotonicityAnnotation = DefaultMonotonicityAnnotation{parentAnnotation.globalMonotonicity->copy()};
                    }
                }
            }
        }
    }

    static void propagateMonotonicityAnnotationsToSubregions(std::vector<AnnotatedRegion<ParametricType>>& subRegions,
                                                             OrderBasedMonotonicityAnnotation& parentAnnotation, bool allowDeleteAnnotationsOfThis) {
        if (parentAnnotation.stateOrder) {
            STORM_LOG_ASSERT(parentAnnotation.localMonotonicityResult, "Local monotonicity result must be set iff state order is set");
            bool const stateOrderDone = parentAnnotation.stateOrder->getDoneBuilding();
            bool const localMonotonicityResultDone = stateOrderDone && parentAnnotation.localMonotonicityResult->isDone();
            for (auto& r : subRegions) {
                if (!r.monotonicityAnnotationsInitialized()) {
                    // Propagate order and local monotonicity.
                    // If they are done, they will not change anymore and ownership is shared among the subregions. If they are not done, we copy them.
                    // However, if the data is not used anywhere else, we might transfer ownership from parent to (first) child instead to avoid one unnecessary
                    // copy.
                    if (allowDeleteAnnotationsOfThis && parentAnnotation.stateOrder.use_count() == 1 &&
                        parentAnnotation.localMonotonicityResult.use_count() == 1) {
                        // Transfer ownership to avoid one unnecessary copy
                        r.monotonicityAnnotation = parentAnnotation;
                    } else {
                        OrderBasedMonotonicityAnnotation newAnnotation;
                        if (stateOrderDone) {
                            // Share ownership of order and (potentially) local monotonicity
                            newAnnotation.stateOrder = parentAnnotation.stateOrder;
                            newAnnotation.localMonotonicityResult =
                                localMonotonicityResultDone ? parentAnnotation.localMonotonicityResult : parentAnnotation.localMonotonicityResult->copy();
                        } else {
                            // Copy both
                            newAnnotation.stateOrder = parentAnnotation.stateOrder->copy();  // TODO: Or copyOrder(order)?
                            newAnnotation.localMonotonicityResult = parentAnnotation.localMonotonicityResult->copy();
                        }
                        r.monotonicityAnnotation = std::move(newAnnotation);
                    }
                }
            }
        }
    }
};
}  // namespace storm::modelchecker::detail