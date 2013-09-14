#include "ConversionHelper.h"

std::vector<unsigned long long, std::allocator<unsigned long long>>* storm::utility::ConversionHelper::toUnsignedLongLong(std::vector<uint_fast64_t, std::allocator<uint_fast64_t>>* vectorPtr) {
	return reinterpret_cast<std::vector<unsigned long long, std::allocator<unsigned long long>> *>(vectorPtr);
}