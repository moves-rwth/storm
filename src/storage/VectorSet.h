/*
 * VectorSet.h
 *
 *  Created on: 23.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_STORAGE_VECTORSET_H
#define STORM_STORAGE_VECTORSET_H

#include <set>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

#include "src/exceptions/InvalidStateException.h"

namespace storm  {
    namespace storage {
        
        template<typename T>
        class VectorSet {
        public:
            typedef T* difference_type;
            typedef T value_type;
            typedef T* pointer;
            typedef T& reference;
            typedef typename std::vector<T>::iterator iterator;
            typedef typename std::vector<T>::const_iterator const_iterator;
            
            VectorSet() : data(), dirty(false) {
                // Intentionally left empty.
            }
            
            VectorSet(uint_fast64_t size) : data(), dirty(false) {
                data.reserve(size);
            }
            
            VectorSet(std::vector<T> const& data) : data(data), dirty(true) {
                ensureSet();
            }
            
            VectorSet(std::set<T> const& data) : dirty(false) {
                this->data.reserve(data.size());
                for (auto const& element : data) {
                    this->data.push_back(element);
                }
            }
            
            VectorSet(uint_fast64_t from, uint_fast64_t to) : dirty(false) {
                data.reserve(to - from);
                
                for (uint_fast64_t element = from; element < to; ++element) {
                    data.push_back(element);
                }
            }
                        
            template<typename InputIterator>
            VectorSet(InputIterator first, InputIterator last) : data(first, last), dirty(true) {
                ensureSet();
            }
            
            VectorSet(VectorSet const& other) : dirty(false) {
                other.ensureSet();
                data = other.data;
            }
            
            VectorSet& operator=(VectorSet const& other) {
                data = other.data;
                dirty = other.dirty;
                return *this;
            }
            
            VectorSet(VectorSet&& other) : data(std::move(other.data)), dirty(std::move(other.dirty)) {
                // Intentionally left empty.
            }
            
            VectorSet& operator=(VectorSet&& other) {
                data = std::move(other.data);
                dirty = std::move(other.dirty);
                return *this;
            }
            
            bool operator==(VectorSet const& other) const {
                ensureSet();
                if (this->size() != other.size()) return false;
                return std::equal(data.begin(), data.end(), other.begin());
            }
            
            bool operator<(VectorSet const& other) const {
                ensureSet();
                if (this->size() < other.size()) return true;
                if (this->size() > other.size()) return false;
                for (auto it1 = this->begin(), it2 = other.begin(); it1 != this->end(); ++it1, ++it2) {
                    if (*it1 < *it2) return true;
                    if (*it1 > *it2) return false;
                }
                return false;
            }
            
            void ensureSet() const {
                if (dirty) {
                    std::sort(data.begin(), data.end());
                    data.erase(std::unique(data.begin(), data.end()), data.end());
                    dirty = false;
                }
            }
            
            bool contains(T const& element) const {
                ensureSet();
                return std::binary_search(data.begin(), data.end(), element);
            }
            
            bool subsetOf(VectorSet const& other) const {
                ensureSet();
                other.ensureSet();
                return std::includes(other.begin(), other.end(), data.begin(), data.end());
            }
            
            bool supersetOf(VectorSet const& other) const {
                ensureSet();
                return other.subsetOf(*this);
            }
            
            VectorSet intersect(VectorSet const& other) {
                ensureSet();
                other.ensureSet();
                VectorSet result;
                std::set_intersection(data.begin(), data.end(), other.begin(), other.end(), std::inserter(result.data, result.data.end()));
                return result;
            }
            
            VectorSet join(VectorSet const& other) {
                ensureSet();
                other.ensureSet();
                VectorSet result;
                std::set_union(data.begin(), data.end(), other.begin(), other.end(), std::inserter(result.data, result.data.end()));
                return result;
            }
            
            iterator begin() {
                ensureSet();
                return data.begin();
            }
            
            iterator end() {
                ensureSet();
                return data.end();
            }
            
            const_iterator begin() const {
                ensureSet();
                return data.begin();
            }
            
            const_iterator end() const {
                ensureSet();
                return data.end();
            }
            
            T const& min() const {
                if (this->size() == 0) {
                    throw storm::exceptions::InvalidStateException() << "Cannot retrieve minimum of empty set.";
                }
                
                ensureSet();
                return data.first;
            }
            
            T const& max() const {
                if (this->size() == 0) {
                    throw storm::exceptions::InvalidStateException() << "Cannot retrieve minimum of empty set.";
                }
                
                ensureSet();
                return data.back;
            }
            
            void insert(T const& element) {
                data.push_back(element);
                dirty = true;
            }
            
            template<typename InputIterator>
            iterator insert(InputIterator first, InputIterator last) {
                dirty = true;
                return data.insert(data.end(), first, last);
            }
            
            template<typename InputIterator>
            iterator insert(InputIterator pos, T element) {
                dirty = true;
                return data.insert(pos, element);
            }
            
            bool empty() const {
                ensureSet();
                return data.empty();
            }
            
            size_t size() const {
                ensureSet();
                return data.size();
            }
            
            void clear() {
                data.clear();
                dirty = false;
            }
            
            bool erase(T const& element) {
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
            
            void erase(VectorSet const& eraseSet) {
                if (eraseSet.size() > 0) {
                    ensureSet();
                    eraseSet.ensureSet();
                
                    for (typename std::vector<T>::reverse_iterator delIt = eraseSet.data.rbegin(), setIt = data.rbegin(); delIt != eraseSet.data.rend() && setIt != eraseSet.data.rend(); ++delIt) {
                        while (setIt != eraseSet.data.rend() && *setIt > *delIt) {
                            ++setIt;
                        }
                        if (setIt != data.rend()) break;
                        
                        if (*setIt == *delIt) {
                            data.erase((setIt + 1).base());
                            ++setIt;
                        }
                    }
                }
            }
            
            friend std::ostream& operator<< (std::ostream& stream, VectorSet const& set) {
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
            
        private:
            mutable std::vector<T> data;
            mutable bool dirty;
        };
    }
}

#endif /* STORM_STORAGE_VECTORSET_H */
