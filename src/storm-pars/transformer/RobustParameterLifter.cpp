#include "storm-pars/transformer/RobustParameterLifter.h"
#include <_types/_uint64_t.h>
#include <cmath>
#include <memory>
#include <optional>
#include <set>
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"

#include "environment/Environment.h"
#include "settings/SettingsManager.h"
#include "settings/modules/GeneralSettings.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/vector.h"
#include "utility/constants.h"
#include "utility/logging.h"
#include "utility/macros.h"

namespace storm {
namespace transformer {

typedef storm::utility::parametric::CoefficientType<RationalFunction>::type CoefficientType;

template<typename ParametricType, typename ConstantType>
RobustParameterLifter<ParametricType, ConstantType>::RobustParameterLifter(storm::storage::SparseMatrix<ParametricType> const& pMatrix,
                                                                           std::vector<ParametricType> const& pVector,
                                                                           storm::storage::BitVector const& selectedRows,
                                                                           storm::storage::BitVector const& selectedColumns, bool generateRowLabels) {
    oldToNewColumnIndexMapping = std::vector<uint64_t>(selectedColumns.size(), selectedColumns.size());
    uint64_t newIndexColumns = 0;
    for (auto const& oldColumn : selectedColumns) {
        oldToNewColumnIndexMapping[oldColumn] = newIndexColumns++;
    }

    oldToNewRowIndexMapping = std::vector<uint64_t>(selectedRows.size(), selectedRows.size());
    uint64_t newIndexRows = 0;
    for (auto const& oldRow : selectedRows) {
        oldToNewRowIndexMapping[oldRow] = newIndexRows++;
    }

    // Stores which entries of the original matrix/vector are non-constant. Entries for non-selected rows/columns are omitted
    auto nonConstMatrixEntries = storm::storage::BitVector(pMatrix.getEntryCount(), false);  // this vector has to be resized later
    auto nonConstVectorEntries = storm::storage::BitVector(selectedRows.getNumberOfSetBits(), false);
    // Counters for selected entries in the pMatrix and the pVector
    uint64_t pMatrixEntryCount = 0;
    uint64_t pVectorEntryCount = 0;

    // The matrix builder for the new matrix. The correct number of rows and entries is not known yet.
    storm::storage::SparseMatrixBuilder<Interval> builder(newIndexRows, newIndexColumns, 0, true, false);

    for (uint64_t row = 0; row < pMatrix.getRowCount(); row++) {
        if (!selectedRows.get(row)) {
            continue;
        }
        for (auto const& entry : pMatrix.getRow(row)) {
            auto column = entry.getColumn();
            if (!selectedColumns.get(column)) {
                continue;
            }
            auto transition = entry.getValue();
            if (storm::utility::isConstant(transition)) {
                builder.addNextValue(oldToNewColumnIndexMapping[row], oldToNewColumnIndexMapping[column], utility::convertNumber<double>(transition));
            } else {
                nonConstMatrixEntries.set(pMatrixEntryCount, true);
                auto valuation = RobustAbstractValuation(transition);
                builder.addNextValue(oldToNewColumnIndexMapping[row], oldToNewColumnIndexMapping[column], Interval());
                Interval& placeholder = functionValuationCollector.add(valuation);
                matrixAssignment.push_back(std::pair<typename storm::storage::SparseMatrix<Interval>::iterator, Interval&>(
                    typename storm::storage::SparseMatrix<Interval>::iterator(), placeholder));
            }
            pMatrixEntryCount++;
        }
    }

    if (generateRowLabels) {
        for (uint64_t i = 0; i < pVector.size(); i++) {
            auto const transition = pVector[i];
            if (!selectedRows.get(i)) {
                continue;
            }
            if (storm::utility::isConstant(transition)) {
                vector.push_back(utility::convertNumber<double>(transition));
            } else {
                nonConstVectorEntries.set(pVectorEntryCount, true);
                auto valuation = RobustAbstractValuation(transition);
                vector.push_back(Interval());
                Interval& placeholder = functionValuationCollector.add(valuation);
                vectorAssignment.push_back(
                    std::pair<typename std::vector<Interval>::iterator, Interval&>(typename std::vector<Interval>::iterator(), placeholder));
            }
            pVectorEntryCount++;
        }
    }

    matrix = builder.build();
    vector.shrink_to_fit();
    matrixAssignment.shrink_to_fit();
    vectorAssignment.shrink_to_fit();
    nonConstMatrixEntries.resize(pMatrixEntryCount);

    // Now insert the correct iterators for the matrix and vector assignment
    auto matrixAssignmentIt = matrixAssignment.begin();
    uint64_t startEntryOfRow = 0;
    for (uint64_t group = 0; group < matrix.getRowGroupCount(); ++group) {
        uint64_t startEntryOfNextRow = startEntryOfRow + matrix.getRow(group, 0).getNumberOfEntries();
        for (uint64_t matrixRow = matrix.getRowGroupIndices()[group]; matrixRow < matrix.getRowGroupIndices()[group + 1]; ++matrixRow) {
            auto matrixEntryIt = matrix.getRow(matrixRow).begin();
            for (uint64_t nonConstEntryIndex = nonConstMatrixEntries.getNextSetIndex(startEntryOfRow); nonConstEntryIndex < startEntryOfNextRow;
                 nonConstEntryIndex = nonConstMatrixEntries.getNextSetIndex(nonConstEntryIndex + 1)) {
                matrixAssignmentIt->first = matrixEntryIt + (nonConstEntryIndex - startEntryOfRow);
                ++matrixAssignmentIt;
            }
        }
        startEntryOfRow = startEntryOfNextRow;
    }
    STORM_LOG_ASSERT(matrixAssignmentIt == matrixAssignment.end(), "Unexpected number of entries in the matrix assignment.");

    auto vectorAssignmentIt = vectorAssignment.begin();
    for (auto const& nonConstVectorEntry : nonConstVectorEntries) {
        for (uint64_t vectorIndex = matrix.getRowGroupIndices()[nonConstVectorEntry]; vectorIndex != matrix.getRowGroupIndices()[nonConstVectorEntry + 1];
             ++vectorIndex) {
            vectorAssignmentIt->first = vector.begin() + vectorIndex;
            ++vectorAssignmentIt;
        }
    }
    STORM_LOG_ASSERT(vectorAssignmentIt == vectorAssignment.end(), "Unexpected number of entries in the vector assignment.");
}

template<typename ParametricType, typename ConstantType>
void RobustParameterLifter<ParametricType, ConstantType>::specifyRegion(storm::storage::ParameterRegion<ParametricType> const& region,
                                                                        storm::solver::OptimizationDirection const& dirForParameters) {
    // write the evaluation result of each function,evaluation pair into the placeholders
    this->currentRegionAllIllDefined = functionValuationCollector.evaluateCollectedFunctions(region, dirForParameters);

    // TODO Return if currentRegionAllIllDefined? Or write to matrix?

    // apply the matrix and vector assignments to write the contents of the placeholder into the matrix/vector
    for (auto& assignment : matrixAssignment) {
        STORM_LOG_WARN_COND(
            !storm::utility::isZero(assignment.second),
            "Parameter lifting on region "
                << region.toString()
                << " affects the underlying graph structure (the region is not strictly well defined). The result for this region might be incorrect.");
        assignment.first->setValue(assignment.second);
    }

    for (auto& assignment : vectorAssignment) {
        *assignment.first = assignment.second;
    }
}

template<typename ParametricType, typename ConstantType>
boost::optional<std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::pair<typename storm::utility::parametric::CoefficientType<ParametricType>::type,
                                                                             typename storm::utility::parametric::CoefficientType<ParametricType>::type>>>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::recursiveDecompose(RawPolynomial polynomial, VariableType parameter,
                                                                                                 bool firstIteration) {
    auto parameterPol = RawPolynomial(parameter);
    auto oneMinusParameter = RawPolynomial(1) - parameterPol;
    if (polynomial.isConstant()) {
        return std::make_pair(std::make_pair((uint64_t)0, (uint64_t)0),
                              std::make_pair(utility::convertNumber<CoefficientType>(polynomial.constantPart()), utility::zero<CoefficientType>()));
    }
    auto byOneMinusP = polynomial.divideBy(oneMinusParameter);
    if (byOneMinusP.remainder.isZero() && byOneMinusP.quotient > storm::utility::zero<CoefficientType>()) {
        auto recursiveResult = recursiveDecompose(byOneMinusP.quotient, parameter, false);
        if (recursiveResult) {
            return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1), recursiveResult->second);
        }
    }
    auto byP = polynomial.divideBy(parameterPol);
    if (byP.remainder.isZero() && byP.quotient > storm::utility::zero<CoefficientType>()) {
        auto recursiveResult = recursiveDecompose(byP.quotient, parameter, false);
        if (recursiveResult) {
            return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second), recursiveResult->second);
        }
    }
    if (firstIteration && byOneMinusP.remainder.isConstant() && byOneMinusP.quotient > storm::utility::zero<CoefficientType>()) {
        auto rem1 = utility::convertNumber<CoefficientType>(byOneMinusP.remainder.constantPart());
        auto recursiveResult = recursiveDecompose(byOneMinusP.quotient, parameter, false);
        if (recursiveResult) {
            STORM_LOG_ASSERT(recursiveResult->second.second == 0, "");
            return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1),
                                  std::pair<CoefficientType, CoefficientType>(recursiveResult->second.first, rem1));
        }
    }
    if (firstIteration && byP.remainder.isConstant() && byP.quotient > storm::utility::zero<CoefficientType>()) {
        auto rem2 = utility::convertNumber<CoefficientType>(byP.remainder.constantPart());
        auto recursiveResult = recursiveDecompose(byP.quotient, parameter, false);
        if (recursiveResult) {
            STORM_LOG_ASSERT(recursiveResult->second.second == 0, "");
            return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second),
                                  std::pair<CoefficientType, CoefficientType>(recursiveResult->second.first, rem2));
        }
    }
    return boost::none;
}

// template<typename ParametricType, typename ConstantType>
// boost::optional<std::pair<std::pair<uint64_t, uint64_t>, typename storm::utility::parametric::CoefficientType<ParametricType>::type>>
// RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::recursiveDecompose(RawPolynomial polynomial, VariableType parameter) {
//     auto parameterPol = RawPolynomial(parameter);
//     auto oneMinusParameter = RawPolynomial(1) - parameterPol;
//     if (polynomial.isConstant()) {
//         return std::make_pair(std::make_pair((uint64_t)0, (uint64_t)0), utility::convertNumber<CoefficientType>(polynomial.constantPart()));
//     }
//     auto byOneMinusP = polynomial.divideBy(oneMinusParameter);
//     if (byOneMinusP.remainder.isZero()) {
//         auto recursiveResult = recursiveDecompose(byOneMinusP.quotient, parameter);
//         if (recursiveResult) {
//             return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1), recursiveResult->second);
//         }
//     }
//     auto byP = polynomial.divideBy(parameterPol);
//     if (byP.remainder.isZero()) {
//         auto recursiveResult = recursiveDecompose(byP.quotient, parameter);
//         if (recursiveResult) {
//             return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second), recursiveResult->second);
//         }
//     }
//     return boost::none;
// }

template<typename ParametricType, typename ConstantType>
std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::cubicEquationZeroes(
    RawPolynomial polynomial, typename RobustParameterLifter<ParametricType, ConstantType>::VariableType parameter) {
    // Polynomial is a*p^3 + b*p^2 + c*p + d

    // Recover factors from polynomial
    CoefficientType a = utility::zero<CoefficientType>(), b = a, c = a, d = a;
    utility::convertNumber<ConstantType>(a);
    for (auto const& term : polynomial.getTerms()) {
        STORM_LOG_ASSERT(term.getNrVariables() <= 1, "No terms with more than one variable allowed but " << term << " has " << term.getNrVariables());
        if (!term.isConstant() && term.getSingleVariable() != parameter) {
            continue;
        }
        CoefficientType coefficient = term.coeff();
        STORM_LOG_ASSERT(term.tdeg() < 4, "Transitions are only allowed to have a maximum degree of four.");
        switch (term.tdeg()) {
            case 0:
                d = coefficient;
                break;
            case 1:
                c = coefficient;
                break;
            case 2:
                b = coefficient;
                break;
            case 3:
                a = coefficient;
                break;
        }
    }
    // Translated from https://stackoverflow.com/questions/27176423/function-to-solve-cubic-equation-analytically

    // Quadratic case
    if (utility::isZero(a)) {
        a = b;
        b = c;
        c = d;
        // Linear case
        if (utility::isZero(a)) {
            a = b;
            b = c;
            // Constant case
            if (utility::isZero(a)) {
                return {};
            }
            return {-b / a};
        }

        CoefficientType D = b * b - 4 * a * c;
        if (utility::isZero(D)) {
            return {-b / (2 * a)};
        } else if (D > 0) {
            return {(-b + utility::sqrt(D)) / (2 * a), (-b - utility::sqrt(D)) / (2 * a)};
        }
        return {};
    }
    std::set<CoefficientType> roots;

    // Convert to depressed cubic t^3+pt+q = 0 (subst x = t - b/3a)
    CoefficientType p = (3 * a * c - b * b) / (3 * a * a);
    CoefficientType q = (2 * b * b * b - 9 * a * b * c + 27 * a * a * d) / (27 * a * a * a);
    double pDouble = utility::convertNumber<ConstantType>(p);
    double qDouble = utility::convertNumber<ConstantType>(q);

    if (utility::isZero(p)) {  // p = 0 -> t^3 = -q -> t = -q^1/3
        roots = {CoefficientType(std::cbrt(-qDouble))};
    } else if (utility::isZero(q)) {  // q = 0 -> t^3 + pt = 0 -> t(t^2+p)=0
        roots = {0};
        if (p < 0) {
            roots.emplace(utility::sqrt(-pDouble));
            roots.emplace(-utility::sqrt(-pDouble));
        }
    } else {
        // These are all coefficients (we also plug the values into RationalFunctions later), i.e., they are rational numbers,
        // but some of these operations are strictly real, so we convert to double and back (i.e., approximate).
        CoefficientType D = q * q / 4 + p * p * p / 27;
        if (utility::isZero(D)) {  // D = 0 -> two roots
            roots = {-1.5 * q / p, 3 * q / p};
        } else if (D > 0) {  // Only one real root
            double Ddouble = utility::convertNumber<ConstantType>(D);
            CoefficientType u = utility::convertNumber<CoefficientType>(std::cbrt(-qDouble / 2 - utility::sqrt(Ddouble)));
            roots = {u - p / (3 * u)};
        } else {  // D < 0, three roots, but needs to use complex numbers/trigonometric solution
            double u = 2 * utility::sqrt(-pDouble / 3);
            double t = std::acos(3 * qDouble / pDouble / u) / 3;  // D < 0 implies p < 0 and acos argument in [-1..1]
            double k = 2 * M_PI / 3;

            roots = {utility::convertNumber<CoefficientType>(u * std::cos(t)), utility::convertNumber<CoefficientType>(u * std::cos(t - k)),
                     utility::convertNumber<CoefficientType>(u * std::cos(t - 2 * k))};
        }
    }
    return roots;
}

template<typename ParametricType, typename ConstantType>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::RobustAbstractValuation(RationalFunction transition) : transition(transition) {
    std::set<VariableType> occurringVariables;
    storm::utility::parametric::gatherOccurringVariables(transition, occurringVariables);

    STORM_LOG_ERROR_COND(transition.denominator().isConstant(), "Robust PLA only supports transitions with constant denominators.");

    transition.simplify();

    auto constantPart = transition.constantPart();

    CoefficientType denominator = transition.denominator().constantPart();

    auto nominator = RawPolynomial(transition.nominator());

    std::map<VariableType, RawPolynomial> termsPerParameter;

    for (auto const& p : occurringVariables) {
        STORM_LOG_ASSERT(transition.nominator().totalDegree() <= 4,
                         "Degree of all polynomials needs to be <=4 for robust PLA, but the degree of " << transition << " is larger");
        this->extrema[p] = {};
        auto const derivative = RawPolynomial(transition.derivative(p).nominator());
        // Compute zeros of derivative (= maxima/minima of function) and emplace those between 0 and 1 into the maxima set
        auto zeroes = cubicEquationZeroes(derivative, p);
        for (auto const& zero : zeroes) {
            if (zero >= utility::zero<CoefficientType>() && zero <= utility::one<CoefficientType>()) {
                this->extrema[p].emplace(zero);
            }
        }
        this->parameters.emplace(p);

        // auto resultForParameter = recursiveDecompose(term, p);

        // STORM_LOG_ASSERT(resultForParameter, "Polynomial cannot be decomposed: " << transition << std::endl);

        // uint64_t a = resultForParameter->first.first;
        // uint64_t b = resultForParameter->first.second;
        // CoefficientType constant = resultForParameter->second.first / denominator;

        // // Term is constant * p^a * (1-p)^b
        // this->aAndB.emplace(p, std::make_pair(a, b));
        // this->constants.emplace(p, utility::convertNumber<ConstantType>(constant));

        // auto cleanedTerm = term / constant;

        // // The maximum of the polynomial part lies at a / (a + b), so compute that
        // // It is corrected for constant and offset later, not now
        // CoefficientType maximumCoeff;
        // if (a != 0 || b != 0) {
        //     maximumCoeff = utility::convertNumber<CoefficientType>(a) / utility::convertNumber<CoefficientType>(a + b);
        // } else {
        //     maximumCoeff = utility::zero<CoefficientType>();
        // }
        // ConstantType maximum = utility::convertNumber<ConstantType>(maximumCoeff);

        // std::map<VariableType, CoefficientType> substitution;
        // substitution.emplace(p, maximumCoeff);
        // this->maximum.emplace(p, std::make_pair(maximum, utility::convertNumber<ConstantType>(cleanedTerm.evaluate(substitution))));
    }
}

template<typename ParametricType, typename ConstantType>
storm::storage::SparseMatrix<Interval> const& RobustParameterLifter<ParametricType, ConstantType>::getMatrix() const {
    return matrix;
}

template<typename ParametricType, typename ConstantType>
std::vector<Interval> const& RobustParameterLifter<ParametricType, ConstantType>::getVector() const {
    return vector;
}

template<typename ParametricType, typename ConstantType>
bool RobustParameterLifter<ParametricType, ConstantType>::isCurrentRegionAllIllDefined() const {
    return currentRegionAllIllDefined;
}

template<typename ParametricType, typename ConstantType>
bool RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::operator==(RobustAbstractValuation const& other) const {
    return this->transition == other.transition;
}

template<typename ParametricType, typename ConstantType>
std::set<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getParameters() const {
    return parameters;
}

template<typename ParametricType, typename ConstantType>
RationalFunction const& RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getTransition() const {
    return this->transition;
}

template<typename ParametricType, typename ConstantType>
std::map<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType,
         std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getExtrema() const {
    return this->extrema;
}

template<typename ParametricType, typename ConstantType>
std::size_t RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getHashValue() const {
    std::size_t seed = 0;
    carl::hash_add(seed, transition);
    return seed;
}

template<typename ParametricType, typename ConstantType>
Interval& RobustParameterLifter<ParametricType, ConstantType>::FunctionValuationCollector::add(RobustAbstractValuation const& valuation) {
    // insert the function and the valuation
    // Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
    auto insertionRes = collectedValuations.insert(std::pair<RobustAbstractValuation, Interval>(std::move(valuation), storm::Interval(0, 1)));
    return insertionRes.first->second;
}

template<typename ParametricType, typename ConstantType>
bool RobustParameterLifter<ParametricType, ConstantType>::FunctionValuationCollector::evaluateCollectedFunctions(
    storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForUnspecifiedParameters) {
    for (auto& collectedFunctionValuationPlaceholder : collectedValuations) {
        RobustAbstractValuation const& abstrValuation = collectedFunctionValuationPlaceholder.first;
        Interval& placeholder = collectedFunctionValuationPlaceholder.second;

        RationalFunction const& transition = abstrValuation.getTransition();

        // We first figure out the positions of the lower and upper bounds per parameter
        // Lower/upper bound of every parameter is independent because the transitions are sums of terms with one parameter each
        // At the end, we compute the value
        std::map<VariableType, CoefficientType> lowerPositions;
        std::map<VariableType, CoefficientType> upperPositions;

        for (auto const& p : abstrValuation.getParameters()) {
            CoefficientType lowerP = region.getLowerBoundary(p);
            CoefficientType upperP = region.getUpperBoundary(p);

            std::set<CoefficientType> potentialExtrema = {lowerP, upperP};
            for (auto const& maximum : abstrValuation.getExtrema().at(p)) {
                if (maximum >= lowerP && maximum <= upperP) {
                    potentialExtrema.emplace(maximum);
                }
            }

            CoefficientType minPosP;
            CoefficientType maxPosP;
            CoefficientType minValue = utility::infinity<CoefficientType>();
            CoefficientType maxValue = -utility::infinity<CoefficientType>();

            auto instantiation = std::map<VariableType, CoefficientType>(region.getLowerBoundaries());

            for (auto const& potentialExtremum : potentialExtrema) {
                instantiation[p] = potentialExtremum;
                auto value = abstrValuation.getTransition().evaluate(instantiation);
                if (value > maxValue) {
                    maxValue = value;
                    maxPosP = potentialExtremum;
                }
                if (value < minValue) {
                    minValue = value;
                    minPosP = potentialExtremum;
                }
            }

            lowerPositions[p] = minPosP;
            upperPositions[p] = maxPosP;
        }

        // Compute function values at left and right ends
        ConstantType lowerBound = utility::convertNumber<ConstantType>(abstrValuation.getTransition().evaluate(lowerPositions));
        ConstantType upperBound = utility::convertNumber<ConstantType>(abstrValuation.getTransition().evaluate(upperPositions));

        bool graphPresering = true;
        const ConstantType epsilon =
            graphPresering ? utility::convertNumber<ConstantType>(storm::settings::getModule<storm::settings::modules::GeneralSettings>().getPrecision())
                           : utility::zero<ConstantType>();

        if (upperBound < utility::zero<ConstantType>() || lowerBound > utility::one<ConstantType>()) {
            // Current region is entirely ill-defined (partially ill-defined is fine:)
            return true;
        }

        // We want to check in the realm of feasible instantiations, even if our not our entire parameter space if feasible
        lowerBound = utility::max(utility::min(lowerBound, utility::one<ConstantType>() - epsilon), epsilon);
        upperBound = utility::max(utility::min(upperBound, utility::one<ConstantType>() - epsilon), epsilon);

        STORM_LOG_ASSERT(lowerBound <= upperBound, "Whoops");

        placeholder = Interval(lowerBound, upperBound);
    }

    return false;
}

template class RobustParameterLifter<storm::RationalFunction, double>;
}  // namespace transformer
}  // namespace storm
