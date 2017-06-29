#include "storm/builder/jit/Choice.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/constants.h"

namespace storm {
    namespace builder {
        namespace jit {
            
            template <typename IndexType, typename ValueType>
            Choice<IndexType, ValueType>::Choice(bool markovian) : markovian(markovian) {
                // Intentionally left empty.
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::setMarkovian(bool value) {
                markovian = value;
            }
            
            template <typename IndexType, typename ValueType>
            bool Choice<IndexType, ValueType>::isMarkovian() const {
                return markovian;
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(DistributionEntry<IndexType, ValueType> const& entry) {
                distribution.add(entry);
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(IndexType const& index, ValueType const& value) {
                distribution.add(index, value);
            }

            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::add(Choice<IndexType, ValueType>&& choice) {
                distribution.add(std::move(choice.getMutableDistribution()));
            }
            
            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType> const& Choice<IndexType, ValueType>::getDistribution() const {
                return distribution;
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::divideDistribution(ValueType const& value) {
                distribution.divide(value);
            }

            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::addReward(ValueType const& value) {
                rewards.push_back(value);
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::addReward(uint64_t index, ValueType const& value) {
                rewards[index] += value;
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::addRewards(std::vector<ValueType>&& values) {
                rewards = std::move(values);
            }
            
            template <typename IndexType, typename ValueType>
            std::vector<ValueType> const& Choice<IndexType, ValueType>::getRewards() const {
                return rewards;
            }
            
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::setRewards(std::vector<ValueType>&& rewards) {
                this->rewards = std::move(rewards);
            }
        
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::resizeRewards(std::size_t numberOfRewards) {
                rewards.resize(numberOfRewards, storm::utility::zero<ValueType>());
            }
            
            template <typename IndexType, typename ValueType>
            std::size_t Choice<IndexType, ValueType>::getNumberOfRewards() const {
                return rewards.size();
            }
        
            template <typename IndexType, typename ValueType>
            void Choice<IndexType, ValueType>::compress() {
                distribution.compress();
            }

            template <typename IndexType, typename ValueType>
            Distribution<IndexType, ValueType>& Choice<IndexType, ValueType>::getMutableDistribution() {
                return distribution;
            }
            
            template class Choice<uint32_t, double>;
            template class Choice<uint32_t, storm::RationalNumber>;
            template class Choice<uint32_t, storm::RationalFunction>;

        }
    }
}
