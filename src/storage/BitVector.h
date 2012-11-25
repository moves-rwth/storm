#ifndef MRMC_VECTOR_BITVECTOR_H_
#define MRMC_VECTOR_BITVECTOR_H_

#include <exception>
#include <new>
#include <cmath>
#include "boost/integer/integer_mask.hpp"

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"

#include <string.h>
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace mrmc {

namespace storage {

/*!
 * A bit vector that is internally represented by an array of 64-bit values.
 */
class BitVector {

public:
	class constIndexIterator {
	public:
		constIndexIterator(uint_fast64_t* bucketPtr, uint_fast64_t* endBucketPtr) : bucketPtr(bucketPtr), endBucketPtr(endBucketPtr), offset(0), currentBitInByte(0) { }
		constIndexIterator& operator++() {
			do {
				uint_fast64_t remainingInBucket = *bucketPtr >> ++currentBitInByte;
				if (remainingInBucket != 0) {
					while ((remainingInBucket & 1) == 0) {
						remainingInBucket >>= 1;
						++currentBitInByte;
					}
				}
				offset <<= 6; ++bucketPtr; currentBitInByte = 0;
			} while (bucketPtr != endBucketPtr);
			return *this;
		}
		uint_fast64_t operator*() { return offset + currentBitInByte; }
		bool operator!=(constIndexIterator& rhs) { return bucketPtr != rhs.bucketPtr; }
	private:
		uint_fast64_t* bucketPtr;
		uint_fast64_t* endBucketPtr;
		uint_fast64_t offset;
		uint_fast8_t currentBitInByte;
	};

	//! Constructor
	/*!
	 * Constructs a bit vector which can hold the given number of bits.
	 * @param initialLength The number of bits the bit vector should be able to hold.
	 */
	BitVector(uint_fast64_t initialLength) {
		// Check whether the given length is valid.
		if (initialLength == 0) {
			LOG4CPLUS_ERROR(logger, "Trying to create bit vector of size 0.");
			throw mrmc::exceptions::invalid_argument("Trying to create a bit vector of size 0.");
		}

		// Compute the correct number of buckets needed to store the given number of bits
		bucketCount = initialLength >> 6;
		if ((initialLength & mod64mask) != 0) {
			++bucketCount;
		}

		// Finally, create the full bucket array. This should initialize the array
		// with 0s (notice the parentheses at the end) for standard conforming
		// compilers.
		bucketArray = new uint_fast64_t[bucketCount]();
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given bit vector.
	 * @param bv A reference to the bit vector to be copied.
	 */
	BitVector(const BitVector &bv) : bucketCount(bv.bucketCount) {
		LOG4CPLUS_WARN(logger, "Invoking copy constructor.");
		bucketArray = new uint_fast64_t[bucketCount];
		memcpy(bucketArray, bv.bucketArray, sizeof(uint_fast64_t) * bucketCount);
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

	/*!
	 * Resizes the bit vector to hold the given new number of bits.
	 * @param newLength The new number of bits the bit vector can hold.
	 */
	void resize(uint_fast64_t newLength) {
		uint_fast64_t newBucketCount = newLength >> 6;
		if ((newLength & mod64mask) != 0) {
			++bucketCount;
		}

		// Reserve a temporary array for copying.
		uint_fast64_t* tempArray = new uint_fast64_t[newBucketCount];

		// Copy over the elements from the old bit vector.
		uint_fast64_t copySize = (newBucketCount <= bucketCount) ? newBucketCount : bucketCount;
		memcpy(tempArray, bucketArray, sizeof(uint_fast64_t) * copySize);

		// Initialize missing values in the new bit vector.
		for (uint_fast64_t i = copySize; i < bucketCount; ++i) {
			bucketArray[i] = 0;
		}

		// Dispose of the old bit vector and set the new one.
		delete[] bucketArray;
		bucketArray = tempArray;
	}

	/*!
	 * Sets the given truth value at the given index.
	 * @param index The index where to set the truth value.
	 * @param value The truth value to set.
	 */
	void set(const uint_fast64_t index, const bool value) {
		uint_fast64_t bucket = index >> 6;
		uint_fast64_t mask = static_cast<uint_fast64_t>(1) << (index & mod64mask);
		if (value) {
			bucketArray[bucket] |= mask;
		} else {
			bucketArray[bucket] &= ~mask;
		}
	}

	/*!
	 * Retrieves the truth value at the given index.
	 * @param index The index from which to retrieve the truth value.
	 */
	bool get(const uint_fast64_t index) {
		uint_fast64_t bucket = index >> 6;
		uint_fast64_t mask = static_cast<uint_fast64_t>(1) << (index & mod64mask);
		return ((bucketArray[bucket] & mask) == mask);
	}

	/*!
	 * Performs a logical "and" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "and" of the two bit vectors.
	 */
	BitVector operator &(BitVector const &bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize << 6);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucketArray[i] = this->bucketArray[i] & bv.bucketArray[i];
		}

		return result;
	}

	/*!
	 * Performs a logical "or" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "or" of the two bit vectors.
	 */
	BitVector operator |(BitVector const &bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize << 6);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucketArray[i] = this->bucketArray[i] | bv.bucketArray[i];
		}

		return result;
	}

	/*!
	 * Performs a logical "xor" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "xor" of the two bit vectors.
	 */
	BitVector operator ^(BitVector const &bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize << 6);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucketArray[i] = this->bucketArray[i] ^ bv.bucketArray[i];
		}

		return result;
	}

	/*!
	 * Performs a logical "not" on the bit vector.
	 * @return A bit vector corresponding to the logical "not" of the bit vector.
	 */
	BitVector operator ~() {
		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(this->bucketCount << 6);
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			result.bucketArray[i] = ~this->bucketArray[i];
		}

		return result;
	}

	/*!
	 * Performs a logical "implies" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "implies" of the two bit vectors.
	 */
	BitVector implies(BitVector& bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize << 6);
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			result.bucketArray[i] = ~this->bucketArray[i]	| bv.bucketArray[i];
		}

		return result;
	}

	/*!
	 * Returns the number of bits that are set (to one) in this bit vector.
	 * @return The number of bits that are set (to one) in this bit vector.
	 */
	uint_fast64_t getNumberOfSetBits() {
		uint_fast64_t set_bits = 0;
		for (uint_fast64_t i = 0; i < bucketCount; ++i) {
			// Check if we are using g++ or clang++ and, if so, use the built-in function
#if (defined (__GNUG__) || defined(__clang__))
			set_bits += __builtin_popcountll(this->bucketArray[i]);
#else
			uint_fast32_t cnt;
			uint_fast64_t bitset = this->bucketArray[i];
			for (cnt = 0; bitset; cnt++) {
				bitset &= bitset - 1;
			}
			set_bits += cnt;
#endif
		}
		return set_bits;
	}

	/*!
	 * Retrieves the number of bits this bit vector can store.
	 */
	uint_fast64_t getSize() {
		return bucketCount << 6;
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
		return constIndexIterator(this->bucketArray, this->bucketArray + bucketCount);
	}

	/*!
	 * Returns an iterator pointing at the element past the bit vector.
	 */
	constIndexIterator end() const {
		return constIndexIterator(this->bucketArray + bucketCount, 0);
	}

private:
	/*! The number of 64-bit buckets we use as internal storage. */
	uint_fast64_t bucketCount;

	/*! Array of 64-bit buckets to store the bits. */
	uint64_t* bucketArray;

	/*! A bit mask that can be used to reduce a modulo operation to a logical "and".  */
	static const uint_fast64_t mod64mask = (1 << 6) - 1;
};

} // namespace vector

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
