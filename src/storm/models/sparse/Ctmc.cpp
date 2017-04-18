#include "storm/models/sparse/Ctmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/adapters/CarlAdapter.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            Ctmc<ValueType, RewardModelType>::Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                  boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Ctmc, rateMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                exitRates = createExitRateVector(this->getTransitionMatrix());
            }
            
            template <typename ValueType, typename RewardModelType>
            Ctmc<ValueType, RewardModelType>::Ctmc(storm::storage::SparseMatrix<ValueType>&& rateMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                  boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Ctmc, std::move(rateMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                // It is important to refer to the transition matrix here, because the given rate matrix has been move elsewhere.
                exitRates = createExitRateVector(this->getTransitionMatrix());
            }
            
            template <typename ValueType, typename RewardModelType>
            Ctmc<ValueType, RewardModelType>::Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates, storm::models::sparse::StateLabeling const& stateLabeling,
                                std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Ctmc, std::move(rateMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)), exitRates(exitRates) {
            }
            
            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> const& Ctmc<ValueType, RewardModelType>::getExitRateVector() const {
                return exitRates;
            }

            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType>& Ctmc<ValueType, RewardModelType>::getExitRateVector() {
                return exitRates;
            }

            template <typename ValueType, typename RewardModelType>
            std::vector<ValueType> Ctmc<ValueType, RewardModelType>::createExitRateVector(storm::storage::SparseMatrix<ValueType> const& rateMatrix) {
                std::vector<ValueType> exitRates(rateMatrix.getRowCount());
                for (uint_fast64_t row = 0; row < rateMatrix.getRowCount(); ++row) {
                    exitRates[row] = rateMatrix.getRowSum(row);
                }
                return exitRates;
            }
            
            template class Ctmc<double>;

#ifdef STORM_HAVE_CARL
            template class Ctmc<storm::RationalNumber>;

            template class Ctmc<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Ctmc<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
