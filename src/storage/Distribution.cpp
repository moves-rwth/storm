#include "src/storage/Distribution.h"

#include <algorithm>
#include <iostream>

#include "src/utility/macros.h"
#include "src/utility/constants.h"
#include "src/utility/ConstantsComparator.h"

#include "src/settings/SettingsManager.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        Distribution<ValueType>::Distribution() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool Distribution<ValueType>::equals(Distribution<ValueType> const& other, storm::utility::ConstantsComparator<ValueType> const& comparator) const {
            // We need to check equality by ourselves, because we need to account for epsilon differences.
            if (this->distribution.size() != other.distribution.size()) {
                return false;
            }
            
            auto first1 = this->distribution.begin();
            auto last1 = this->distribution.end();
            auto first2 = other.distribution.begin();
            
            for (; first1 != last1; ++first1, ++first2) {
                if (first1->first != first2->first) {
                    std::cout << "false in first" << std::endl;
                    return false;
                }
                if (!comparator.isEqual(first1->second, first2->second)) {
                    std::cout << "false in second " << std::setprecision(15) << first1->second << " vs " << first2->second << std::endl;
                    return false;
                }
            }
            
            return true;
        }
        
        template<typename ValueType>
        void Distribution<ValueType>::addProbability(storm::storage::sparse::state_type const& state, ValueType const& probability) {
            auto it = this->distribution.find(state);
            if (it == this->distribution.end()) {
                this->distribution.emplace_hint(it, state, probability);
            } else {
                it->second += probability;
            }
        }
        
        template<typename ValueType>
        void Distribution<ValueType>::removeProbability(storm::storage::sparse::state_type const& state, ValueType const& probability, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            auto it = this->distribution.find(state);
            STORM_LOG_ASSERT(it != this->distribution.end(), "Cannot remove probability, because the state is not in the support of the distribution.");
            it->second -= probability;
            if (comparator.isZero(it->second)) {
                this->distribution.erase(it);
            }
        }
        
        template<typename ValueType>
        void Distribution<ValueType>::shiftProbability(storm::storage::sparse::state_type const& fromState, storm::storage::sparse::state_type const& toState, ValueType const& probability, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            removeProbability(fromState, probability, comparator);
            addProbability(toState, probability);
        }
        
        template<typename ValueType>
        typename Distribution<ValueType>::iterator Distribution<ValueType>::begin() {
            return this->distribution.begin();
        }

        template<typename ValueType>
        typename Distribution<ValueType>::const_iterator Distribution<ValueType>::begin() const {
            return this->distribution.begin();
        }
        
        template<typename ValueType>
        typename Distribution<ValueType>::iterator Distribution<ValueType>::end() {
            return this->distribution.end();
        }
        
        template<typename ValueType>
        typename Distribution<ValueType>::const_iterator Distribution<ValueType>::end() const {
            return this->distribution.end();
        }
        
        template<typename ValueType>
        void Distribution<ValueType>::scale(storm::storage::sparse::state_type const& state) {
            auto probabilityIterator = this->distribution.find(state);
            if (probabilityIterator != this->distribution.end()) {
                ValueType scaleValue = storm::utility::one<ValueType>() / probabilityIterator->second;
                this->distribution.erase(probabilityIterator);
                
                for (auto& entry : this->distribution) {
                    entry.second *= scaleValue;
                }
            }
        }
                
        template<typename ValueType>
        std::size_t Distribution<ValueType>::size() const {
            return this->distribution.size();
        }
        
        template<typename ValueType>
        std::ostream& operator<<(std::ostream& out, Distribution<ValueType> const& distribution) {
            out << "{";
            for (auto const& entry : distribution) {
                out << "[" << entry.second << ": " << entry.first << "], ";
            }
            out << "}";
            
            return out;
        }
        
        template<typename ValueType>
        bool Distribution<ValueType>::less(Distribution<ValueType> const& other, storm::utility::ConstantsComparator<ValueType> const& comparator) const {
            if (this->size() != other.size()) {
                return this->size() < other.size();
            }
            auto firstIt = this->begin();
            auto firstIte = this->end();
            auto secondIt = other.begin();
            for (; firstIt != firstIte; ++firstIt, ++secondIt) {
                // If the two blocks already differ, we can decide which distribution is smaller.
                if (firstIt->first != secondIt->first) {
                    return firstIt->first < secondIt->first;
                }
                
                // If the blocks are the same, but the probability differs, we can also decide which distribution is smaller.
                if (!comparator.isEqual(firstIt->second, secondIt->second)) {
                    return comparator.isLess(firstIt->second, secondIt->second);
                }
            }
            return false;
        }
        
        template class Distribution<double>;
        template std::ostream& operator<<(std::ostream& out, Distribution<double> const& distribution);
        
#ifdef STORM_HAVE_CARL
        template class Distribution<storm::RationalFunction>;
        template std::ostream& operator<<(std::ostream& out, Distribution<storm::RationalFunction> const& distribution);
#endif
    }
}
