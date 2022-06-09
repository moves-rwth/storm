#ifndef STORM_STORAGE_BITVECTOR_H_
#define STORM_STORAGE_BITVECTOR_H_

#include <cmath>
#include <cstdint>
#include <functional>
#include <iterator>
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
     * A class that enables iterating over the indices of the bit vector whose corresponding bits are set to
     * true. Note that this is a const iterator, which cannot alter the bit vector.
     */
    class const_iterator {
        // Declare the BitVector class as a friend class to access its internal storage.
        friend class BitVector;

       public:
        // Define iterator
        using iterator_category = std::input_iterator_tag;
        using value_type = uint_fast64_t;
        using difference_type = std::ptrdiff_t;
        using pointer = uint_fast64_t*;
        using reference = uint_fast64_t&;

        /*!
         * Constructs an iterator over the indices of the set bits in the given bit vector, starting and
         * stopping, respectively, at the given indices.
         *
         * @param dataPtr A pointer to the first bucket of the underlying bit vector.
         * @param startIndex The index where to begin looking for set bits.
         * @param setOnFirstBit A flag that indicates whether the iterator is to be moved to the index of the
         * first bit upon construction.
         * @param endIndex The index at which to abort the iteration process.
         */
        const_iterator(uint64_t const* dataPtr, uint_fast64_t startIndex, uint_fast64_t endIndex, bool setOnFirstBit = true);

        /*!
         * Constructs an iterator by copying the given iterator.
         *
         * @param other The iterator to copy.
         */
        const_iterator(const_iterator const& other);

        /*!
         * Assigns the contents of the given iterator to the current one via copying the former's contents.
         *
         * @param other The iterator from which to copy-assign.
         */
        const_iterator& operator=(const_iterator const& other);

        /*!
         * Increases the position of the iterator to the position of the next bit that is set to true in the
         * underlying bit vector.
         *
         * @return A reference to this iterator.
         */
        const_iterator& operator++();

        /*!
         * Increases the position of the iterator to the position of the n'th next bit that is set to true in the
         * underlying bit vector.
         */
        const_iterator& operator+=(size_t n);

        /*!
         * Returns the index of the current bit to which this iterator points.
         *
         * @return The index of the current bit to which this iterator points.
         */
        uint_fast64_t operator*() const;

        /*!
         * Compares the iterator with another iterator for inequality.
         *
         * @param other The iterator with respect to which inequality is checked.
         * @return True if the two iterators are unequal.
         */
        bool operator!=(const_iterator const& other) const;

        /*!
         * Compares the iterator with another iterator for equality.
         *
         * @param other The iterator with respect to which equality is checked.
         * @return True if the two iterators are equal.
         */
        bool operator==(const_iterator const& other) const;

       private:
        // The underlying bit vector of this iterator.
        uint64_t const* dataPtr;

        // The index of the bit this iterator currently points to.
        uint_fast64_t currentIndex;

        // The index of the bit that is past the end of the range of this iterator.
        uint_fast64_t endIndex;
    };

    /*!
     * Constructs an empty bit vector of length 0.
     */
    BitVector();

    /*!
     * Deconstructs a bit vector by deleting the underlying storage.
     */
    ~BitVector();

    /*!
     * Constructs a bit vector which can hold the given number of bits and initializes all bits with the
     * provided truth value.
     *
     * @param length The number of bits the bit vector should be able to hold.
     * @param init The initial value of the first |length| bits.
     */
    explicit BitVector(uint_fast64_t length, bool init = false);

    /*!
     * Creates a bit vector that has exactly the bits set that are given by the provided input iterator range
     * [first, last).
     *
     * @param The length of the bit vector.
     * @param first The begin of the iterator range.
     * @param last The end of the iterator range.
     */
    template<typename InputIterator>
    BitVector(uint_fast64_t length, InputIterator first, InputIterator last);

    /*!
     * Creates a bit vector that has exactly the bits set that are given by the vector
     */
    BitVector(uint_fast64_t length, std::vector<uint_fast64_t> setEntries);

    /*!
     * Performs a deep copy of the given bit vector.
     *
     * @param other A reference to the bit vector to be copied.
     */
    BitVector(BitVector const& other);

    /*!
     * Move constructs the bit vector from the given bit vector.
     *
     * @param other The bit vector from which to move-construct.
     */
    BitVector(BitVector&& other);

    /*!
     * Compares the given bit vector with the current one.
     *
     * @param other The bitvector with which to compare the current one.
     * @return True iff the other bit vector is semantically the same as the current one.
     */
    bool operator==(BitVector const& other) const;

    /*!
     * Compares the given bit vector with the current one.
     *
     * @param other The bitvector with which to compare the current one.
     * @return True iff the other bit vector is semantically not the same as the current one.
     */
    bool operator!=(BitVector const& other) const;

    /*!
     * Assigns the contents of the given bit vector to the current bit vector via a deep copy.
     *
     * @param other The bit vector to assign to the current bit vector.
     * @return A reference to this bit vector after it has been assigned the given bit vector by means of a
     * deep copy.
     */
    BitVector& operator=(BitVector const& other);

    /*!
     * Move assigns the given bit vector to the current bit vector.
     *
     * @param other The bit vector whose content is moved to the current bit vector.
     * @return A reference to this bit vector after the contents of the given bit vector have been moved
     * into it.
     */
    BitVector& operator=(BitVector&& other);

    /*!
     * Retrieves whether the current bit vector is (in some order) smaller than the given one.
     *
     * @param other The other bit vector.
     * @return True iff the current bit vector is smaller than the given one.
     */
    bool operator<(BitVector const& other) const;

    /*!
     * Sets the given truth value at the given index.
     *
     * @param index The index where to set the truth value.
     * @param value The truth value to set.
     */
    void set(uint_fast64_t index, bool value = true);

    /*!
     * Sets all bits in the given iterator range [first, last).
     *
     * @param first The begin of the iterator range.
     * @param last The element past the last element of the iterator range.
     */
    template<typename InputIterator>
    void set(InputIterator first, InputIterator last, bool value = true);

    /*!
     * Retrieves the truth value of the bit at the given index. Note: this does not check whether the given
     * index is within bounds.
     *
     * @param index The index of the bit to access.
     * @return True iff the bit at the given index is set.
     */
    bool operator[](uint_fast64_t index) const;

    /*!
     * Retrieves the truth value of the bit at the given index and performs a bound check. If the access is
     * out-of-bounds an OutOfBoundsException is thrown.
     *
     * @param index The index of the bit to access.
     * @return True iff the bit at the given index is set.
     */
    bool get(uint_fast64_t index) const;

    /*!
     * Resizes the bit vector to hold the given new number of bits. If the bit vector becomes smaller this way,
     * the bits are truncated. Otherwise, the new bits are initialized to the given value.
     *
     * @param newLength The new number of bits the bit vector can hold.
     * @param init The truth value to which to initialize newly created bits.
     */
    void resize(uint_fast64_t newLength, bool init = false);

    /*!
     * Concatenate this bitvector with another bitvector. The result is stored in this bitvector.
     * During the operation, the bitvector is extended to match the size of the sum of the two bitvectors
     * The current implementation expects this->size() to be a multiple of 64. This can be ensured by a call to
     * expandSize
     */
    void concat(BitVector const& extension);

    /*
     * Expands the size to a multiple of 64.
     */
    void expandSize(bool init = false);

    /*!
     * Enlarges the bit vector such that it holds at least the given number of bits (but possibly more).
     * This can be used to diminish reallocations when the final size of the bit vector is not known yet.
     * The bit vector does not become smaller.
     * New bits are initialized to the given value.
     *
     * @param minimumLength The minimum number of bits that the bit vector should hold.
     * @param init The truth value to which to initialize newly created bits.
     */
    void grow(uint_fast64_t minimumLength, bool init = false);

    /*!
     * Performs a logical "and" with the given bit vector. In case the sizes of the bit vectors do not match,
     * only the matching portion is considered and the overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A bit vector corresponding to the logical "and" of the two bit vectors.
     */
    BitVector operator&(BitVector const& other) const;

    /*!
     * Performs a logical "and" with the given bit vector and assigns the result to the current bit vector.
     * In case the sizes of the bit vectors do not match, only the matching portion is considered and the
     * overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A reference to the current bit vector corresponding to the logical "and"
     * of the two bit vectors.
     */
    BitVector& operator&=(BitVector const& other);

    /*!
     * Performs a logical "or" with the given bit vector. In case the sizes of the bit vectors do not match,
     * only the matching portion is considered and the overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A bit vector corresponding to the logical "or" of the two bit vectors.
     */
    BitVector operator|(BitVector const& other) const;

    /*!
     * Performs a logical "or" with the given bit vector and assigns the result to the current bit vector.
     * In case the sizes of the bit vectors do not match, only the matching portion is considered and the
     * overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A reference to the current bit vector corresponding to the logical "or"
     * of the two bit vectors.
     */
    BitVector& operator|=(BitVector const& other);

    /*!
     * Performs a logical "xor" with the given bit vector. In case the sizes of the bit vectors do not match,
     * only the matching portion is considered and the overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A bit vector corresponding to the logical "xor" of the two bit vectors.
     */
    BitVector operator^(BitVector const& other) const;

    /*!
     * Computes a bit vector that contains only the values of the bits given by the filter.
     * The returned bit vector is as long as the number of set bits in the given filter.
     * Bit i is set in the returned bit vector iff the i-th set bit of the current bit vector is set in the filter.
     *
     * For example: 00100110 % 10101010 returns 0101
     *
     * @param filter A reference bit vector to use as the filter.
     * @return A bit vector that is as long as the number of set bits in the given filter and
     * contains only the values of the bits set in the filter.
     */
    BitVector operator%(BitVector const& filter) const;

    /*!
     * Performs a logical "not" on the bit vector.
     *
     * @return A bit vector corresponding to the logical "not" of the bit vector.
     */
    BitVector operator~() const;

    /*!
     * Negates all bits in the bit vector.
     */
    void complement();

    /*!
     * Increments the (unsigned) number represented by this BitVector by one.
     * @note Index 0 is assumed to be the least significant bit.
     * @note Wraps around, i.e., incrementing 11..1 yields 00..0.
     */
    void increment();

    /*!
     * Performs a logical "implies" with the given bit vector. In case the sizes of the bit vectors do not
     * match, only the matching portion is considered and the overlapping bits are set to 0.
     *
     * @param other A reference to the bit vector to use for the operation.
     * @return A bit vector corresponding to the logical "implies" of the two bit vectors.
     */
    BitVector implies(BitVector const& other) const;

    /*!
     * Checks whether all bits that are set in the current bit vector are also set in the given bit vector.
     *
     * @param other A reference to the bit vector whose bits are (possibly) a superset of the bits of
     * the current bit vector.
     * @return True iff all bits that are set in the current bit vector are also set in the given bit
     * vector.
     */
    bool isSubsetOf(BitVector const& other) const;

    /*!
     * Checks whether none of the bits that are set in the current bit vector are also set in the given bit
     * vector.
     *
     * @param other A reference to the bit vector whose bits are (possibly) disjoint from the bits in
     * the current bit vector.
     * @return True iff none of the bits that are set in the current bit vector are also set in the
     * given bit vector.
     */
    bool isDisjointFrom(BitVector const& other) const;

    /*!
     * Checks whether the given bit vector matches the bits starting from the given index in the current bit
     * vector. Note: the given bit vector must be shorter than the current one minus the given index.
     *
     * @param bitIndex The index of the first bit that it supposed to match. This value must be a multiple of 64.
     * @param other The bit vector with which to compare.
     * @return bool True iff the bits match exactly.
     */
    bool matches(uint_fast64_t bitIndex, BitVector const& other) const;

    /*!
     * Sets the exact bit pattern of the given bit vector starting at the given bit index. Note: the given bit
     * vector must be shorter than the current one minus the given index.
     *
     * @param bitIndex The index of the first bit that it supposed to be set. This value must be a multiple of
     * 64.
     * @param other The bit vector whose pattern to set.
     */
    void set(uint_fast64_t bitIndex, BitVector const& other);

    /*!
     * Apply a permutation of entries. That is, in row i, write the entry of row inversePermutation[i].
     * @param inversePermutation.
     * @return
     * TODO this operation is slow.
     */
    BitVector permute(std::vector<uint64_t> const& inversePermutation) const;

    /*!
     * Retrieves the content of the current bit vector at the given index for the given number of bits as a new
     * bit vector.
     *
     * @param bitIndex The index of the first bit to get. This value must be a multiple of 64.
     * @param numberOfBits The number of bits to get. This value must be a multiple of 64.
     * @return A new bit vector holding the selected bits.
     */
    storm::storage::BitVector get(uint_fast64_t bitIndex, uint_fast64_t numberOfBits) const;

    /*!
     * Retrieves the content of the current bit vector at the given index for the given number of bits as an
     * unsigned integer value.
     *
     * @param bitIndex The index of the first bit to get.
     * @param numberOfBits The number of bits to get. This value must be less or equal than 64.
     */
    uint_fast64_t getAsInt(uint_fast64_t bitIndex, uint_fast64_t numberOfBits) const;

    /*!
     *
     * @param bitIndex The index of the first of the two bits to get
     * @return A value between 0 and 3, encoded as a byte.
     */
    uint_fast64_t getTwoBitsAligned(uint_fast64_t bitIndex) const;

    /*!
     * Sets the selected number of lowermost bits of the provided value at the given bit index.
     *
     * @param bitIndex The index of the first bit to set.
     * @param numberOfBits The number of bits to set.
     * @param value The integer whose lowermost bits to set.
     */
    void setFromInt(uint_fast64_t bitIndex, uint_fast64_t numberOfBits, uint64_t value);

    /*!
     * Retrieves whether no bits are set to true in this bit vector.
     *
     * @return False iff there is at least one bit set in this vector.
     */
    bool empty() const;

    /*!
     * Retrieves whether all bits are set in this bit vector.
     * If the bit vector has size 0, this method always returns true.
     *
     * @return True iff all bits in the bit vector are set.
     */
    bool full() const;

    /*!
     * Removes all set bits from the bit vector. Calling empty() after this operation will yield true.
     */
    void clear();

    /*!
     * Sets all bits from the bit vector. Calling full() after this operation will yield true.
     */
    void fill();

    /*!
     * Returns the number of bits that are set to true in this bit vector.
     *
     * @return The number of bits that are set to true in this bit vector.
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
     * Retrieves a vector that holds at position i the number of bits set before index i.
     *
     * @return The resulting vector of 'offsets'.
     */
    std::vector<uint_fast64_t> getNumberOfSetBitsBeforeIndices() const;

    /*!
     * Retrieves the number of bits this bit vector can store.
     *
     * @return The number of bits this bit vector can hold.
     */
    size_t size() const;

    /*!
     * Returns (an approximation of) the size of the bit vector measured in bytes.
     *
     * @return The size of the bit vector measured in bytes.
     */
    std::size_t getSizeInBytes() const;

    /*!
     * Returns an iterator to the indices of the set bits in the bit vector.
     */
    const_iterator begin() const;

    /*!
     * Returns an iterator pointing at the element past the bit vector.
     */
    const_iterator end() const;

    /*!
     * Retrieves the index of the bit that is the next bit set to true in the bit vector. If there is none,
     * this function returns the number of bits this vector holds in total. Put differently, if the return
     * value is equal to a call to size(), then there is no bit set after the specified position.
     *
     * @param startingIndex The index at which to start the search for the next bit that is set. The
     * bit at this index itself is included in the search range.
     * @return The index of the next bit that is set after the given index.
     */
    uint_fast64_t getNextSetIndex(uint_fast64_t startingIndex) const;

    /*!
     * Retrieves the index of the bit that is the next bit set to false in the bit vector. If there is none,
     * this function returns the number of bits this vector holds in total. Put differently, if the return
     * value is equal to a call to size(), then there is no unset bit after the specified position.
     *
     * @param startingIndex The index at which to start the search for the next bit that is not set. The
     * bit at this index itself is included in the search range.
     * @return The index of the next bit that is set after the given index.
     */
    uint_fast64_t getNextUnsetIndex(uint_fast64_t startingIndex) const;

    /*!
     * Compare two intervals [start1, start1+length] and [start2, start2+length] and swap them if the second
     * one is larger than the first one. After the method the intervals are sorted in decreasing order.
     *
     * @param start1 Starting index of first interval.
     * @param start2 Starting index of second interval.
     * @param length Length of both intervals.
     * @return True, if the intervals were swapped, false if nothing changed.
     */
    bool compareAndSwap(uint_fast64_t start1, uint_fast64_t start2, uint_fast64_t length);

    friend std::ostream& operator<<(std::ostream& out, BitVector const& bitVector);

    void store(std::ostream&) const;
    static BitVector load(std::string const& description);

    friend struct std::hash<storm::storage::BitVector>;
    friend struct FNV1aBitVectorHash;

    template<typename StateType>
    friend struct Murmur3BitVectorHash;

   private:
    /*!
     * Creates an empty bit vector with the given number of buckets.
     *
     * @param bucketCount The number of buckets to create.
     * @param bitCount This must be the number of buckets times 64.
     */
    BitVector(uint_fast64_t bucketCount, uint_fast64_t bitCount);

    /*!
     * Retrieves the index of the next bit that is set to the given value after (and including) the given starting index.
     *
     * @param value the value of the bit whose index is to be found.
     * @param dataPtr A pointer to the first bucket of the data storage.
     * @param startingIndex The index where to start the search.
     * @param endIndex The index at which to stop the search.
     * @return The index of the bit that is set after the given starting index, but before the given end index
     * in the given bit vector or endIndex in case the end index was reached.
     */
    static uint_fast64_t getNextIndexWithValue(bool value, uint64_t const* dataPtr, uint_fast64_t startingIndex, uint_fast64_t endIndex);

    /*!
     * Truncate the last bucket so that no bits are set starting from bitCount.
     */
    void truncateLastBucket();

    /*! Retrieves the content of the current bit vector at the given index for the given number of bits as a new
     * bit vector.
     *
     * @param start The index of the first bit to get.
     * @param length The number of bits to get.
     * @return A new bit vector holding the selected bits.
     */
    BitVector getAsBitVector(uint_fast64_t start, uint_fast64_t length) const;

    /*!
     * Sets the exact bit pattern of the given bit vector starting at the given bit index. Note: the given bit
     * vector must be shorter than the current one minus the given index.
     *
     * @param start The index of the first bit that is supposed to be set.
     * @param other The bit vector whose pattern to set.
     */
    void setFromBitVector(uint_fast64_t start, BitVector const& other);

    /*!
     * Print bit vector and display all bits.
     *
     * @param out Stream to print to.
     */
    void printBits(std::ostream& out) const;

    /*!
     * Retrieves the number of buckets of the underlying storage.
     *
     * @return The number of buckets of the underlying storage.
     */
    size_t bucketCount() const;

    // The number of bits that this bit vector can hold.
    uint_fast64_t bitCount;

    // The underlying storage of 64-bit buckets for all bits of this bit vector.
    uint64_t* buckets;

    // A bit mask that can be used to reduce a modulo 64 operation to a logical "and".
    static const uint_fast64_t mod64mask = (1 << 6) - 1;
};

struct FNV1aBitVectorHash {
    std::size_t operator()(storm::storage::BitVector const& bv) const;
};

template<typename StateType>
struct Murmur3BitVectorHash {
    StateType operator()(storm::storage::BitVector const& bv) const;
};

}  // namespace storage
}  // namespace storm

namespace std {
template<>
struct hash<storm::storage::BitVector> {
    std::size_t operator()(storm::storage::BitVector const& bv) const;
};
}  // namespace std

#endif  // STORM_STORAGE_BITVECTOR_H_
