#ifndef STORM_STORAGE_BITVECTOR_H_
#define STORM_STORAGE_BITVECTOR_H_

#include <cmath>
#include <cstdint>
#include <ostream>
#include <vector>

namespace storm {
    namespace storage {
        
        /*!
         * A bit vector that is internally represented as a vector of 64-bit values.
         */
        class BitVector {
        public:
            /*!
             * A class that enables iterating over the indices of the bit vector whose corresponding bits are set to true.
             * Note that this is a const iterator, which cannot alter the bit vector.
             */
            class const_iterator {
                // Declare the BitVector class as a friend class to access its internal storage.
                friend class BitVector;
                
            public:
                /*!
                 * Constructs an iterator over the indices of the set bits in the given bit vector, starting and stopping,
                 * respectively, at the given indices.
                 *
                 * @param bitVector The bit vector over whose bits to iterate.
                 * @param startIndex The index where to begin looking for set bits.
                 * @param setOnFirstBit A flag that indicates whether the iterator is to be moved to the index of the first
                 * bit upon construction.
                 * @param endIndex The index at which to abort the iteration process.
                 */
                const_iterator(BitVector const& bitVector, uint_fast64_t startIndex, uint_fast64_t endIndex, bool setOnFirstBit = true);
                
                /*!
                 * Increases the position of the iterator to the position of the next bit that is set to true in the underlying
                 * bit vector.
                 *
                 * @return A reference to this iterator.
                 */
                const_iterator& operator++();
                
                /*!
                 * Returns the index of the current bit to which this iterator points.
                 *
                 * @return The index of the current bit to which this iterator points.
                 */
                uint_fast64_t operator*() const;
                
                /*!
                 * Compares the iterator with another iterator for inequality.
                 *
                 * @param otherIterator The iterator with respect to which inequality is checked.
                 * @return True if the two iterators are unequal.
                 */
                bool operator!=(const const_iterator& otherIterator) const;
                
            private:
                // The underlying bit vector of this iterator.
                BitVector const& bitVector;
                
                // The index of the bit this iterator currently points to.
                uint_fast64_t currentIndex;
                
                // The index of the bit that is past the end of the range of this iterator. 
                uint_fast64_t endIndex;
            };
            
            /*
             * Constructs an empty bit vector of length 0.
             */
            BitVector();
            
            /*!
             * Constructs a bit vector which can hold the given number of bits and
             * initializes all bits to the provided truth value.
             * @param length The number of bits the bit vector should be able to hold.
             * @param initTrue The initial value of the first |length| bits.
             */
            BitVector(uint_fast64_t length, bool initTrue = false);
            
            /*!
             * Creates a bit vector that has exactly the bits set that are given by the provided iterator range.
             *
             * @param The length of the bit vector.
             * @param begin The begin of the iterator range.
             * @param end The end of the iterator range.
             */
            template<typename InputIterator>
            BitVector(uint_fast64_t length, InputIterator begin, InputIterator end);
            
            /*!
             * Copy Constructor. Performs a deep copy of the given bit vector.
             * @param bv A reference to the bit vector to be copied.
             */
            BitVector(BitVector const& other);
            
            /*!
             * Copy Constructor. Performs a deep copy of the bits in the given bit vector that are given by the filter.
             * @param bv A reference to the bit vector to be copied.
             * @param filter The filter to apply for copying.
             */
            BitVector(BitVector const& other, BitVector const& filter);
            
            /*!
             * Move constructor. Move constructs the bit vector from the given bit vector.
             *
             */
            BitVector(BitVector&& bv);
            
            /*!
             * Compares the given bit vector with the current one.
             */
            bool operator==(BitVector const& bv);
            
            /*!
             * Assigns the given bit vector to the current bit vector by a deep copy.
             * @param bv The bit vector to assign to the current bit vector.
             * @return A reference to this bit vector after it has been assigned the
             * given bit vector by means of a deep copy.
             */
            BitVector& operator=(BitVector const& bv);
            
            /*!
             * Move assigns the given bit vector to the current bit vector.
             *
             * @param bv The bit vector whose content is moved to the current bit vector.
             * @return A reference to this bit vector after the contents of the given bit vector
             * have been moved into it.
             */
            BitVector& operator=(BitVector&& bv);
                        
            /*!
             * Sets the given truth value at the given index.
             * @param index The index where to set the truth value.
             * @param value The truth value to set.
             */
            void set(const uint_fast64_t index, bool value = true);
            
            /*!
             * Sets all bits in the given iterator range.
             *
             * @param begin The begin of the iterator range.
             * @param end The element past the last element of the iterator range.
             */
            template<typename InputIterator>
            void set(InputIterator begin, InputIterator end);
            
            /*!
             * Retrieves the truth value of the bit at the given index. Note: this does not check whether the 
             * given index is within bounds.
             *
             * @param index The index of the bit to access.
             * @return True if the bit at the given index is set.
             */
            bool operator[](uint_fast64_t index) const;
            
            /*!
             * Retrieves the truth value of the bit at the given index and performs a bound check.
             *
             * @param index The index of the bit to access.
             * @return True if the bit at the given index is set.
             */
            bool get(const uint_fast64_t index) const;
            
            /*!
             * Resizes the bit vector to hold the given new number of bits. If the bit vector becomes smaller this way, the 
             * bits are truncated. Otherwise, the new bits are initialized to the given value.
             *
             * @param newLength The new number of bits the bit vector can hold.
             * @param initTrue Sets whether newly created bits are initialized to true instead of false.
             */
            void resize(uint_fast64_t newLength, bool initTrue = false);
            
            /*!
             * Performs a logical "and" with the given bit vector. In case the sizes of the bit vectors
             * do not match, only the matching portion is considered and the overlapping bits
             * are set to 0.
             * @param bv A reference to the bit vector to use for the operation.
             * @return A bit vector corresponding to the logical "and" of the two bit vectors.
             */
            BitVector operator&(BitVector const& bv) const;
            
            /*!
             * Performs a logical "and" with the given bit vector and assigns the result to the
             * current bit vector. In case the sizes of the bit vectors do not match,
             * only the matching portion is considered and the overlapping bits are set to 0.
             * @param bv A reference to the bit vector to use for the operation.
             * @return A reference to the current bit vector corresponding to the logical "and"
             * of the two bit vectors.
             */
            BitVector& operator&=(BitVector const& bv);
            
            /*!
             * Performs a logical "or" with the given bit vector. In case the sizes of the bit vectors
             * do not match, only the matching portion is considered and the overlapping bits
             * are set to 0.
             * @param bv A reference to the bit vector to use for the operation.
             * @return A bit vector corresponding to the logical "or" of the two bit vectors.
             */
            BitVector operator|(BitVector const& bv) const;
            
            /*!
             * Performs a logical "or" with the given bit vector and assigns the result to the
             * current bit vector. In case the sizes of the bit vectors do not match,
             * only the matching portion is considered and the overlapping bits are set to 0.
             * @param bv A reference to the bit vector to use for the operation.
             * @return A reference to the current bit vector corresponding to the logical "or"
             * of the two bit vectors.
             */
            BitVector& operator|=(BitVector const& bv);
            
            /*!
             * Performs a logical "xor" with the given bit vector. In case the sizes of the bit vectors
             * do not match, only the matching portion is considered and the overlapping bits
             * are set to 0.
             * @param bv A reference to the bit vector to use for the operation.
             * @return A bit vector corresponding to the logical "xor" of the two bit vectors.
             */
            BitVector operator^(BitVector const& bv) const;
            
            /*!
             * Performs a logical "not" on the bit vector.
             * @return A bit vector corresponding to the logical "not" of the bit vector.
             */
            BitVector operator~() const;
            
            /*!
             * Negates all bits in the bit vector.
             */
            void complement();
            
            /*!
             * Performs a logical "implies" with the given bit vector. In case the sizes of the bit vectors
             * do not match, only the matching portion is considered and the overlapping bits
             * are set to 0.
             *
             * @param bv A reference to the bit vector to use for the operation.
             * @return A bit vector corresponding to the logical "implies" of the two bit vectors.
             */
            BitVector implies(BitVector const& bv) const;
            
            /*!
             * Checks whether all bits that are set in the current bit vector are also set in the given bit
             * vector.
             *
             * @param bv A reference to the bit vector whose bits are (possibly) a superset of the bits of
             * the current bit vector.
             * @return True iff all bits that are set in the current bit vector are also set in the given bit
             * vector.
             */
            bool isSubsetOf(BitVector const& bv) const;
            
            /*!
             * Checks whether none of the bits that are set in the current bit vector are also set in the
             * given bit vector.
             *
             * @param bv A reference to the bit vector whose bits are (possibly) disjoint from the bits in
             * the current bit vector.
             * @returns True iff none of the bits that are set in the current bit vector are also set in the
             * given bit vector.
             */
            bool isDisjointFrom(BitVector const& bv) const;
            
            /*!
             * Computes a bit vector such that bit i is set iff the i-th set bit of the current bit vector is also contained
             * in the given bit vector.
             *
             * @param bv A reference the bit vector to be used.
             * @return A bit vector whose i-th bit is set iff the i-th set bit of the current bit vector is also contained
             * in the given bit vector.
             */
            BitVector operator%(BitVector const& bv) const;
            
            /*!
             * Retrieves whether there is at least one bit set in the vector.
             *
             * @return True if there is at least one bit set in this vector.
             */
            bool empty() const;
            
            /*!
             * Removes all set bits from the bit vector.
             */
            void clear();
            
            /*!
             * Returns a list containing all indices such that the bits at these indices are set to true
             * in the bit vector.
             *
             * @return A vector of indices of set bits in the bit vector.
             */
            std::vector<uint_fast64_t> getSetIndicesList() const;
            
            /*!
             * Adds all indices of bits set to one to the given list.
             *
             * @param list The list to which to append the indices.
             */
            void addSetIndicesToVector(std::vector<uint_fast64_t>& vector) const;
            
            /*!
             * Returns the number of bits that are set (to one) in this bit vector.
             *
             * @return The number of bits that are set (to one) in this bit vector.
             */
            uint_fast64_t getNumberOfSetBits() const;
            
            /*!
             * Retrieves the number of bits set in this bit vector with an index strictly smaller than the given one.
             *
             * @param index The index for which to retrieve the number of set bits with a smaller index.
             * @return The number of bits set in this bit vector with an index strictly smaller than the given one.
             */
            uint_fast64_t getNumberOfSetBitsBeforeIndex(uint_fast64_t index) const;
            
            /*!
             * Retrieves the number of bits this bit vector can store.
             *
             * @return The number of bits this bit vector can hold.
             */
            size_t size() const;
            
            /*!
             * Returns the size of the bit vector in memory measured in bytes.
             *
             * @return The size of the bit vector in memory measured in bytes.
             */
            uint_fast64_t getSizeInMemory() const;
            
            /*!
             * Returns an iterator to the indices of the set bits in the bit vector.
             */
            const_iterator begin() const;
            
            /*!
             * Returns an iterator pointing at the element past the bit vector.
             */
            const_iterator end() const;
            
            /*!
             * Calculates a hash over all values contained in this Sparse Matrix.
             * @return size_t A Hash Value
             */
            std::size_t hash() const;
            
            /*
             * Retrieves the index of the bit that is the next bit set to true in the bit vector. If there
             * is none, this function returns the number of bits this vector holds in total.
             * Put differently, if the return value is equal to a call to size(), then there is 
             * no bit set after the specified position.
             *
             * @param startingIndex The index at which to start the search for the next bit that is set. The
             * bit at this index itself is not considered.
             * @return The index of the next bit that is set after the given index.
             */
            uint_fast64_t getNextSetIndex(uint_fast64_t startingIndex) const;
            
            friend std::ostream& operator<<(std::ostream& out, BitVector const& bitVector);
            
        private:
            
            /*!
             * Retrieves the index of the bit that is set after the given starting index,
             * but before the given end index in the given bit vector.
             * @param bitVector The bit vector to search.
             * @param startingIndex The index where to start the search.
             * @param endIndex The end index at which to stop the search.
             * @return The index of the bit that is set after the given starting index,
             * but before the given end index in the given bit vector or endIndex in case
             * the end index was reached.
             */
            uint_fast64_t getNextSetIndex(uint_fast64_t startingIndex, uint_fast64_t endIndex) const;
            
            /*!
             * Truncate the last bucket so that no bits are set in the range starting from (bitCount + 1).
             */
            void truncateLastBucket();
            
            /*!
             * Updates internal structures in case the size of the bit vector changed. Needs to be called
             * after the size of the bit vector changed.
             */
            void updateSizeChange();
            
            /*! The number of bits that have to be stored */
            uint_fast64_t bitCount;
            
            /*! Array of 64-bit buckets to store the bits. */
            std::vector<uint64_t> bucketVector;
            
            /*! A bit mask that can be used to reduce a modulo operation to a logical "and".  */
            static const uint_fast64_t mod64mask = (1 << 6) - 1;
        };
        
    } // namespace storage
} // namespace storm

#endif // STORM_STORAGE_BITVECTOR_H_
