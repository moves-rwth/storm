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
        std::size_t BitVectorHashMap<ValueType, Hash>::size() const {
            return numberOfElements;
        }
        
        template<class ValueType, class Hash>
        std::size_t BitVectorHashMap<ValueType, Hash>::capacity() const {
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
            numberOfElements = 0;
            storm::storage::BitVector oldBuckets(bucketSize * (1ull << currentSize));
            std::swap(oldBuckets, buckets);
            storm::storage::BitVector oldOccupied = storm::storage::BitVector(1ull << currentSize);
            std::swap(oldOccupied, occupied);
            std::vector<ValueType> oldValues = std::vector<ValueType>(1ull << currentSize);
            std::swap(oldValues, values);
            
            // Now iterate through the elements and reinsert them in the new storage.
            bool fail = false;
            for (auto bucketIndex : oldOccupied) {
                fail = !this->insertWithoutIncreasingSize(oldBuckets.get(bucketIndex * bucketSize, bucketSize), oldValues[bucketIndex]);
                STORM_LOG_ASSERT(!fail, "Expected to be able to insert all elements.");
            }
        }
        
        template<class ValueType, class Hash>
        bool BitVectorHashMap<ValueType, Hash>::insertWithoutIncreasingSize(storm::storage::BitVector const& key, ValueType const& value) {
            std::tuple<bool, std::size_t, bool> flagBucketTuple = this->findBucketToInsert<false>(key);
            if (std::get<2>(flagBucketTuple)) {
                return false;
            }
            
            if (std::get<0>(flagBucketTuple)) {
                return true;
            } else {
                // Insert the new bits into the bucket.
                buckets.set(std::get<1>(flagBucketTuple) * bucketSize, key);
                occupied.set(std::get<1>(flagBucketTuple));
                values[std::get<1>(flagBucketTuple)] = value;
                ++numberOfElements;
                return true;
            }
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::findOrAdd(storm::storage::BitVector const& key, ValueType const& value) {
            return findOrAddAndGetBucket(key, value).first;
        }
        
        template<class ValueType, class Hash>
        void BitVectorHashMap<ValueType, Hash>::setOrAdd(storm::storage::BitVector const& key, ValueType const& value) {
            setOrAddAndGetBucket(key, value);
        }
        
        template<class ValueType, class Hash>
        std::pair<ValueType, std::size_t> BitVectorHashMap<ValueType, Hash>::findOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType const& value) {
            // If the load of the map is too high, we increase the size.
            if (numberOfElements >= loadFactor * (1ull << currentSize)) {
                this->increaseSize();
            }
            
            std::tuple<bool, std::size_t, bool> flagBucketTuple = this->findBucketToInsert<true>(key);
            STORM_LOG_ASSERT(!std::get<2>(flagBucketTuple), "Failed to find bucket for insertion.");
            if (std::get<0>(flagBucketTuple)) {
                return std::make_pair(values[std::get<1>(flagBucketTuple)], std::get<1>(flagBucketTuple));
            } else {
                // Insert the new bits into the bucket.
                buckets.set(std::get<1>(flagBucketTuple) * bucketSize, key);
                occupied.set(std::get<1>(flagBucketTuple));
                values[std::get<1>(flagBucketTuple)] = value;
                ++numberOfElements;
                return std::make_pair(value, std::get<1>(flagBucketTuple));
            }
        }
        
        template<class ValueType, class Hash>
        std::size_t BitVectorHashMap<ValueType, Hash>::setOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType const& value) {
            // If the load of the map is too high, we increase the size.
            if (numberOfElements >= loadFactor * (1ull << currentSize)) {
                this->increaseSize();
            }
            
            std::tuple<bool, std::size_t, bool> flagBucketTuple = this->findBucketToInsert<true>(key);
            STORM_LOG_ASSERT(!std::get<2>(flagBucketTuple), "Failed to find bucket for insertion.");
            if (!std::get<0>(flagBucketTuple)) {
                // Insert the new bits into the bucket.
                buckets.set(std::get<1>(flagBucketTuple) * bucketSize, key);
                occupied.set(std::get<1>(flagBucketTuple));
                ++numberOfElements;
            }
            values[std::get<1>(flagBucketTuple)] = value;
            return std::get<1>(flagBucketTuple);
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::getValue(storm::storage::BitVector const& key) const {
            std::pair<bool, std::size_t> flagBucketPair = this->findBucket(key);
            STORM_LOG_ASSERT(flagBucketPair.first, "Unknown key.");
            return values[flagBucketPair.second];
        }
        
        template<class ValueType, class Hash>
        ValueType BitVectorHashMap<ValueType, Hash>::getValue(std::size_t bucket) const {
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
        std::pair<bool, std::size_t> BitVectorHashMap<ValueType, Hash>::findBucket(storm::storage::BitVector const& key) const {
#ifndef NDEBUG
            ++numberOfFinds;
#endif
            uint_fast64_t initialHash = hasher(key) % (1ull << currentSize);
            uint_fast64_t bucket = initialHash;
            
            uint_fast64_t i = 0;
            while (isBucketOccupied(bucket)) {
                ++i;
#ifndef NDEBUG
                ++numberOfFindProbingSteps;
#endif
                if (buckets.matches(bucket * bucketSize, key)) {
                    return std::make_pair(true, bucket);
                }
                bucket += 1;
                if (bucket == (1ull << currentSize)) {
                    bucket = 0;
                }

                if (bucket == initialHash) {
                    return std::make_pair(false, bucket);
                }
            }

            return std::make_pair(false, bucket);
        }
        
        template<class ValueType, class Hash>
        template<bool increaseStorage>
        std::tuple<bool, std::size_t, bool> BitVectorHashMap<ValueType, Hash>::findBucketToInsert(storm::storage::BitVector const& key) {
#ifndef NDEBUG
            ++numberOfInsertions;
#endif
            uint_fast64_t initialHash = hasher(key) % (1ull << currentSize);
            uint_fast64_t bucket = initialHash;

            uint64_t i = 0;
            while (isBucketOccupied(bucket)) {
                ++i;
#ifndef NDEBUG
                ++numberOfInsertionProbingSteps;
#endif
                if (buckets.matches(bucket * bucketSize, key)) {
                    return std::make_tuple(true, bucket, false);
                }
                bucket += 1;
                if (bucket == (1ull << currentSize)) {
                    bucket = 0;
                }

                if (bucket == initialHash) {
                    if (increaseStorage) {
                        this->increaseSize();
                        bucket = initialHash = hasher(key) % (1ull << currentSize);
                    } else {
                        return std::make_tuple(false, bucket, true);
                    }
                }
            }

            return std::make_tuple(false, bucket, false);
        }
        
        template<class ValueType, class Hash>
        std::pair<storm::storage::BitVector, ValueType> BitVectorHashMap<ValueType, Hash>::getBucketAndValue(std::size_t bucket) const {
            return std::make_pair(buckets.get(bucket * bucketSize, bucketSize), values[bucket]);
        }
        
        template<class ValueType, class Hash>
        void BitVectorHashMap<ValueType, Hash>::remap(std::function<ValueType(ValueType const&)> const& remapping) {
            for (auto pos : occupied) {
                values[pos] = remapping(values[pos]);
            }
        }
        
        template class BitVectorHashMap<uint_fast64_t>;
        template class BitVectorHashMap<uint32_t>;
    }
}
