#pragma once

#include <boost/optional.hpp>
#include <map>

#include "storm/modelchecker/multiobjective/Objective.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {

class Environment;

namespace modelchecker {
namespace multiobjective {

template<typename ModelType>
class DeterministicSchedsObjectiveHelper {
   public:
    typedef typename ModelType::ValueType ValueType;
    DeterministicSchedsObjectiveHelper(ModelType const& model, Objective<ValueType> const& objective);

    /*!
     * Returns states and values for states that are independent of the scheduler.
     */
    std::map<uint64_t, ValueType> const& getSchedulerIndependentStateValues() const;

    /*!
     * Returns offsets of each choice value (e.g., the reward) if non-zero.
     * This does not include choices of states with independent state values
     */
    std::map<uint64_t, ValueType> const& getChoiceValueOffsets() const;

    ValueType const& getUpperValueBoundAtState(Environment const& env, uint64_t state) const;
    ValueType const& getLowerValueBoundAtState(Environment const& env, uint64_t state) const;

    ValueType const& getLargestUpperBound(Environment const& env) const;

    bool minimizing() const;

    /*!
     * Returns true if this is a total reward objective
     */
    bool isTotalRewardObjective() const;

    ValueType evaluateOnModel(Environment const& env, ModelType const& evaluatedModel) const;

    static std::vector<ValueType> computeUpperBoundOnExpectedVisitingTimes(storm::storage::SparseMatrix<ValueType> const& modelTransitions,
                                                                           storm::storage::BitVector const& bottomStates,
                                                                           storm::storage::BitVector const& nonBottomStates, bool hasEndComponents);

   private:
    void computeUpperBounds(Environment const& env) const;
    void computeLowerBounds(Environment const& env) const;

    mutable boost::optional<std::map<uint64_t, ValueType>> schedulerIndependentStateValues;
    mutable boost::optional<std::map<uint64_t, ValueType>> choiceValueOffsets;
    mutable boost::optional<std::vector<ValueType>> lowerResultBounds;
    mutable boost::optional<std::vector<ValueType>> upperResultBounds;

    ModelType const& model;
    Objective<ValueType> const& objective;
};
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm