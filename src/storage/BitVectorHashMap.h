#ifndef STORM_STORAGE_BITVECTORHASHMAP_H_
#define STORM_STORAGE_BITVECTORHASHMAP_H_

#include <cstdint>
#include <functional>

#include "src/storage/BitVector.h"

namespace storm {
    namespace storage {

        /*!
         * This class represents a hash-map whose keys are bit vectors. The value type is arbitrary. Currently, only
         * queries and insertions are supported. Also, the keys must be bit vectors with a length that is a multiple of
         * 64.
         */
        template<class ValueType, class Hash1 = std::hash<storm::storage::BitVector>, class Hash2 = std::hash<storm::storage::BitVector>>
        class BitVectorHashMap {
        public:
            /*!
             * Creates a new hash map with the given bucket size and initial size.
             *
             * @param bucketSize The size of the buckets that this map can hold. This value must be a multiple of 64.
             * @param initialSize The number of buckets that is initially available.
             * @param loadFactor The load factor that determines at which point the size of the underlying storage is
             * increased.
             */
            BitVectorHashMap(uint64_t bucketSize, uint64_t initialSize, double loadFactor = 0.75);
            
            /*!
             * Searches for the given key in the map. If it is found, the mapped-to value is returned. Otherwise, the
             * key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value that is inserted if the key is not already found in the map.
             * @return The found value if the key is already contained in the map and the provided new value otherwise.
             */
            ValueType findOrAdd(storm::storage::BitVector const& key, ValueType value);

            /*!
             * Searches for the given key in the map. If it is found, the mapped-to value is returned. Otherwise, the
             * key is inserted with the given value.
             *
             * @param key The key to search or insert.
             * @param value The value that is inserted if the key is not already found in the map.
             * @return A pair whose first component is the found value if the key is already contained in the map and
             * the provided new value otherwise and whose second component is the index of the bucket into which the key
             * was inserted.
             */
            std::pair<ValueType, std::size_t> findOrAddAndGetBucket(storm::storage::BitVector const& key, ValueType value);
            
            /*!
             * Retrieves the key stored in the given bucket (if any).
             *
             * @param bucket The index of the bucket.
             * @return The content of the named bucket.
             */
            storm::storage::BitVector getBucket(std::size_t bucket) const;
            
            /*!
             * Retrieves the size of the map in terms of the number of key-value pairs it stores.
             *
             * @return The size of the map.
             */
            std::size_t size() const;
            
            /*!
             * Retrieves the capacity of the underlying container.
             *
             * @return The capacity of the underlying container.
             */
            std::size_t capacity() const;
            
        private:
            /*!
             * Retrieves whether the given bucket holds a value.
             *
             * @param bucket The bucket to check.
             * @return True iff the bucket is occupied.
             */
            bool isBucketOccupied(uint_fast64_t bucket);
            
            /*!
             * Increases the size of the hash map and performs the necessary rehashing of all entries.
             */
            void increaseSize();
            
            // The load factor determining when the size of the map is increased.
            double loadFactor;
            
            // The size of one bucket.
            uint64_t bucketSize;
            
            // The number of buckets.
            std::size_t numberOfBuckets;
            
            // The buckets that hold the elements of the map.
            storm::storage::BitVector buckets;
            
            // A bit vector that stores which buckets actually hold a value.
            storm::storage::BitVector occupied;
            
            // A vector of the mapped-to values. The entry at position i is the "target" of the key in bucket i.
            std::vector<ValueType> values;
            
            // The number of elements in this map.
            std::size_t numberOfElements;
            
            // An iterator to a value in the static sizes table.
            std::vector<std::size_t>::const_iterator currentSizeIterator;
            
            // Functor object that are used to perform the actual hashing.
            Hash1 hasher1;
            Hash2 hasher2;
            
            // A static table that produces the next possible size of the hash table.
            static const std::vector<std::size_t> sizes;
        };
    }
}

#endif /* STORM_STORAGE_BITVECTORHASHMAP_H_ */