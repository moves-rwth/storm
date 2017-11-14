#pragma once

#include <type_traits>
#include <typeinfo>

#include "storm/utility/constants.h"
#include "gtest/gtest.h"

#define STORM_EXPECT_NEAR(val1, val2, abs_error)                       \
do {  \
    if (std::is_same<typename std::remove_const<typename std::remove_reference<decltype(val1)>::type>::type, double>::value) {     \
        EXPECT_NEAR(storm::utility::convertNumber<double>(val1),       \
                    storm::utility::convertNumber<double>(val2),       \
                    storm::utility::convertNumber<double>(abs_error)); \
    } else if (storm::utility::isZero(abs_error)) {                    \
        EXPECT_EQ(val1, val2);                                         \
    } else {                                                           \
        auto valueDifference = val1;                                   \
        valueDifference -= val2;                                       \
        valueDifference = storm::utility::abs(valueDifference);        \
        EXPECT_LT(storm::utility::abs(valueDifference), abs_error);    \
        if (valueDifference > abs_error) {                             \
            std::cout << "Expected " << val2                           \
                      << " (approx " << storm::utility::convertNumber<double>(val2) \
                      << ") but got "  << val1                         \
                      << " (approx " << storm::utility::convertNumber<double>(val1) \
                      << ")." << std::endl;                            \
        }                                                              \
    }                                                                  \
} while (false)