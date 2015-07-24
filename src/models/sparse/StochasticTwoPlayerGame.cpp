#include "src/models/sparse/StochasticTwoPlayerGame.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType>
            StochasticTwoPlayerGame<ValueType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix,
                                                                        storm::storage::SparseMatrix<ValueType> const& player2Matrix,
                                                                        storm::models::sparse::StateLabeling const& stateLabeling,
                                                                        boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                                                        boost::optional<std::vector<LabelSet>> const& optionalPlayer1ChoiceLabeling,
                                                                        boost::optional<std::vector<LabelSet>> const& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, player2Matrix, stateLabeling, optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<ValueType>>(), optionalPlayer2ChoiceLabeling), player1Matrix(player1Matrix), player1ChoiceLabeling(optionalPlayer1ChoiceLabeling) {
                // Intentionally left empty.
            }
            
            
            template <typename ValueType>
            StochasticTwoPlayerGame<ValueType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix,
                                                                        storm::storage::SparseMatrix<ValueType>&& player2Matrix,
                                                                        storm::models::sparse::StateLabeling&& stateLabeling,
                                                                        boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                                                        boost::optional<std::vector<LabelSet>>&& optionalPlayer1ChoiceLabeling,
                                                                        boost::optional<std::vector<LabelSet>>&& optionalPlayer2ChoiceLabeling)
            : NondeterministicModel<ValueType>(storm::models::ModelType::S2pg, std::move(player2Matrix), std::move(stateLabeling), std::move(optionalStateRewardVector), boost::optional<storm::storage::SparseMatrix<ValueType>>(), std::move(optionalPlayer2ChoiceLabeling)), player1Matrix(std::move(player1Matrix)), player1ChoiceLabeling(std::move(optionalPlayer1ChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& StochasticTwoPlayerGame<ValueType>::getPlayer1Matrix() const {
                return player1Matrix;
            }
            
            template <typename ValueType>
            storm::storage::SparseMatrix<ValueType> const& StochasticTwoPlayerGame<ValueType>::getPlayer2Matrix() const {
                return this->getTransitionMatrix();
            }
            
            template <typename ValueType>
            bool StochasticTwoPlayerGame<ValueType>::hasPlayer1ChoiceLabeling() const {
                return static_cast<bool>(player1ChoiceLabeling);
            }
            
            template <typename ValueType>
            std::vector<LabelSet> const& StochasticTwoPlayerGame<ValueType>::getPlayer1ChoiceLabeling() const {
                return player1ChoiceLabeling.get();
            }

            template <typename ValueType>
            bool StochasticTwoPlayerGame<ValueType>::hasPlayer2ChoiceLabeling() const {
                return this->hasChoiceLabeling();
            }
            
            template <typename ValueType>
            std::vector<LabelSet> const& StochasticTwoPlayerGame<ValueType>::getPlayer2ChoiceLabeling() const {
                return this->getChoiceLabeling();
            }
            
            template class StochasticTwoPlayerGame<double>;
            template class StochasticTwoPlayerGame<float>;
            
#ifdef STORM_HAVE_CARL
            template class StochasticTwoPlayerGame<storm::RationalFunction>;
#endif
            
        } // namespace sparse
    } // namespace models
} // namespace storm