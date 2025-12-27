#pragma once

#include "storm/models/sparse/Model.h"
#include "storm/simulator/ModelSimulator.h"

namespace storm {
namespace simulator {

/*!
 * This class is a low-level interface to quickly sample from sparse models.
 * Additional information about state, actions, should be obtained via the model itself.
 *
 * TODO: It may be nice to write a CPP wrapper that does not require to actually obtain such information yourself.
 */
template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
class SparseModelSimulator : public storm::simulator::ModelSimulator<ValueType> {
   public:
    /*!
     * Constructor.
     * @param model Sparse model.
     */
    SparseModelSimulator(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType> const> model);

    void resetToInitial() override;

    bool step(uint64_t action) override;

    /*!
     * Perform action and then select transition corresponding to column.
     */
    bool step(uint64_t action, uint64_t column);

    /*!
     * Get current state id.
     * @return Current state id.
     */
    uint64_t getCurrentState() const;

    uint64_t getCurrentNumberOfChoices() const override;

    std::set<std::string> getCurrentStateLabelling() const override;

    std::vector<std::string> getRewardNames() const override;

    ValueType getCurrentExitRate() const override;

    bool isCurrentStateDeadlock() const override;

    bool isContinuousTimeModel() const override;

   private:
    /*!
     * Select choice, update state-action rewards and return corresponding row.
     *
     * @param choice Choice index.
     * @return Row in sparse matrix corresponding to chosen state-action.
     */
    uint64_t choice(uint64_t choice);

    /*!
     * Perform transition from selected row to given column.
     * @param row Row in sparse matrix corresponding to chosen state-action.
     * @param column Successor state.
     */
    void transition(uint64_t row, uint64_t column);

    /// The underlying sparse model
    std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType> const> model;

    /// The current state id
    uint64_t currentState;

    /// Exit rates for continuous-time models
    std::vector<ValueType> exitRates;
};
}  // namespace simulator
}  // namespace storm
