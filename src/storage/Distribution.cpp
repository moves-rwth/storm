#include "src/storage/Distribution.h"

#include <algorithm>

namespace storm {
    namespace storage {
        template<typename ValueType>
        void Distribution<ValueType>::addProbability(storm::storage::sparse::state_type const& state, ValueType const& probability) {
            this->distribution[state] += probability;
            this->hash += static_cast<std::size_t>(state);
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
        
        template class Distribution<double>;
    }
}