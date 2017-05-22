#include "storm/models/sparse/StochasticTwoPlayerGame.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix,
                                                                                         storm::storage::SparseMatrix<ValueType> const& player2Matrix,
                                                                                         storm::models::sparse::StateLabeling const& stateLabeling,
                                                                                         std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                                                         boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalPlayer1ChoiceLabeling,
                                                                                         boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, player2Matrix, stateLabeling, rewardModels, optionalPlayer2ChoiceLabeling), player1Matrix(player1Matrix), player1ChoiceLabeling(optionalPlayer1ChoiceLabeling) {
                // Intentionally left empty.
            }
            
            
            template <typename ValueType, typename RewardModelType>
            StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix,
                                                                                         storm::storage::SparseMatrix<ValueType>&& player2Matrix,
                                                                                         storm::models::sparse::StateLabeling&& stateLabeling,
                                                                                         std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                                                         boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalPlayer1ChoiceLabeling,
                                                                                         boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, std::move(player2Matrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalPlayer2ChoiceLabeling)), player1Matrix(std::move(player1Matrix)), player1ChoiceLabeling(std::move(optionalPlayer1ChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer1Matrix() const {
                return player1Matrix;
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType> const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer2Matrix() const {
                return this->getTransitionMatrix();
            }
            
            template <typename ValueType, typename RewardModelType>
            bool StochasticTwoPlayerGame<ValueType, RewardModelType>::hasPlayer1ChoiceLabeling() const {
                return static_cast<bool>(player1ChoiceLabeling);
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::models::sparse::ChoiceLabeling const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer1ChoiceLabeling() const {
                return player1ChoiceLabeling.get();
            }

            template <typename ValueType, typename RewardModelType>
            bool StochasticTwoPlayerGame<ValueType, RewardModelType>::hasPlayer2ChoiceLabeling() const {
                return this->hasChoiceLabeling();
            }
            
            template <typename ValueType, typename RewardModelType>
            storm::models::sparse::ChoiceLabeling const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer2ChoiceLabeling() const {
                return this->getChoiceLabeling();
            }
            
            template class StochasticTwoPlayerGame<double>;
//            template class StochasticTwoPlayerGame<float>;
            
#ifdef STORM_HAVE_CARL
            template class StochasticTwoPlayerGame<storm::RationalFunction>;
            template class StochasticTwoPlayerGame<storm::RationalNumber>;
#endif
            
        } // namespace sparse
    } // namespace models
} // namespace storm
