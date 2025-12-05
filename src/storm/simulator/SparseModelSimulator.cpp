#include "storm/simulator/SparseModelSimulator.h"

#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/MarkovAutomaton.h"

namespace storm {
namespace simulator {
template<typename ValueType, typename RewardModelType>
SparseModelSimulator<ValueType, RewardModelType>::SparseModelSimulator(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType> const> model)
    : model(model), zeroRewards(model->getNumberOfRewardModels(), storm::utility::zero<ValueType>()), generator() {
    resetToInitial();

    if (!model->isDiscreteTimeModel()) {
        if (model->getType() == storm::models::ModelType::Ctmc) {
            exitRates = model->template as<storm::models::sparse::Ctmc<ValueType, RewardModelType>>()->getExitRateVector();
        } else {
            STORM_LOG_ASSERT(model->getType() == storm::models::ModelType::MarkovAutomaton, "Model is not a Markov automaton.");
            auto ma = model->template as<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>();
            exitRates = ma->getExitRates();
#ifndef NDEBUG
            for (size_t i = 0; i < exitRates.size(); ++i) {
                STORM_LOG_ASSERT((storm::utility::isZero<ValueType>(exitRates[i]) || ma->isMarkovianState(i)) &&
                                     (!storm::utility::isZero<ValueType>(exitRates[i]) || !ma->isMarkovianState(i)),
                                 "Exit rate and Markovian state do not match.");
            }
#endif
        }
    }
}

template<typename ValueType, typename RewardModelType>
void SparseModelSimulator<ValueType, RewardModelType>::setSeed(uint64_t seed) {
    generator = storm::utility::RandomProbabilityGenerator<ValueType>(seed);
}
template<typename ValueType, typename RewardModelType>
void SparseModelSimulator<ValueType, RewardModelType>::resetToInitial() {
    STORM_LOG_WARN_COND(model->getInitialStates().getNumberOfSetBits() == 1,
                        "The model has multiple initial states. This simulator assumes it starts from the initial state with the lowest index.");
    currentState = *model->getInitialStates().begin();
    currentTime = storm::utility::zero<ValueType>();
    lastRewards = zeroRewards;
    uint64_t i = 0;
    for (auto const& rewModPair : model->getRewardModels()) {
        if (rewModPair.second.hasStateRewards()) {
            lastRewards[i] += rewModPair.second.getStateReward(currentState);
        }
        ++i;
    }
}

template<typename ValueType, typename RewardModelType>
uint64_t SparseModelSimulator<ValueType, RewardModelType>::choice(uint64_t choice) {
    STORM_LOG_ASSERT(choice < getCurrentNumberOfChoices(), "Action index higher than number of actions");
    lastRewards = zeroRewards;
    uint64_t row = model->getTransitionMatrix().getRowGroupIndices()[currentState] + choice;

    // Add state-action reward
    uint64_t i = 0;
    for (auto const& rewModPair : model->getRewardModels()) {
        if (rewModPair.second.hasStateActionRewards()) {
            lastRewards[i] += rewModPair.second.getStateActionReward(row);
        }
        ++i;
    }
    return row;
}

template<typename ValueType, typename RewardModelType>
void SparseModelSimulator<ValueType, RewardModelType>::transition(uint64_t row, uint64_t column) {
#ifndef NDEBUG
    bool columnExists = false;
    for (auto const& entry : model->getTransitionMatrix().getRow(row)) {
        if (entry.getColumn() == column) {
            columnExists = true;
        }
    }
    STORM_LOG_ASSERT(columnExists, "Column " << column << " does not exist");
#endif

    currentState = column;
    // Add state reward
    uint64_t i = 0;
    for (auto const& rewModPair : model->getRewardModels()) {
        if (rewModPair.second.hasStateRewards()) {
            lastRewards[i] += rewModPair.second.getStateReward(currentState);
        }
        ++i;
    }
}

template<typename ValueType, typename RewardModelType>
bool SparseModelSimulator<ValueType, RewardModelType>::randomStep() {
    if (!model->isDiscreteTimeModel()) {
        // First choose time when to leave the state
        randomTime();
    }

    if (model->getTransitionMatrix().getRowGroupSize(currentState) == 0) {
        return false;
    }
    if (model->getTransitionMatrix().getRowGroupSize(currentState) == 1) {
        return step(0);
    }
    // Select action by uniform distribution
    return step(generator.randomSelect(0, model->getTransitionMatrix().getRowGroupSize(currentState) - 1));
}

template<typename ValueType, typename RewardModelType>
bool SparseModelSimulator<ValueType, RewardModelType>::step(uint64_t action) {
    // Perform action
    uint64_t row = choice(action);

    if (model->getTransitionMatrix().getRow(row).getNumberOfEntries() == 1) {
        // Select only transition
        uint64_t column = model->getTransitionMatrix().getRow(row).begin()->getColumn();
        transition(row, column);
        return true;
    }

    // Randomly select transition
    ValueType probability = generator.randomProbability();
    ValueType sum = storm::utility::zero<ValueType>();
    for (auto const& entry : model->getTransitionMatrix().getRow(row)) {
        if (model->getType() == storm::models::ModelType::Ctmc) {
            // Scale rates to probabilities
            sum += entry.getValue() / exitRates[currentState];
        } else {
            sum += entry.getValue();
        }
        if (sum >= probability) {
            uint64_t column = entry.getColumn();
            transition(row, column);
            return true;
        }
    }

    // This position should never be reached
    return false;
}

template<typename ValueType, typename RewardModelType>
bool SparseModelSimulator<ValueType, RewardModelType>::step(uint64_t action, uint64_t column) {
    uint64_t row = choice(action);
    transition(row, column);
    return true;
}

template<typename ValueType, typename RewardModelType>
void SparseModelSimulator<ValueType, RewardModelType>::randomTime() {
    STORM_LOG_ASSERT(!model->isDiscreteTimeModel(), "Model must be continuous-time model");
    ValueType exitRate = exitRates[currentState];
    if (!storm::utility::isZero(exitRate)) {
        STORM_LOG_ASSERT(model->getTransitionMatrix().getRowGroupSize(currentState) == 1, "Markovian state should have a trivial row group.");
        ValueType time = generator.randomExponential(exitRate);
        currentTime += time;
    }
}

template<typename ValueType, typename RewardModelType>
uint64_t SparseModelSimulator<ValueType, RewardModelType>::getCurrentState() const {
    return currentState;
}

template<typename ValueType, typename RewardModelType>
ValueType SparseModelSimulator<ValueType, RewardModelType>::getCurrentTime() const {
    return currentTime;
}
template<typename ValueType, typename RewardModelType>
uint64_t SparseModelSimulator<ValueType, RewardModelType>::getCurrentNumberOfChoices() const {
    return model->getTransitionMatrix().getRowGroupSize(currentState);
}

template<typename ValueType, typename RewardModelType>
std::set<std::string> SparseModelSimulator<ValueType, RewardModelType>::getCurrentStateLabelling() const {
    return model->getStateLabeling().getLabelsOfState(currentState);
}

template<typename ValueType, typename RewardModelType>
std::vector<ValueType> const& SparseModelSimulator<ValueType, RewardModelType>::getLastRewards() const {
    return lastRewards;
}

template class SparseModelSimulator<double>;
template class SparseModelSimulator<storm::RationalNumber>;

}  // namespace simulator
}  // namespace storm
