#include "storm-pars/transformer/RobustParameterLifter.h"
#include <_types/_uint64_t.h>
#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/VariablePool.h>
#include <carl/core/rootfinder/IncrementalRootFinder.h>
#include <carl/core/rootfinder/RootFinder.h>
#include <carl/formula/model/ran/RealAlgebraicNumber.h>
#include <carl/thom/ThomRootFinder.h>
#include <cmath>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <type_traits>
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"

#include "environment/Environment.h"
#include "modelchecker/results/CheckResult.h"
#include "settings/SettingsManager.h"
#include "settings/modules/GeneralSettings.h"
#include "solver/SmtSolver.h"
#include "solver/SmtlibSmtSolver.h"
#include "solver/Z3SmtSolver.h"
#include "storage/expressions/Expression.h"
#include "storage/expressions/RationalFunctionToExpression.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/transformer/TimeTravelling.h"
#include "storm-pars/utility/parametric.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/vector.h"
#include "utility/constants.h"
#include "utility/logging.h"
#include "utility/macros.h"
#include "utility/solver.h"

std::map<storm::transformer::UniPoly, storm::transformer::Annotation> storm::transformer::TimeTravelling::lastSavedAnnotations;

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
std::optional<std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::zeroesSMT(
    std::vector<UniPoly> polynomials,
    std::shared_ptr<RawPolynomialCache> rawPolynomialCache,
    typename RobustParameterLifter<ParametricType, ConstantType>::VariableType parameter) {
    if (polynomials.size() == 0 || (polynomials.size() == 1 && polynomials.begin()->isConstant())) {
        return std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>();
    }
    std::shared_ptr<storm::expressions::ExpressionManager> expressionManager = std::make_shared<storm::expressions::ExpressionManager>();

    utility::solver::Z3SmtSolverFactory factory;
    auto smtSolver = factory.create(*expressionManager);

    expressions::RationalFunctionToExpression<RationalFunction> rfte(expressionManager);

    auto expression = expressionManager->rational(0);
    for (auto const& summand : polynomials) {
        auto multivariatePol = carl::MultivariatePolynomial<RationalNumber>(summand);
        auto multiNominator = carl::FactorizedPolynomial(multivariatePol, rawPolynomialCache);
        expression = expression + rfte.toExpression(RationalFunction(multiNominator));
    }
    expression = expression == expressionManager->rational(0);

    auto variables = expressionManager->getVariables();
    // Sum the summands together directly in the expression so we pass this info to the solver
    expressions::Expression exprBounds = expressionManager->boolean(true);
    for (auto const& var : variables) {
        exprBounds = exprBounds && expressionManager->rational(0) <= var && var <= expressionManager->rational(1);
    }

    smtSolver->setTimeout(50);

    smtSolver->add(exprBounds);
    smtSolver->add(expression);

    std::set<CoefficientType> zeroes = {};

    while (true) {
        auto checkResult = smtSolver->check();

        if (checkResult == solver::SmtSolver::CheckResult::Sat) {
            auto model = smtSolver->getModel();

            STORM_LOG_ERROR_COND(variables.size() == 1, "Should be one variable.");
            if (variables.size() != 1) {
                return {};
            }
            auto const var = *variables.begin();

            double value = model->getRationalValue(var);

            zeroes.emplace(RationalFunctionCoefficient(value));

            // Add new constraint so we search for the next zero in the polynomial
            // Get another model (or unsat)
            // For some reason, this only really works when we then make a new 
            smtSolver->addNotCurrentModel();
        } else if (checkResult == solver::SmtSolver::CheckResult::Unknown) {
            return std::nullopt;
            break;
        } else {
            // Unsat => found all zeroes :)
            break;
        }
    }
    return zeroes;
}

template<typename ParametricType, typename ConstantType>
std::optional<std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::zeroesCarl(
        UniPoly polynomial, typename RobustParameterLifter<ParametricType, ConstantType>::VariableType parameter) {
    std::cout << "Roots of " << polynomial << std::endl;
    auto const& carlRoots = carl::rootfinder::realRoots(polynomial, carl::rootfinder::SplittingStrategy::DEFAULT, carl::Interval<CoefficientType>(utility::zero<CoefficientType>(), utility::one<CoefficientType>()));
    std::set<CoefficientType> zeroes = {};
    for (carl::RealAlgebraicNumber<CoefficientType> const& root : carlRoots) {
        CoefficientType rootCoefficient;
        if (root.isNumeric()) {
            rootCoefficient = CoefficientType(root.value());
        } else {
            // TODO incorrect
            rootCoefficient = CoefficientType(root.lower());
        }
        zeroes.emplace(rootCoefficient);
        std::cout << rootCoefficient << " ";
    }
    std::cout << std::endl;
    return zeroes;
}


template<typename ParametricType, typename ConstantType>
std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::cubicEquationZeroes(
    RawPolynomial polynomial, typename RobustParameterLifter<ParametricType, ConstantType>::VariableType parameter) {
    if (polynomial.isConstant()) {
        return {};
    }
    STORM_LOG_ERROR_COND(polynomial.gatherVariables().size() == 1, "Multi-variate polynomials currently not supported");
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
        roots = {utility::convertNumber<CoefficientType>(std::cbrt(-qDouble))};
    } else if (utility::isZero(q)) {  // q = 0 -> t^3 + pt = 0 -> t(t^2+p)=0
        roots = {0};
        if (p < 0) {
            roots.emplace(utility::convertNumber<CoefficientType>(utility::sqrt(-pDouble)));
            roots.emplace(utility::convertNumber<CoefficientType>(-utility::sqrt(-pDouble)));
        }
    } else {
        // These are all coefficients (we also plug the values into RationalFunctions later), i.e., they are rational numbers,
        // but some of these operations are strictly real, so we convert to double and back (i.e., approximate).
        CoefficientType D = q * q / 4 + p * p * p / 27;
        if (utility::isZero(D)) {  // D = 0 -> two roots
            roots = {-3 * q / (p * 2), 3 * q / p};
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
    STORM_LOG_ERROR_COND(transition.denominator().isConstant(), "Robust PLA only supports transitions with constant denominators.");
    transition.simplify();
    std::set<VariableType> occurringVariables;
    storm::utility::parametric::gatherOccurringVariables(transition, occurringVariables);
    for (auto const& var : occurringVariables) {
        parameters.emplace(var);
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
void RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::initializeExtrema() {
    // Compute all zeroes

    if (this->extrema || this->extremaAnnotations) {
        // Extrema already initialized
        return;
    }

    if (!TimeTravelling::lastSavedAnnotations.empty()) {
        this->hasAnnotation = true;

        // There is an annotation for this transition:
        auto nominatorAsUnivariate = transition.nominator().toUnivariatePolynomial();
        // Constant denominator is now distributed in the factors, not in the denominator of the rational function
        nominatorAsUnivariate /= transition.denominator().coefficient();
        auto const& annotation = TimeTravelling::lastSavedAnnotations.at(nominatorAsUnivariate);

        auto const& terms = annotation.getTerms();

        std::vector<UniPoly> derivatives;
        for (auto const& term : terms) {
            auto const& derivative = term.derivative();
            derivatives.push_back(derivative);
        }

        // Try to find all zeroes of all derivatives with the SMT solver.
        // TODO: Are we even using that this is a sum of terms?
        auto smtResult = zeroesSMT(derivatives, this->transition.nominator().pCache(), annotation.getParameter());
        
        if (smtResult) {
            // Hooray, we found the zeroes with the SMT solver.
            this->extrema = std::map<VariableType, std::set<CoefficientType>>();
            (*this->extrema)[annotation.getParameter()];
            for (auto const& zero : *smtResult) {
                (*this->extrema).at(annotation.getParameter()).emplace(utility::convertNumber<CoefficientType>(zero));
            }
        } else {
            std::cout << "Have to find zeroes in terms for " << annotation << std::endl;
            // We can find the zeroes of the terms of the annotation.
            this->extremaAnnotations = std::map<UniPoly, std::set<double>>();
            for (uint64_t i = 0; i < terms.size(); i++) {
                auto const& term = terms[i];
                auto const& derivative = derivatives[i];
                auto const& zeroes = zeroesSMT({derivative}, this->transition.nominator().pCache(), annotation.getParameter());
                STORM_LOG_ERROR_COND(zeroes, "Could not find zeroes of " << derivative << ".");

                for (auto const& zero : *zeroes) {
                    (*this->extremaAnnotations)[term].emplace(utility::convertNumber<double>(zero));
                }
            }
        }

    } else {
        this->extrema = std::map<VariableType, std::set<CoefficientType>>();

        for (auto const& p : parameters) {
            (*this->extrema)[p] = {};

            auto const& derivative = transition.derivative(p);

            // There is an annotation for this transition:
            auto nominatorAsUnivariate = derivative.nominator().toUnivariatePolynomial();
            // Constant denominator is now distributed in the factors, not in the denominator of the rational function
            nominatorAsUnivariate /= derivative.denominator().coefficient();

            auto const& annotation = TimeTravelling::lastSavedAnnotations.at(nominatorAsUnivariate);
            // Compute zeros of derivative (= maxima/minima of function) and emplace those between 0 and 1 into the maxima set

            std::optional<std::set<CoefficientType>> zeroes;
            // Find zeroes with straight-forward method for degrees <4, find them with SMT for degrees above that
            if (derivative.nominator().totalDegree() < 4) {
                zeroes = cubicEquationZeroes(RawPolynomial(derivative.nominator()), p);
            } else {
                zeroes = zeroesSMT({nominatorAsUnivariate}, transition.nominator().pCache(), p);
            }
            STORM_LOG_ERROR_COND(zeroes, "Zeroes of " << derivative << " could not be found.");
            for (auto const& zero : *zeroes) {
                if (zero >= utility::zero<CoefficientType>() && zero <= utility::one<CoefficientType>()) {
                    this->extrema->at(p).emplace(zero);
                }
            }
        }
    }
}

template<typename ParametricType, typename ConstantType>
std::optional<std::map<typename RobustParameterLifter<ParametricType, ConstantType>::VariableType,
         std::set<typename storm::utility::parametric::CoefficientType<ParametricType>::type>>> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getExtrema() const {
    return this->extrema;
}

template<typename ParametricType, typename ConstantType>
std::optional<std::map<UniPoly, std::set<double>>> const&
RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getExtremaAnnotations() const {
    return this->extremaAnnotations;
}

template<typename ParametricType, typename ConstantType>
std::size_t RobustParameterLifter<ParametricType, ConstantType>::RobustAbstractValuation::getHashValue() const {
    std::size_t seed = 0;
    carl::hash_add(seed, transition);
    return seed;
}

template<typename ParametricType, typename ConstantType>
Interval& RobustParameterLifter<ParametricType, ConstantType>::FunctionValuationCollector::add(RobustAbstractValuation& valuation) {
    // If no valuation like this is present in the collectedValuations, initialize the extrema
    if (!collectedValuations.count(valuation)) {
        valuation.initializeExtrema();
    }
    // insert the function and the valuation
    // Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
    auto insertionRes = collectedValuations.insert(std::pair<RobustAbstractValuation, Interval>(std::move(valuation), storm::Interval(0, 1)));
    return insertionRes.first->second;
}

Interval evaluateExtremaAnnotations(std::map<UniPoly, std::set<double>> extremaAnnotations, Interval input) {
    Interval sumOfTerms(0.0, 0.0);
    for (auto const& [poly, roots] : extremaAnnotations) {
        std::set<double> potentialExtrema = {input.lower(), input.upper()};
        for (auto const& root : roots) {
            if (root >= input.lower() && root <= input.upper()) {
                potentialExtrema.emplace(root);
            }
        }

        double minValue = utility::infinity<double>();
        double maxValue = -utility::infinity<double>();

        for (auto const& potentialExtremum : potentialExtrema) {
            // TODO use double or rational number for storage?
            auto value = poly.evaluate(utility::convertNumber<RationalNumber>(potentialExtremum));
            if (value > maxValue) {
                maxValue = utility::convertNumber<double>(value);
            }
            if (value < minValue) {
                minValue = utility::convertNumber<double>(value);
            }
        }
        sumOfTerms += Interval(minValue, maxValue);
    }
    return sumOfTerms;
}

template<typename ParametricType, typename ConstantType>
bool RobustParameterLifter<ParametricType, ConstantType>::FunctionValuationCollector::evaluateCollectedFunctions(
    storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForUnspecifiedParameters) {
    for (auto& collectedFunctionValuationPlaceholder : this->collectedValuations) {
        RobustAbstractValuation const& abstrValuation = collectedFunctionValuationPlaceholder.first;
        Interval& placeholder = collectedFunctionValuationPlaceholder.second;

        RationalFunction const& transition = abstrValuation.getTransition();

        if (abstrValuation.getExtrema()) {
            // We know the extrema of this abstract valuation => we can get the exact bounds easily

            // We first figure out the positions of the lower and upper bounds per parameter
            // Lower/upper bound of every parameter is independent because the transitions are sums of terms with one parameter each
            // At the end, we compute the value
            std::map<VariableType, CoefficientType> lowerPositions;
            std::map<VariableType, CoefficientType> upperPositions;

            for (auto const& p : abstrValuation.getParameters()) {
                CoefficientType lowerP = region.getLowerBoundary(p);
                CoefficientType upperP = region.getUpperBoundary(p);

                std::set<CoefficientType> potentialExtrema = {lowerP, upperP};
                for (auto const& maximum : abstrValuation.getExtrema()->at(p)) {
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
        } else {
            // We do not know the extrema of this abstract valuation but the extrema of all terms
            STORM_LOG_ERROR_COND(abstrValuation.getExtremaAnnotations(), "Abstract valuation has neither extrema nor extrema on terms of annotations.");

            auto nominatorAsUnivariate = transition.nominator().toUnivariatePolynomial();
            // Constant denominator is now distributed in the factors, not in the denominator of the rational function
            nominatorAsUnivariate /= transition.denominator().coefficient();

            // TODO pass this this through arguments
            Annotation annotation = TimeTravelling::lastSavedAnnotations.at(nominatorAsUnivariate);

            Interval interval = Interval(region.getLowerBoundary(annotation.getParameter()), region.getUpperBoundary(annotation.getParameter()));
            double width = interval.upper() - interval.lower();

            double minLower = 1.0;
            double maxUpper = 0.0;

            const float fraction = 0.01;
            for (float start = 0; start < 1; start += fraction) {
                Interval subInterval = Interval(interval.lower() + width * start, interval.lower() + width * (start + fraction));
                auto result = evaluateExtremaAnnotations(*abstrValuation.getExtremaAnnotations(), subInterval);
                if (result.lower() < minLower) {
                    minLower = result.lower();
                }
                if (result.upper() > maxUpper) {
                    maxUpper = result.upper();
                }
            }

            placeholder = Interval(minLower, maxUpper);

            std::cout << "Found bounds on " << annotation << std::endl;
            std::cout << placeholder << std::endl;
        }
    }

    return false;
}

template class RobustParameterLifter<storm::RationalFunction, double>;
}  // namespace transformer
}  // namespace storm
