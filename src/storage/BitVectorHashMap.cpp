#include "src/storage/BitVectorHashMap.h"

#include <iostream>

#include "src/utility/macros.h"

namespace storm {
    namespace storage {
        template<class ValueType, class Hash1, class Hash2>
        const std::vector<std::size_t> BitVectorHashMap<ValueType, Hash1, Hash2>::sizes = {5, 13, 31, 79, 163, 277, 499, 1021, 2029, 3989, 8059, 16001, 32099, 64301, 127921, 256499, 511111, 1024901, 2048003, 4096891, 8192411, 15485863, 32142191};
        
        template<class ValueType, class Hash1, class Hash2>
        BitVectorHashMap<ValueType, Hash1, Hash2>::BitVectorHashMap(uint64_t bucketSize, uint64_t initialSize, double loadFactor) : loadFactor(loadFactor), bucketSize(bucketSize), numberOfElements(0) {
            STORM_LOG_ASSERT(bucketSize % 64 == 0, "Bucket size must be a multiple of 64.");
            currentSizeIterator = std::find_if(sizes.begin(), sizes.end(), [=] (uint64_t value) { return value > initialSize; } );
            
            // Create the underlying containers.
            buckets = storm::storage::BitVector(bucketSize * *currentSizeIterator);
            occupied = storm::storage::BitVector(*currentSizeIterator);
            values = std::vector<ValueType>(*currentSizeIterator);
        }
        
        template<class ValueType, class Hash1, class Hash2>
        bool BitVectorHashMap<ValueType, Hash1, Hash2>::isBucketOccupied(uint_fast64_t bucket) const {
            return occupied.get(bucket);
        }
        
        template<class ValueType, class Hash1, class Hash2>
        std::size_t BitVectorHashMap<ValueType, Hash1, Hash2>::size() const {
            return numberOfElements;
        }
        
        template<class ValueType, class Hash1, class Hash2>
        std::size_t BitVectorHashMap<ValueType, Hash1, Hash2>::capacity() const {
            return *currentSizeIterator;
        }
        
        template<class ValueType, class Hash1, class Hash2>
        void BitVectorHashMap<ValueType, Hash1, Hash2>::increaseSize() {
            ++currentSizeIterator;
            STORM_LOG_ASSERT(currentSizeIterator != sizes.end(), "Hash map became to big.");
            
            storm::storage::BitVector oldBuckets(bucketSize * *currentSizeIterator);
            std::swap(oldBuckets, buckets);
            storm::storage::BitVector oldOccupied = storm::storage::BitVector(*currentSizeIterator);
            std::swap(oldOccupied, occupied);
            std::vector<ValueType> oldValues = std::vector<ValueType>(*currentSizeIterator);
            std::swap(oldValues, values);
            
            // Now iterate through the elements and reinsert them in the new storage.
            numberOfElements = 0;
            for (auto const& bucketIndex : oldOccupied) {
                this->findOrAdd(oldBuckets.get(bucketIndex * bucketSize, bucketSize), oldValues[bucketIndex]);
            }
        }
        
        template<class ValueType, class Hash1, class Hash2>
        ValueType BitVectorHashMap<ValueType, Hash1, Hash2>::findOrAdd(storm::storage::BitVector const& key, ValueType value) {
            return findOrAddAndGetBucket(key, value).first;
        }
        
        template<class ValueType, class Hash1, class Hash2>
        std::pair<ValueType, std::size_t> BitVectorHashMap<ValueType, Hash1, Hash2>::findOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType value) {
            // If the load of the map is too high, we increase the size.
            if (numberOfElements >= loadFactor * *currentSizeIterator) {
                this->increaseSize();
            }
            
            std::pair<bool, std::size_t> flagBucketPair = this->findBucket(key);
            if (flagBucketPair.first) {
                return std::make_pair(values[flagBucketPair.second], flagBucketPair.second);
            } else {
                // Insert the new bits into the bucket.
                buckets.set(flagBucketPair.second * bucketSize, key);
                occupied.set(flagBucketPair.second);
                values[flagBucketPair.second] = value;
                ++numberOfElements;
                return std::make_pair(value, flagBucketPair.second);
            }
        }
        
        template<class ValueType, class Hash1, class Hash2>
        ValueType BitVectorHashMap<ValueType, Hash1, Hash2>::getValue(storm::storage::BitVector const& key) const {
            std::cout << "calling getValue with key " << key << std::endl;
            std::pair<bool, std::size_t> flagBucketPair = this->findBucket(key);
            STORM_LOG_ASSERT(flagBucketPair.first, "Unknown key.");
            return flagBucketPair.second;
        }
        
        template<class ValueType, class Hash1, class Hash2>
        std::pair<bool, std::size_t> BitVectorHashMap<ValueType, Hash1, Hash2>::findBucket(storm::storage::BitVector const& key) const {
            uint_fast64_t initialHash = hasher1(key) % *currentSizeIterator;
            uint_fast64_t bucket = initialHash;
            
            while (isBucketOccupied(bucket)) {
                if (buckets.matches(bucket * bucketSize, key)) {
                    return std::make_pair(true, bucket);
                }
                bucket += hasher2(key);
                bucket %= *currentSizeIterator;
            }

            return std::make_pair(false, bucket);
        }
        
        template<class ValueType, class Hash1, class Hash2>
        std::pair<storm::storage::BitVector, ValueType> BitVectorHashMap<ValueType, Hash1, Hash2>::getBucketAndValue(std::size_t bucket) const {
            return std::make_pair(buckets.get(bucket * bucketSize, bucketSize), values[bucket]);
        }
        
        template class BitVectorHashMap<uint_fast64_t>;
        template class BitVectorHashMap<uint32_t>;
    }
}
