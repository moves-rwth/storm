#ifndef STORM_MODELS_SPARSE_CTMC_H_
#define STORM_MODELS_SPARSE_CTMC_H_

#include "storm/models/sparse/DeterministicModel.h"
#include "storm/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            /*!
             * This class represents a continuous-time Markov chain.
             */
            template<class ValueType, typename RewardModelType = StandardRewardModel<ValueType>>
            class Ctmc : public DeterministicModel<ValueType, RewardModelType> {
            public:
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param rateMatrix The matrix representing the transitions in the model.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
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
                Ctmc(storm::storage::SparseMatrix<ValueType>&& rateMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                     std::unordered_map<std::string, RewardModelType>&& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param rateMatrix The matrix representing the transitions in the model.
                 * @param exitRates The exit rates of all states.
                 * @param stateLabeling The labeling of the states.
                 * @param rewardModels A mapping of reward model names to reward models.
                 * @param optionalChoiceLabeling A vector that represents the labels associated with the choices of each state.
                 */
                Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, std::vector<ValueType> const& exitRates, storm::models::sparse::StateLabeling const& stateLabeling,
                     std::unordered_map<std::string, RewardModelType> const& rewardModels = std::unordered_map<std::string, RewardModelType>(),
                     boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling = boost::optional<std::vector<LabelSet>>());

                
                Ctmc(Ctmc<ValueType, RewardModelType> const& ctmc) = default;
                Ctmc& operator=(Ctmc<ValueType, RewardModelType> const& ctmc) = default;
                
#ifndef WINDOWS
                Ctmc(Ctmc<ValueType, RewardModelType>&& ctmc) = default;
                Ctmc& operator=(Ctmc<ValueType, RewardModelType>&& ctmc) = default;
#endif
                /*!
                 * Retrieves the vector of exit rates of the model.
                 *
                 * @return The exit rate vector.
                 */
                std::vector<ValueType> const& getExitRateVector() const;

                /*!
                 * Retrieves the vector of exit rates of the model.
                 *
                 * @return The exit rate vector.
                 */
                std::vector<ValueType>& getExitRateVector();

            private:
                /*!
                 * Computes the exit rate vector based on the given rate matrix.
                 *
                 * @param rateMatrix The rate matrix.
                 * @return The exit rate vector.
                 */
                static std::vector<ValueType> createExitRateVector(storm::storage::SparseMatrix<ValueType> const& rateMatrix);
                
                // A vector containing the exit rates of all states.
                std::vector<ValueType> exitRates;
            };
            
        } // namespace sparse
    } // namespace models
} // namespace storm

#endif /* STORM_MODELS_SPARSE_CTMC_H_ */
