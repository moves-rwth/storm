#include "storm/models/sparse/StochasticTwoPlayerGame.h"

#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
namespace models {
namespace sparse {

template<typename ValueType, typename RewardModelType>
StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(
    storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix,
    storm::models::sparse::StateLabeling const& stateLabeling, std::unordered_map<std::string, RewardModelType> const& rewardModels)
    : StochasticTwoPlayerGame<ValueType, RewardModelType>(
          storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(player2Matrix, stateLabeling, rewardModels, false, boost::none, player1Matrix)) {
    // Intentionally left empty.
}

template<typename ValueType, typename RewardModelType>
StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::SparseMatrix<storm::storage::sparse::state_type>&& player1Matrix,
                                                                             storm::storage::SparseMatrix<ValueType>&& player2Matrix,
                                                                             storm::models::sparse::StateLabeling&& stateLabeling,
                                                                             std::unordered_map<std::string, RewardModelType>&& rewardModels)
    : StochasticTwoPlayerGame<ValueType, RewardModelType>(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(
          std::move(player2Matrix), std::move(stateLabeling), std::move(rewardModels), false, boost::none, std::move(player1Matrix))) {
    // Intentionally left empty.
}

template<typename ValueType, typename RewardModelType>
StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(
    storm::storage::sparse::ModelComponents<ValueType, RewardModelType> const& components)
    : NondeterministicModel<ValueType, RewardModelType>(ModelType::S2pg, components), player1Matrix(components.player1Matrix.get()) {
    // Intentionally left empty
}

template<typename ValueType, typename RewardModelType>
StochasticTwoPlayerGame<ValueType, RewardModelType>::StochasticTwoPlayerGame(storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components)
    : NondeterministicModel<ValueType, RewardModelType>(ModelType::S2pg, std::move(components)), player1Matrix(std::move(components.player1Matrix.get())) {
    // Intentionally left empty
}

template<typename ValueType, typename RewardModelType>
storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer1Matrix() const {
    return player1Matrix;
}

template<typename ValueType, typename RewardModelType>
storm::storage::SparseMatrix<ValueType> const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer2Matrix() const {
    return this->getTransitionMatrix();
}

template<typename ValueType, typename RewardModelType>
bool StochasticTwoPlayerGame<ValueType, RewardModelType>::hasPlayer2ChoiceLabeling() const {
    return this->hasChoiceLabeling();
}

template<typename ValueType, typename RewardModelType>
storm::models::sparse::ChoiceLabeling const& StochasticTwoPlayerGame<ValueType, RewardModelType>::getPlayer2ChoiceLabeling() const {
    return this->getChoiceLabeling();
}

template class StochasticTwoPlayerGame<double>;

#ifdef STORM_HAVE_CARL
template class StochasticTwoPlayerGame<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
template class StochasticTwoPlayerGame<storm::RationalFunction>;
template class StochasticTwoPlayerGame<storm::RationalNumber>;
#endif

}  // namespace sparse
}  // namespace models
}  // namespace storm
