#pragma once

#include <map>
#include <optional>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/storage/BitVector.h"

namespace storm {

namespace storage {
template<typename ValueType>
class MaximalEndComponentDecomposition;
}

class Environment;

namespace modelchecker::multiobjective {

template<typename ModelType>
class DeterministicSchedsObjectiveHelper {
   public:
    typedef typename ModelType::ValueType ValueType;
    DeterministicSchedsObjectiveHelper(ModelType const& model, Objective<ValueType> const& objective);

    /*!
     * Returns true iff the value at the (unique) initial state is a constant (that is independent of the scheduler)
     */
    bool hasConstantInitialStateValue() const;

    /*!
     * If hasConstantInitialStateValue is true, this returns that constant value.
     */
    ValueType getConstantInitialStateValue() const;

    /*!
     * Returns those states for which the scheduler potentially influences the value.
     *
     * @return
     */
    storm::storage::BitVector const& getMaybeStates() const;

    /*!
     * Returns the set of states for which value -inf is possible
     * @pre getInfinityCase() == HasNegativeInfinite has to hold
     * @return
     */
    storm::storage::BitVector const& getRewMinusInfEStates() const;

    /*!
     * Returns the choice rewards if non-zero.
     * This does not include choices of non-maybe states
     */
    std::map<uint64_t, ValueType> const& getChoiceRewards() const;

    /*!
     * Returns the choices that (i) originate from a maybestate and (ii) have zero value
     *
     */
    storm::storage::BitVector const& getRelevantZeroRewardChoices() const;

    ValueType const& getUpperValueBoundAtState(uint64_t state) const;
    ValueType const& getLowerValueBoundAtState(uint64_t state) const;

    enum class InfinityCase { AlwaysFinite, HasPositiveInfinite, HasNegativeInfinite };

    InfinityCase const& getInfinityCase() const;

    bool minimizing() const;

    void computeLowerUpperBounds(Environment const& env) const;

    /*!
     * Returns true if this is a total reward objective
     */
    bool isTotalRewardObjective() const;

    /*!
     * Returns true, if this objective specifies a threshold
     */
    bool hasThreshold() const;

    /*!
     * Returns the threshold (if specified)
     */
    ValueType getThreshold() const;

    /*!
     * Computes the value at the initial state under the given scheduler.
     * @return
     */
    ValueType evaluateScheduler(Environment const& env, storm::storage::BitVector const& selectedChoices) const;

   private:
    void initialize();

    storm::storage::BitVector maybeStates;         // S_?^j
    storm::storage::BitVector rewMinusInfEStates;  // S_{-infty}^j
    std::optional<ValueType> constantInitialStateValue;
    storm::storage::BitVector relevantZeroRewardChoices;
    std::map<uint64_t, ValueType> choiceRewards;
    InfinityCase infinityCase;

    // Computed on demand:
    mutable std::optional<std::vector<ValueType>> lowerResultBounds;
    mutable std::optional<std::vector<ValueType>> upperResultBounds;

    ModelType const& model;
    Objective<ValueType> const& objective;
};
}  // namespace modelchecker::multiobjective
}  // namespace storm