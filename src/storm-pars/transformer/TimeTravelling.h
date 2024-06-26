#pragma once

#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/formula/model/ran/RealAlgebraicNumber.h>
#include <carl/numbers/constants.h>
#include <carl/util/Cache.h>
#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>
#include "adapters/RationalFunctionAdapter.h"
#include "adapters/RationalFunctionAdapter_Private.h"
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"
#include "modelchecker/CheckTask.h"
#include "models/sparse/Dtmc.h"
#include "models/sparse/StateLabeling.h"
#include "storage/BitVector.h"
#include "storage/FlexibleSparseMatrix.h"
#include "storm-pars/utility/parametric.h"
#include "utility/constants.h"
#include "utility/macros.h"

namespace storm {
namespace transformer {

using UniPoly = carl::UnivariatePolynomial<RationalNumber>;

struct PolynomialCache : std::map<RationalFunctionVariable, std::vector<UniPoly>> {
    /**
     * Look up the index of this polynomial in the cache. If it doesn't exist, adds it to the cache.
     *
     * @param f The polynomial.
     * @param p The main parameter of the polynomial.
     * @return uint64_t The index of the polynomial.
     */
    uint64_t lookUpInCache(UniPoly const& f, RationalFunctionVariable const& p) {
        for (uint64_t i = 0; i < (*this)[p].size(); i++) {
            if (this->at(p)[i] == f) {
                return i;
            }
        }
        this->at(p).push_back(f);
        return this->at(p).size() - 1;
    }

    /**
     * @brief Computes a univariate polynomial from a factorization.
     *
     * @param factorization The factorization (a vector of exponents, indices = position in cache).
     * @param p The parameter.
     * @return UniPoly The univariate polynomial.
     */
    UniPoly polynomialFromFactorization(std::vector<uint64_t> const& factorization, RationalFunctionVariable const& p) const {
        static std::map<std::pair<std::vector<uint64_t>, RationalFunctionVariable>, UniPoly> localCache;
        auto key = std::make_pair(factorization, p);
        if (localCache.count(key)) {
            return localCache.at(key);
        }
        UniPoly polynomial = UniPoly(p);
        polynomial = polynomial.one();
        for (uint64_t i = 0; i < factorization.size(); i++) {
            for (uint64_t j = 0; j < factorization[i]; j++) {
                polynomial *= this->at(p)[i];
            }
        }
        localCache.emplace(key, polynomial);
        return polynomial;
    }
};

class Annotation : public std::map<std::vector<uint64_t>, RationalNumber> {
   public:
    Annotation(RationalFunctionVariable parameter, std::shared_ptr<PolynomialCache> polynomialCache) : parameter(parameter), polynomialCache(polynomialCache) {
        // Intentionally left empty
    }

    /**
     * Add another annotation to this annotation.
     *
     * @param other The other annotation.
     */
    void operator+=(const Annotation other) {
        STORM_LOG_ASSERT(other.parameter == this->parameter, "Can only add annotations with equal parameters.");
        for (auto const& [factors, number] : other) {
            if (this->count(factors)) {
                this->at(factors) += number;
            } else {
                this->emplace(factors, number);
            }
        }
    }

    /**
     * Multiply this annotation with a rational number.
     *
     * @param n The rational number.
     */
    void operator*=(RationalNumber n) {
        for (auto& [factors, number] : *this) {
            number *= n;
        }
    }

    /**
     * Multiply this annotation with a rational number to get a new annotation.
     *
     * @param n The rational number.
     */
    Annotation operator*(RationalNumber n) const {
        Annotation annotationCopy(*this);
        annotationCopy *= n;
        return annotationCopy;
    }

    /**
     * Adds another annotation times a constant to this annotation.
     *
     * @param other The other annotation.
     * @param timesConstant The constant.
     */
    void addAnnotationTimesConstant(Annotation const& other, RationalNumber timesConstant) {
        for (auto const& [info, constant] : other) {
            if (!this->count(info)) {
                this->emplace(info, utility::zero<RationalNumber>());
            }
            this->at(info) += constant * timesConstant;
        }
    }

    /**
     * Adds another annotation times a polynomial to this annotation.
     *
     * @param other The other annotation.
     * @param polynomial The polynomial.
     * @param parameter The parameter in the polynomial.
     */
    void addAnnotationTimesPolynomial(Annotation const& other, UniPoly polynomial) {
        for (auto const& [info, constant] : other) {
            // Copy array
            auto newCounter = info;

            // Write new polynomial into array
            auto const cacheNum = this->polynomialCache->lookUpInCache(polynomial, parameter);
            while (newCounter.size() <= cacheNum) {
                newCounter.push_back(0);
            }
            newCounter[cacheNum]++;

            if (!this->count(newCounter)) {
                this->emplace(newCounter, utility::zero<RationalNumber>());
            }
            this->at(newCounter) += constant;
        }
    }

    /**
     * @brief Get the probability of this annotation as a univariate polynomial (which isn't factorized).
     *
     * @return UniPoly The probability.
     */
    UniPoly getProbability() const {
        UniPoly prob = UniPoly(parameter);  // Creates a zero polynomial
        for (auto const& [info, constant] : *this) {
            prob += polynomialCache->polynomialFromFactorization(info, parameter) * constant;
        }
        return prob;
    }

    /**
     * @brief Get all of the terms of the UniPoly.
     *
     * @return std::vector<UniPoly> The terms.
     */
    std::vector<UniPoly> getTerms() const {
        std::vector<UniPoly> terms;
        for (auto const& [info, constant] : *this) {
            terms.push_back(polynomialCache->polynomialFromFactorization(info, parameter) * constant);
        }
        return terms;
    }

    /**
     * Evaluate the polynomial represented by this annotation on an interval.
     *
     * @return Interval The resulting interval.
     */
    template<typename Number>
    Number evaluate(Number input) const {
        Number sumOfTerms = utility::zero<Number>();
        for (auto const& [info, constant] : *this) {
            Number outerMult = utility::one<Number>();
            for (uint64_t i = 0; i < info.size(); i++) {
                auto polynomial = this->polynomialCache->at(parameter)[i];
                // Evaluate the inner polynomial by its coefficients
                auto coefficients = polynomial.coefficients();
                Number innerSum = utility::zero<Number>();
                for (uint64_t exponent = 0; exponent < coefficients.size(); exponent++) {
                    if (exponent != 0) {
                        innerSum += carl::pow(input, exponent) * utility::convertNumber<double>(coefficients[exponent]);
                    } else {
                        innerSum += utility::convertNumber<Number>(coefficients[exponent]);
                    }
                }
                // Inner polynomial ^ exponent
                outerMult *= carl::pow(innerSum, info[i]);
            }
            sumOfTerms += outerMult * utility::convertNumber<Number>(constant);
        }
        return sumOfTerms;
    }

    Interval evaluateOnIntervalMidpointTheorem(Interval input, bool higherOrderBounds = false) const {
        if (!derivativeOfThis) {
            return evaluate<Interval>(input);
        } else {
            Interval boundDerivative = derivativeOfThis->evaluateOnIntervalMidpointTheorem(input, higherOrderBounds);
            double maxSlope = utility::abs(boundDerivative.upper());
            double fMid = evaluate<double>(input.center());
            double fMin = fMid - (input.diameter() / 2) * maxSlope;
            double fMax = fMid + (input.diameter() / 2) * maxSlope;
            if (higherOrderBounds) {
                Interval boundsHere = evaluate<Interval>(input);
                return Interval(utility::max(fMin, boundsHere.lower()), utility::min(fMax, boundsHere.upper()));
            } else {
                return Interval(fMin, fMax);
            }
        }
    }

    RationalFunctionVariable getParameter() const {
        return parameter;
    }

    void computeDerivative(uint64_t nth) {
        if (nth == 0 || derivativeOfThis) {
            return;
        }
        derivativeOfThis = std::make_shared<Annotation>(this->parameter, this->polynomialCache);
        for (auto const& [info, constant] : *this) {
            // Product rule
            for (uint64_t i = 0; i < polynomialCache->at(parameter).size(); i++) {
                if (info.size() <= i) {
                    break;
                }
                if (info[i] == 0) {
                    continue;
                }

                std::vector<uint64_t> insert(info);
                insert[i]--;

                RationalNumber newConstant = constant;
                newConstant += utility::convertNumber<RationalNumber>(i);

                auto polynomial = polynomialCache->at(parameter).at(i);
                auto derivative = polynomial.derivative();
                if (derivative.isConstant()) {
                    newConstant *= derivative.constantPart();
                } else {
                    uint64_t derivativeIndex = this->polynomialCache->lookUpInCache(derivative, parameter);
                    while (insert.size() <= derivativeIndex) {
                        insert.push_back(0);
                    }
                    insert[derivativeIndex]++;
                }
                derivativeOfThis->emplace(insert, constant);
            }
        }
        derivativeOfThis->computeDerivative(nth - 1);
    }

    // If our factors are p and 1-p, we can read off the zeroes of the first derivative of all terms
    // These are *not* the zeroes of the derivative of this annotation
    std::optional<std::vector<double>> zeroesOfDerivativeOfTerms() {
        // Check if cache is indeed p and 1-p
        auto cacheHere = this->polynomialCache->at(this->parameter);
        if (cacheHere.empty() || cacheHere.size() > 2) {
            return std::nullopt;
        }
        auto const pPoly = UniPoly(parameter, {1, 0});
        auto const oneMinusPPoly = UniPoly(parameter, {-1, 1});
        // TODO finish this

        // for now
        return std::nullopt;
    }

    uint64_t maxDegree() const {
        uint64_t maxDegree = 0;
        for (auto const& [info, constant] : *this) {
            if (!info.empty()) {
                maxDegree = std::max(maxDegree, *std::max_element(info.begin(), info.end()));
            }
        }
        return maxDegree;
    }

    std::shared_ptr<Annotation> derivative() {
        computeDerivative(1);
        return derivativeOfThis;
    }

    friend std::ostream& operator<<(std::ostream& os, const Annotation& annotation);

   private:
    const RationalFunctionVariable parameter;
    const std::shared_ptr<PolynomialCache> polynomialCache;
    std::shared_ptr<Annotation> derivativeOfThis;
};

inline std::ostream& operator<<(std::ostream& os, const Annotation& annotation) {
    auto iterator = annotation.begin();
    while (iterator != annotation.end()) {
        auto const& factors = iterator->first;
        auto const& constant = iterator->second;
        os << constant << " * (";
        bool alreadyPrintedFactor = false;
        for (uint64_t i = 0; i < factors.size(); i++) {
            if (factors[i] > 0) {
                if (alreadyPrintedFactor) {
                    os << "*";
                } else {
                    alreadyPrintedFactor = true;
                }
                os << "(" << annotation.polynomialCache->at(annotation.parameter)[i] << ")" << "^" << factors[i];
            }
        }
        if (factors.empty()) {
            os << "1";
        }
        os << ")";
        iterator++;
        if (iterator != annotation.end()) {
            os << " + ";
        }
    }
    return os;
}

/**
 * Shorthand for std::unordered_map<T, uint64_t>. Counts elements (which elements, how many of them).
 *
 * @tparam T
 */
class TimeTravelling {
   public:
    /**
     * This class re-orders parameteric transitions of a pMC so bounds computed by Parameter Lifting will eventually be better.
     * The parametric reachability probability for the given check task will be the same in the time-travelled and in the original model.
     */
    TimeTravelling() : polynomialCache(std::make_shared<PolynomialCache>()) {
        // Intentionally left empty
    }

    RationalFunction uniPolyToRationalFunction(UniPoly poly);

    /**
     * Perform big-step on the given model and the given checkTask.
     *
     * @param model A pMC.
     * @param checkTask A property (probability or reward) on the pMC.
     * @return models::sparse::Dtmc<RationalFunction> The time-travelled pMC.
     */
    std::pair<models::sparse::Dtmc<RationalFunction>, std::map<UniPoly, Annotation>> bigStep(
        models::sparse::Dtmc<RationalFunction> const& model, modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask);

    static std::unordered_map<RationalFunction, Annotation> lastSavedAnnotations;

   private:
    /**
     * Find the paths we can big-step from this state using this parameter.
     *
     * @param start Root state of search.
     * @param parameter Parameter w.r.t. which we search.
     * @param flexibleMatrix Matrix of the pMC.
     * @param backwardsFlexibleMatrix Transposed matrix of the pMC.
     * @param treeStates Tree states (see updateTreeStates).
     * @param stateRewardVector State-reward vector of the pMC (because we are not big-stepping states with rewards.)
     * @return std::pair<std::vector<std::shared_ptr<searchingPath>>, std::vector<uint64_t>> Resulting paths, all states we visited while searching paths.
     */
    std::pair<std::map<uint64_t, Annotation>, std::vector<uint64_t>> bigStepBFS(
        uint64_t start, const RationalFunctionVariable& parameter, const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
        const storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
        const std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
        const boost::optional<std::vector<RationalFunction>>& stateRewardVector);

    /**
     * Find time-travelling on the given big-step paths, i.e., identify transitions that are linear to each other and put them into seperate states,
     * transforming the pMC's topology. This modifies the matrix because it's already putting in the new states.
     *
     * @param bigStepPaths The big-step paths that time-travelling will be performed on.
     * @param parameter The parameter w.r.t. which we are time-travelling.
     * @param flexibleMatrix Flexible matrix, modifies this!
     * @param backwardsFlexibleMatrix Backwards matrix, modifies this!
     * @param alreadyTimeTravelledToThis Map of stuff we already time-travelled w.r.t. (modifies this!)
     * @param treeStatesNeedUpdate Map of the tree states that need updating (modifies this!)
     * @param originalNumStates Numbers of original states in pMC (for alreadyTimeTravelledToThis map)
     * @return std::optional<std::vector<std::shared_ptr<searchingPath>>>
     */
    std::vector<std::pair<uint64_t, Annotation>> findTimeTravelling(
        const std::map<uint64_t, Annotation> bigStepAnnotations, const RationalFunctionVariable& parameter,
        storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix, storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
        std::map<RationalFunctionVariable, std::set<std::set<uint64_t>>>& alreadyTimeTravelledToThis,
        std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate, uint64_t root, uint64_t originalNumStates);

    /**
     * Actually eliminate transitions in flexibleMatrix and backwardFlexibleMatrix according to the paths we found and want to eliminate.
     *
     * @param state The root state for the paths.
     * @param paths The paths.
     * @param flexibleMatrix The flexible matrix (modifies this!)
     * @param backwardsFlexibleMatrix The backwards flexible matrix (modifies this!)
     * @param treeStatesNeedUpdate The map of tree states that need updating (modifies this!)
     */
    std::map<UniPoly, Annotation> replaceWithNewTransitions(uint64_t state, const std::vector<std::pair<uint64_t, Annotation>> transitions,
                                                            storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                                                            storage::FlexibleSparseMatrix<RationalFunction>& backwardsFlexibleMatrix,
                                                            storage::BitVector& reachableStates,
                                                            std::map<RationalFunctionVariable, std::set<uint64_t>>& treeStatesNeedUpdate);

    /**
     * Updates which states are unreachable after the previous transformation without needing a model checking procedure.
     *
     * @param reachableStates Reachable states to the best of our knowledge (modifies this!)
     * @param statesMaybeUnreachable States that may have become unreachable (= the visited states during the big step search)
     * @param backwardsFlexibleMatrix The backwards flexible matrix in which to look for predecessors.
     */
    void updateUnreachableStates(storage::BitVector& reachableStates, std::vector<uint64_t> const& statesMaybeUnreachable,
                                 storage::FlexibleSparseMatrix<RationalFunction> const& backwardsFlexibleMatrix, uint64_t initialState);

    /**
     * updateTreeStates updates the `treeStates` map on the given states.
     * The `treeStates` map keeps track of the parametric transitions reachable with constants from any given state: for some parameter, for some state, this
     * set of parametric transitions is reachable by constant transitions. This function creates or updates this map by searching from the transitions in the
     * working sets upwards.
     *
     * @param treeStates The tree states map to update (mutates this).
     * @param workingSets Where to start the search. When creating the tree states map: set this to all states with parametric transitions (mutates this).
     * @param flexibleMatrix The flexible matrix of the pMC.
     * @param flexibleMatrix The transposed flexibleMatrix.
     * @param allParameters The set of all parameters of the pMC.
     * @param stateRewardVector The state reward vector of the pMC.
     * @param stateLabelling The state labelling of the pMC.
     */
    void updateTreeStates(std::map<RationalFunctionVariable, std::map<uint64_t, std::set<uint64_t>>>& treeStates,
                          std::map<RationalFunctionVariable, std::set<uint64_t>>& workingSets,
                          const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
                          const storage::FlexibleSparseMatrix<RationalFunction>& backwardsTransitions, const std::set<RationalFunctionVariable>& allParameters,
                          const boost::optional<std::vector<RationalFunction>>& stateRewardVector, const models::sparse::StateLabeling stateLabelling);

    /**
     * extendStateLabeling extends the given state labeling to newly created states. It will set the new labels to the labels on the given state.
     *
     * @param oldLabeling The old labeling.
     * @param oldSize The size of the old labeling.
     * @param newSize The size of the new labeling (>= oldSize).
     * @param stateWithLabels The new states will have the labels that this state has.
     * @param labelsInFormula The labels that occur in the property.
     * @return models::sparse::StateLabeling
     */
    models::sparse::StateLabeling extendStateLabeling(const models::sparse::StateLabeling& oldLabeling, uint64_t oldSize, uint64_t newSize,
                                                      uint64_t stateWithLabels, const std::set<std::string>& labelsInFormula);
    /**
     * Sums duplicate transitions in a vector of MatrixEntries into one MatrixEntry.
     *
     * @param entries
     * @return std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>>
     */
    std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> joinDuplicateTransitions(
        std::vector<storm::storage::MatrixEntry<uint64_t, RationalFunction>> const& entries);

    std::shared_ptr<RawPolynomialCache> rawPolynomialCache;

    const std::shared_ptr<PolynomialCache> polynomialCache;
};

}  // namespace transformer
}  // namespace storm
