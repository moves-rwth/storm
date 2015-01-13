#include <boost/container/flat_set.hpp>
#include <iostream>
#include <algorithm>

#include "src/storage/BitVector.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

#include "src/utility/OsDetection.h"
#include "src/utility/Hash.h"
#include "src/utility/macros.h"

namespace storm {
    namespace storage {
        
		BitVector::const_iterator::const_iterator(uint64_t const* dataPtr, uint_fast64_t startIndex, uint_fast64_t endIndex, bool setOnFirstBit) : dataPtr(dataPtr), endIndex(endIndex) {
            if (setOnFirstBit) {
                // Set the index of the first set bit in the vector.
                currentIndex = getNextSetIndex(dataPtr, startIndex, endIndex);
            } else {
                currentIndex = startIndex;
            }
		}
        
        BitVector::const_iterator::const_iterator(const_iterator const& other) : dataPtr(other.dataPtr), currentIndex(other.currentIndex), endIndex(other.endIndex) {
            // Intentionally left empty.
        }
        
        BitVector::const_iterator& BitVector::const_iterator::operator=(const_iterator const& other) {
            // Only assign contents if the source and target are not the same.
            if (this != &other) {
                dataPtr = other.dataPtr;
                currentIndex = other.currentIndex;
                endIndex = other.endIndex;
            }
            return *this;
        }
        
		BitVector::const_iterator& BitVector::const_iterator::operator++() {
			currentIndex = getNextSetIndex(dataPtr, ++currentIndex, endIndex);
			return *this;
		}
        
		uint_fast64_t BitVector::const_iterator::operator*() const {
			return currentIndex;
		}
        
		bool BitVector::const_iterator::operator!=(const_iterator const& other) const {
			return currentIndex != other.currentIndex;
		}
        
        bool BitVector::const_iterator::operator==(const_iterator const& other) const {
            return currentIndex == other.currentIndex;
        }

        BitVector::BitVector() : bitCount(0), bucketVector() {
            // Intentionally left empty.
        }
        
        BitVector::BitVector(uint_fast64_t length, bool init) : bitCount(length) {
            // Compute the correct number of buckets needed to store the given number of bits.
            uint_fast64_t bucketCount = length >> 6;
            if ((length & mod64mask) != 0) {
                ++bucketCount;
            }

            // Initialize the storage with the required values.
            if (init) {
                bucketVector = std::vector<uint64_t>(bucketCount, -1ll);
                truncateLastBucket();
            } else {
                bucketVector = std::vector<uint64_t>(bucketCount, 0);
            }
        }
        
        template<typename InputIterator>
        BitVector::BitVector(uint_fast64_t length, InputIterator begin, InputIterator end) : BitVector(length) {
            set(begin, end);
        }
        
        BitVector::BitVector(uint_fast64_t bucketCount, uint_fast64_t bitCount) : bitCount(bitCount), bucketVector(bucketCount) {
            STORM_LOG_ASSERT((bucketCount << 6) == bitCount, "Bit count does not match number of buckets.");
        }
        
        BitVector::BitVector(BitVector const& other) : bitCount(other.bitCount), bucketVector(other.bucketVector) {
            // Intentionally left empty.
        }
        
        BitVector::BitVector(BitVector&& other) : bitCount(other.bitCount), bucketVector(std::move(other.bucketVector)) {
            // Intentionally left empty.
        }
        
        BitVector& BitVector::operator=(BitVector const& other) {
            // Only perform the assignment if the source and target are not identical.
            if (this != &other) {
                bitCount = other.bitCount;
                bucketVector = std::vector<uint64_t>(other.bucketVector);
            }
            return *this;
        }
        
        bool BitVector::operator<(BitVector const& other) const {
            if (this->size() < other.size()) {
                return true;
            } else if (this->size() > other.size()) {
                return false;
            }
            
            std::vector<uint64_t>::const_iterator first1 = this->bucketVector.begin();
            std::vector<uint64_t>::const_iterator last1 = this->bucketVector.end();
            std::vector<uint64_t>::const_iterator first2 = other.bucketVector.begin();
            std::vector<uint64_t>::const_iterator last2 = other.bucketVector.end();
            
            for (; first1 != last1; ++first1, ++first2) {
                if (*first1 < *first2) {
                    return true;
                } else if (*first1 > *first2) {
                    return false;
                }
            }
            return false;
        }
        
        BitVector& BitVector::operator=(BitVector&& other) {
            // Only perform the assignment if the source and target are not identical.
            if (this != &other) {
                bitCount = other.bitCount;
                bucketVector = std::move(other.bucketVector);
            }
            
            return *this;
        }
        
        bool BitVector::operator==(BitVector const& other) {
            // If the lengths of the vectors do not match, they are considered unequal.
            if (this->bitCount != other.bitCount) return false;
            
            // If the lengths match, we compare the buckets one by one.
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it1 != bucketVector.end(); ++it1, ++it2) {
                if (*it1 != *it2) {
                    return false;
                }
            }
            
            // All buckets were equal, so the bit vectors are equal.
            return true;
        }
        
        bool BitVector::operator!=(BitVector const& other) {
            return !(*this == other);
        }
        
        void BitVector::set(uint_fast64_t index, bool value) {
            if (index >= bitCount) throw storm::exceptions::OutOfRangeException() << "Invalid call to BitVector::set: written index " << index << " out of bounds.";
            uint64_t bucket = index >> 6;
            
            uint64_t mask = 1ull << (index & mod64mask);
            if (value) {
                bucketVector[bucket] |= mask;
            } else {
                bucketVector[bucket] &= ~mask;
            }
        }
        
        template<typename InputIterator>
        void BitVector::set(InputIterator begin, InputIterator end) {
            for (InputIterator it = begin; it != end; ++it) {
                this->set(*it);
            }
        }
        
        bool BitVector::operator[](uint_fast64_t index) const {
            uint_fast64_t bucket = index >> 6;
            uint_fast64_t mask = static_cast<uint_fast64_t>(1) << (index & mod64mask);
            return (this->bucketVector[bucket] & mask) == mask;
        }
        
        bool BitVector::get(uint_fast64_t index) const {
            if (index >= bitCount) throw storm::exceptions::OutOfRangeException() << "Invalid call to BitVector::get: read index " << index << " out of bounds.";
            return (*this)[index];
        }
        
        void BitVector::resize(uint_fast64_t newLength, bool init) {
            if (newLength > bitCount) {
                uint_fast64_t newBucketCount = newLength >> 6;
                if ((newLength & mod64mask) != 0) {
                    ++newBucketCount;
                }
                
                if (newBucketCount > bucketVector.size()) {
                    if (init) {
                        bucketVector.back() |= ~((1ll << (bitCount & mod64mask)) - 1ll);
                        bucketVector.resize(newBucketCount, -1ll);
                    } else {
                        bucketVector.resize(newBucketCount, 0);
                    }
                    bitCount = newLength;
                } else {
                    // If the underlying storage does not need to grow, we have to insert the missing bits.
                    if (init) {
                        bucketVector.back() |= ~((1ll << (bitCount & mod64mask)) - 1ll);
                    }
                    bitCount = newLength;
                }
                truncateLastBucket();
            } else {
                bitCount = newLength;
                bitCount = newLength;
                uint_fast64_t newBucketCount = newLength >> 6;
                if ((newLength & mod64mask) != 0) {
                    ++newBucketCount;
                }

                bucketVector.resize(newBucketCount);
                truncateLastBucket();
            }
        }
        
        BitVector BitVector::operator&(BitVector const& other) const {
            uint_fast64_t minSize =	std::min(other.bitCount, bitCount);
            
            BitVector result(minSize);
            std::vector<uint64_t>::iterator it = result.bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it != result.bucketVector.end(); ++it1, ++it2, ++it) {
                *it = *it1 & *it2;
            }
            
            return result;
        }
        
        BitVector& BitVector::operator&=(BitVector const& other) {
            
            std::vector<uint64_t>::iterator it = bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator otherIt = other.bucketVector.begin(); it != bucketVector.end() && otherIt != other.bucketVector.end(); ++it, ++otherIt) {
                *it &= *otherIt;
            }
            
            return *this;
        }
        
        BitVector BitVector::operator|(BitVector const& other) const {
            uint_fast64_t minSize =	std::min(other.bitCount, bitCount);
            
            BitVector result(minSize);
            std::vector<uint64_t>::iterator it = result.bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it != result.bucketVector.end(); ++it1, ++it2, ++it) {
                *it = *it1 | *it2;
            }
            
            result.truncateLastBucket();
            return result;
        }
        
        BitVector& BitVector::operator|=(BitVector const& other) {
            
            std::vector<uint64_t>::iterator it = bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator otherIt = other.bucketVector.begin(); it != bucketVector.end() && otherIt != other.bucketVector.end(); ++it, ++otherIt) {
                *it |= *otherIt;
            }
            
            truncateLastBucket();
            return *this;
        }
        
        BitVector BitVector::operator^(BitVector const& other) const {
            uint_fast64_t minSize =	std::min(other.bitCount, bitCount);
            
            BitVector result(minSize);
            std::vector<uint64_t>::iterator it = result.bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it != result.bucketVector.end(); ++it1, ++it2, ++it) {
                *it = *it1 ^ *it2   ;
            }
            
            result.truncateLastBucket();
            return result;
        }
        
        BitVector BitVector::operator%(BitVector const& filter) const {
            if (bitCount != filter.bitCount) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to BitVector::operator%: length mismatch of operands.";
            }
            
            BitVector result(filter.getNumberOfSetBits());
            
            // If the current bit vector has not too many elements compared to the given bit vector we prefer iterating
            // over its elements.
            if (filter.getNumberOfSetBits() / 10 < this->getNumberOfSetBits()) {
                uint_fast64_t position = 0;
                for (auto bit : filter) {
                    if ((*this)[bit]) {
                        result.set(position);
                    }
                    ++position;
                }
            } else {
                // If the given bit vector had much less elements, we iterate over its elements and accept calling the more
                // costly operation getNumberOfSetBitsBeforeIndex on the current bit vector.
                for (auto bit : (*this)) {
                    if (filter[bit]) {
                        result.set(filter.getNumberOfSetBitsBeforeIndex(bit));
                    }
                }
            }
            
            return result;
        }
        
        BitVector BitVector::operator~() const {
            BitVector result(this->bitCount);
            std::vector<uint64_t>::iterator it = result.bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(); it != result.bucketVector.end(); ++it1, ++it) {
                *it = ~(*it1);
            }
            
            result.truncateLastBucket();
            return result;
        }
        
        void BitVector::complement() {
            for (auto& element : bucketVector) {
                element = ~element;
            }
            truncateLastBucket();
        }
        
        BitVector BitVector::implies(BitVector const& other) const {
            uint_fast64_t minSize =	std::min(other.bitCount, bitCount);
            
            BitVector result(minSize);
            std::vector<uint64_t>::iterator it = result.bucketVector.begin();
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it != result.bucketVector.end(); ++it1, ++it2, ++it) {
                *it = ~(*it1) | *it2;
            }
            
            result.truncateLastBucket();
            return result;
        }
        
        bool BitVector::isSubsetOf(BitVector const& other) const {
            if (bitCount != other.bitCount) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to BitVector::isSubsetOf: length mismatch of operands.";
            }
                
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it1 != bucketVector.end(); ++it1, ++it2) {
                if ((*it1 & *it2) != *it1) {
                    return false;
                }
            }
            return true;
        }
        
        bool BitVector::isDisjointFrom(BitVector const& other) const {
            if (bitCount != other.bitCount) {
                throw storm::exceptions::InvalidArgumentException() << "Invalid call to BitVector::isDisjointFrom: length mismatch of operands.";
            }
            
            for (std::vector<uint64_t>::const_iterator it1 = bucketVector.begin(), it2 = other.bucketVector.begin(); it1 != bucketVector.end(); ++it1, ++it2) {
                if ((*it1 & *it2) != 0) {
                    return false;
                }
            }
            return true;
        }
        
        bool BitVector::matches(uint_fast64_t bitIndex, BitVector const& other) const {
            STORM_LOG_ASSERT((bitIndex & mod64mask) == 0, "Bit index must be a multiple of 64.");
            STORM_LOG_ASSERT(other.size() <= this->size() - bitIndex, "Bit vector argument is too long.");

            // Compute the first bucket that needs to be checked and the number of buckets.
            uint64_t index = bitIndex >> 6;
            
            std::vector<uint64_t>::const_iterator first1 = bucketVector.begin() + index;
            std::vector<uint64_t>::const_iterator first2 = other.bucketVector.begin();
            std::vector<uint64_t>::const_iterator last2 = other.bucketVector.end();
            
            for (; first2 != last2; ++first1, ++first2) {
                if (*first1 != *first2) {
                    return false;
                }
            }
            return true;
        }
        
        void BitVector::set(uint_fast64_t bitIndex, BitVector const& other) {
            STORM_LOG_ASSERT((bitIndex & mod64mask) == 0, "Bit index must be a multiple of 64.");
            STORM_LOG_ASSERT(other.size() <= this->size() - bitIndex, "Bit vector argument is too long.");
            
            // Compute the first bucket that needs to be checked and the number of buckets.
            uint64_t index = bitIndex >> 6;
            
            std::vector<uint64_t>::iterator first1 = bucketVector.begin() + index;
            std::vector<uint64_t>::const_iterator first2 = other.bucketVector.begin();
            std::vector<uint64_t>::const_iterator last2 = other.bucketVector.end();
            
            for (; first2 != last2; ++first1, ++first2) {
                *first1 = *first2;
            }
        }
        
        storm::storage::BitVector BitVector::get(uint_fast64_t bitIndex, uint_fast64_t numberOfBits) const {
            uint64_t numberOfBuckets = numberOfBits >> 6;
            uint64_t index = bitIndex >> 6;
            STORM_LOG_ASSERT(index + numberOfBuckets <= this->bucketCount(), "Argument is out-of-range.");
            
            storm::storage::BitVector result(numberOfBuckets, numberOfBits);
            std::copy(this->bucketVector.begin() + index, this->bucketVector.begin() + index + numberOfBuckets, result.bucketVector.begin());
            
            return result;
        }
        
        uint_fast64_t BitVector::getAsInt(uint_fast64_t bitIndex, uint_fast64_t numberOfBits) const {
            uint64_t bucket = bitIndex >> 6;
            uint64_t bitIndexInBucket = bitIndex & mod64mask;
            
            uint64_t mask = (1ull << (64 - bitIndexInBucket)) - 1;
            
            if (bitIndexInBucket + numberOfBits < 64) {
                // If the value stops before the end of the bucket, we need to erase some lower bits.
                mask &= ~((1ull << (64 - (bitIndexInBucket - numberOfBits + 2))) - 1);
                return bucketVector[bucket] & mask;
            } else if (bitIndexInBucket + numberOfBits > 64) {
                // In this case, the integer "crosses" the bucket line.
                uint64_t result = bucketVector[bucket] & mask;
                result <<= (64 - bitIndexInBucket);
                
                ++bucket;
                numberOfBits -= (64 - bitIndexInBucket);
                mask = !(1ull << (64 - numberOfBits));
                uint64_t lowerBits = bucketVector[bucket] & mask;
                result |= (lowerBits >> (64 - numberOfBits));
                
                return result;
            } else {
                // In this case, it suffices to take the current mask.
                return bucketVector[bucket] & mask;
            }
        }
        
        bool BitVector::empty() const {
            for (auto& element : bucketVector) {
                if (element != 0) {
                    return false;
                }
            }
            return true;
        }
        
        bool BitVector::full() const {
            // Check that all buckets except the last one have all bits set.
            for (uint_fast64_t index = 0; index < bucketVector.size() - 1; ++index) {
                if (bucketVector[index] != -1ull) {
                    return false;
                }
            }
            
            // Now check whether the relevant bits are set in the last bucket.
            if ((bucketVector.back() & ((1ull << (bitCount & mod64mask)) - 1ull)) != ((1ull << (bitCount & mod64mask)) - 1ull)) {
                return false;
            }
            return true;
        }
        
        void BitVector::clear() {
            for (auto& element : bucketVector) {
                element = 0;
            }
        }
                
        uint_fast64_t BitVector::getNumberOfSetBits() const {
            return getNumberOfSetBitsBeforeIndex(bucketVector.size() << 6);
        }
        
        uint_fast64_t BitVector::getNumberOfSetBitsBeforeIndex(uint_fast64_t index) const {
            uint_fast64_t result = 0;
            
            // First, count all full buckets.
            uint_fast64_t bucket = index >> 6;
            for (uint_fast64_t i = 0; i < bucket; ++i) {
                // Check if we are using g++ or clang++ and, if so, use the built-in function
#if (defined (__GNUG__) || defined(__clang__))
                result += __builtin_popcountll(bucketVector[i]);
#elif defined WINDOWS
#include <nmmintrin.h>
                // if the target machine does not support SSE4, this will fail.
                result += _mm_popcnt_u64(bucketVector[i]);
#else
                uint_fast32_t cnt;
                uint_fast64_t bitset = bucketVector[i];
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
                tmp &= bucketVector[bucket];
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
        
        size_t BitVector::size() const {
            return static_cast<size_t>(bitCount);
        }
        
        uint_fast64_t BitVector::getSizeInMemory() const {
            return sizeof(*this) + sizeof(uint64_t) * bucketVector.size();
        }
        
        BitVector::const_iterator BitVector::begin() const {
            return const_iterator(bucketVector.data(), 0, bitCount);
        }
        
        BitVector::const_iterator BitVector::end() const {
            return const_iterator(bucketVector.data(), bitCount, bitCount, false);
        }
        
        std::size_t BitVector::hash() const {
            std::size_t result = 0;
            
            boost::hash_combine(result, bucketVector.size());
            boost::hash_combine(result, bitCount);
            
            for (auto& element : bucketVector) {
                boost::hash_combine(result, element);
            }
            
            return result;
        }
        
        uint_fast64_t BitVector::getNextSetIndex(uint_fast64_t startingIndex) const {
            return getNextSetIndex(bucketVector.data(), startingIndex, bitCount);
        }
        
        uint_fast64_t BitVector::getNextSetIndex(uint64_t const* dataPtr, uint_fast64_t startingIndex, uint_fast64_t endIndex) {
            uint_fast8_t currentBitInByte = startingIndex & mod64mask;
            startingIndex >>= 6;
            uint64_t const* bucketIt = dataPtr + startingIndex;
            
            while ((startingIndex << 6) < endIndex) {
                // Compute the remaining bucket content by a right shift
                // to the current bit.
                uint_fast64_t remainingInBucket = *bucketIt >> currentBitInByte;
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
                ++startingIndex; ++bucketIt; currentBitInByte = 0;
            }
            return endIndex;
        }
        
        void BitVector::truncateLastBucket() {
            if ((bitCount & mod64mask) != 0) {
                bucketVector.back() &= (1ll << (bitCount & mod64mask)) - 1ll;
            }
        }
        
        size_t BitVector::bucketCount() const {
            return bucketVector.size();
        }
        
        std::ostream& operator<<(std::ostream& out, BitVector const& bitVector) {
            out << "bit vector(" << bitVector.getNumberOfSetBits() << "/" << bitVector.bitCount << ") [";
            for (auto index : bitVector) {
                out << index << " ";
            }
            out << "]";
            
            return out;
        }
        
        // All necessary explicit template instantiations.
        template BitVector::BitVector(uint_fast64_t length, std::vector<uint_fast64_t>::iterator begin, std::vector<uint_fast64_t>::iterator end);
        template BitVector::BitVector(uint_fast64_t length, std::vector<uint_fast64_t>::const_iterator begin, std::vector<uint_fast64_t>::const_iterator end);
        template BitVector::BitVector(uint_fast64_t length, boost::container::flat_set<uint_fast64_t>::iterator begin, boost::container::flat_set<uint_fast64_t>::iterator end);
        template BitVector::BitVector(uint_fast64_t length, boost::container::flat_set<uint_fast64_t>::const_iterator begin, boost::container::flat_set<uint_fast64_t>::const_iterator end);
        template void BitVector::set(std::vector<uint_fast64_t>::iterator begin, std::vector<uint_fast64_t>::iterator end);
        template void BitVector::set(std::vector<uint_fast64_t>::const_iterator begin, std::vector<uint_fast64_t>::const_iterator end);
        template void BitVector::set(boost::container::flat_set<uint_fast64_t>::iterator begin, boost::container::flat_set<uint_fast64_t>::iterator end);
        template void BitVector::set(boost::container::flat_set<uint_fast64_t>::const_iterator begin, boost::container::flat_set<uint_fast64_t>::const_iterator end);
    }
}

namespace std {
    std::size_t hash<storm::storage::BitVector>::operator()(storm::storage::BitVector const& bv) {
        return boost::hash_range(bv.bucketVector.begin(), bv.bucketVector.end());
    }
}