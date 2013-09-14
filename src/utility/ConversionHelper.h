/*
 * ConversionHelper.h
 *
 *  Created on: 14.09.2013
 *      Author: Philipp Berger
 * 
 * WARNING: This file REQUIRES -no-strict-aliasing!
 */

#ifndef STORM_UTILITY_CONVERSIONHELPER_H_
#define STORM_UTILITY_CONVERSIONHELPER_H_

#include <iostream>
#include <vector>
#include <cstdint>

static_assert(sizeof(unsigned long long) == sizeof(uint_fast64_t), "This program uses the GMM Backend and therefor requires unsigned long long and uint_fast64_t to be of the same size!");


namespace storm {
	namespace utility {

		class ConversionHelper {
		public:
			/*!
			* Converts a pointer to a std::vector<uint_fast64_t> to std::vector<unsigned long long>
			*/
			static std::vector<unsigned long long, std::allocator<unsigned long long>>* toUnsignedLongLong(std::vector<uint_fast64_t, std::allocator<uint_fast64_t>>* vectorPtr);

		private:
			ConversionHelper() {}
			ConversionHelper(ConversionHelper& other) {}
			~ConversionHelper() {}
		};
	}
}

#endif // STORM_UTILITY_CONVERSIONHELPER_H_