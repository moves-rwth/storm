#include "src/models/sparse/StochasticTwoPlayerGame.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix,
                                                                                         storm::storage::SparseMatrix<ValueType> const& player2Matrix,
                                                                                         storm::models::sparse::StateLabeling const& stateLabeling,
                                                                                         std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                                                         boost::optional<std::vector<LabelSet>> const& optionalPlayer1ChoiceLabeling,
                                                                                         boost::optional<std::vector<LabelSet>> const& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, player2Matrix, stateLabeling, rewardModels, optionalPlayer2ChoiceLabeling), player1Matrix(player1Matrix), player1Labels(optionalPlayer1ChoiceLabeling) {
                // Intentionally left empty.
            }
            
            
            template <typename ValueType, typename RewardModelType>
            StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix,
                                                                                         storm::storage::SparseMatrix<ValueType>&& player2Matrix,
                                                                                         storm::models::sparse::StateLabeling&& stateLabeling,
                                                                                         std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                                         boost::optional<std::vector<LabelSet>>&& optionalPlayer1ChoiceLabeling,
                                                                                         boost::optional<std::vector<LabelSet>>&& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, std::move(player2Matrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalPlayer2ChoiceLabeling)), player1Matrix(std::move(player1Matrix)), player1Labels(std::move(optionalPlayer1ChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template class StochasticTwoPlayerGame<double>;
            template class StochasticTwoPlayerGame<float>;
            
#ifdef STORM_HAVE_CARL
            template class StochasticTwoPlayerGame<storm::RationalFunction>;
#endif
            
        } // namespace sparse
    } // namespace models
} // namespace storm