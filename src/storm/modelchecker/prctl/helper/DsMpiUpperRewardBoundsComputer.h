#pragma once

#include <cstdint>
#include <vector>

namespace storm {
namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace modelchecker {
namespace helper {

template<typename ValueType>
class DsMpiDtmcPriorityLess;

template<typename ValueType>
class DsMpiDtmcUpperRewardBoundsComputer {
   public:
    /*!
     * Creates an object that can compute upper bounds on the expected rewards for the provided DTMC.
     * @see http://doi.org/10.1145/1102351.1102423
     * @param transitionMatrix The matrix defining the transitions of the system without the transitions
     * that lead directly to the goal state.
     * @param rewards The rewards of each state.
     * @param oneStepTargetProbabilities For each state the probability to go to a goal state in one step.
     */
    DsMpiDtmcUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& rewards,
                                       std::vector<ValueType> const& oneStepTargetProbabilities);

    virtual ~DsMpiDtmcUpperRewardBoundsComputer() = default;

    /*!
     * Computes upper bounds on the expected rewards.
     */
    std::vector<ValueType> computeUpperBounds();

   protected:
    /*!
     * Performs a Dijkstra sweep.
     */
    virtual void sweep();

    /*!
     * Computes the lambda used for the estimation.
     */
    virtual ValueType computeLambda() const;

    /*!
     * Computes the lambda just for the provided choice.
     */
    ValueType computeLambdaForChoice(uint64_t choice) const;

    /*!
     * Retrieves the state associated with the given choice.
     */
    virtual uint64_t getStateForChoice(uint64_t choice) const;

    // References to input data.
    storm::storage::SparseMatrix<ValueType> const& transitionMatrix;
    std::vector<ValueType> const& originalRewards;
    std::vector<ValueType> const& originalOneStepTargetProbabilities;

    // Derived from input data.
    storm::storage::SparseMatrix<ValueType> backwardTransitions;

    // Data that the algorithm uses internally.
    std::vector<ValueType> p;
    std::vector<ValueType> w;
    std::vector<ValueType> rewards;
    std::vector<ValueType> targetProbabilities;

    friend class DsMpiDtmcPriorityLess<ValueType>;
};

template<typename ValueType>
class DsMpiMdpPriorityLess;

template<typename ValueType>
class DsMpiMdpUpperRewardBoundsComputer : public DsMpiDtmcUpperRewardBoundsComputer<ValueType> {
   public:
    /*!
     * Creates an object that can compute upper bounds on the *minimal* expected rewards for the provided MDP.
     * @see http://doi.org/10.1145/1102351.1102423
     * @param transitionMatrix The matrix defining the transitions of the system without the transitions
     * that lead directly to the goal state.
     * @param rewards The rewards of each choice.
     * @param oneStepTargetProbabilities For each choice the probability to go to a goal state in one step.
     */
    DsMpiMdpUpperRewardBoundsComputer(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType> const& rewards,
                                      std::vector<ValueType> const& oneStepTargetProbabilities);

   private:
    virtual void sweep() override;
    virtual ValueType computeLambda() const override;
    virtual uint64_t getStateForChoice(uint64_t choice) const override;
    uint64_t getChoiceInState(uint64_t state) const;
    void setChoiceInState(uint64_t state, uint64_t choice);

    std::vector<uint64_t> choiceToState;
    std::vector<uint64_t> policy;

    friend class DsMpiMdpPriorityLess<ValueType>;
};
}  // namespace helper
}  // namespace modelchecker
}  // namespace storm
