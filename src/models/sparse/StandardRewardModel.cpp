#include "src/models/sparse/StandardRewardModel.h"

#include "src/utility/vector.h"

#include "src/exceptions/InvalidOperationException.h"

#include "src/adapters/CarlAdapter.h"

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
            bool StandardRewardModel<ValueType>::hasOnlyStateRewards() const {
                return static_cast<bool>(this->optionalStateRewardVector) && !static_cast<bool>(this->optionalStateActionRewardVector) && !static_cast<bool>(this->optionalTransitionRewardMatrix);
            }
            
            template<typename ValueType>
            std::vector<ValueType> const& StandardRewardModel<ValueType>::getStateRewardVector() const {
                assert(this->hasStateRewards());
                return this->optionalStateRewardVector.get();
            }

            template<typename ValueType>
            std::vector<ValueType>& StandardRewardModel<ValueType>::getStateRewardVector() {
                assert(this->hasStateRewards());
                return this->optionalStateRewardVector.get();
            }

            template<typename ValueType>
            boost::optional<std::vector<ValueType>> const& StandardRewardModel<ValueType>::getOptionalStateRewardVector() const {
                return this->optionalStateRewardVector;
            }

            template<typename ValueType>
            ValueType const& StandardRewardModel<ValueType>::getStateReward(uint_fast64_t state) const {
                assert(this->hasStateRewards());
                assert(state < this->optionalStateRewardVector.get().size());
                return this->optionalStateRewardVector.get()[state];
            }

            template<typename ValueType>
            template<typename T>
            void StandardRewardModel<ValueType>::setStateReward(uint_fast64_t state, T const & newReward) {
                assert(this->hasStateRewards());
                assert(state < this->optionalStateRewardVector.get().size());
                this->optionalStateRewardVector.get()[state] = newReward;
            }

//            template<typename ValueType>
//            ValueType StandardRewardModel<ValueType>::maximalStateReward(uint_fast64_t state) const {
//                assert(this->hasStateRewards());
//                assert(!this->optionalStateRewardVector.get().empty());
//                ValueType max = *(this->optionalStateRewardVector.get().begin());
//                for(auto const& entry : this->optionalStateRewardVector.get()) {
//                    if(entry > max) {
//                        max = entry;
//                    }
//                }
//                return max;
//            }


            template<typename ValueType>
            bool StandardRewardModel<ValueType>::hasStateActionRewards() const {
                return static_cast<bool>(this->optionalStateActionRewardVector);
            }
            
            template<typename ValueType>
            std::vector<ValueType> const& StandardRewardModel<ValueType>::getStateActionRewardVector() const {
                assert(this->hasStateActionRewards());
                return this->optionalStateActionRewardVector.get();
            }
            
            template<typename ValueType>
            std::vector<ValueType>& StandardRewardModel<ValueType>::getStateActionRewardVector() {
                assert(this->hasStateActionRewards());
                return this->optionalStateActionRewardVector.get();
            }

            template<typename ValueType>
            ValueType const& StandardRewardModel<ValueType>::getStateActionReward(uint_fast64_t choiceIndex) const {
                assert(this->hasStateActionRewards());
                assert(choiceIndex < this->optionalStateActionRewardVector.get().size());
                return this->optionalStateActionRewardVector.get()[choiceIndex];
            }

            template<typename ValueType>
            template<typename T>
            void StandardRewardModel<ValueType>::setStateActionReward(uint_fast64_t choiceIndex, T const &newValue) {
                assert(this->hasStateActionRewards());
                assert(choiceIndex < this->optionalStateActionRewardVector.get().size());
                this->optionalStateActionRewardVector.get()[choiceIndex] = newValue;
            }

//            template<typename ValueType>
//            ValueType  StandardRewardModel<ValueType>::maximalStateActionReward() const {
//                assert(this->hasStateActionRewards());
//                assert(!this->optionalStateActionRewardVector.get().empty());
//                ValueType  max = *(this->optionalStateActionRewardVector.get().begin());
//                for(auto const& entry : this->optionalStateActionRewardVector.get()) {
//                    if(entry > max) {
//                        max = entry;
//                    }
//                }
//                return max;
//            }

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
            storm::storage::SparseMatrix<ValueType>& StandardRewardModel<ValueType>::getTransitionRewardMatrix() {
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
            void StandardRewardModel<ValueType>::reduceToStateBasedRewards(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, bool reduceToStateRewards) {
                if (this->hasTransitionRewards()) {
                    if (this->hasStateActionRewards()) {
                        storm::utility::vector::addVectors<ValueType>(this->getStateActionRewardVector(), transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix()), this->getStateActionRewardVector());
                    } else {
                        this->optionalStateActionRewardVector = transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix());
                    }
                }
                
                if (reduceToStateRewards && this->hasStateActionRewards()) {
                    if (this->hasStateRewards()) {
                        STORM_LOG_THROW(this->getStateRewardVector().size() == this->getStateActionRewardVector().size(), storm::exceptions::InvalidOperationException, "The reduction to state rewards is only possible of both the state and the state-action rewards have the same dimension.");
                        storm::utility::vector::addVectors<ValueType>(this->getStateActionRewardVector(), this->getStateRewardVector(), this->getStateRewardVector());
                    } else {
                        this->optionalStateRewardVector = std::move(this->optionalStateRewardVector);
                    }
                }
            }
            
            template<typename ValueType>
            template<typename MatrixValueType>
            std::vector<ValueType> StandardRewardModel<ValueType>::getTotalRewardVector(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix) const {
                std::vector<ValueType> result = this->hasTransitionRewards() ? transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix()) : (this->hasStateActionRewards() ? this->getStateActionRewardVector() : std::vector<ValueType>(transitionMatrix.getRowCount()));
                if (this->hasStateActionRewards() && this->hasTransitionRewards()) {
                    storm::utility::vector::addVectors(result, this->getStateActionRewardVector(), result);
                }
                if (this->hasStateRewards()) {
                    storm::utility::vector::addVectorToGroupedVector(result, this->getStateRewardVector(), transitionMatrix.getRowGroupIndices());
                }
                return result;
            }
            
            template<typename ValueType>
            template<typename MatrixValueType>
            std::vector<ValueType> StandardRewardModel<ValueType>::getTotalRewardVector(storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, std::vector<MatrixValueType> const& weights) const {
                std::vector<ValueType> result = this->hasTransitionRewards() ? transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix()) : (this->hasStateActionRewards() ? this->getStateActionRewardVector() : std::vector<ValueType>(transitionMatrix.getRowCount()));
                if (!this->hasTransitionRewards() && this->hasStateActionRewards()) {
                    // If we initialized the result with the state-action rewards we can scale the result in place.
                    storm::utility::vector::multiplyVectorsPointwise(result, weights, result);
                }
                if (this->hasStateActionRewards() && this->hasTransitionRewards()) {
                    // If we initialized the result with the transition rewards and still have state-action rewards,
                    // we need to add the scaled vector directly.
                    storm::utility::vector::applyPointwise<MatrixValueType, ValueType, ValueType>(weights, this->getStateActionRewardVector(), result, [] (MatrixValueType const& weight, ValueType const& rewardElement, ValueType const& resultElement) { return resultElement + weight * rewardElement; } );
                }
                if (this->hasStateRewards()) {
                    storm::utility::vector::addVectorToGroupedVector(result, this->getStateRewardVector(), transitionMatrix.getRowGroupIndices());
                }
                return result;
            }
            
            template<typename ValueType>
            template<typename MatrixValueType>
            std::vector<ValueType> StandardRewardModel<ValueType>::getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<MatrixValueType> const& transitionMatrix, storm::storage::BitVector const& filter) const {
                std::vector<ValueType> result(numberOfRows);
                if (this->hasTransitionRewards()) {
                    std::vector<ValueType> pointwiseProductRowSumVector = transitionMatrix.getPointwiseProductRowSumVector(this->getTransitionRewardMatrix());
                    storm::utility::vector::selectVectorValues(result, filter, transitionMatrix.getRowGroupIndices(), pointwiseProductRowSumVector);
                }

                if (this->hasStateActionRewards()) {
                    storm::utility::vector::addFilteredVectorGroupsToGroupedVector(result, this->getStateActionRewardVector(), filter, transitionMatrix.getRowGroupIndices());
                }
                if (this->hasStateRewards()) {
                    storm::utility::vector::addFilteredVectorToGroupedVector(result, this->getStateRewardVector(), filter, transitionMatrix.getRowGroupIndices());
                }
                return result;
            }
            
            template<typename ValueType>
            std::vector<ValueType> StandardRewardModel<ValueType>::getTotalStateActionRewardVector(uint_fast64_t numberOfRows, std::vector<uint_fast64_t> const& rowGroupIndices) const {
                std::vector<ValueType> result = this->hasStateActionRewards() ? this->getStateActionRewardVector() : std::vector<ValueType>(numberOfRows);
                if (this->hasStateRewards()) {
                    storm::utility::vector::addVectorToGroupedVector(result, this->getStateRewardVector(), rowGroupIndices);
                }
                return result;
            }
            
            template<typename ValueType>
            std::vector<ValueType> StandardRewardModel<ValueType>::getTotalStateActionRewardVector(uint_fast64_t numberOfRows, std::vector<uint_fast64_t> const& rowGroupIndices, storm::storage::BitVector const& filter) const {
                std::vector<ValueType> result(numberOfRows);
                if (this->hasStateRewards()) {
                    storm::utility::vector::selectVectorValuesRepeatedly(result, filter, rowGroupIndices, this->getStateRewardVector());
                }
                if (this->hasStateActionRewards()) {
                    storm::utility::vector::addFilteredVectorGroupsToGroupedVector(result, this->getStateActionRewardVector(), filter, rowGroupIndices);
                }
                return result;
            }
            
            template<typename ValueType>
            void StandardRewardModel<ValueType>::setStateActionRewardValue(uint_fast64_t row, ValueType const& value) {
                this->optionalStateActionRewardVector.get()[row] = value;
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
            
            template <typename ValueType>
            std::ostream& operator<<(std::ostream& out, StandardRewardModel<ValueType> const& rewardModel) {
                out << std::boolalpha << "reward model [state reward: "
                << rewardModel.hasStateRewards()
                << ", state-action rewards: "
                << rewardModel.hasStateActionRewards()
                << ", transition rewards: "
                << rewardModel.hasTransitionRewards()
                << "]"
                << std::noboolalpha;
                return out;
            }
            
            // Explicitly instantiate the class.
            template std::vector<double> StandardRewardModel<double>::getTotalRewardVector(storm::storage::SparseMatrix<double> const& transitionMatrix) const;
            template std::vector<double> StandardRewardModel<double>::getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::BitVector const& filter) const;
            template std::vector<double> StandardRewardModel<double>::getTotalRewardVector(storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& weights) const;
            template void StandardRewardModel<double>::reduceToStateBasedRewards(storm::storage::SparseMatrix<double> const& transitionMatrix, bool reduceToStateRewards);
            template void StandardRewardModel<double>::setStateActionReward(uint_fast64_t choiceIndex, double const & newValue);
            template void StandardRewardModel<double>::setStateReward(uint_fast64_t state, double const & newValue);
            template class StandardRewardModel<double>;
            template std::ostream& operator<<<double>(std::ostream& out, StandardRewardModel<double> const& rewardModel);
            
            template std::vector<float> StandardRewardModel<float>::getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<float> const& transitionMatrix, storm::storage::BitVector const& filter) const;
            template std::vector<float> StandardRewardModel<float>::getTotalRewardVector(storm::storage::SparseMatrix<float> const& transitionMatrix) const;
            template std::vector<float> StandardRewardModel<float>::getTotalRewardVector(storm::storage::SparseMatrix<float> const& transitionMatrix, std::vector<float> const& weights) const;
            template void StandardRewardModel<float>::reduceToStateBasedRewards(storm::storage::SparseMatrix<float> const& transitionMatrix, bool reduceToStateRewards);
            template void StandardRewardModel<float>::setStateActionReward(uint_fast64_t choiceIndex, float const & newValue);
            template void StandardRewardModel<float>::setStateReward(uint_fast64_t state, float const & newValue);
            template class StandardRewardModel<float>;
            template std::ostream& operator<<<float>(std::ostream& out, StandardRewardModel<float> const& rewardModel);
            
#ifdef STORM_HAVE_CARL
            template std::vector<storm::RationalFunction> StandardRewardModel<storm::RationalFunction>::getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix, storm::storage::BitVector const& filter) const;
            template std::vector<storm::RationalFunction> StandardRewardModel<storm::RationalFunction>::getTotalRewardVector(storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix) const;
            template std::vector<storm::RationalFunction> StandardRewardModel<storm::RationalFunction>::getTotalRewardVector(storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix, std::vector<storm::RationalFunction> const& weights) const;
            template void StandardRewardModel<storm::RationalFunction>::reduceToStateBasedRewards(storm::storage::SparseMatrix<storm::RationalFunction> const& transitionMatrix, bool reduceToStateRewards);
            template void StandardRewardModel<storm::RationalFunction>::setStateActionReward(uint_fast64_t choiceIndex, storm::RationalFunction const & newValue);
            template void StandardRewardModel<storm::RationalFunction>::setStateReward(uint_fast64_t state, storm::RationalFunction const & newValue);
            template class StandardRewardModel<storm::RationalFunction>;
            template std::ostream& operator<<<storm::RationalFunction>(std::ostream& out, StandardRewardModel<storm::RationalFunction> const& rewardModel);
            
            template std::vector<storm::Interval> StandardRewardModel<storm::Interval>::getTotalRewardVector(uint_fast64_t numberOfRows, storm::storage::SparseMatrix<double> const& transitionMatrix, storm::storage::BitVector const& filter) const;
            template std::vector<storm::Interval> StandardRewardModel<storm::Interval>::getTotalRewardVector(storm::storage::SparseMatrix<double> const& transitionMatrix) const;
            template std::vector<storm::Interval> StandardRewardModel<storm::Interval>::getTotalRewardVector(storm::storage::SparseMatrix<double> const& transitionMatrix, std::vector<double> const& weights) const;
            template void StandardRewardModel<storm::Interval>::setStateActionReward(uint_fast64_t choiceIndex, double const & newValue);
            template void StandardRewardModel<storm::Interval>::setStateActionReward(uint_fast64_t choiceIndex, storm::Interval const & newValue);
            template void StandardRewardModel<storm::Interval>::setStateReward(uint_fast64_t state, double const & newValue);
            template void StandardRewardModel<storm::Interval>::setStateReward(uint_fast64_t state, storm::Interval const & newValue);
            template void StandardRewardModel<storm::Interval>::reduceToStateBasedRewards(storm::storage::SparseMatrix<double> const& transitionMatrix, bool reduceToStateRewards);
            template class StandardRewardModel<storm::Interval>;
            template std::ostream& operator<<<storm::Interval>(std::ostream& out, StandardRewardModel<storm::Interval> const& rewardModel);
#endif
        }
        
    }
}