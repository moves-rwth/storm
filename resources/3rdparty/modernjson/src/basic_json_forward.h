#pragma once
#include <map>
#include <string>
#include <vector>
#include <cstdint>

namespace nlohmann {
template<template<typename U, typename V, typename... Args> class ObjectType = std::map, template<typename U, typename... Args> class ArrayType = std::vector,
         class StringType = std::string, class BooleanType = bool, class NumberIntegerType = std::int64_t, class NumberUnsignedType = std::uint64_t,
         class NumberFloatType = double, template<typename U> class AllocatorType = std::allocator>
class basic_json;
}
