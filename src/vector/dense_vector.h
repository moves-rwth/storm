#ifndef MRMC_VECTOR_BITVECTOR_H_
#define MRMC_VECTOR_BITVECTOR_H_

#include <exception>
#include <new>
#include <cmath>
#include "boost/integer/integer_mask.hpp"

#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>

#include "src/exceptions/invalid_state.h"
#include "src/exceptions/invalid_argument.h"
#include "src/exceptions/out_of_range.h"

namespace mrmc {

namespace vector {

//! A Vector
/*! 
	A bit vector for boolean fields or quick selection schemas on Matrix entries.
	Does NOT perform index bound checks!
 */
template <class T>
class DenseVector {
 public:
	//! Constructor
	 /*!
		\param initial_length The initial size of the boolean Array. Can be changed later on via BitVector::resize()
	 */
	BitVector(uint_fast64_t initial_length) {
		bucket_count = initial_length / 64;
		if (initial_length % 64 != 0) {
			++bucket_count;
		}
		bucket_array = new uint_fast64_t[bucket_count]();

		// init all 0
		for (uint_fast64_t i = 0; i < bucket_count; ++i) {
			bucket_array[i] = 0;
		}
	}

	//! Copy Constructor
	/*!
		Copy Constructor. Creates an exact copy of the source bit vector bv. Modification of either bit vector does not affect the other.
		@param bv A reference to the bit vector that should be copied from
	 */
	BitVector(const BitVector &bv) : bucket_count(bv.bucket_count)
	{
		pantheios::log_DEBUG("BitVector::CopyCTor: Using Copy() Ctor.");
		bucket_array = new uint_fast64_t[bucket_count]();
		memcpy(bucket_array, bv.bucket_array, sizeof(uint_fast64_t) * bucket_count);
	}

	~BitVector() {
		if (bucket_array != NULL) {
			delete[] bucket_array;
		}
	}

	void resize(uint_fast64_t new_length) {
		uint_fast64_t* tempArray = new uint_fast64_t[new_length]();

		// 64 bit/entries per uint_fast64_t
		uint_fast64_t copySize = (new_length <= (bucket_count * 64)) ? (new_length/64) : (bucket_count);
		memcpy(tempArray, bucket_array, sizeof(uint_fast64_t) * copySize);
		
		bucket_count = new_length / 64;
		if (new_length % 64 != 0) {
			++bucket_count;
		}

		delete[] bucket_array;
		bucket_array = tempArray;
	}

	void set(const uint_fast64_t index, const bool value) {
		uint_fast64_t bucket = index / 64;
		// Taking the step with mask is crucial as we NEED a 64bit shift, not a 32bit one.
		// MSVC: C4334, use 1i64 or cast to __int64.
		// result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
		uint_fast64_t mask = 1;
		mask = mask << (index % 64);
		if (value) {
			bucket_array[bucket] |= mask;
		} else {
			bucket_array[bucket] &= ~mask;
		}
	}

	bool get(const uint_fast64_t index) {
		uint_fast64_t bucket = index / 64;
		// Taking the step with mask is crucial as we NEED a 64bit shift, not a 32bit one.
		// MSVC: C4334, use 1i64 or cast to __int64.
		// result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
		uint_fast64_t mask = 1;
		mask = mask << (index % 64);
		return ((bucket_array[bucket] & mask) == mask);
	}

	// Operators
	BitVector operator &(BitVector const &bv) {
		uint_fast64_t minSize = (bv.bucket_count < this->bucket_count) ? bv.bucket_count : this->bucket_count;
		BitVector result(minSize * 64);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucket_array[i] = this->bucket_array[i] & bv.bucket_array[i];
		}
		
		return result;
	}	
	BitVector operator |(BitVector const &bv) {
		uint_fast64_t minSize = (bv.bucket_count < this->bucket_count) ? bv.bucket_count : this->bucket_count;
		BitVector result(minSize * 64);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucket_array[i] = this->bucket_array[i] | bv.bucket_array[i];
		}
		
		return result;
	}

	BitVector operator ^(BitVector const &bv) {
		uint_fast64_t minSize = (bv.bucket_count < this->bucket_count) ? bv.bucket_count : this->bucket_count;
		BitVector result(minSize * 64);
		for (uint_fast64_t i = 0; i < minSize; ++i) {
			result.bucket_array[i] = this->bucket_array[i] ^ bv.bucket_array[i];
		}
		
		return result;
	}

	BitVector operator ~() {
		BitVector result(this->bucket_count * 64);
		for (uint_fast64_t i = 0; i < this->bucket_count; ++i) {
			result.bucket_array[i] = ~this->bucket_array[i];
		}

		return result;
	}

	/*!
	 * Returns the number of bits that are set (to one) in this bit vector.
	 * @return The number of bits that are set (to one) in this bit vector.
	 */
	uint_fast64_t getNumberOfSetBits() {
		uint_fast64_t set_bits = 0;
		for (uint_fast64_t i = 0; i < bucket_count; i++) {
#ifdef __GNUG__ // check if we are using g++ and use built-in function if available
			set_bits += __builtin_popcount (this->bucket_array[i]);
#else
			uint_fast32_t cnt;
			uint_fast64_t bitset = this->bucket_array[i];
			for (cnt = 0; bitset; cnt++) {
				bitset &= bitset - 1;
			}
			set_bits += cnt;
#endif
		}
		return set_bits;
	}

	/*!
	 * Returns the size of the bit vector in memory measured in bytes.
	 * @return The size of the bit vector in memory measured in bytes.
	 */
	uint_fast64_t getSizeInMemory() {
		return sizeof(*this) + sizeof(uint_fast64_t) * bucket_count;
	}

 private:
	uint_fast64_t bucket_count;

	/*! Array containing the boolean bits for each node, 64bits/64nodes per element */
	uint_fast64_t* bucket_array;

};

} // namespace vector

} // namespace mrmc

#endif // MRMC_SPARSE_STATIC_SPARSE_MATRIX_H_
