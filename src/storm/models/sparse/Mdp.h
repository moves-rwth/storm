#ifndef STORM_MODELS_SPARSE_MDP_H_
#define STORM_MODELS_SPARSE_MDP_H_

#include "storm/storage/StateActionPair.h"
#include "storm/models/sparse/NondeterministicModel.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a (discrete-time) Markov decision process.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Mdp : public NondeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Mdp(storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                    storm::models::sparse::StateLabeling const& stateLabeling,
                    std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                    boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                /*!
                 * Constructs a model by moving the given data.
                 *
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Mdp(storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                    storm::models::sparse::StateLabeling&& stateLabeling,
                    std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                    boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                Mdp(Mdp<ValueType, RewardModelType> const& other) = default;
                Mdp& operator=(Mdp<ValueType, RewardModelType> const& other) = default;
                
                Mdp(Mdp<ValueType, RewardModelType>&& other) = default;
                Mdp& operator=(Mdp<ValueType, RewardModelType>&& other) = default;

                /*!
                 * Constructs an MDP by copying the current MDP and restricting the choices of each state to the ones
                 * whose label set is contained in the given label set. Note that is is only valid to call this method
                 * if the MDP has choice labels.
                 *
                 * @param enabledChoiceLabels A set of labels that determines which choices of the original model can be taken
                 * and which ones need to be ignored.
                 * @return A restricted version of the current MDP that only uses choice labels from the given set.
                 */
                Mdp<ValueType, RewardModelType> restrictChoiceLabels(LabelSet const& enabledChoiceLabels) const;
                
                /*!
                 * Constructs an MDP by copying the current MDP and restricting the choices of each state to the ones given by the bitvector.
                 * 
                 * @param enabledActions A BitVector of lenght numberOfChoices(), which is one iff the action should be kept.
                 * @return A subMDP.
                 */
                Mdp<ValueType, RewardModelType> restrictChoices(storm::storage::BitVector const& enabledActions) const;

                /*!
                 *  For a state/action pair, get the choice index referring to the state-action pair.
                 */
                uint_fast64_t getChoiceIndex(storm::storage::StateActionPair const& stateactPair) const;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_MDP_H_ */
