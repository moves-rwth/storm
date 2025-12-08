#include "storm/simulator/ModelSimulator.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm {
namespace simulator {

template<typename ValueType>
ModelSimulator<ValueType>::ModelSimulator() : currentTime(storm::utility::zero<ValueType>()), randomGenerator() {
    // Intentionally left empty
}

template<typename ValueType>
void ModelSimulator<ValueType>::setSeed(uint64_t newSeed) {
    randomGenerator = storm::utility::RandomProbabilityGenerator<ValueType>(newSeed);
}

template<typename ValueType>
bool ModelSimulator<ValueType>::randomStep() {
    if (isContinuousTimeModel()) {
        // First choose time when to leave the state
        randomTime();
    }

    if (isCurrentStateDeadlock()) {
        return false;
    }
    if (getCurrentNumberOfChoices() == 1) {
        return step(0);
    }
    // Select action by uniform distribution
    return step(this->randomGenerator.randomSelect(0, getCurrentNumberOfChoices() - 1));
}

template<typename ValueType>
void ModelSimulator<ValueType>::randomTime() {
    STORM_LOG_ASSERT(isContinuousTimeModel(), "Model must be continuous-time model");
    ValueType exitRate = getCurrentExitRate();
    if (!storm::utility::isZero(exitRate)) {
        STORM_LOG_ASSERT(getCurrentNumberOfChoices() == 1, "Markovian state should have a trivial row group.");
        ValueType time = this->randomGenerator.randomExponential(exitRate);
        this->currentTime += time;
    }
}

template<typename ValueType>
ValueType ModelSimulator<ValueType>::getCurrentTime() const {
    return currentTime;
}

template<typename ValueType>
std::vector<ValueType> const& ModelSimulator<ValueType>::getCurrentRewards() const {
    return currentRewards;
}

template class ModelSimulator<double>;
template class ModelSimulator<storm::RationalNumber>;

}  // namespace simulator
}  // namespace storm
