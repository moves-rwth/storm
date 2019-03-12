#include "storm/modelchecker/prctl/helper/rewardbounded/CostLimitClosure.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/IllegalArgumentException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            namespace rewardbounded {
                CostLimit::CostLimit(uint64_t const &costLimit) : value(costLimit) {
                    // Intentionally left empty
                }

                bool CostLimit::isInfinity() const {
                    return value == std::numeric_limits<uint64_t>::max();
                }

                uint64_t const& CostLimit::get() const {
                    STORM_LOG_ASSERT(!isInfinity(), "Tried to get an infinite cost limit as int.");
                    return value;
                }

                uint64_t& CostLimit::get() {
                    STORM_LOG_ASSERT(!isInfinity(), "Tried to get an infinite cost limit as int.");
                    return value;
                }

                bool CostLimit::operator<(CostLimit const& other) const {
                    // Since infinity is represented by the max integer, we can compare this way.
                    return value < other.value;
                }

                bool CostLimit::operator==(CostLimit const& other) const {
                    return value == other.value;
                }

                CostLimit CostLimit::infinity() {
                    return CostLimit(std::numeric_limits<uint64_t>::max());
                }

                bool CostLimitClosure::CostLimitsCompare::operator()(storm::modelchecker::helper::rewardbounded::CostLimits const& lhs, storm::modelchecker::helper::rewardbounded::CostLimits const& rhs) const {
                    for (uint64_t i = 0; i < lhs.size(); ++i) {
                        if (lhs[i] < rhs[i]) {
                            return true;
                        } else if (rhs[i] < lhs[i]) {
                            return false;
                        }
                    }
                    return false;
                }

                CostLimitClosure::CostLimitClosure(storm::storage::BitVector const &downwardDimensions)
                        : downwardDimensions(downwardDimensions) {
                    // Intentionally left empty
                }

                bool CostLimitClosure::insert(CostLimits const& costLimits) {
                    // Iterate over all points in the generator and check whether they dominate the given point or vice versa
                    // TODO: make this more efficient by exploiting the order of the generator set.
                    std::vector<CostLimits> pointsToErase;
                    for (auto const& b : generator) {
                        if (dominates(b, costLimits)) {
                            // The given point is already contained in this closure.
                            // Since domination is transitive, this should not happen:
                            STORM_LOG_ASSERT(pointsToErase.empty(), "Inconsistent generator of CostLimitClosure.");
                            return false;
                        }
                        if (dominates(costLimits, b)) {
                            // b will be dominated by the new point so we erase it later.
                            // Note that b != newPoint holds if we reach this point
                            pointsToErase.push_back(b);
                        }
                    }
                    for (auto const& b : pointsToErase) {
                        generator.erase(b);
                    }
                    generator.insert(std::move(costLimits));
                    return true;
                }

                bool CostLimitClosure::contains(CostLimits const& costLimits) const {
                    // Iterate over all points in the generator and check whether they dominate the given point.
                    // TODO: make this more efficient by exploiting the order of the generator set.
                    for (auto const&b : generator) {
                        if (dominates(b,costLimits)) {
                            return true;
                        }
                    }
                    return false;
                }

                bool CostLimitClosure::containsUpwardClosure(CostLimits const& costLimits) const {
                    CostLimits infinityProjection(costLimits);
                    for (auto const& dim : downwardDimensions) {
                        infinityProjection[dim] = CostLimit::infinity();
                    }
                    return contains(infinityProjection);
                }

                bool CostLimitClosure::dominates(CostLimits const& lhs, CostLimits const& rhs) const {
                    for (uint64_t i = 0; i < lhs.size(); ++i) {
                        if (downwardDimensions.get(i)) {
                            if (lhs[i] < rhs[i]) {
                                return false;
                            }
                        } else {
                            if (rhs[i] < lhs[i]) {
                                return false;
                            }
                        }
                    }
                    return true;
                }

                std::vector<CostLimits> CostLimitClosure::getDominatingCostLimits(CostLimits const& costLimits) const {
                    std::vector<CostLimits> result;
                    for (auto const &b : generator) {
                        if (dominates(b, costLimits)) {
                            result.push_back(b);
                        }
                    }
                    return result;
                }

                typename CostLimitClosure::GeneratorType const &CostLimitClosure::getGenerator() const {
                    return generator;
                }

                uint64_t CostLimitClosure::dimension() const {
                    return downwardDimensions.size();
                }
            }
        }
    }
}