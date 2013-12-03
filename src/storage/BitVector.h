#ifndef STORM_STORAGE_BITVECTOR_H_
#define STORM_STORAGE_BITVECTOR_H_

#include <exception>
#include <new>
#include <cmath>
#include <cstdint>

#include "src/storage/VectorSet.h"

#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

#include "src/utility/OsDetection.h"
#include "src/utility/Hash.h"
#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

#include <iostream>

namespace storm {

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
		uint_fast64_t operator*() const {
			return currentIndex;
		}

		/*!
		 * Compares the iterator with another iterator to determine whether
		 * the iteration process has reached the end.
		 */
		bool operator!=(const constIndexIterator& rhs) const {
			return currentIndex != rhs.currentIndex;
		}

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

    /*
     * Standard constructor. Constructs an empty bit vector of length 0.
     */
    BitVector() : bucketCount(0), bitCount(0), bucketArray(nullptr), endIterator(*this, 0, 0, false), truncateMask(0) {
        // Intentionally left empty.
    }
    
	/*!
	 * Constructs a bit vector which can hold the given number of bits and
	 * initializes all bits to the provided truth value.
	 * @param length The number of bits the bit vector should be able to hold.
	 * @param initTrue The initial value of the first |length| bits.
	 */
	BitVector(uint_fast64_t length, bool initTrue = false) : bitCount(length), endIterator(*this, length, length, false), truncateMask((1ll << (bitCount & mod64mask)) - 1ll) {
		// Check whether the given length is valid.
		if (length == 0) {
			LOG4CPLUS_ERROR(logger, "Cannot create bit vector of size 0.");
			throw storm::exceptions::InvalidArgumentException() << "Cannot create bit vector of size 0.";
		}

		// Compute the correct number of buckets needed to store the given number of bits
		bucketCount = length >> 6;
		if ((length & mod64mask) != 0) {
			++bucketCount;
		}

		if (initTrue) {
			// Finally, create the full bucket array.
			this->bucketArray = new uint64_t[bucketCount];

			// Now initialize the values.
			for (uint_fast64_t i = 0; i < bucketCount; ++i) {
				this->bucketArray[i] = -1ll;
			}

			truncateLastBucket();
		} else {
			// Finally, create the full bucket array.
			this->bucketArray = new uint64_t[bucketCount]();
		}
	}
    
    /*!
     * Creates a bit vector that has exactly those bits set that are defined by the given set.
     *
     * @param length The length of the bit vector to create.
     * @param bitsToSet A set of indices whose bits to set in the bit vector.
	 * @param initTrue The initial value of the first |length| bits.
     */
    BitVector(uint_fast64_t length, storm::storage::VectorSet<uint_fast64_t> const& bitsToSet, bool initTrue = false) : BitVector(length, initTrue) {
        for (auto bit : bitsToSet) {
            this->set(bit, true);
        }
    }

    /*!
     * Creates a bit vector that has exactly the bits set that are given by the provided iterator range.
     *
     * @param The length of the bit vector.
     * @param begin The begin of the iterator range.
     * @param end The end of the iterator range.
     */
    template<typename InputIterator>
    BitVector(uint_fast64_t length, InputIterator begin, InputIterator end) : BitVector(length) {
        for (InputIterator it = begin; it != end; ++it) {
            this->set(*it, true);
        }
    }
    
	/*!
	 * Copy Constructor. Performs a deep copy of the given bit vector.
	 * @param bv A reference to the bit vector to be copied.
	 */
	BitVector(const BitVector & bv) : bucketCount(bv.bucketCount), bitCount(bv.bitCount), endIterator(*this, bitCount, bitCount, false), truncateMask((1ll << (bitCount & mod64mask)) - 1ll) {
		LOG4CPLUS_DEBUG(logger, "Invoking copy constructor.");
		bucketArray = new uint64_t[bucketCount];
		std::copy(bv.bucketArray, bv.bucketArray + this->bucketCount, this->bucketArray);
	}
    
    /*!
	 * Copy Constructor. Performs a deep copy of the bits in the given bit vector that are given by the filter.
	 * @param bv A reference to the bit vector to be copied.
     * @param filter The filter to apply for copying.
	 */
    BitVector(BitVector const& other, BitVector const& filter) : bitCount(filter.getNumberOfSetBits()), endIterator(*this, bitCount, bitCount, false), truncateMask((1ll << (bitCount & mod64mask)) - 1ll) {
        // Determine the number of buckets we need for the given bit count and create bucket array.
        bucketCount = bitCount >> 6;
        if ((bitCount & mod64mask) != 0) {
            ++bucketCount;
        }
        this->bucketArray = new uint64_t[bucketCount]();
        
        // Now copy over all bits given by the filter.
        uint_fast64_t nextPosition = 0;
        for (auto position : filter) {
            this->set(nextPosition, other.get(position));
            nextPosition++;
        }
    }
    
    /*!
     * Move constructor. Move constructs the bit vector from the given bit vector.
     *
     */
    BitVector(BitVector&& bv) : bucketCount(bv.bucketCount), bitCount(bv.bitCount), bucketArray(bv.bucketArray), endIterator(*this, bitCount, bitCount, false), truncateMask((1ll << (bitCount & mod64mask)) - 1ll) {
        LOG4CPLUS_DEBUG(logger, "Invoking move constructor.");
        bv.bucketArray = nullptr;
    }

	/*!
	 * Destructor. Frees the underlying bucket array.
	 */
	~BitVector() {
		if (this->bucketArray != nullptr) {
			delete[] this->bucketArray;
		}
	}

	/*!
	 * Compares the given bit vector with the current one.
	 */
	bool operator==(BitVector const& bv) {
		// If the lengths of the vectors do not match, they are considered unequal.
		if (this->bitCount != bv.bitCount) return false;

		// If the lengths match, we compare the buckets one by one.
		for (uint_fast64_t index = 0; index < this->bucketCount; ++index) {
			if (this->bucketArray[index] != bv.bucketArray[index]) {
				return false;
			}
		}

		// All buckets were equal, so the bit vectors are equal.
		return true;
	}

	/*!
	 * Assigns the given bit vector to the current bit vector by a deep copy.
	 * @param bv The bit vector to assign to the current bit vector.
	 * @return A reference to this bit vector after it has been assigned the
	 * given bit vector by means of a deep copy.
	 */
	BitVector& operator=(BitVector const& bv) {
        LOG4CPLUS_DEBUG(logger, "Performing copy assignment.");
        // Check if we need to dispose of our current storage.
		if (this->bucketArray != nullptr) {
			delete[] this->bucketArray;
		}
        // Copy the values from the other bit vector.
		bucketCount = bv.bucketCount;
		bitCount = bv.bitCount;
		bucketArray = new uint64_t[bucketCount];
		std::copy(bv.bucketArray, bv.bucketArray + bucketCount, bucketArray);
		updateSizeChange();
		return *this;
	}
    
    /*!
     * Move assigns the given bit vector to the current bit vector.
     *
     * @param bv The bit vector whose content is moved to the current bit vector.
     * @return A reference to this bit vector after the contents of the given bit vector
     * have been moved into it.
     */
    BitVector& operator=(BitVector&& bv) {
        LOG4CPLUS_DEBUG(logger, "Performing move assignment.");
        // Only perform the assignment if the source and target are not identical.
        if (this != &bv) {
            // Check if we need to dispose of our current storage.
            if (this->bucketArray != nullptr) {
                delete[] this->bucketArray;
            }
        
            // Copy the values from the other bit vector, but directly steal its storage.
            bucketCount = bv.bucketCount;
            bitCount = bv.bitCount;
            bucketArray = bv.bucketArray;
            updateSizeChange();
        
            // Now alter the other bit vector such that it does not dispose of our stolen storage.
            bv.bucketArray = nullptr;
        }
        
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
		uint64_t* tempArray = new uint64_t[newBucketCount];

		// Copy over the elements from the old bit vector.
		uint_fast64_t copySize = (newBucketCount <= bucketCount) ? newBucketCount : bucketCount;
		std::copy(this->bucketArray, this->bucketArray + copySize, tempArray);

		// Initialize missing values in the new bit vector.
		for (uint_fast64_t i = copySize; i < newBucketCount; ++i) {
			tempArray[i] = 0;
		}

		updateSizeChange();

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
	void set(const uint_fast64_t index, bool value = true) {
		uint_fast64_t bucket = index >> 6;
		if (bucket >= this->bucketCount) throw storm::exceptions::OutOfRangeException() << "Written index " << index << " out of bounds.";
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
     * Sets all bits in the given iterator range.
     *
     * @param begin The begin of the iterator range.
     * @param end The element past the last element of the iterator range.
     */
    template<typename InputIterator>
    void set(InputIterator begin, InputIterator end) {
        for (InputIterator it = begin; it != end; ++it) {
            this->set(*it);
        }
    }

	/*!
	 * Retrieves the truth value at the given index.
	 * @param index The index from which to retrieve the truth value.
	 */
	bool get(const uint_fast64_t index) const {
		uint_fast64_t bucket = index >> 6;
		if (bucket >= this->bucketCount) throw storm::exceptions::OutOfRangeException() << "Read index " << index << " out of bounds.";
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
	BitVector operator&(BitVector const& bv) const {
		uint_fast64_t minSize =	(bv.bitCount < this->bitCount) ? bv.bitCount : this->bitCount;

		// Create resulting bit vector and perform the operation on the individual elements.
		BitVector result(minSize);
		for (uint_fast64_t i = 0; i < result.bucketCount; ++i) {
			result.bucketArray[i] = this->bucketArray[i] & bv.bucketArray[i];
		}

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
	BitVector& operator&=(BitVector const& bv) {
		uint_fast64_t minSize =	(bv.bucketCount < this->bucketCount) ? bv.bucketCount : this->bucketCount;

		for (uint_fast64_t i = 0; i < minSize; ++i) {
			this->bucketArray[i] &= bv.bucketArray[i];
		}

		return *this;
	}

	/*!
	 * Performs a logical "or" with the given bit vector. In case the sizes of the bit vectors
	 * do not match, only the matching portion is considered and the overlapping bits
	 * are set to 0.
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "or" of the two bit vectors.
	 */
	BitVector operator|(BitVector const& bv) const {
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
	BitVector& operator|=(BitVector const& bv) {
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
	BitVector operator^(BitVector const& bv) const {
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
	BitVector operator~() const {
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
     *
	 * @param bv A reference to the bit vector to use for the operation.
	 * @return A bit vector corresponding to the logical "implies" of the two bit vectors.
	 */
	BitVector implies(BitVector const& bv) const {
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
	 * Checks whether all bits that are set in the current bit vector are also set in the given bit
	 * vector.
     *
	 * @param bv A reference to the bit vector whose bits are (possibly) a superset of the bits of
	 * the current bit vector.
	 * @return True iff all bits that are set in the current bit vector are also set in the given bit
	 * vector.
	 */
	bool isSubsetOf(BitVector const& bv) const {
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			if ((this->bucketArray[i] & bv.bucketArray[i]) != this->bucketArray[i]) {
				return false;
			}
		}
		return true;
	}

	/*!
	 * Checks whether none of the bits that are set in the current bit vector are also set in the
	 * given bit vector.
     *
	 * @param bv A reference to the bit vector whose bits are (possibly) disjoint from the bits in
	 * the current bit vector.
	 * @returns True iff none of the bits that are set in the current bit vector are also set in the
	 * given bit vector.
	 */
	bool isDisjointFrom(BitVector const& bv) const {
		for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
			if ((this->bucketArray[i] & bv.bucketArray[i]) != 0) {
				return false;
			}
		}
		return true;
	}
    
    /*!
	 * Computes a bit vector such that bit i is set iff the i-th set bit of the current bit vector is also contained
     * in the given bit vector.
     *
	 * @param bv A reference the bit vector to be used.
	 * @return A bit vector whose i-th bit is set iff the i-th set bit of the current bit vector is also contained
     * in the given bit vector.
	 */
	BitVector operator%(BitVector const& bv) const {
		// Create resulting bit vector.
		BitVector result(this->getNumberOfSetBits());
        
        // If the current bit vector has not too many elements compared to the given bit vector we prefer iterating
        // over its elements.
        if (this->getNumberOfSetBits() / 10 < bv.getNumberOfSetBits()) {
            uint_fast64_t position = 0;
            for (auto bit : *this) {
                if (bv.get(bit)) {
                    result.set(position, true);
                }
                ++position;
            }
        } else {
            // If the given bit vector had much less elements, we iterate over its elements and accept calling the more
            // costly operation getNumberOfSetBitsBeforeIndex on the current bit vector.
            for (auto bit : bv) {
                if (this->get(bit)) {
                    result.set(this->getNumberOfSetBitsBeforeIndex(bit), true);
                }
            }
        }
        
		return result;
	}
    
    /*!
     * Retrieves whether there is at least one bit set in the vector.
     *
     * @return True if there is at least one bit set in this vector.
     */
    bool empty() const {
        for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
            if (this->bucketArray[i] != 0) {
                return false;
            }
        }
        return true;
    }
    
    /*!
     * Removes all set bits from the bit vector.
     */
    void clear() {
        for (uint_fast64_t i = 0; i < this->bucketCount; ++i) {
            this->bucketArray[i] = 0;
        }
    }
    
    /*!
     * Returns a list containing all indices such that the bits at these indices are set to true
     * in the bit vector.
     *
     * @return A vector of indices of set bits in the bit vector.
     */
    std::vector<uint_fast64_t> getSetIndicesList() const {
        std::vector<uint_fast64_t> result;
        result.reserve(this->getNumberOfSetBits());
        for (auto index : *this) {
			result.push_back(index);
		}
        return result;
    }

	/*!
	 * Adds all indices of bits set to one to the given list.
     *
	 * @param list The list to which to append the indices.
	 */
	void addSetIndicesToVector(std::vector<uint_fast64_t>& vector) const {
		for (auto index : *this) {
			vector.push_back(index);
		}
	}

	/*!
	 * Returns the number of bits that are set (to one) in this bit vector.
	 * @return The number of bits that are set (to one) in this bit vector.
	 */
	uint_fast64_t getNumberOfSetBits() const {
		return getNumberOfSetBitsBeforeIndex(bucketCount << 6);
	}

	uint_fast64_t getNumberOfSetBitsBeforeIndex(uint_fast64_t index) const {
		uint_fast64_t result = 0;
		// First, count all full buckets.
		uint_fast64_t bucket = index >> 6;
		for (uint_fast64_t i = 0; i < bucket; ++i) {
			// Check if we are using g++ or clang++ and, if so, use the built-in function
#if (defined (__GNUG__) || defined(__clang__))
			result += __builtin_popcountll(this->bucketArray[i]);
#elif defined WINDOWS
			#include <nmmintrin.h>
			// if the target machine does not support SSE4, this will fail.
			result += _mm_popcnt_u64(this->bucketArray[i]);
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
	size_t size() const {
		return static_cast<size_t>(bitCount);
	}

	/*!
	 * Returns the size of the bit vector in memory measured in bytes.
	 * @return The size of the bit vector in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() const {
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

	/*!
	 * Returns a string representation of the bit vector.
	 */
	std::string toString() const {
		std::stringstream result;
		result << "bit vector(" << this->getNumberOfSetBits() << "/" << bitCount << ") [";
		for (auto index : *this) {
			result << index << " ";
		}
		result << "]";

		return result.str();
	}

	/*!
	 * Calculates a hash over all values contained in this Sparse Matrix.
	 * @return size_t A Hash Value
	 */
	std::size_t getHash() const {
		std::size_t result = 0;

		boost::hash_combine(result, bucketCount);
		boost::hash_combine(result, bitCount);
		
		for (uint_fast64_t i = 0; i < bucketCount; ++i) {
			boost::hash_combine(result, bucketArray[i]);
		}

		return result;
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

		while ((startingIndex << 6) < endIndex) {
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
		}
		return endIndex;
	}

	/*!
	 * Truncate the last bucket so that no bits are set in the range starting
	 * from (bitCount + 1).
	 */
	void truncateLastBucket() {
		if ((bitCount & mod64mask) != 0) {
			bucketArray[bucketCount - 1] = bucketArray[bucketCount - 1] & truncateMask;
		}
	}

	/*!
	 * Updates internal structures in case the size of the bit vector changed. Needs to be called
	 * after the size of the bit vector changed.
	 */
	void updateSizeChange() {
		endIterator.currentIndex = bitCount;
		truncateMask = (1ll << (bitCount & mod64mask)) - 1ll;
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

	uint64_t truncateMask;
};

} // namespace storage

} // namespace storm

#endif // STORM_STORAGE_BITVECTOR_H_
