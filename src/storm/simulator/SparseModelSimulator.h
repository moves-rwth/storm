#include <cstdint>

#include "storm/models/sparse/Model.h"
#include "storm/utility/random.h"

namespace storm {
namespace simulator {

/**
 * This class is a low-level interface to quickly sample from sparse models.
 * Additional information about state, actions, should be obtained via the model itself.
 *
 * TODO: It may be nice to write a CPP wrapper that does not require to actually obtain such informations yourself.
 */
template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class SparseModelSimulator {
   public:
    SparseModelSimulator(storm::models::sparse::Model<ValueType, RewardModelType> const& model);
    void setSeed(uint64_t);
    void resetToInitial();

    /**
     * For probabilistic states: Randomly select action and then randomly select transition.
     * For Markovian states (only in continous-time models): Increase time according to exit rate and then randomly select transition.
     */
    bool randomStep();
    /**
     * Perform action and then randomly select transition.
     */
    bool step(uint64_t action);
    /**
     * Perform action and then select transition corresponding to column.
     */
    bool step(uint64_t action, uint64_t column);

    void randomTime();

    uint64_t getCurrentState() const;
    ValueType getCurrentTime() const;
    uint64_t getCurrentNumberOfChoices() const;
    std::set<std::string> getCurrentStateLabelling() const;
    std::vector<ValueType> const& getLastRewards() const;

   private:
    /**
     * Select choice, update state-action rewards and return corresponding row.
     */
    uint64_t choice(uint64_t choice);
    void transition(uint64_t row, uint64_t column);

    storm::models::sparse::Model<ValueType, RewardModelType> const& model;
    uint64_t currentState;
    // Time which has progressed so far. Only relevant for continous-time models
    ValueType currentTime;
    std::vector<ValueType> lastRewards;
    std::vector<ValueType> zeroRewards;
    storm::utility::RandomProbabilityGenerator<ValueType> generator;

    // Exit rates for continuous-time models
    std::vector<ValueType> exitRates;
};
}  // namespace simulator
}  // namespace storm