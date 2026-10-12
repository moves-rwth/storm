#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "storm-pars/storage/ParameterRegion.h"
#include "storm/analysis/GraphConditions.h"

namespace storm {
namespace pars {
namespace analysis {

/*! The type of region this checker operates on. */
using RationalFunctionRegion = storm::storage::ParameterRegion<storm::RationalFunction>;

/*! The exact rational type used for the bounds of the regions checked by this checker. */
using CoefficientType = RationalFunctionRegion::CoefficientType;

/*!
 * The outcome of checking a single constraint over an entire parameter region.
 */
enum class ConstraintVerdict {
    /*! The constraint was proven to hold for every point of the region. */
    Satisfied,
    /*! The constraint was proven not to hold for every point of the region. A witness is available. */
    Violated,
    /*! Neither could be established cheaply, e.g. because the constraint is non-linear. */
    Undecided
};

/*!
 * A point of a parameter region, given as exact rational values. The map is keyed by variable name.
 */
using RegionWitness = std::map<std::string, CoefficientType>;

/*!
 * The aggregated outcome of checking a whole constraint set over a parameter region.
 */
struct RegionCheckResult {
    /*! The aggregated verdict over all checked constraints. */
    ConstraintVerdict verdict = ConstraintVerdict::Undecided;

    /*! A point of the region at which a violated constraint fails. Only set if the verdict is Violated. */
    std::optional<RegionWitness> witness;

    /*! The printed form of a constraint that does not hold throughout the region. */
    std::string violatedConstraint;

    /*! The printed forms of all constraints that could not be decided. */
    std::vector<std::string> undecidedConstraints;

    /*! The number of constraints proven to hold throughout the region. */
    size_t numSatisfied = 0;

    /*! The number of constraints proven not to hold throughout the region. */
    size_t numViolated = 0;

    /*! The number of constraints that could not be decided. */
    size_t numUndecided = 0;

    /*!
     * Returns whether every checked constraint was proven to hold throughout the region.
     */
    bool holdsThroughout() const {
        return this->verdict == ConstraintVerdict::Satisfied;
    }
};

/*!
 * Checks whether the constraints collected for a parametric model hold over an entire parameter region.
 *
 * Constraints are given in the form `f(x) <relation> 0` and are decomposed into an exact affine form
 * `f(x) = c + sum_i a_i x_i`. For a rectangular region, the extrema of an affine function are attained at
 * the vertices of the box, so the exact minimum and maximum of `f` over the region are computed in
 * linear time. The relation can then be decided for the whole region in O(#variables) time without any
 * sampling, SMT solving, or explicit enumeration of the 2^(#variables) vertices.
 *
 * Non-linear constraints are never claimed to hold. They are probed at a few points of the region, which
 * is sound for detecting violations, and otherwise reported as undecided.
 */
class RegionConstraintChecker {
   public:
    /*!
     * Creates a checker for the constraints collected by the given collector.
     *
     * @param collector The collector holding the well-formedness and graph-preserving constraints.
     */
    explicit RegionConstraintChecker(storm::analysis::ConstraintCollector const& collector);

    /*!
     * Checks whether all well-formedness constraints hold throughout the given region.
     */
    RegionCheckResult checkWellformedness(RationalFunctionRegion const& region) const;

    /*!
     * Checks whether all graph-preserving constraints hold throughout the given region.
     */
    RegionCheckResult checkGraphPreservation(RationalFunctionRegion const& region) const;

    /*!
     * Returns whether the model is well-defined on the whole region, that is, all well-formedness
     * constraints were proven to hold. Returns false if any constraint is violated or undecided.
     */
    bool isWellDefined(RationalFunctionRegion const& region) const;

    /*!
     * Returns whether the model preserves its graph on the whole region, that is, all graph-preserving
     * constraints were proven to hold. Returns false if any constraint is violated or undecided.
     */
    bool isGraphPreserving(RationalFunctionRegion const& region) const;

    /*!
     * Checks an arbitrary constraint set over a region.
     *
     * @param constraints The constraints to check, each given as an expression that is compared to zero.
     * @param region The region to check the constraints over.
     * @param label A description used in exception messages.
     */
    static RegionCheckResult checkConstraints(storm::analysis::ConstraintCollector::ConstraintSet const& constraints, RationalFunctionRegion const& region,
                                              std::string const& label);

   private:
    storm::analysis::ConstraintCollector const& collector;
};

}  // namespace analysis
}  // namespace pars
}  // namespace storm
