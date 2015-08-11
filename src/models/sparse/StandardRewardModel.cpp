#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/vector.h"

#include "src/exceptions/InvalidOperationException.h"

namespace storm {
    namespace models {
        namespace sparse {
            template<typename ValueType>
            StandardRewardModel<ValueType>::StandardRewardModel(boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                                                boost::optional<std::vector<ValueType>> const& optionalStateActionRewardVector,
                                                                boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix)
            : optionalStateRewardVector(optionalStateRewardVector), optionalStateActionRewardVector(optionalStateActionRewardVector), optionalTransitionRewardMatrix(optionalTransitionRewardMatrix) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            StandardRewardModel<ValueType>::StandardRewardModel(boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                                                boost::optional<std::vector<ValueType>>&& optionalStateActionRewardVector,
                                                                boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix)
            : optionalStateRewardVector(std::move(optionalStateRewardVector)), optionalStateActionRewardVector(std::move(optionalStateActionRewardVector)), optionalTransitionRewardMatrix(std::move(optionalTransitionRewardMatrix)) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            bool StandardRewardModel<ValueType>::hasStateRewards() const {
                return static_cast<bool>(this->optionalStateRewardVector);
            }
            
            template<typename ValueType>
            std::vector<ValueType> const& StandardRewardModel<ValueType>::getStateRewardVector() const {
                return this->optionalStateRewardVector.get();
            }
            
            template<typename ValueType>
            boost::optional<std::vector<ValueType>> const& StandardRewardModel<ValueType>::getOptionalStateRewardVector() const {
                return this->optionalStateRewardVector;
            }
            
            template<typename ValueType>
            bool StandardRewardModel<ValueType>::hasStateActionRewards() const {
                return static_cast<bool>(this->optionalStateActionRewardVector);
            }
            
            template<typename ValueType>
            std::vector<ValueType> const& StandardRewardModel<ValueType>::getStateActionRewardVector() const {
                return this->optionalStateActionRewardVector.get();
            }
            
            template<typename ValueType>
            boost::optional<std::vector<ValueType>> const& StandardRewardModel<ValueType>::getOptionalStateActionRewardVector() const {
                return this->optionalStateActionRewardVector;
            }

            template<typename ValueType>
            bool StandardRewardModel<ValueType>::hasTransitionRewards() const {
                return static_cast<bool>(this->optionalTransitionRewardMatrix);
            }
            
            template<typename ValueType>
            storm::storage::SparseMatrix<ValueType> const& StandardRewardModel<ValueType>::getTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix.get();
            }

            template<typename ValueType>
            boost::optional<storm::storage::SparseMatrix<ValueType>> const& StandardRewardModel<ValueType>::getOptionalTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix;
            }
            
            template<typename ValueType>
            StandardRewardModel<ValueType> StandardRewardModel<ValueType>::restrictActions(storm::storage::BitVector const& enabledActions) const {
                boost::optional<std::vector<ValueType>> newStateRewardVector(this->getOptionalStateRewardVector());
                boost::optional<std::vector<ValueType>> newStateActionRewardVector;
                if (this->hasStateActionRewards()) {
                    newStateActionRewardVector = std::vector<ValueType>(enabledActions.getNumberOfSetBits());
                    storm::utility::vector::selectVectorValues(newStateActionRewardVector.get(), enabledActions, this->getStateActionRewardVector());
                }
                boost::optional<storm::storage::SparseMatrix<ValueType>> newTransitionRewardMatrix;
                if (this->hasTransitionRewards()) {
                    newTransitionRewardMatrix = this->getTransitionRewardMatrix().restrictRows(enabledActions);
                }
                return StandardRewardModel(std::move(newStateRewardVector), std::move(newStateActionRewardVector), std::move(newTransitionRewardMatrix));
            }
            
            template<typename ValueType>
            template<typename MatrixValueType>
            void StandardRewardModel<ValueType>::convertTransitionRewardsToStateActionRewards(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix) {
                STORM_LOG_THROW(this->hasTransitionRewards(), storm::exceptions::InvalidOperationException, "Cannot reduce non-existant transition rewards to state rewards.");
                if (this->hasStateActionRewards()) {
                    storm::utility::vector::addVectors(this->getStateActionRewardVector(), transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix()), this->getStateActionRewardVector);
                } else {
                    this->optionalStateActionRewardVector = transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix());
                }
                this->optionalTransitionRewardMatrix = boost::optional<storm::storage::SparseMatrix<ValueType>>();
            }
            
            template<typename ValueType>
            bool StandardRewardModel<ValueType>::empty() const {
                return !(static_cast<bool>(this->optionalStateRewardVector) || static_cast<bool>(this->optionalStateActionRewardVector) || static_cast<bool>(this->optionalTransitionRewardMatrix));
            }
            
            template<typename ValueType>
            std::size_t StandardRewardModel<ValueType>::getSizeInBytes() const {
                std::size_t result = 0;
                if (this->hasStateRewards()) {
                    result += this->getStateRewardVector().size() * sizeof(ValueType);
                }
                if (this->hasStateActionRewards()) {
                    result += this->getStateActionRewardVector().size() * sizeof(ValueType);
                }
                if (this->hasTransitionRewards()) {
                    result += this->getTransitionRewardMatrix().getSizeInBytes();
                }
                return result;
            }
            
            // Explicitly instantiate the class.
            template class StandardRewardModel<double>;
        }
    }
}