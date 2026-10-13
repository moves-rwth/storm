#include "storm-pars/analysis/RegionConstraintChecker.h"

#include <optional>
#include <utility>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/LinearCoefficientVisitor.h"
#include "storm/utility/constants.h"

namespace storm::pars::analysis {

namespace {

using RelationType = storm::expressions::RelationType;
using BaseExpression = storm::expressions::BaseExpression;
using BinaryRelationExpression = storm::expressions::BinaryRelationExpression;
using BinaryNumericalFunctionExpression = storm::expressions::BinaryNumericalFunctionExpression;
using UnaryNumericalFunctionExpression = storm::expressions::UnaryNumericalFunctionExpression;

// The (lower, upper) boundary of every variable of a region, keyed by variable name.
using BoundariesByName = std::map<std::string, std::pair<CoefficientType, CoefficientType>>;

BoundariesByName getBoundariesByName(RationalFunctionRegion const& region) {
    BoundariesByName boundaries;
    for (auto const& [variable, lowerBoundary] : region.getLowerBoundaries()) {
        boundaries.emplace(variable.name(), std::make_pair(lowerBoundary, region.getUpperBoundary(variable)));
    }
    return boundaries;
}

// The exact extrema of an affine form over a region, with the points attaining them.
struct Extremes {
    CoefficientType minimum;
    CoefficientType maximum;
    RegionWitness minimizingPoint;
    RegionWitness maximizingPoint;
};

// Every variable can be optimized independently because the region is rectangular.
Extremes computeExtremes(CoefficientType const& constantPart, std::map<std::string, CoefficientType> const& coefficients,
                         BoundariesByName const& boundaries) {
    CoefficientType minimum = constantPart;
    CoefficientType maximum = constantPart;
    RegionWitness minimizingPoint;
    RegionWitness maximizingPoint;

    for (auto const& [variable, boundary] : boundaries) {
        CoefficientType const& lower = boundary.first;
        CoefficientType const& upper = boundary.second;
        auto it = coefficients.find(variable);
        if (it == coefficients.end()) {
            // The variable does not influence the form, so it can be fixed arbitrarily.
            minimizingPoint.emplace(variable, lower);
            maximizingPoint.emplace(variable, lower);
            continue;
        }
        CoefficientType const& coefficient = it->second;
        CoefficientType const atLower = coefficient * lower;
        CoefficientType const atUpper = coefficient * upper;
        if (atLower <= atUpper) {
            minimum += atLower;
            maximum += atUpper;
            minimizingPoint.emplace(variable, lower);
            maximizingPoint.emplace(variable, upper);
        } else {
            minimum += atUpper;
            maximum += atLower;
            minimizingPoint.emplace(variable, upper);
            maximizingPoint.emplace(variable, lower);
        }
    }

    return Extremes{minimum, maximum, std::move(minimizingPoint), std::move(maximizingPoint)};
}

// The outcome of checking a single constraint.
struct SingleVerdict {
    ConstraintVerdict verdict = ConstraintVerdict::Unknown;
    std::optional<RegionWitness> witness;
};

// Decides `f <relation> 0` from the extrema of the affine form f; constructs a witness if it fails.
SingleVerdict checkAffineConstraint(RelationType relation, Extremes const& extremes) {
    CoefficientType const zero = storm::utility::zero<CoefficientType>();

    bool satisfiedThroughout;
    switch (relation) {
        case RelationType::Less:
            satisfiedThroughout = extremes.maximum < zero;
            break;
        case RelationType::LessOrEqual:
            satisfiedThroughout = extremes.maximum <= zero;
            break;
        case RelationType::Greater:
            satisfiedThroughout = extremes.minimum > zero;
            break;
        case RelationType::GreaterOrEqual:
            satisfiedThroughout = extremes.minimum >= zero;
            break;
        case RelationType::Equal:
            satisfiedThroughout = extremes.minimum >= zero && extremes.maximum <= zero;
            break;
        case RelationType::NotEqual:
            satisfiedThroughout = extremes.maximum < zero || extremes.minimum > zero;
            break;
        default:
            return SingleVerdict{};
    }

    if (satisfiedThroughout) {
        return SingleVerdict{ConstraintVerdict::Sat, std::nullopt};
    }

    switch (relation) {
        case RelationType::Less:
        case RelationType::LessOrEqual:
            return SingleVerdict{ConstraintVerdict::Unsat, extremes.maximizingPoint};
        case RelationType::Greater:
        case RelationType::GreaterOrEqual:
            return SingleVerdict{ConstraintVerdict::Unsat, extremes.minimizingPoint};
        case RelationType::Equal:
            // The form is not identically zero, so one of the two extremizing points witnesses the violation.
            if (extremes.maximum != zero) {
                return SingleVerdict{ConstraintVerdict::Unsat, extremes.maximizingPoint};
            }
            return SingleVerdict{ConstraintVerdict::Unsat, extremes.minimizingPoint};
        case RelationType::NotEqual: {
            if (extremes.minimum == zero) {
                return SingleVerdict{ConstraintVerdict::Unsat, extremes.minimizingPoint};
            }
            if (extremes.maximum == zero) {
                return SingleVerdict{ConstraintVerdict::Unsat, extremes.maximizingPoint};
            }
            // minimum < 0 < maximum: f is zero at the interpolated point for t = minimum / (minimum - maximum).
            CoefficientType const t = extremes.minimum / (extremes.minimum - extremes.maximum);
            RegionWitness witness;
            for (auto const& [variable, value] : extremes.minimizingPoint) {
                CoefficientType const other = extremes.maximizingPoint.at(variable);
                witness.emplace(variable, value + t * (other - value));
            }
            return SingleVerdict{ConstraintVerdict::Unsat, std::move(witness)};
        }
        default:
            return SingleVerdict{};
    }
}

// The center of the region.
RegionWitness getCenter(BoundariesByName const& boundaries) {
    RegionWitness center;
    for (auto const& [variable, boundary] : boundaries) {
        center.emplace(variable, (boundary.first + boundary.second) / 2);
    }
    return center;
}

}  // namespace

std::ostream& operator<<(std::ostream& out, RegionWitness const& witness) {
    bool first = true;
    for (auto const& [variable, value] : witness) {
        out << (first ? "" : ", ") << variable << " = " << value;
        first = false;
    }
    return out;
}

RegionConstraintChecker::RegionConstraintChecker(storm::analysis::ConstraintCollector const& collector) : collector(collector) {}

RegionCheckResult RegionConstraintChecker::checkWellformedness(RationalFunctionRegion const& region) const {
    return checkConstraints(this->collector.getWellformedConstraints(), region, "well-formedness");
}

RegionCheckResult RegionConstraintChecker::checkGraphPreservation(RationalFunctionRegion const& region) const {
    return checkConstraints(this->collector.getGraphPreservingConstraints(), region, "graph-preserving");
}

bool RegionConstraintChecker::isWellDefined(RationalFunctionRegion const& region) const {
    return checkWellformedness(region).holdsThroughout();
}

bool RegionConstraintChecker::isGraphPreserving(RationalFunctionRegion const& region) const {
    return checkGraphPreservation(region).holdsThroughout();
}

RegionCheckResult RegionConstraintChecker::checkConstraints(storm::analysis::ConstraintCollector::ConstraintSet const& constraints,
                                                            RationalFunctionRegion const& region, std::string const& label) {
    RegionCheckResult result;
    BoundariesByName const boundaries = getBoundariesByName(region);
    storm::expressions::LinearCoefficientVisitor<CoefficientType> coefficientVisitor;
    bool violated = false;
    bool undecided = false;

    for (storm::expressions::Expression const& constraint : constraints) {
        BaseExpression const& baseExpression = constraint.getBaseExpression();

        SingleVerdict singleVerdict;
        if (baseExpression.isBooleanLiteralExpression()) {
            // The collector emits `false` for rejected constant entries, e.g. a fixed probability outside [0, 1].
            if (baseExpression.asBooleanLiteralExpression().getValue()) {
                singleVerdict = SingleVerdict{ConstraintVerdict::Sat, std::nullopt};
            } else {
                singleVerdict = SingleVerdict{ConstraintVerdict::Unsat, getCenter(boundaries)};
            }
        } else if (baseExpression.isBinaryRelationExpression()) {
            BinaryRelationExpression const& relationExpression = baseExpression.asBinaryRelationExpression();
            auto form = coefficientVisitor.tryGetLinearCoefficients(constraint.getOperand(0));
            auto rhs = coefficientVisitor.tryGetLinearCoefficients(constraint.getOperand(1));

            if (form.has_value() && rhs.has_value()) {
                // The relation is compared against zero for the form of `lhs - rhs`.
                *form -= std::move(*rhs);

                std::map<std::string, CoefficientType> coefficients;
                for (auto const& [variable, coefficient] : *form) {
                    STORM_LOG_THROW(boundaries.count(variable.getName()) != 0, storm::exceptions::InvalidArgumentException,
                                    "Cannot check " << label << " constraint '" << constraint.toString()
                                                    << "' over the given region, because it constrains the variable '" << variable.getName()
                                                    << "', which is not part of the region. Please provide a region covering all constrained variables.");
                    coefficients.emplace(variable.getName(), coefficient);
                }

                Extremes const extremes = computeExtremes(form->getConstantPart(), coefficients, boundaries);
                singleVerdict = checkAffineConstraint(relationExpression.getRelationType(), extremes);
            }
        }

        switch (singleVerdict.verdict) {
            case ConstraintVerdict::Sat:
                break;
            case ConstraintVerdict::Unsat:
                violated = true;
                if (!result.violatedConstraint.has_value()) {
                    result.witness = singleVerdict.witness;
                    result.violatedConstraint = constraint;
                }
                break;
            case ConstraintVerdict::Unknown:
                undecided = true;
                result.undecidedConstraints.push_back(constraint);
                break;
        }
    }

    if (violated) {
        result.verdict = ConstraintVerdict::Unsat;
    } else if (undecided) {
        result.verdict = ConstraintVerdict::Unknown;
    } else {
        result.verdict = ConstraintVerdict::Sat;
    }

    return result;
}

}  // namespace storm::pars::analysis
