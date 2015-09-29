#ifndef STORM_MODELS_SPARSE_STANDARDREWARDMODEL_H_
#define STORM_MODELS_SPARSE_STANDARDREWARDMODEL_H_

#include <vector>
#include <boost/optional.hpp>

#include "src/storage/SparseMatrix.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace models {
        namespace sparse {
            template <typename CValueType>
            class StandardRewardModel {
            public:
                typedef CValueType ValueType;
                
                /*!
                 * Constructs a reward model by copying the given data.
                 *
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalStateActionRewardVector The reward values associated with state-action pairs.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 */
                StandardRewardModel(boost::optional<std::vector<ValueType>> const& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                                    boost::optional<std::vector<ValueType>> const& optionalStateActionRewardVector = boost::optional<std::vector<ValueType>>(),
                                    boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>());
                
                /*!
                 * Constructs a reward model by moving the given data.
                 *
                 * @param optionalStateRewardVector The reward values associated with the states.
                 * @param optionalStateActionRewardVector The reward values associated with state-action pairs.
                 * @param optionalTransitionRewardMatrix The reward values associated with the transitions of the model.
                 */
                StandardRewardModel(boost::optional<std::vector<ValueType>>&& optionalStateRewardVector = boost::optional<std::vector<ValueType>>(),
                            boost::optional<std::vector<ValueType>>&& optionalStateActionRewardVector = boost::optional<std::vector<ValueType>>(),
                            boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>());
                
                StandardRewardModel(StandardRewardModel<ValueType> const& dtmc) = default;
                StandardRewardModel& operator=(StandardRewardModel<ValueType> const& dtmc) = default;
                
#ifndef WINDOWS
                StandardRewardModel(StandardRewardModel<ValueType>&& dtmc) = default;
                StandardRewardModel& operator=(StandardRewardModel<ValueType>&& dtmc) = default;
#endif
                
                /*!
                 * Retrieves whether the reward model has state rewards.
                 *
                 * @return True iff the reward model has state rewards.
                 */
                bool hasStateRewards() const;
                
                /*!
                 * Retrieves whether the reward model only has state rewards (and hence no other rewards).
                 *
                 * @return True iff the reward model only has state rewards.
                 */
                bool hasOnlyStateRewards() const;

                /*!
                 * Retrieves the state rewards of the reward model. Note that it is illegal to call this function if the
                 * reward model does not have state rewards.
                 *
                 * @return The state reward vector.
                 */
                std::vector<ValueType> const& getStateRewardVector() const;

                /*!
                 * Retrieves the state rewards of the reward model. Note that it is illegal to call this function if the
                 * reward model does not have state rewards.
                 *
                 * @return The state reward vector.
                 */
                std::vector<ValueType>& getStateRewardVector();

                ValueType const& getStateReward(uint_fast64_t state) const;
                
                /*!
                 * Retrieves an optional value that contains the state reward vector if there is one.
                 *
                 * @return The state reward vector if there is one.
                 */
                boost::optional<std::vector<ValueType>> const& getOptionalStateRewardVector() const;
                
                /*!
                 * Retrieves whether the reward model has state-action rewards.
                 *
                 * @return True iff the reward model has state-action rewards.
                 */
                bool hasStateActionRewards() const;

                /*!
                 * Retrieves the state-action rewards of the reward model. Note that it is illegal to call this function
                 * if the reward model does not have state-action rewards.
                 *
                 * @return The state-action reward vector.
                 */
                std::vector<ValueType> const& getStateActionRewardVector() const;

                /*!
                 * Retrieves the state-action rewards of the reward model. Note that it is illegal to call this function
                 * if the reward model does not have state-action rewards.
                 *
                 * @return The state-action reward vector.
                 */
                std::vector<ValueType>& getStateActionRewardVector();

                /*!
                 * Retrieves the state-action reward for the given choice.
                 */
                ValueType const& getStateActionReward(uint_fast64_t choiceIndex) const;

                /*!
                 * Sets the state-action reward for the given choice
                 */
                template<typename T>
                void setStateActionReward(uint_fast64_t choiceIndex, T const& newValue);

                /*!
                 * Retrieves an optional value that contains the state-action reward vector if there is one.
                 *
                 * @return The state-action reward vector if there is one.
                 */
                boost::optional<std::vector<ValueType>> const& getOptionalStateActionRewardVector() const;
                
                /*!
                 * Retrieves whether the reward model has transition rewards.
                 *
                 * @return True iff the reward model has transition rewards.
                 */
                bool hasTransitionRewards() const;
                
                /*!
                 * Retrieves the transition rewards of the reward model. Note that it is illegal to call this function
                 * if the reward model does not have transition rewards.
                 *
                 * @return The transition reward matrix.
                 */
                storm::storage::SparseMatrix<ValueType> const& getTransitionRewardMatrix() const;

                /*!
                 * Retrieves the transition rewards of the reward model. Note that it is illegal to call this function
                 * if the reward model does not have transition rewards.
                 *
                 * @return The transition reward matrix.
                 */
                storm::storage::SparseMatrix<ValueType>& getTransitionRewardMatrix();

                /*!
                 * Retrieves an optional value that contains the transition reward matrix if there is one.
                 *
                 * @return The transition reward matrix if there is one.
                 */
                boost::optional<storm::storage::SparseMatrix<ValueType>> const& getOptionalTransitionRewardMatrix() const;
                
                /*!
                 * Creates a new reward model by restricting the actions of the action-based rewards.
                 *
                 * @param enabledActions A bit vector representing the enabled actions.
                 * @return The restricted reward model.
                 */
                StandardRewardModel<ValueType> restrictActions(storm::storage::BitVector const& enabledActions) const;
                
                /*!
                 * Reduces the transition-based rewards to state-action rewards by taking the average of each row. If
                 * the corresponding flag is set, the state-action rewards and the state rewards are summed so the model
                 * only has a state reward vector left. Note that this transformation only  preserves expected rewards,
                 * but not all reward-based properties.
                 *
                 * @param transitionMatrix The transition matrix that is used to weight the rewards in the reward matrix.
                 */
                template<typename MatrixValueType>
                void reduceToStateBasedRewards(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, bool reduceToStateRewards = false);
                
                /*!
                 * Creates a vector representing the complete reward vector based on the state-, state-action- and
                 * transition-based rewards in the reward model.
                 *
                 * @param transitionMatrix The matrix that is used to weight the values of the transition reward matrix.
                 * @return The full state-action reward vector.
                 */
                template<typename MatrixValueType>
                std::vector<ValueType> getTotalRewardVector(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix) const;

                /*!
                 * Creates a vector representing the complete reward vector based on the state-, state-action- and
                 * transition-based rewards in the reward model.
                 *
                 * @param transitionMatrix The matrix that is used to weight the values of the transition reward matrix.
                 * @param weights A vector used for scaling the entries of the state-action rewards (if present).
                 * @return The full state-action reward vector.
                 */
                template<typename MatrixValueType>
                std::vector<ValueType> getTotalRewardVector(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, std::vector<MatrixValueType> const& weights) const;
                
                /*!
                 * Creates a vector representing the complete reward vector based on the state-, state-action- and
                 * transition-based rewards in the reward model.
                 *
                 * @param transitionMatrix The matrix that is used to weight the values of the transition reward matrix.
                 * @param numberOfRows The total number of rows of the resulting vector.
                 * @param filter A bit vector indicating which row groups to select.
                 * @return The full state-action reward vector.
                 */
                template<typename MatrixValueType>
                std::vector<ValueType> getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, storm::storage::BitVector const& filter) const;
                
                /*!
                 * Creates a vector representing the complete state action reward vector based on the state-, state-action-
                 * and transition-based rewards in the reward model.
                 *
                 * @param numberOfRows The total number of rows of the resulting vector.
                 * @param rowGroupIndices The starting indices of the row groups.
                 * @return The full state-action reward vector.
                 */
                std::vector<ValueType> getTotalStateActionRewardVector(uint_fast64_t numberOfRows, std::vector<uint_fast64_t> const& rowGroupIndices) const;
                
                /*!
                 * Creates a vector representing the complete state action reward vector based on the state- and
                 * state-action rewards in the reward model.
                 *
                 * @param numberOfRows The total number of rows of the resulting vector.
                 * @param rowGroupIndices The starting indices of the row groups.
                 * @param filter A bit vector indicating which row groups to select.
                 * @return The full state-action reward vector.
                 */
                std::vector<ValueType> getTotalStateActionRewardVector(uint_fast64_t numberOfRows, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& filter) const;
                
                /*!
                 * Sets the given value in the state-action reward vector at the given row. This assumes that the reward
                 * model has state-action rewards.
                 *
                 * @param row The row at which to set the given value.
                 * @param value The value to set.
                 */
                void setStateActionRewardValue(uint_fast64_t row, ValueType const& value);
                
                /*!
                 * Retrieves whether the reward model is empty, i.e. contains no state-, state-action- or transition-based
                 * rewards.
                 *
                 * @return True iff the reward model is empty.
                 */
                bool empty() const;
                
                /*!
                 * Retrieves (an approximation of) the size of the model in bytes.
                 *
                 * @return The size of the internal representation of the model measured in bytes.
                 */
                std::size_t getSizeInBytes() const;
                
                template <typename ValueTypePrime>
                friend std::ostream& operator<<(std::ostream& out, StandardRewardModel<ValueTypePrime> const& rewardModel);
                
            private:
                // An (optional) vector representing the state rewards.
                boost::optional<std::vector<ValueType>> optionalStateRewardVector;
                
                // An (optional) vector representing the state-action rewards.
                boost::optional<std::vector<ValueType>> optionalStateActionRewardVector;
                
                // An (optional) matrix representing the transition rewards.
                boost::optional<storm::storage::SparseMatrix<ValueType>> optionalTransitionRewardMatrix;
            };
            
            template <typename ValueType>
            std::ostream& operator<<(std::ostream& out, StandardRewardModel<ValueType> const& rewardModel);
        }
    }
}

#endif /* STORM_MODELS_SPARSE_STANDARDREWARDMODEL_H_ */