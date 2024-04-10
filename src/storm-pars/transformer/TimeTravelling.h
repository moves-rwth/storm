#pragma once

#include <_types/_uint64_t.h>
#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/UnivariatePolynomial.h>
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
    uint64_t lookUpInCache(UniPoly f, RationalFunctionVariable p) {
        for (uint64_t i = 0; i < (*this)[p].size(); i++) {
            if (this->at(p)[i] == f) {
                return i;
            }
        }
        this->at(p).push_back(f);
        return this->at(p).size() - 1;
    }

    UniPoly polynomialFromFactorization(std::vector<uint64_t> factorization, RationalFunctionVariable p) const {
        UniPoly polynomial = UniPoly(p);
        polynomial = polynomial.one();
        for (uint64_t i = 0; i < factorization.size(); i++) {
            for (uint64_t j = 0; j < factorization[i]; j++) {
                polynomial *= this->at(p)[i];
            }
        }
        return polynomial;
    }
};

struct Annotation : std::map<std::vector<uint64_t>, RationalNumber> {
    // Annotation(Annotation& other) = default;
    // Annotation(Annotation const& other) = default;
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

    UniPoly getProbability() const {
        UniPoly prob = UniPoly(parameter); // Creates a zero polynomial
        std::vector<RationalFunction> vector;
        for (auto const& [info, constant] : *this) {
            prob += polynomialCache->polynomialFromFactorization(info, parameter) * constant;
        }
        return prob;
    }
    friend std::ostream& operator<<(std::ostream& os, const Annotation& annotation);

   private:
    const RationalFunctionVariable parameter;
    const std::shared_ptr<PolynomialCache> polynomialCache;
};

inline std::ostream& operator<<(std::ostream& os, const Annotation& annotation) {
    auto iterator = annotation.begin();
    while (iterator != annotation.end()) {
        auto const& factors = iterator->first;
        auto const& constant = iterator->second;
        for (uint64_t i = 0; i < factors.size(); i++) {
            if (factors[i] > 0) {
                os << "(" << annotation.polynomialCache->at(annotation.parameter)[i] << ")" << "^" << factors[i];
            }
        }
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

    /**
     * Perform big-step on the given model and the given checkTask.
     *
     * @param model A pMC.
     * @param checkTask A property (probability or reward) on the pMC.
     * @param horizon How big to step: maximal degree of the resulting polynomials.
     * @param timeTravel Whether to time-travel, i.e., add new states to consolidate similar transitions.
     * @return models::sparse::Dtmc<RationalFunction> The time-travelled pMC.
     */
    models::sparse::Dtmc<RationalFunction> bigStep(models::sparse::Dtmc<RationalFunction> const& model,
                                                   modelchecker::CheckTask<logic::Formula, RationalFunction> const& checkTask, uint64_t horizon = 4,
                                                   bool timeTravel = true);

   private:


    /**
     * Find the paths we can big-step from this state using this parameter and this horizon.
     *
     * @param start Root state of search.
     * @param parameter Parameter w.r.t. which we search.
     * @param horizon How big to step: maximal degree of the resulting polynomials.
     * @param flexibleMatrix Matrix of the pMC.
     * @param treeStates Tree states (see updateTreeStates).
     * @param stateRewardVector State-reward vector of the pMC (because we are not big-stepping states with rewards.)
     * @return std::pair<std::vector<std::shared_ptr<searchingPath>>, std::vector<uint64_t>> Resulting paths, all states we visited while searching paths.
     */
    std::pair<std::map<uint64_t, Annotation>, std::vector<uint64_t>> bigStepBFS(
        uint64_t start, const RationalFunctionVariable& parameter, uint64_t horizon, const storage::FlexibleSparseMatrix<RationalFunction>& flexibleMatrix,
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
    void replaceWithNewTransitions(uint64_t state, const std::vector<std::pair<uint64_t, Annotation>> transitions,
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
                                 const storage::FlexibleSparseMatrix<RationalFunction>& backwardsTransitions,
                                 const std::set<RationalFunctionVariable>& allParameters,
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
