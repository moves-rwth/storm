#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/adapters/CarlAdapter.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix,
                 storm::models::sparse::StateLabeling const& stateLabeling,
                 std::unordered_map<std::string, RewardModelType> const& rewardModels,
                 boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Dtmc, probabilityMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType>&& probabilityMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                std::unordered_map<std::string, RewardModelType>&& rewardModels, boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Dtmc, std::move(probabilityMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            

            
            template class Dtmc<double>;

#ifdef STORM_HAVE_CARL
            template class Dtmc<storm::RationalNumber>;

            template class Dtmc<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Dtmc<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
