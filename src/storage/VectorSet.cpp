#include "src/storage/VectorSet.h"

namespace storm {
    namespace storage {
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet() : data(), dirty(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(uint_fast64_t size) : data(), dirty(false) {
            data.reserve(size);
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(std::vector<ValueType> const& data) : data(data), dirty(true) {
            ensureSet();
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(std::set<ValueType> const& data) : dirty(false) {
            this->data.reserve(data.size());
            for (auto const& element : data) {
                this->data.push_back(element);
            }
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(uint_fast64_t from, uint_fast64_t to) : dirty(false) {
            data.reserve(to - from);
            
            for (uint_fast64_t element = from; element < to; ++element) {
                data.push_back(element);
            }
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(VectorSet const& other) : dirty(false) {
            other.ensureSet();
            data = other.data;
        }
        
        template<typename ValueType>
        VectorSet<ValueType>& VectorSet<ValueType>::operator=(VectorSet<ValueType> const& other) {
            data = other.data;
            dirty = other.dirty;
            return *this;
        }
        
        template<typename ValueType>
        VectorSet<ValueType>::VectorSet(VectorSet<ValueType>&& other) : data(std::move(other.data)), dirty(std::move(other.dirty)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        VectorSet<ValueType>& VectorSet<ValueType>::operator=(VectorSet&& other) {
            data = std::move(other.data);
            dirty = std::move(other.dirty);
            return *this;
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::operator==(VectorSet<ValueType> const& other) const {
            ensureSet();
            if (this->size() != other.size()) return false;
            return std::equal(data.begin(), data.end(), other.begin());
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::operator<(VectorSet<ValueType> const& other) const {
            ensureSet();
            if (this->size() < other.size()) return true;
            if (this->size() > other.size()) return false;
            for (auto it1 = this->begin(), it2 = other.begin(); it1 != this->end(); ++it1, ++it2) {
                if (*it1 < *it2) return true;
                if (*it1 > *it2) return false;
            }
            return false;
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::operator>(VectorSet<ValueType> const& other) const {
            ensureSet();
            if (this->size() > other.size()) return true;
            if (this->size() < other.size()) return false;
            for (auto it1 = this->begin(), it2 = other.begin(); it1 != this->end(); ++it1, ++it2) {
                if (*it1 > *it2) return true;
                if (*it1 < *it2) return false;
            }
            return false;
        }
        
        template<typename ValueType>
        void VectorSet<ValueType>::ensureSet() const {
            if (dirty) {
                std::sort(data.begin(), data.end());
                data.erase(std::unique(data.begin(), data.end()), data.end());
                dirty = false;
            }
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::contains(ValueType const& element) const {
            ensureSet();
            return std::binary_search(data.begin(), data.end(), element);
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::subsetOf(VectorSet<ValueType> const& other) const {
            ensureSet();
            other.ensureSet();
            return std::includes(other.begin(), other.end(), data.begin(), data.end());
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::supersetOf(VectorSet<ValueType> const& other) const {
            ensureSet();
            return other.subsetOf(*this);
        }
        
        template<typename ValueType>
        VectorSet<ValueType> VectorSet<ValueType>::intersect(VectorSet<ValueType> const& other) {
            ensureSet();
            other.ensureSet();
            VectorSet result;
            std::set_intersection(data.begin(), data.end(), other.begin(), other.end(), std::inserter(result.data, result.data.end()));
            return result;
        }
        
        template<typename ValueType>
        VectorSet<ValueType> VectorSet<ValueType>::join(VectorSet<ValueType> const& other) {
            ensureSet();
            other.ensureSet();
            VectorSet result;
            std::set_union(data.begin(), data.end(), other.begin(), other.end(), std::inserter(result.data, result.data.end()));
            return result;
        }
        
        template<typename ValueType>
        typename VectorSet<ValueType>::iterator VectorSet<ValueType>::begin() {
            ensureSet();
            return data.begin();
        }
        
        template<typename ValueType>
        typename VectorSet<ValueType>::iterator VectorSet<ValueType>::end() {
            ensureSet();
            return data.end();
        }
        
        template<typename ValueType>
        typename VectorSet<ValueType>::const_iterator VectorSet<ValueType>::begin() const {
            ensureSet();
            return data.begin();
        }
        
        template<typename ValueType>
        typename VectorSet<ValueType>::const_iterator VectorSet<ValueType>::end() const {
            ensureSet();
            return data.end();
        }
        
        template<typename ValueType>
        ValueType const& VectorSet<ValueType>::min() const {
            if (this->size() == 0) {
                throw storm::exceptions::InvalidStateException() << "Cannot retrieve minimum of empty set.";
            }
            
            ensureSet();
            return data.front();
        }
        
        template<typename ValueType>
        ValueType const& VectorSet<ValueType>::max() const {
            if (this->size() == 0) {
                throw storm::exceptions::InvalidStateException() << "Cannot retrieve minimum of empty set.";
            }
            
            ensureSet();
            return data.back();
        }
        
        template<typename ValueType>
        void VectorSet<ValueType>::insert(ValueType const& element) {
            data.push_back(element);
            dirty = true;
        }
        
        template<typename ValueType>
        typename VectorSet<ValueType>::iterator VectorSet<ValueType>::insert(typename VectorSet<ValueType>::const_iterator pos, ValueType const& element) {
            dirty = true;
            return data.insert(pos, element);
        }
        
        template<typename ValueType>
        void VectorSet<ValueType>::insert(VectorSet<ValueType> const& other) {
            data.insert(data.end(), other.data.begin(), other.data.end());
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::empty() const {
            ensureSet();
            return data.empty();
        }
        
        template<typename ValueType>
        size_t VectorSet<ValueType>::size() const {
            ensureSet();
            return data.size();
        }
        
        template<typename ValueType>
        void VectorSet<ValueType>::clear() {
            data.clear();
            dirty = false;
        }
        
        template<typename ValueType>
        bool VectorSet<ValueType>::erase(ValueType const& element) {
            ensureSet();
            uint_fast64_t lowerBound = 0;
            uint_fast64_t upperBound = data.size();
            while (lowerBound != upperBound) {
                uint_fast64_t currentPosition = lowerBound + (upperBound - lowerBound) / 2;
                bool searchInLowerHalf = element < data[currentPosition];
                if (searchInLowerHalf) {
                    upperBound = currentPosition;
                } else {
                    bool searchInRightHalf = element > data[currentPosition];
                    if (searchInRightHalf) {
                        lowerBound = currentPosition + 1;
                    } else {
                        // At this point we have found the element.
                        data.erase(data.begin() + currentPosition);
                        return true;
                    }
                }
            }
            return false;
        }
        
        template<typename ValueType>
        void VectorSet<ValueType>::erase(VectorSet<ValueType> const& eraseSet) {
            if (eraseSet.size() > 0 && this->size() > 0) {
                ensureSet();
                eraseSet.ensureSet();
                
                for (typename std::vector<ValueType>::reverse_iterator delIt = eraseSet.data.rbegin(), setIt = data.rbegin(); delIt != eraseSet.data.rend() && setIt != eraseSet.data.rend(); ++delIt) {
                    while (setIt != eraseSet.data.rend() && *setIt > *delIt) {
                        ++setIt;
                    }
                    if (setIt == data.rend()) break;
                    
                    if (*setIt == *delIt) {
                        data.erase((setIt + 1).base());
                        ++setIt;
                    }
                }
            }
        }
        
        template<typename ValueType>
        std::ostream& operator<<(std::ostream& stream, VectorSet<ValueType> const& set) {
            set.ensureSet();
            stream << "VectorSet(" << set.size() << ") { ";
            if (set.size() > 0) {
                for (uint_fast64_t index = 0; index < set.size() - 1; ++index) {
                    stream << set.data[index] << ", ";
                }
                stream << set.data[set.size() - 1] << " }";
            } else {
                stream << "}";
            }
            return stream;
        }
        
        template class VectorSet<uint_fast64_t>;
        template class VectorSet<VectorSet<uint_fast64_t>>;
        template std::ostream& operator<<(std::ostream& stream, VectorSet<uint_fast64_t> const& set);
        template std::ostream& operator<<(std::ostream& stream, VectorSet<VectorSet<uint_fast64_t>> const& set);
    }
}