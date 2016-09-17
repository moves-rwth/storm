#include "src/generator/Choice.h"

#include "src/adapters/CarlAdapter.h"

#include "src/utility/constants.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType>
        Choice<ValueType, StateType>::Choice(uint_fast64_t actionIndex, bool markovian) : markovian(markovian), actionIndex(actionIndex), distribution(), totalMass(storm::utility::zero<ValueType>()), rewards(), labels() {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        typename storm::storage::Distribution<ValueType, StateType>::iterator Choice<ValueType, StateType>::begin() {
            return distribution.begin();
        }
        
        template<typename ValueType, typename StateType>
        typename storm::storage::Distribution<ValueType, StateType>::const_iterator Choice<ValueType, StateType>::begin() const {
            return distribution.cbegin();
        }
        
        template<typename ValueType, typename StateType>
        typename storm::storage::Distribution<ValueType, StateType>::iterator Choice<ValueType, StateType>::end() {
            return distribution.end();
        }
        
        template<typename ValueType, typename StateType>
        typename storm::storage::Distribution<ValueType, StateType>::const_iterator Choice<ValueType, StateType>::end() const {
            return distribution.cend();
        }
        
        template<typename ValueType, typename StateType>
        void Choice<ValueType, StateType>::addLabel(uint_fast64_t label) {
            if (!labels) {
                labels = LabelSet();
            }
            labels->insert(label);
        }
        
        template<typename ValueType, typename StateType>
        void Choice<ValueType, StateType>::addLabels(LabelSet const& labelSet) {
            if (!labels) {
                labels = LabelSet();
            }
            labels->insert(labelSet.begin(), labelSet.end());
        }
        
        template<typename ValueType, typename StateType>
        boost::container::flat_set<uint_fast64_t> const& Choice<ValueType, StateType>::getLabels() const {
            return *labels;
        }
        
        template<typename ValueType, typename StateType>
        uint_fast64_t Choice<ValueType, StateType>::getActionIndex() const {
            return actionIndex;
        }
        
        template<typename ValueType, typename StateType>
        ValueType Choice<ValueType, StateType>::getTotalMass() const {
            return totalMass;
        }
        
        template<typename ValueType, typename StateType>
        void Choice<ValueType, StateType>::addProbability(StateType const& state, ValueType const& value) {
            totalMass += value;
            distribution.addProbability(state, value);
        }
        
        template<typename ValueType, typename StateType>
        void Choice<ValueType, StateType>::addReward(ValueType const& value) {
            rewards.push_back(value);
        }
        
        template<typename ValueType, typename StateType>
        void Choice<ValueType, StateType>::addRewards(std::vector<ValueType>&& values) {
            this->rewards = std::move(values);
        }
        
        template<typename ValueType, typename StateType>
        std::vector<ValueType> const& Choice<ValueType, StateType>::getRewards() const {
            return rewards;
        }
        
        template<typename ValueType, typename StateType>
        bool Choice<ValueType, StateType>::isMarkovian() const {
            return markovian;
        }
        
        template<typename ValueType, typename StateType>
        std::size_t Choice<ValueType, StateType>::size() const {
            return distribution.size();
        }
        
        template<typename ValueType, typename StateType>
        std::ostream& operator<<(std::ostream& out, Choice<ValueType, StateType> const& choice) {
            out << "<";
            for (auto const& stateProbabilityPair : choice) {
                out << stateProbabilityPair.first << " : " << stateProbabilityPair.second << ", ";
            }
            out << ">";
            return out;
        }
        
        template class Choice<double>;

#ifdef STORM_HAVE_CARL
        template class Choice<storm::RationalNumber>;
        template class Choice<storm::RationalFunction>;
#endif
    }
}
