#include "src/storage/Distribution.h"

#include <algorithm>
#include <iostream>

#include "src/settings/SettingsManager.h"

namespace storm {
    namespace storage {
        
        template<typename ValueType>
        Distribution<ValueType>::Distribution() : hash(0) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool Distribution<ValueType>::operator==(Distribution<ValueType> const& other) const {
            // We need to check equality by ourselves, because we need to account for epsilon differences.
            if (this->distribution.size() != other.distribution.size() || this->getHash() != other.getHash()) {
                return false;
            }
            
            auto first1 = this->distribution.begin();
            auto last1 = this->distribution.end();
            auto first2 = other.distribution.begin();
            auto last2 = other.distribution.end();
            
            for (; first1 != last1; ++first1, ++first2) {
                if (first1->first != first2->first) {
                    return false;
                }
                if (std::abs(first1->second - first2->second) > 1e-6) {
                    return false;
                }
            }
            
            return true;
        }
        
        template<typename ValueType>
        void Distribution<ValueType>::addProbability(storm::storage::sparse::state_type const& state, ValueType const& probability) {
            if (this->distribution.find(state) == this->distribution.end()) {
                this->hash += static_cast<std::size_t>(state);
            }
            this->distribution[state] += probability;
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
        std::size_t Distribution<ValueType>::getHash() const {
            return this->hash ^ (this->distribution.size() << 8);
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
        
        template class Distribution<double>;
        template std::ostream& operator<<(std::ostream& out, Distribution<double> const& distribution);
    }
}
