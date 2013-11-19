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
        
        template<typename ValueType>
        class VectorSet {
        public:
            typedef ValueType* difference_type;
            typedef ValueType value_type;
            typedef ValueType* pointer;
            typedef ValueType& reference;
            typedef typename std::vector<ValueType>::iterator iterator;
            typedef typename std::vector<ValueType>::const_iterator const_iterator;
            
            VectorSet();
            
            VectorSet(uint_fast64_t size);
            
            VectorSet(std::vector<ValueType> const& data);
            
            VectorSet(std::set<ValueType> const& data);
            
            VectorSet(uint_fast64_t from, uint_fast64_t to);
                        
            VectorSet(VectorSet const& other);
            
            VectorSet& operator=(VectorSet const& other);
            
            VectorSet(VectorSet&& other);
            
            VectorSet& operator=(VectorSet&& other);
            
            bool operator==(VectorSet const& other) const;
            
            bool operator<(VectorSet const& other) const;
            
            bool operator>(VectorSet const& other) const;
            
            void ensureSet() const;
            
            bool contains(ValueType const& element) const;
            
            bool subsetOf(VectorSet const& other) const;
            
            bool supersetOf(VectorSet const& other) const;
            
            VectorSet intersect(VectorSet const& other);
            
            VectorSet join(VectorSet const& other);
            
            iterator begin();
            
            iterator end();
            
            const_iterator begin() const;
            
            const_iterator end() const;
            
            ValueType const& min() const;
            
            ValueType const& max() const;
            
            void insert(ValueType const& element);
            
            iterator insert(const_iterator pos, ValueType const& element);
            
            void insert(VectorSet<ValueType> const& other);
            
            bool empty() const;
            
            size_t size() const;
            
            void clear();
            
            bool erase(ValueType const& element);
            
            void erase(VectorSet const& eraseSet);
            
            template<typename ValueTypePrime>
            friend std::ostream& operator<< (std::ostream& stream, VectorSet<ValueTypePrime> const& set);
            
        private:
            mutable std::vector<ValueType> data;
            mutable bool dirty;
        };
    }
}

#endif /* STORM_STORAGE_VECTORSET_H */
