#include "storm/storage/BitVectorHashMap.h"

#include <algorithm>
#include <iostream>
#include <algorithm>

#include "storm/utility/macros.h"
#include "storm/exceptions/InternalException.h"

namespace storm {
    namespace storage {
        template<class ValueType, class Hash>
        BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::BitVectorHashMapIterator(BitVectorHashMap const& map, BitVector::const_iterator indexIt) : map(map), indexIt(indexIt) {
            // Intentionally left empty.
        }
        
        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::operator==(BitVectorHashMapIterator const& other) {
            return &map == &other.map && *indexIt == *other.indexIt;
        }

        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::operator!=(BitVectorHashMapIterator const& other) {
            return !(*this == other);
        }
        
        template<class ValueType, class Hash>
        typename BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator& BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::operator++(int) {
            ++indexIt;
            return *this;
        }

        template<class ValueType, class Hash>
        typename BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator& BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::operator++() {
            ++indexIt;
            return *this;
        }
        
        template<class ValueType, class Hash>
        std::pair<storm::storage::BitVector, ValueType> BitVectorHashMap<ValueType, Hash>::BitVectorHashMapIterator::operator*() const {
            return map.getBucketAndValue(*indexIt);
        }
                
        template<class ValueType, class Hash>
        BitVectorHashMap<ValueType, Hash>::BitVectorHashMap(uint64_t bucketSize, uint64_t initialSize, double loadFactor) : loadFactor(loadFactor), bucketSize(bucketSize), currentSize(1), numberOfElements(0) {
            STORM_LOG_ASSERT(bucketSize % 64 == 0, "Bucket size must be a multiple of 64.");

            while (initialSize > 0) {
                ++currentSize;
                initialSize >>= 1;
            }
            
            // Create the underlying containers.
            buckets = storm::storage::BitVector(bucketSize * (1ull << currentSize));
            occupied = storm::storage::BitVector(1ull << currentSize);
            values = std::vector<ValueType>(1ull << currentSize);

#ifndef NDEBUG
            numberOfInsertions = 0;
            numberOfInsertionProbingSteps = 0;
            numberOfFinds = 0;
            numberOfFindProbingSteps = 0;
#endif
        }
        
        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::isBucketOccupied(uint_fast64_t bucket) const {
            return occupied.get(bucket);
        }
        
        template<class ValueType, class Hash>
        uint64_t BitVectorHashMap<ValueType, Hash>::size() const {
            return numberOfElements;
        }
        
        template<class ValueType, class Hash>
        uint64_t BitVectorHashMap<ValueType, Hash>::capacity() const {
            return 1ull << currentSize;
        }
        
        template<class ValueType, class Hash>
        void BitVectorHashMap<ValueType, Hash>::increaseSize() {
            ++currentSize;
#ifndef NDEBUG
            STORM_LOG_TRACE("Increasing size of hash map from " << (1ull << (currentSize - 1)) << " to " << (1ull << currentSize) << ". Stats: " << numberOfFinds << " finds (avg. " << (numberOfFindProbingSteps / static_cast<double>(numberOfFinds)) << " probing steps), " << numberOfInsertions << " insertions (avg. " << (numberOfInsertionProbingSteps / static_cast<double>(numberOfInsertions)) << " probing steps).");
#else
            STORM_LOG_TRACE("Increasing size of hash map from " << (1ull << (currentSize - 1)) << " to " << (1ull << currentSize) << ".");
#endif
            
            // Create new containers and swap them with the old ones.
            storm::storage::BitVector oldBuckets(bucketSize * (1ull << currentSize));
            std::swap(oldBuckets, buckets);
            storm::storage::BitVector oldOccupied = storm::storage::BitVector(1ull << currentSize);
            std::swap(oldOccupied, occupied);
            std::vector<ValueType> oldValues = std::vector<ValueType>(1ull << currentSize);
            std::swap(oldValues, values);
            
            // Now iterate through the elements and reinsert them in the new storage.
            numberOfElements = 0;
            for (auto bucketIndex : oldOccupied) {
                findOrAddAndGetBucket(oldBuckets.get(bucketIndex * bucketSize, bucketSize), oldValues[bucketIndex]);
            }
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::findOrAdd(storm::storage::BitVector const& key, ValueType const& value) {
            return findOrAddAndGetBucket(key, value).first;
        }
        
        template<class ValueType, class Hash>
        std::pair<ValueType, uint64_t> BitVectorHashMap<ValueType, Hash>::findOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType const& value) {
            checkIncreaseSize();
            
            std::pair<bool, uint64_t> flagAndBucket = this->findBucketToInsert(key);
            if (flagAndBucket.first) {
                return std::make_pair(values[flagAndBucket.second], flagAndBucket.second);
            } else {
                // Insert the new bits into the bucket.
                buckets.set(flagAndBucket.second * bucketSize, key);
                occupied.set(flagAndBucket.second);
                values[flagAndBucket.second] = value;
                ++numberOfElements;
                return std::make_pair(value, flagAndBucket.second);
            }
        }
        
        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::checkIncreaseSize() {
            // If the load of the map is too high, we increase the size.
            if (numberOfElements >= loadFactor * (1ull << currentSize)) {
                this->increaseSize();
                return true;
            }
            return false;
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::getValue(storm::storage::BitVector const& key) const {
            std::pair<bool, uint64_t> flagBucketPair = this->findBucket(key);
            STORM_LOG_ASSERT(flagBucketPair.first, "Unknown key.");
            return values[flagBucketPair.second];
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::getValue(uint64_t bucket) const {
            return values[bucket];
        }
        
        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::contains(storm::storage::BitVector const& key) const {
            return findBucket(key).first;
        }

        template<class ValueType, class Hash>
        typename BitVectorHashMap<ValueType, Hash>::const_iterator BitVectorHashMap<ValueType, Hash>::begin() const {
            return const_iterator(*this, occupied.begin());
        }
        
        template<class ValueType, class Hash>
        typename BitVectorHashMap<ValueType, Hash>::const_iterator BitVectorHashMap<ValueType, Hash>::end() const {
            return const_iterator(*this, occupied.end());
        }
        
        template<class ValueType, class Hash>
        uint64_t BitVectorHashMap<ValueType, Hash>::getCurrentShiftWidth() const {
            return (sizeof(decltype(hasher(storm::storage::BitVector()))) * 8 - currentSize);
        }
        
        template<class ValueType, class Hash>
        std::pair<bool, uint64_t> BitVectorHashMap<ValueType, Hash>::findBucket(storm::storage::BitVector const& key) const {
#ifndef NDEBUG
            ++numberOfFinds;
#endif
            uint64_t bucket = hasher(key) >> this->getCurrentShiftWidth();
            
            while (isBucketOccupied(bucket)) {
#ifndef NDEBUG
                ++numberOfFindProbingSteps;
#endif
                if (buckets.matches(bucket * bucketSize, key)) {
                    return std::make_pair(true, bucket);
                }
                ++bucket;
                if (bucket == (1ull << currentSize)) {
                    bucket = 0;
                }
            }

            return std::make_pair(false, bucket);
        }
        
        template<class ValueType, class Hash>
        std::pair<bool, uint64_t> BitVectorHashMap<ValueType, Hash>::findBucketToInsert(storm::storage::BitVector const& key) {
#ifndef NDEBUG
            ++numberOfInsertions;
#endif
            uint64_t bucket = hasher(key) >> this->getCurrentShiftWidth();

            while (isBucketOccupied(bucket)) {
#ifndef NDEBUG
                ++numberOfInsertionProbingSteps;
#endif
                if (buckets.matches(bucket * bucketSize, key)) {
                    return std::make_pair(true, bucket);
                }
                ++bucket;
                if (bucket == (1ull << currentSize)) {
                    bucket = 0;
                }
            }

            return std::make_tuple(false, bucket);
        }
        
        template<class ValueType, class Hash>
        std::pair<storm::storage::BitVector, ValueType> BitVectorHashMap<ValueType, Hash>::getBucketAndValue(uint64_t bucket) const {
            return std::make_pair(buckets.get(bucket * bucketSize, bucketSize), values[bucket]);
        }
        
        template<class ValueType, class Hash>
        void BitVectorHashMap<ValueType, Hash>::remap(std::function<ValueType(ValueType const&)> const& remapping) {
            for (auto pos : occupied) {
                values[pos] = remapping(values[pos]);
            }
        }
        
        template class BitVectorHashMap<uint64_t>;
        template class BitVectorHashMap<uint32_t>;
    }
}
