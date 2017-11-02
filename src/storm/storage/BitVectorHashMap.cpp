#include "storm/storage/BitVectorHashMap.h"

#include <algorithm>
#include <iostream>
#include <algorithm>

#include "storm/utility/macros.h"
#include "storm/exceptions/InternalException.h"

namespace storm {
    namespace storage {
        template<class ValueType, class Hash>
        const std::vector<std::size_t> BitVectorHashMap<ValueType, Hash>::sizes = {5, 13, 31, 79, 163, 277, 499, 1021, 2029, 3989, 8059, 16001, 32099, 64301, 127921, 256499, 511111, 1024901, 2048003, 4096891, 8192411, 15485863, 32142191, 64285127, 128572517, 257148523, 514299959, 1028599919, 2057199839, 4114399697, 8228799419, 16457598791, 32915197603, 65830395223};
        
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
        BitVectorHashMap<ValueType, Hash>::BitVectorHashMap(uint64_t bucketSize, uint64_t initialSize, double loadFactor) : loadFactor(loadFactor), bucketSize(bucketSize), numberOfElements(0) {
            STORM_LOG_ASSERT(bucketSize % 64 == 0, "Bucket size must be a multiple of 64.");
            currentSizeIterator = std::find_if(sizes.begin(), sizes.end(), [=] (uint64_t value) { return value > initialSize; } );
            
            // Create the underlying containers.
            buckets = storm::storage::BitVector(bucketSize * *currentSizeIterator);
            occupied = storm::storage::BitVector(*currentSizeIterator);
            values = std::vector<ValueType>(*currentSizeIterator);
            
#ifndef NDEBUG
            for (uint64_t i = 0; i < sizes.size() - 1; ++i) {
                STORM_LOG_ASSERT(sizes[i] < sizes[i + 1], "Expected stricly increasing sizes.");
            }
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
            return *currentSizeIterator;
        }
        
        template<class ValueType, class Hash>
        void BitVectorHashMap<ValueType, Hash>::increaseSize() {
            ++currentSizeIterator;
            STORM_LOG_ASSERT(currentSizeIterator != sizes.end(), "Hash map became to big.");
#ifndef NDEBUG
            STORM_LOG_TRACE("Increasing size of hash map from " << *(currentSizeIterator - 1) << " to " << *currentSizeIterator << ". Stats: " << numberOfFinds << " finds (avg. " << (numberOfFindProbingSteps / static_cast<double>(numberOfFinds)) << " probing steps), " << numberOfInsertions << " insertions (avg. " << (numberOfInsertionProbingSteps / static_cast<double>(numberOfInsertions)) << " probing steps).");
#else
            STORM_LOG_TRACE("Increasing size of hash map from " << *(currentSizeIterator - 1) << " to " << *currentSizeIterator << ".");
#endif
            
            // Create new containers and swap them with the old ones.
            numberOfElements = 0;
            storm::storage::BitVector oldBuckets(bucketSize * *currentSizeIterator);
            std::swap(oldBuckets, buckets);
            storm::storage::BitVector oldOccupied = storm::storage::BitVector(*currentSizeIterator);
            std::swap(oldOccupied, occupied);
            std::vector<ValueType> oldValues = std::vector<ValueType>(*currentSizeIterator);
            std::swap(oldValues, values);
            
            // Now iterate through the elements and reinsert them in the new storage.
            bool fail = false;
            for (auto bucketIndex : oldOccupied) {
                fail = !this->insertWithoutIncreasingSize(oldBuckets.get(bucketIndex * bucketSize, bucketSize), oldValues[bucketIndex]);
                
                // If we failed to insert just one element, we have to redo the procedure with a larger container size.
                if (fail) {
                    break;
                }
            }
            
            uint_fast64_t failCount = 0;
            while (fail) {
                ++failCount;
                STORM_LOG_THROW(failCount < 2, storm::exceptions::InternalException, "Increasing size failed too often.");
                
                ++currentSizeIterator;
                STORM_LOG_THROW(currentSizeIterator != sizes.end(), storm::exceptions::InternalException, "Hash map became to big.");
                
                numberOfElements = 0;
                buckets = storm::storage::BitVector(bucketSize * *currentSizeIterator);
                occupied  = storm::storage::BitVector(*currentSizeIterator);
                values = std::vector<ValueType>(*currentSizeIterator);
                
                for (auto bucketIndex : oldOccupied) {
                    fail = !this->insertWithoutIncreasingSize(oldBuckets.get(bucketIndex * bucketSize, bucketSize), oldValues[bucketIndex]);
                    
                    // If we failed to insert just one element, we have to redo the procedure with a larger container size.
                    if (fail) {
                        break;
                    }
                }
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
            if (numberOfElements >= loadFactor * *currentSizeIterator) {
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
            if (numberOfElements >= loadFactor * *currentSizeIterator) {
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
        uint_fast64_t BitVectorHashMap<ValueType, Hash>::getNextBucketInProbingSequence(uint_fast64_t, uint_fast64_t currentValue, uint_fast64_t step) const {
            return (currentValue + step + step*step) % *currentSizeIterator;
        }
        
        template<class ValueType, class Hash>
        std::pair<bool, std::size_t> BitVectorHashMap<ValueType, Hash>::findBucket(storm::storage::BitVector const& key) const {
#ifndef NDEBUG
            ++numberOfFinds;
#endif
            uint_fast64_t initialHash = hasher(key) % *currentSizeIterator;
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
                bucket = getNextBucketInProbingSequence(initialHash, bucket, i);
                
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
            uint_fast64_t initialHash = hasher(key) % *currentSizeIterator;
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
                bucket = getNextBucketInProbingSequence(initialHash, bucket, i);
                
                if (bucket == initialHash) {
                    if (increaseStorage) {
                        this->increaseSize();
                        bucket = initialHash = hasher(key) % *currentSizeIterator;
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
