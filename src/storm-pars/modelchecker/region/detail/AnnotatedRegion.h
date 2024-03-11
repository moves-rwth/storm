#pragma once

#include <memory>

#include "storm-pars/analysis/LocalMonotonicityResult.h"
#include "storm-pars/analysis/Order.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm::modelchecker::detail {
template<typename ParametricType>
struct AnnotatedRegion {
    using Region = storm::storage::ParameterRegion<ParametricType>;
    using VariableType = typename Region::VariableType;

    AnnotatedRegion(Region const& region) : region(region) {
        // Intentionally left empty
    }

    void propagateAnnotationsToSubregions(std::vector<AnnotatedRegion<ParametricType>>& subRegions, bool allowDeleteAnnotationsOfThis) {
        bool const stateOrderDone = stateOrder && stateOrder->getDoneBuilding();
        bool const localMonotonicityResultDone = stateOrderDone && localMonotonicityResult && localMonotonicityResult->isDone();

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
            r.resultKnownThroughMonotonicity |= resultKnownThroughMonotonicity;

            if (stateOrder && !r.stateOrder) {
                STORM_LOG_ASSERT(localMonotonicityResult && !r.localMonotonicityResult, "Local monotonicity result must be set iff state order is set");
                // Propagate order and local monotonicity.
                // If they are done, they will not change anymore and ownership is shared among the subregions. If they are not done, we copy them.
                // However, if the data is not used anywhere else, we might move ownership instead to avoid one unnecessary copy.
                if (allowDeleteAnnotationsOfThis && stateOrder.use_count() == 1 && localMonotonicityResult.use_count() == 1) {
                    // Transfer ownership to avoid one unnecessary copy
                    r.stateOrder = stateOrder;
                    r.localMonotonicityResult = localMonotonicityResult;
                } else {
                    if (stateOrderDone) {
                        // Share ownership of order and (potentially) local monotonicity
                        r.stateOrder = stateOrder;
                        r.localMonotonicityResult = localMonotonicityResultDone ? localMonotonicityResult : localMonotonicityResult->copy();
                    } else {
                        // Copy both
                        r.stateOrder = stateOrder->copy();  // TODO: Or copyOrder(order)?
                        r.localMonotonicityResult = localMonotonicityResult->copy();
                    }
                }
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

    std::shared_ptr<storm::analysis::Order> stateOrder{nullptr};
    std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult{nullptr};

    std::optional<typename Region::CoefficientType> knownLowerValueBound;  // Lower bound on the value of the region
    std::optional<typename Region::CoefficientType> knownUpperValueBound;  // Upper bound on the value of the region
};
}  // namespace storm::modelchecker::detail