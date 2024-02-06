#include "storm-pars/transformer/RobustParameterLifter.h"
#include <_types/_uint64_t.h>
#include <memory>
#include <optional>
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"

#include "storm-pars/storage/ParameterRegion.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/vector.h"
#include "utility/constants.h"
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
    functionValuationCollector.evaluateCollectedFunctions(region, dirForParameters);

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
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::recursiveDecompose(RawPolynomial polynomial, VariableType parameter, bool firstIteration) {
    auto parameterPol = RawPolynomial(parameter);
    auto oneMinusParameter = RawPolynomial(1) - parameterPol;
    if (polynomial.isConstant()) {
        return std::make_pair(std::make_pair((uint64_t)0, (uint64_t)0),
                              std::make_pair(utility::convertNumber<CoefficientType>(polynomial.constantPart()), utility::zero<CoefficientType>()));
    }
    auto byOneMinusP = polynomial.divideBy(oneMinusParameter);
    if (byOneMinusP.remainder.isZero()) {
        auto recursiveResult = recursiveDecompose(byOneMinusP.quotient, parameter, false);
        if (recursiveResult) {
            return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1), recursiveResult->second);
        }
    }
    auto byP = polynomial.divideBy(parameterPol);
    if (byP.remainder.isZero()) {
        auto recursiveResult = recursiveDecompose(byP.quotient, parameter, false);
        if (recursiveResult) {
            return std::make_pair(std::make_pair(recursiveResult->first.first + 1, recursiveResult->first.second), recursiveResult->second);
        }
    }
    if (!firstIteration) {
        return boost::none;
    }
    if (byOneMinusP.remainder.isConstant()) {
        auto rem1 = utility::convertNumber<CoefficientType>(byOneMinusP.remainder.constantPart());
        auto recursiveResult = recursiveDecompose(byOneMinusP.quotient, parameter, false);
        if (recursiveResult) {
            STORM_LOG_ASSERT(recursiveResult->second.second == 0, "");
            return std::make_pair(std::make_pair(recursiveResult->first.first, recursiveResult->first.second + 1),
                                  std::pair<CoefficientType, CoefficientType>(recursiveResult->second.first, rem1));
        }
    }
    if (byP.remainder.isConstant()) {
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
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::RobustAbstractValuation(RationalFunction transition) : transition(transition) {
    std::set<VariableType> occurringVariables;
    storm::utility::parametric::gatherOccurringVariables(transition, occurringVariables);

    STORM_LOG_ERROR_COND(transition.denominator().isConstant(), "Robust PLA only supports transitions of the form sum_i (c_i * p_i^a * (1-p_i)^b) + d.");

    transition.simplify();
    auto constantPart = transition.constantPart();

    CoefficientType denominator = transition.denominator().constantPart();

    auto nominator = RawPolynomial(transition.nominator());

    CoefficientType offset = utility::zero<CoefficientType>();

    std::map<VariableType, RawPolynomial> termsPerParameter;

    // Collect terms and add them together
    for (auto const& term : nominator.getTerms()) {
        if (term.isConstant()) {
            STORM_LOG_ASSERT(utility::isZero(offset), "Transition has two offsets? " << transition);
            offset = utility::convertNumber<CoefficientType>(RawPolynomial(term).constantPart());
            continue;
        }

        std::set<VariableType> variables;
        term.gatherVariables(variables);
        STORM_LOG_ASSERT(variables.size() == 1, "There may only be a single variable in each term of each transition, but the term "
                                                    << term << " has " << term.getNrVariables() << ".");
        auto const p = *variables.begin();

        if (termsPerParameter.count(p)) {
            termsPerParameter[p] = termsPerParameter[p] + RawPolynomial(term);
        } else {
            termsPerParameter[p] = RawPolynomial(term);
        }
    }

    for (auto const& entry : termsPerParameter) {
        auto const& p = entry.first;
        auto const& term = entry.second;

        auto resultForParameter = recursiveDecompose(term, p);

        uint64_t a = resultForParameter->first.first;
        uint64_t b = resultForParameter->first.second;
        CoefficientType constant = resultForParameter->second.first / denominator;
        offset += resultForParameter->second.second;

        // Term is constant * p^a * (1-p)^b
        this->parameters.emplace(p);
        this->aAndB.emplace(p, std::make_pair(a, b));
        this->constants.emplace(p, utility::convertNumber<ConstantType>(constant));

        auto cleanedTerm = term / constant;

        // The maximum of the polynomial part lies at a / (a + b), so compute that
        // It is corrected for constant and offset later, not now
        CoefficientType maximumCoeff;
        if (a != 0 || b != 0) {
            maximumCoeff = utility::convertNumber<CoefficientType>(a) / utility::convertNumber<CoefficientType>(a + b);
        } else {
            maximumCoeff = utility::zero<CoefficientType>();
        }
        ConstantType maximum = utility::convertNumber<ConstantType>(maximumCoeff);

        std::map<VariableType, CoefficientType> substitution;
        substitution.emplace(p, maximumCoeff);
        this->maximum.emplace(p, std::make_pair(maximum, utility::convertNumber<ConstantType>(cleanedTerm.evaluate(substitution))));
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
std::map<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType, ConstantType> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getConstants() const {
    return this->constants;
}

template<typename ParametricType, typename ConstantType>
ConstantType const& RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getOffset() const {
    return this->offset;
}

template<typename ParametricType, typename ConstantType>
std::map<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType, std::pair<uint64_t, uint64_t>> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getAandB() const {
    return this->aAndB;
}

template<typename ParametricType, typename ConstantType>
std::map<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType, std::pair<ConstantType, ConstantType>> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getMaximum() const {
    return this->maximum;
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
void RobustParameterLifter<ParametricType, ConstantType>::FunctionValuationCollector::evaluateCollectedFunctions(
    storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForUnspecifiedParameters) {
    for (auto& collectedFunctionValuationPlaceholder : collectedValuations) {
        RobustAbstractValuation const& abstrValuation = collectedFunctionValuationPlaceholder.first;
        Interval& placeholder = collectedFunctionValuationPlaceholder.second;

        RationalFunction const& transition = abstrValuation.getTransition();

        // We sometimes need to compute function values. This is a cache for the transition evaluated at the lower boundary of the region, because we can re-use
        // that.
        std::optional<CoefficientType> lowerValueFunction;

        // We first figure out the positions of the lower and upper bounds per parameter
        // Lower/upper bound of every parameter is independent because the transitions are sums of terms with one parameter each
        // At the end, we compute the value
        std::map<VariableType, CoefficientType> lowerBounds;
        std::map<VariableType, CoefficientType> upperBounds;
        for (auto const& p : abstrValuation.getParameters()) {
            CoefficientType lowerPCoeff = region.getLowerBoundary(p);
            CoefficientType upperPCoeff = region.getUpperBoundary(p);

            CoefficientType lowerP = region.getLowerBoundary(p);
            CoefficientType upperP = region.getUpperBoundary(p);

            CoefficientType maximumPos = abstrValuation.getMaximum().at(p).first;
            CoefficientType constant = abstrValuation.getConstants().at(p);

            CoefficientType minPosP;
            CoefficientType maxPosP;

            if (lowerP <= maximumPos && upperP >= maximumPos) {
                // Case 1: The valuation embraces the maximum of the function.
                // Lower (upper) bound is the minimum of left and right (we need to compute that)
                // Upper (lower) bound is the maximum itself (we already know that)

                // We need to figure out whether the upper or lower bound is the extremum, for that we need the values
                auto instantiation = std::map<VariableType, CoefficientType>(region.getLowerBoundaries());
                // This is cached, because it is the same for each parameter
                if (!lowerValueFunction) {
                    lowerValueFunction = abstrValuation.getTransition().evaluate(instantiation);
                }
                // We set only p to the upper value and keep the rest the same
                instantiation[p] = upperP;
                CoefficientType upperValueFunctionP = abstrValuation.getTransition().evaluate(instantiation);
                if (constant > utility::zero<CoefficientType>()) {
                    // Parabola points up
                    maxPosP = maximumPos;
                    if (upperValueFunctionP > lowerValueFunction) {
                        minPosP = upperP;
                    } else {
                        minPosP = lowerP;
                    }
                } else {
                    // Parabola points down
                    minPosP = maximumPos;
                    if (upperValueFunctionP > lowerValueFunction) {
                        maxPosP = upperP;
                    } else {
                        maxPosP = lowerP;
                    }
                }
            } else {
                // Case 2: The valuation is completely on the right / left of the maximum => monotone decreasing / increasing
                bool isOnLeft = lowerP <= maximumPos && upperP <= maximumPos;
                bool isOnRight = lowerP >= maximumPos && upperP >= maximumPos;

                bool parabolaPointsUp = constant > utility::zero<CoefficientType>();

                if ((isOnRight && parabolaPointsUp) || (isOnLeft && !parabolaPointsUp)) {
                    maxPosP = lowerP;
                    minPosP = upperP;
                } else if ((isOnLeft && parabolaPointsUp) || (isOnRight && !parabolaPointsUp)) {
                    minPosP = lowerP;
                    maxPosP = upperP;
                } else {
                    STORM_LOG_ERROR("This case should never happen!! I made a mistake!");
                }
            }

            lowerBounds.emplace(p, minPosP);
            upperBounds.emplace(p, maxPosP);
        }

        // Compute function values at left and right ends
        ConstantType lowerBound = utility::convertNumber<ConstantType>(abstrValuation.getTransition().evaluate(lowerBounds));
        ConstantType upperBound = utility::convertNumber<ConstantType>(abstrValuation.getTransition().evaluate(upperBounds));

        STORM_LOG_ASSERT(lowerBound <= upperBound, "Whoops");

        placeholder = Interval(lowerBound, upperBound);
    }
}

template class RobustParameterLifter<storm::RationalFunction, double>;
}  // namespace transformer
}  // namespace storm
