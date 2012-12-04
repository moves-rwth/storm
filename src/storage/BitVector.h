#ifndef MRMC_VECTOR_BITVECTOR_H_
#define MRMC_VECTOR_BITVECTOR_H_

#include <exception>
#include <new>
#include <cmath>
#include "boost/integer/integer_mask.hpp"

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

#include <iostream>

namespace mrmc {

namespace storage {

/*!
 * A bit vector that is internally represented by an array of 64-bit values.
 */
class BitVector {

public:
	/*!
	 * A class that enables iterating over the indices of the bit vector whose
	 * bits are set to true. Note that this is a const iterator, which cannot
	 * alter the bit vector.
	 */
	class constIndexIterator {
		friend class BitVector;
	public:

		/*!
		 * Constructs a const iterator over the indices of the set bits in the
		 * given bit vector, starting and stopping, respectively, at the given
		 * indices.
		 * @param bitVector The bit vector to iterate over.
		 * @param startIndex The index where to begin looking for set bits.
		 * @param setOnFirstBit If set, upon construction, the current index is
		 * set to the first set bit in the bit vector.
		 * @param endIndex The number of elements to iterate over.
		 */
		constIndexIterator(const BitVector& bitVector, uint_fast64_t startIndex, uint_fast64_t endIndex, bool setOnFirstBit = true) : bitVector(bitVector), endIndex(endIndex) {
			if (setOnFirstBit) {
				currentIndex = bitVector.getNextSetIndex(startIndex, endIndex);
			} else {
				currentIndex = startIndex;
			}
		}

		/*!
		 * Increases the position of the iterator to the position of the next bit that
		 * is set to true.
		 * @return A reference to this iterator.
		 */
		constIndexIterator& operator++() {
			currentIndex = bitVector.getNextSetIndex(++currentIndex, endIndex);
			return *this;
		}

		/*!
		 * Returns the index of the current bit that is set to true.
		 * @return The index of the current bit that is set to true.
		 */
		uint_fast64_t operator*() const { return currentIndex; }

		/*!
		 * Compares the iterator with another iterator to determine whether
		 * the iteration process has reached the end.
		 */
		bool operator!=(const constIndexIterator& rhs) const { return currentIndex != rhs.currentIndex; }
	private:

		/*! The bit vector to search for set bits. */
		const BitVector& bitVector;

		/*! The index of the most recently discovered set bit. */
		uint_fast64_t currentIndex;

		/*!
		 * The index of the element past the end. Used for properly terminating
		 * the search for set bits.
		 */
		uint_fast64_t endIndex;
	};

	//! Constructor
	/*!
	 * Constructs a bit vector which can hold the given number of bits and
	 * initializes all bits to the provided truth value.
	 * @param length The number of bits the bit vector should be able to hold.
	 * @param initTrue The initial value of the first |length| bits.
	 */
	BitVector(uint_fast64_t length, bool initTrue = false) : endIterator(*this, length, length, false) {
		// Check whether the given length is valid.
		if (length == 0) {
			LOG4CPLUS_ERROR(logger, "Trying to create bit vector of size 0.");
			throw mrmc::exceptions::invalid_argument("Trying to create a bit vector of size 0.");
		}

		bitCount = length;
		// Compute the correct number of buckets needed to store the given number of bits
		bucketCount = length >> 6;
		if ((length & mod64mask) != 0) {
			++bucketCount;
		}

		if (initTrue) {
			// Finally, create the full bucket array.
			bucketArray = new uint64_t[bucketCount];

			// Now initialize the values.
			for (uint_fast64_t i = 0; i < bucketCount; ++i) {
				bucketArray[i] = -1ll;
			}

			truncateLastBucket();
		} else {
			// Finally, create the full bucket array.
			bucketArray = new uint64_t[bucketCount]();
		}
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given bit vector.
	 * @param bv A reference to the bit vector to be copied.
	 */
	BitVector(const BitVector &bv) : bucketCount(bv.bucketCount), bitCount(bv.bitCount), endIterator(*this, bitCount, bitCount, false) {
		LOG4CPLUS_WARN(logger, "Invoking copy constructor.");
		bucketArray = new uint64_t[bucketCount];
		std::copy(bv.bucketArray, bv.bucketArray + bucketCount, bucketArray);
	}

	//! Destructor
	/*!
	 * Destructor. Frees the underlying bucket array.
	 */
	~BitVector() {
		if (bucketArray != nullptr) {
			delete[] bucketArray;
		}
	}

	//! Assignment Operator
	/*!
	 * Assigns the given bit vector to the current bit vector by a deep copy.
	 * @param bv The bit vector to assign to the current bit vector.
	 * @return A reference to this bit vector after it has been assigned the
	 * given bit vector by means of a deep copy.
	 */
	BitVector& operator=(const BitVector& bv) {
		if (this->bucketArray != nullptr) {
			delete[] this->bucketArray;
		}
		bucketCount = bv.bucketCount;
		bitCount = bv.bitCount;
		this->bucketArray = new uint64_t[bucketCount];
		std::copy(bv.bucketArray, bv.bucketArray + bucketCount, this->bucketArray);
		updateEndIterator();
		return *this;
	}

	/*!
	 * Resizes the bit vector to hold the given new number of bits.
	 * @param newLength The new number of bits the bit vector can hold.
	 */
	void resize(uint_fast64_t newLength) {
		bitCount = newLength;
		uint_fast64_t newBucketCount = newLength >> 6;
		if ((newLength & mod64mask) != 0) {
			++newBucketCount;
		}

		// Reserve a temporary array for copying.
		uint_fast64_t* tempArray = new uint_fast64_t[newBucketCount];

		// Copy over the elements from the old bit vector.
		uint_fast64_t copySize = (newBucketCount <= bucketCount) ? newBucketCount : bucketCount;
		std::copy(this->bucketArray, this->bucketArray + copySize, tempArray);

		// Initialize missing values in the new bit vector.
		for (uint_fast64_t i = copySize; i < newBucketCount; ++i) {
			tempArray[i] = 0;
		}

		updateEndIterator();

		// Dispose of the old bit vector and set the new one.
		delete[] this->bucketArray;
		this->bucketArray = tempArray;
		this->bucketCount = newBucketCount;
	}

	/*!
	 * Sets the given truth value at the given index.
	 * @param index The index where to set the truth value.
	 * @param value The truth value to set.
	 */
	void set(const uint_fast64_t index, const bool value) {
		uint_fast64_t bucket = index >> 6;
		if (bucket >= this->bucketCount) throw mrmc::exceptions::out_of_range();
		uint_fast64_t mask = static_cast<uint_fast64_t>(1) << (index & mod64mask);
		if (value) {
			this->bucketArray[bucket] |= mask;
		} else {
			this->bucketArray[bucket] &= ~mask;
		}
		if (bucket == this->bucketCount - 1) {
			truncateLastBucket();
		}
	}

	/*!
	 * Retrieves the truth value at the given index.
	 * @param index The index from which to retrieve the truth value.
	 */
	bool get(const uint_fast64_t index) const {
		uint_fast64_t bucket = index >> 6;
		if (bucket >= this->bucketCount) throw mrmc::exceptions::out_of_range();
		uint_fast64_t mask = static_cast<uint_fast64_t>(1) << (index & mod64mask);
		return ((this->bucketArray[bucket] & mask) == mask);
	}

	/*!
	 * Performs a logical "and" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "and" of the two bit vectors.
	 */
	BitVector operator &(const BitVector &bv) const {
		uint_fast64_t minSize =	(bv.bitCount < this->bitCount) ? bv.bitCount : this->bitCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize);
		for (uint_fast64_t i = 0; i < result.bucketCount; ++i) {
			result.bucketArray[i] = this->bucketArray[i] & bv.bucketArray[i];
		}

		result.truncateLastBucket();
		return result;
	}

	/*!
	 * Performs a logical "and" with the given bit vector and assigns the result to the
	 * current bit vector. In case the sizes of the bit vectors do not match,
	 * only the matching portion is considered and the overlapping bits are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A reference to the current bit vector corresponding to the logical "and"
	 * of the two bit vectors.
	 */
	BitVector operator &=(const BitVector bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		for (uint_fast64_t i = 0; i < minSize; ++i) {
			this->bucketArray[i] &= bv.bucketArray[i];
		}

		truncateLastBucket();
		return *this;
	}

	/*!
	 * Performs a logical "or" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "or" of the two bit vectors.
	 */
	BitVector operator |(const BitVector &bv) const {
		uint_fast64_t minSize =	(bv.bitCount < this->bitCount) ? bv.bitCount : this->bitCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize);
		for (uint_fast64_t i = 0; i < result.bucketCount; ++i) {
			result.bucketArray[i] = this->bucketArray[i] | bv.bucketArray[i];
		}

		result.truncateLastBucket();
		return result;
	}

	/*!
	 * Performs a logical "or" with the given bit vector and assigns the result to the
	 * current bit vector. In case the sizes of the bit vectors do not match,
	 * only the matching portion is considered and the overlapping bits are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A reference to the current bit vector corresponding to the logical "or"
	 * of the two bit vectors.
	 */
	BitVector& operator |=(const BitVector bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		for (uint_fast64_t i = 0; i < minSize; ++i) {
			this->bucketArray[i] |= bv.bucketArray[i];
		}

		truncateLastBucket();
		return *this;
	}

	/*!
	 * Performs a logical "xor" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "xor" of the two bit vectors.
	 */
	BitVector operator ^(const BitVector &bv) const {
		uint_fast64_t minSize =	(bv.bitCount < this->bitCount) ? bv.bitCount : this->bitCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize);
		for (uint_fast64_t i = 0; i < result.bucketCount; ++i) {
			result.bucketArray[i] = this->bucketArray[i] ^ bv.bucketArray[i];
		}

		result.truncateLastBucket();
		return result;
	}

	/*!
	 * Performs a logical "not" on the bit vector.
	 * @return A bit vector corresponding to the logical "not" of the bit vector.
	 */
	BitVector operator ~() const {
		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(this->bitCount);
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			result.bucketArray[i] = ~this->bucketArray[i];
		}

		result.truncateLastBucket();
		return result;
	}

	/*!
	 * Negates all bits in the bit vector.
	 */
	void complement() {
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			this->bucketArray[i] = ~this->bucketArray[i];
		}
		truncateLastBucket();
	}

	/*!
	 * Performs a logical "implies" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "implies" of the two bit vectors.
	 */
	BitVector implies(const BitVector& bv) const {
		uint_fast64_t minSize =	(bv.bitCount < this->bitCount) ? bv.bitCount : this->bitCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize);
		for (uint_fast64_t i = 0; i < result.bucketCount - 1; ++i) {
			result.bucketArray[i] = ~this->bucketArray[i] | bv.bucketArray[i];
		}

		result.truncateLastBucket();
		return result;
	}

	/*!
	 * Adds all indices of bits set to one to the provided list.
	 * @param list The list to which to append the indices.
	 */
	void getList(std::vector<uint_fast64_t>& list) const {
		for (auto index : *this) {
			list.push_back(index);
		}
	}

	/*!
	 * Returns the number of bits that are set (to one) in this bit vector.
	 * @return The number of bits that are set (to one) in this bit vector.
	 */
	uint_fast64_t getNumberOfSetBits() {
		return getNumberOfSetBitsBeforeIndex(bucketCount << 6);
	}

	uint_fast64_t getNumberOfSetBitsBeforeIndex(uint_fast64_t index) {
		uint_fast64_t result = 0;
		// First, count all full buckets.
		uint_fast64_t bucket = index >> 6;
		for (uint_fast64_t i = 0; i < bucket; ++i) {
			// Check if we are using g++ or clang++ and, if so, use the built-in function
#if (defined (__GNUG__) || defined(__clang__))
			result += __builtin_popcountll(this->bucketArray[i]);
#else
			uint_fast32_t cnt;
			uint_fast64_t bitset = this->bucketArray[i];
			for (cnt = 0; bitset; cnt++) {
				bitset &= bitset - 1;
			}
			result += cnt;
#endif
		}

		// Now check if we have to count part of a bucket.
		uint64_t tmp = index & mod64mask;
		if (tmp != 0) {
			tmp = ((1ll << (tmp & mod64mask)) - 1ll);
			tmp &= bucketArray[bucket];
			// Check if we are using g++ or clang++ and, if so, use the built-in function
#if (defined (__GNUG__) || defined(__clang__))
			result += __builtin_popcountll(tmp);
#else
			uint_fast32_t cnt;
			uint64_t bitset = tmp;
			for (cnt = 0; bitset; cnt++) {
				bitset &= bitset - 1;
			}
			result += cnt;
#endif
		}

		return result;
	}

	/*!
	 * Retrieves the number of bits this bit vector can store.
	 */
	uint_fast64_t getSize() {
		return bitCount;
	}

	/*!
	 * Returns the size of the bit vector in memory measured in bytes.
	 * @return The size of the bit vector in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() {
		return sizeof(*this) + sizeof(uint_fast64_t) * bucketCount;
	}

	/*!
	 * Returns an iterator to the indices of the set bits in the bit vector.
	 */
	constIndexIterator begin() const {
		return constIndexIterator(*this, 0, bitCount);
	}

	/*!
	 * Returns an iterator pointing at the element past the bit vector.
	 */
	const constIndexIterator& end() const {
		return endIterator;
	}

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
	uint_fast64_t getNextSetIndex(uint_fast64_t startingIndex, uint_fast64_t endIndex) const {
		uint_fast8_t currentBitInByte = startingIndex & mod64mask;
		startingIndex >>= 6;
		uint64_t* bucketPtr = this->bucketArray + startingIndex;

		do {
			// Compute the remaining bucket content by a right shift
			// to the current bit.
			uint_fast64_t remainingInBucket = *bucketPtr >> currentBitInByte;
			// Check if there is at least one bit in the remainder of the bucket
			// that is set to true.
			if (remainingInBucket != 0) {
				// Find that bit.
				while ((remainingInBucket & 1) == 0) {
					remainingInBucket >>= 1;
					++currentBitInByte;
				}
				// Only return the index of the set bit if we are still in the
				// valid range.
				if ((startingIndex << 6) + currentBitInByte < endIndex) {
					return (startingIndex << 6) + currentBitInByte;
				} else {
					return endIndex;
				}
			}

			// Advance to the next bucket.
			++startingIndex; ++bucketPtr; currentBitInByte = 0;
		} while ((startingIndex << 6) < endIndex);
		return endIndex;
	}

	/*!
	 * Truncate the last bucket so that no bits are set in the range starting
	 * from (bitCount + 1).
	 */
	void truncateLastBucket() {
		if ((bitCount & mod64mask) != 0) {
			uint64_t mask = ((1ll << (bitCount & mod64mask)) - 1ll);
			bucketArray[bucketCount - 1] = bucketArray[bucketCount - 1] & mask;
		}
	}

	/*!
	 * Updates the end iterator to the correct past-the-end position. Needs
	 * to be called whenever the size of the bit vector changed.
	 */
	void updateEndIterator() {
		endIterator.currentIndex = bitCount;
	}

	/*! The number of 64-bit buckets we use as internal storage. */
	uint_fast64_t bucketCount;

	/*! The number of bits that have to be stored */
	uint_fast64_t bitCount;

	/*! Array of 64-bit buckets to store the bits. */
	uint64_t* bucketArray;

	/*! An iterator marking the end of the bit vector. */
	constIndexIterator endIterator;

	/*! A bit mask that can be used to reduce a modulo operation to a logical "and".  */
	static const uint_fast64_t mod64mask = (1 << 6) - 1;
};

} // namespace storage

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
