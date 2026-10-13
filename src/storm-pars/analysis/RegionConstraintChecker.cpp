#include "storm-pars/analysis/RegionConstraintChecker.h"

#include <optional>
#include <utility>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/utility/constants.h"

namespace storm::pars::analysis {

namespace {

using RelationType = storm::expressions::RelationType;
using BaseExpression = storm::expressions::BaseExpression;
using BinaryRelationExpression = storm::expressions::BinaryRelationExpression;
using BinaryNumericalFunctionExpression = storm::expressions::BinaryNumericalFunctionExpression;
using UnaryNumericalFunctionExpression = storm::expressions::UnaryNumericalFunctionExpression;

// Adds to a coefficient and drops it if it vanishes, so that e.g. `x - x` does not mention x.
void addCoefficient(std::map<std::string, CoefficientType>& coefficients, std::string const& variable, CoefficientType const& value) {
    if (storm::utility::isZero(value)) {
        return;
    }
    auto it = coefficients.find(variable);
    if (it == coefficients.end()) {
        coefficients.emplace(variable, value);
    } else {
        it->second = it->second + value;
        if (storm::utility::isZero(it->second)) {
            coefficients.erase(it);
        }
    }
}

// An exact affine form `constantPart + sum_i coefficient_i x_i`, keyed by variable name.
struct AffineForm {
    explicit AffineForm(CoefficientType const& constantPart = storm::utility::zero<CoefficientType>()) : constantPart(constantPart) {}

    CoefficientType constantPart;
    std::map<std::string, CoefficientType> coefficients;

    bool isConstant() const {
        return this->coefficients.empty();
    }

    void addScaled(AffineForm const& other, CoefficientType const& scale) {
        this->constantPart = this->constantPart + scale * other.constantPart;
        for (auto const& [variable, coefficient] : other.coefficients) {
            addCoefficient(this->coefficients, variable, scale * coefficient);
        }
    }

    void negate() {
        this->constantPart = -this->constantPart;
        for (auto& [variable, coefficient] : this->coefficients) {
            coefficient = -coefficient;
        }
    }
};

// Computes the exact affine form of an expression, or `std::nullopt` if it is not affine.
class AffineFormVisitor : public storm::expressions::ExpressionVisitor {
   public:
    std::optional<AffineForm> getAffineForm(BaseExpression const& expression) {
        return boost::any_cast<std::optional<AffineForm>>(expression.accept(*this, boost::none));
    }

    boost::any visit(storm::expressions::IfThenElseExpression const&, boost::any const&) override {
        return std::optional<AffineForm>{};
    }

    boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const&, boost::any const&) override {
        return std::optional<AffineForm>{};
    }

    boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const&, boost::any const&) override {
        return std::optional<AffineForm>{};
    }

    boost::any visit(storm::expressions::BooleanLiteralExpression const&, boost::any const&) override {
        return std::optional<AffineForm>{};
    }

    boost::any visit(storm::expressions::BinaryRelationExpression const&, boost::any const&) override {
        return std::optional<AffineForm>{};
    }

    boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const&) override {
        if (!expression.getType().isNumericalType()) {
            return std::optional<AffineForm>{};
        }
        AffineForm form;
        addCoefficient(form.coefficients, expression.getVariableName(), storm::utility::one<CoefficientType>());
        return std::optional<AffineForm>{form};
    }

    boost::any visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const&) override {
        return std::optional<AffineForm>{AffineForm(storm::utility::convertNumber<CoefficientType>(expression.getValue()))};
    }

    boost::any visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const&) override {
        return std::optional<AffineForm>{AffineForm(storm::utility::convertNumber<CoefficientType>(expression.getValue()))};
    }

    boost::any visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) override {
        auto operand = boost::any_cast<std::optional<AffineForm>>(expression.getOperand()->accept(*this, data));
        if (!operand.has_value()) {
            return std::optional<AffineForm>{};
        }
        if (expression.getOperatorType() != UnaryNumericalFunctionExpression::OperatorType::Minus) {
            return std::optional<AffineForm>{};
        }
        operand->negate();
        return operand;
    }

    boost::any visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) override {
        auto left = boost::any_cast<std::optional<AffineForm>>(expression.getFirstOperand()->accept(*this, data));
        if (!left.has_value()) {
            return std::optional<AffineForm>{};
        }
        auto right = boost::any_cast<std::optional<AffineForm>>(expression.getSecondOperand()->accept(*this, data));
        if (!right.has_value()) {
            return std::optional<AffineForm>{};
        }

        using OperatorType = BinaryNumericalFunctionExpression::OperatorType;
        OperatorType const operatorType = expression.getOperatorType();
        CoefficientType const one = storm::utility::one<CoefficientType>();

        if (operatorType == OperatorType::Plus) {
            left->addScaled(*right, one);
        } else if (operatorType == OperatorType::Minus) {
            right->negate();
            left->addScaled(*right, one);
        } else if (operatorType == OperatorType::Times) {
            AffineForm const* variableForm = nullptr;
            CoefficientType scale = one;
            if (left->isConstant()) {
                scale = left->constantPart;
                variableForm = &right.value();
            } else if (right->isConstant()) {
                scale = right->constantPart;
                variableForm = &left.value();
            }
            if (variableForm == nullptr) {
                return std::optional<AffineForm>{};
            }
            AffineForm result;
            result.addScaled(*variableForm, scale);
            left = std::move(result);
        } else if (operatorType == OperatorType::Divide) {
            if (!right->isConstant() || storm::utility::isZero(right->constantPart)) {
                return std::optional<AffineForm>{};
            }
            AffineForm result;
            result.addScaled(*left, one / right->constantPart);
            left = std::move(result);
        } else if (operatorType == OperatorType::Power) {
            if (!right->isConstant()) {
                return std::optional<AffineForm>{};
            }
            if (storm::utility::isZero(right->constantPart)) {
                left = AffineForm(one);
            } else if (right->constantPart != one) {
                return std::optional<AffineForm>{};
            }
        } else {
            return std::optional<AffineForm>{};
        }

        return left;
    }
};

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
Extremes computeExtremes(AffineForm const& form, BoundariesByName const& boundaries) {
    CoefficientType minimum = form.constantPart;
    CoefficientType maximum = form.constantPart;
    RegionWitness minimizingPoint;
    RegionWitness maximizingPoint;

    for (auto const& [variable, boundary] : boundaries) {
        CoefficientType const& lower = boundary.first;
        CoefficientType const& upper = boundary.second;
        auto it = form.coefficients.find(variable);
        if (it == form.coefficients.end()) {
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

// Applies the relation to two exact values.
bool applyRelation(RelationType relation, CoefficientType const& lhs, CoefficientType const& rhs) {
    switch (relation) {
        case RelationType::Equal:
            return lhs == rhs;
        case RelationType::NotEqual:
            return lhs != rhs;
        case RelationType::Less:
            return lhs < rhs;
        case RelationType::LessOrEqual:
            return lhs <= rhs;
        case RelationType::Greater:
            return lhs > rhs;
        case RelationType::GreaterOrEqual:
            return lhs >= rhs;
        default:
            return false;
    }
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

// Exactly evaluates a numerical expression at a point; `std::nullopt` if it uses unsupported constructs.
std::optional<CoefficientType> evaluateNumeric(BaseExpression const& expression, RegionWitness const& point) {
    if (expression.isIntegerLiteralExpression()) {
        return storm::utility::convertNumber<CoefficientType>(expression.asIntegerLiteralExpression().getValue());
    }
    if (expression.isRationalLiteralExpression()) {
        return storm::utility::convertNumber<CoefficientType>(expression.asRationalLiteralExpression().getValue());
    }
    if (expression.isVariableExpression()) {
        auto const& variableExpression = expression.asVariableExpression();
        auto it = point.find(variableExpression.getVariableName());
        if (it == point.end()) {
            return std::nullopt;
        }
        return it->second;
    }
    if (expression.isUnaryNumericalFunctionExpression()) {
        auto const& unaryExpression = expression.asUnaryNumericalFunctionExpression();
        if (unaryExpression.getOperatorType() != UnaryNumericalFunctionExpression::OperatorType::Minus) {
            return std::nullopt;
        }
        auto operand = evaluateNumeric(*unaryExpression.getOperand(), point);
        if (!operand.has_value()) {
            return std::nullopt;
        }
        return -*operand;
    }
    if (expression.isBinaryNumericalFunctionExpression()) {
        auto const& binaryExpression = expression.asBinaryNumericalFunctionExpression();
        auto left = evaluateNumeric(*binaryExpression.getFirstOperand(), point);
        if (!left.has_value()) {
            return std::nullopt;
        }
        auto right = evaluateNumeric(*binaryExpression.getSecondOperand(), point);
        if (!right.has_value()) {
            return std::nullopt;
        }
        using OperatorType = BinaryNumericalFunctionExpression::OperatorType;
        switch (binaryExpression.getOperatorType()) {
            case OperatorType::Plus:
                return *left + *right;
            case OperatorType::Minus:
                return *left - *right;
            case OperatorType::Times:
                return *left * *right;
            case OperatorType::Divide:
                if (storm::utility::isZero(*right)) {
                    return std::nullopt;
                }
                return *left / *right;
            default:
                return std::nullopt;
        }
    }
    return std::nullopt;
}

// Probing can only find violations, so the verdict is Unknown if no probe fails.
SingleVerdict probeConstraint(BinaryRelationExpression const& relationExpression, std::vector<RegionWitness> const& probePoints) {
    RelationType const relation = relationExpression.getRelationType();
    BaseExpression const& lhs = *relationExpression.getFirstOperand();
    BaseExpression const& rhs = *relationExpression.getSecondOperand();

    for (auto const& probePoint : probePoints) {
        auto left = evaluateNumeric(lhs, probePoint);
        auto right = evaluateNumeric(rhs, probePoint);
        if (left.has_value() && right.has_value() && !applyRelation(relation, *left, *right)) {
            return SingleVerdict{ConstraintVerdict::Unsat, probePoint};
        }
    }

    return SingleVerdict{};
}

// The lower corner, the upper corner, and the center of the region.
std::vector<RegionWitness> getProbePoints(BoundariesByName const& boundaries) {
    RegionWitness lowerCorner;
    RegionWitness upperCorner;
    RegionWitness center;
    for (auto const& [variable, boundary] : boundaries) {
        lowerCorner.emplace(variable, boundary.first);
        upperCorner.emplace(variable, boundary.second);
        center.emplace(variable, (boundary.first + boundary.second) / 2);
    }
    return {std::move(lowerCorner), std::move(upperCorner), std::move(center)};
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
    std::vector<RegionWitness> const probePoints = getProbePoints(boundaries);
    AffineFormVisitor affineFormVisitor;
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
                singleVerdict = SingleVerdict{ConstraintVerdict::Unsat, probePoints.back()};
            }
        } else if (baseExpression.isBinaryRelationExpression()) {
            BinaryRelationExpression const& relationExpression = baseExpression.asBinaryRelationExpression();
            auto lhs = affineFormVisitor.getAffineForm(*relationExpression.getFirstOperand());
            auto rhs = affineFormVisitor.getAffineForm(*relationExpression.getSecondOperand());

            if (lhs.has_value() && rhs.has_value()) {
                // Build the form of `lhs - rhs`, which the relation is compared against zero.
                AffineForm form = std::move(lhs.value());
                rhs->negate();
                form.addScaled(*rhs, storm::utility::one<CoefficientType>());

                for (auto const& [variable, coefficient] : form.coefficients) {
                    STORM_LOG_THROW(boundaries.count(variable) != 0, storm::exceptions::InvalidArgumentException,
                                    "Cannot check " << label << " constraint '" << constraint.toString()
                                                    << "' over the given region, because it constrains the variable '" << variable
                                                    << "', which is not part of the region. Please provide a region covering all constrained variables.");
                }

                Extremes const extremes = computeExtremes(form, boundaries);
                singleVerdict = checkAffineConstraint(relationExpression.getRelationType(), extremes);
            } else {
                singleVerdict = probeConstraint(relationExpression, probePoints);
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
