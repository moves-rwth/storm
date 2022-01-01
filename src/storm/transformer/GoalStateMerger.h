#pragma once

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

namespace storm {
namespace transformer {

/*
 * Merges the given target and sink states into single states with a selfloop
 */
template<typename SparseModelType>
class GoalStateMerger {
   public:
    struct ReturnType {
        std::shared_ptr<SparseModelType> model;                // The output model
        boost::optional<uint_fast64_t> targetState;            // The target state of the output model (if reachable)
        boost::optional<uint_fast64_t> sinkState;              // The sink state of the output model (if reachable)
        std::vector<uint_fast64_t> oldToNewStateIndexMapping;  // maps a state from the input model to the corresponding state of the output model. Invalid
                                                               // index if the state does not exist
        storm::storage::BitVector keptChoices;                 // The choices of the input model that are still present in the output model
    };

    GoalStateMerger(SparseModelType const& model);

    /* Computes a submodel of the specified model that only considers the states given by maybeStates as well as
     *  * one target state to which all transitions to a state selected by targetStates are redirected and
     *  * one sink state to which all transitions to a state selected by sinkStates are redirected.
     *
     * If a choiceFilter is given, choices on maybestates that are not selected by the filter will be removed.
     *
     *  Notes:
     *  * the target (or sink) state is not created, if it is not reachable
     *  * the target (or sink) state will get a label iff it is reachable and at least one of the given targetStates (sinkStates) have that label.
     *  * Only the selected reward models will be kept. The target and sink states will not get any reward.
     *  * Choices that lead from a maybeState to a ~(target | sink) state will be removed. An exception is thrown if this leads to deadlocks.
     *  * It is assumed that maybeStates, targetStates, and sinkStates are pairwise disjoint. Otherwise an exception is thrown.
     *  * The order of the maybeStates will not be affected (i.e. s_1 < s_2 in the input model implies s'_1 < s'_2 in the output model).
     */
    ReturnType mergeTargetAndSinkStates(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates,
                                        storm::storage::BitVector const& sinkStates,
                                        std::vector<std::string> const& selectedRewardModels = std::vector<std::string>(),
                                        boost::optional<storm::storage::BitVector> const& choiceFilter = boost::none) const;

   private:
    SparseModelType const& originalModel;

    /*!
     * Initializes the data required to build the result model
     *
     * @return The initialized result and the number of transitions of the result model
     */
    std::pair<ReturnType, uint_fast64_t> initialize(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates,
                                                    storm::storage::BitVector const& sinkStates,
                                                    boost::optional<storm::storage::BitVector> const& choiceFilter = boost::none) const;

    /*!
     * Builds the transition matrix of the resulting model
     */
    storm::storage::SparseMatrix<typename SparseModelType::ValueType> buildTransitionMatrix(storm::storage::BitVector const& maybeStates,
                                                                                            ReturnType const& resultData, uint_fast64_t transitionCount) const;
    storm::models::sparse::StateLabeling buildStateLabeling(storm::storage::BitVector const& maybeStates, storm::storage::BitVector const& targetStates,
                                                            storm::storage::BitVector const& sinkStates, ReturnType const& resultData) const;
    std::unordered_map<std::string, typename SparseModelType::RewardModelType> buildRewardModels(storm::storage::BitVector const& maybeStates,
                                                                                                 ReturnType const& resultData,
                                                                                                 std::vector<std::string> const& selectedRewardModels) const;
    std::shared_ptr<SparseModelType> buildOutputModel(storm::storage::BitVector const& maybeStates, ReturnType const& resultData,
                                                      storm::storage::SparseMatrix<typename SparseModelType::ValueType>&& transitionMatrix,
                                                      storm::models::sparse::StateLabeling&& labeling,
                                                      std::unordered_map<std::string, typename SparseModelType::RewardModelType>&& rewardModels) const;
};
}  // namespace transformer
}  // namespace storm
