#include "storm/simulator/PrismProgramSimulator.h"

#include "storm/adapters/JsonAdapter.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

using namespace storm::generator;

namespace storm {
namespace simulator {

template<typename ValueType>
PrismProgramSimulator<ValueType>::PrismProgramSimulator(storm::prism::Program program, storm::generator::NextStateGeneratorOptions const& options)
    : storm::simulator::ModelSimulator<ValueType>(),
      program(program),
      currentState(),
      stateGenerator(std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(program, options)),
      stateToId(stateGenerator->getStateSize()),
      idToState() {
    this->zeroRewards = std::vector<ValueType>(stateGenerator->getNumberOfRewardModels(), storm::utility::zero<ValueType>());
    // Current state needs to be overwritten to actual initial state.
    this->resetToInitial();
}

template<typename ValueType>
bool PrismProgramSimulator<ValueType>::step(uint64_t actionNumber) {
    STORM_LOG_ASSERT(actionNumber < getCurrentNumberOfChoices(), "Action index higher than number of actions");
    auto choice = getChoices()[actionNumber];

    // Set state-action reward
    this->currentRewards = this->zeroRewards;
    if (!isContinuousTimeModel()) {
        this->currentRewards = choice.getRewards();
        STORM_LOG_ASSERT(this->currentRewards.size() == stateGenerator->getNumberOfRewardModels(), "Reward vector should have as many rewards as model.");
    } else {
        // TODO add support
        STORM_LOG_WARN("State-action rewards are currently not supported for continuous-time models.");
    }

    uint64_t nextState;
    if (choice.size() == 1) {
        // Select only transition
        nextState = choice.begin()->first;
    } else {
        // Randomly select transition
        ValueType probability = this->randomGenerator.randomProbability();
        if (program.getModelType() == storm::prism::Program::ModelType::CTMC ||
            (program.getModelType() == storm::prism::Program::ModelType::MA && choice.isMarkovian())) {
            // Scale probability to exit rate
            probability *= getCurrentExitRate();
        }
        nextState = choice.sampleFromDistribution(probability);
    }

    currentState = idToState[nextState];
    // TODO we do not need to do this in every step!
    clearStateCaches();
    explore();
    return true;
}

template<typename ValueType>
void PrismProgramSimulator<ValueType>::explore() {
    // Load the current state into the next state generator.
    stateGenerator->load(currentState);
    // TODO: This low-level code currently expands all actions, while this is not necessary.
    // However, using the next state generator ensures compatibility with the model generator.
    behavior = stateGenerator->expand(stateToIdCallback);
    STORM_LOG_ASSERT(behavior.getStateRewards().size() == this->currentRewards.size(), "Reward vectors should have same length.");
    for (uint64_t i = 0; i < behavior.getStateRewards().size(); i++) {
        this->currentRewards[i] += behavior.getStateRewards()[i];
    }
}

template<typename ValueType>
bool PrismProgramSimulator<ValueType>::isCurrentStateDeadlock() const {
    return behavior.empty();
}

template<typename ValueType>
bool PrismProgramSimulator<ValueType>::isSinkState() const {
    if (isCurrentStateDeadlock()) {
        return true;
    }

    std::set<uint32_t> successorIds;
    for (Choice<ValueType, uint32_t> const& choice : behavior.getChoices()) {
        for (auto it = choice.begin(); it != choice.end(); ++it) {
            successorIds.insert(it->first);
            if (successorIds.size() > 1) {
                return false;
            }
        }
    }
    if (idToState.at(*(successorIds.begin())) == currentState) {
        return true;
    }
    return false;
}

template<typename ValueType>
std::set<std::string> PrismProgramSimulator<ValueType>::getCurrentStateLabelling() const {
    std::set<std::string> labels;
    for (auto const& label : program.getLabels()) {
        if (stateGenerator->evaluateBooleanExpressionInCurrentState(label.getStatePredicateExpression())) {
            labels.insert(label.getName());
        }
    }
    return labels;
}

template<typename ValueType>
std::vector<generator::Choice<ValueType, uint32_t>> const& PrismProgramSimulator<ValueType>::getChoices() const {
    return behavior.getChoices();
}

template<typename ValueType>
uint64_t PrismProgramSimulator<ValueType>::getCurrentNumberOfChoices() const {
    return getChoices().size();
}

template<typename ValueType>
ValueType PrismProgramSimulator<ValueType>::getCurrentExitRate() const {
    if (!isContinuousTimeModel()) {
        // Discrete-time models have no exit rate
        return storm::utility::zero<ValueType>();
    }

    if (isCurrentStateDeadlock()) {
        return storm::utility::zero<ValueType>();
    }
    if (getCurrentNumberOfChoices() > 1) {
        // Probabilistic state
        return storm::utility::zero<ValueType>();
    }

    STORM_LOG_ASSERT(getCurrentNumberOfChoices() == 1, "Expected deterministic state.");
    auto choice = getChoices().front();
    ValueType totalMass = choice.getTotalMass();
    // CTMC or (MA and Markovian) => totalMass > 0
    STORM_LOG_ASSERT(!(program.getModelType() == storm::prism::Program::ModelType::CTMC ||
                       (program.getModelType() == storm::prism::Program::ModelType::MA && choice.isMarkovian())) ||
                         storm::utility::isPositive(totalMass),
                     "Expected positive exit rate.");
    // MA and not Markovian => totalMass = 1
    STORM_LOG_ASSERT(!(program.getModelType() == storm::prism::Program::ModelType::MA && !choice.isMarkovian()) || storm::utility::isOne(totalMass),
                     "Probabilities should sum up to one.");
    return totalMass;
}

template<typename ValueType>
CompressedState const& PrismProgramSimulator<ValueType>::getCurrentState() const {
    return currentState;
}

template<typename ValueType>
expressions::SimpleValuation PrismProgramSimulator<ValueType>::getCurrentStateAsValuation() const {
    return unpackStateIntoValuation(currentState, stateGenerator->getVariableInformation(), program.getManager());
}

template<typename ValueType>
std::string PrismProgramSimulator<ValueType>::getCurrentStateString() const {
    return stateGenerator->stateToString(currentState);
}

template<typename ValueType>
storm::json<ValueType> PrismProgramSimulator<ValueType>::getStateAsJson() const {
    return stateGenerator->currentStateToJson(false);
}

template<typename ValueType>
storm::json<ValueType> PrismProgramSimulator<ValueType>::getObservationAsJson() const {
    return stateGenerator->currentStateToJson(true);
}

template<typename ValueType>
void PrismProgramSimulator<ValueType>::resetToInitial() {
    auto indices = stateGenerator->getInitialStates(stateToIdCallback);
    STORM_LOG_WARN_COND(indices.size() == 1,
                        "The model has multiple initial states. This simulator assumes it starts from the initial state with the lowest index.");
    resetToState(idToState[indices[0]], storm::utility::zero<ValueType>());
}

template<typename ValueType>
void PrismProgramSimulator<ValueType>::resetToState(generator::CompressedState const& newState, ValueType time) {
    this->currentRewards = this->zeroRewards;
    this->currentTime = time;
    currentState = newState;
    explore();
}

template<typename ValueType>
void PrismProgramSimulator<ValueType>::resetToState(expressions::SimpleValuation const& valuation, ValueType time) {
    this->currentRewards = this->zeroRewards;
    this->currentTime = time;
    currentState = generator::packStateFromValuation(valuation, stateGenerator->getVariableInformation(), true);
    explore();
}

template<typename ValueType>
std::vector<std::string> PrismProgramSimulator<ValueType>::getRewardNames() const {
    std::vector<std::string> names;
    for (uint64_t i = 0; i < stateGenerator->getNumberOfRewardModels(); ++i) {
        names.push_back(stateGenerator->getRewardModelInformation(i).getName());
    }
    return names;
}

template<typename ValueType>
bool PrismProgramSimulator<ValueType>::isContinuousTimeModel() const {
    return !program.isDiscreteTimeModel();
}

template<typename ValueType>
uint32_t PrismProgramSimulator<ValueType>::getOrAddStateIndex(generator::CompressedState const& state) {
    uint32_t newIndex = static_cast<uint32_t>(stateToId.size());

    // Check, if the state was already registered.
    std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateToId.findOrAddAndGetBucket(state, newIndex);

    uint32_t actualIndex = actualIndexBucketPair.first;
    if (actualIndex == newIndex) {
        idToState[actualIndex] = state;
    }
    return actualIndex;
}

template<typename ValueType>
void PrismProgramSimulator<ValueType>::clearStateCaches() {
    idToState.clear();
    stateToId = storm::storage::BitVectorHashMap<uint32_t>(stateGenerator->getStateSize());
}

template class PrismProgramSimulator<double>;
template class PrismProgramSimulator<storm::RationalNumber>;
}  // namespace simulator
}  // namespace storm
