#pragma once

#include "gtest/gtest.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/utility/constants.h"


namespace testing {
    namespace internal {
    
        inline GTEST_API_ AssertionResult DoubleNearPredFormat(const char* expr1,
                                                const char* expr2,
                                                const char* abs_error_expr,
                                                storm::RationalNumber val1,
                                                storm::RationalNumber val2,
                                                storm::RationalNumber abs_error) {
            const storm::RationalNumber diff = storm::utility::abs<storm::RationalNumber>(val1 - val2);
            if (diff <= abs_error) return AssertionSuccess();
            return AssertionFailure()
                  << "The difference between " << expr1 << " and " << expr2
                  << " is " << diff << " (approx. " << storm::utility::convertNumber<double>(diff) << "), which exceeds " << abs_error_expr << ", where\n"
                  << expr1 << " evaluates to " << val1 << " (approx. " << storm::utility::convertNumber<double>(val1) << "),\n"
                  << expr2 << " evaluates to " << val2 << " (approx. " << storm::utility::convertNumber<double>(val2) << "),\n"
                  << abs_error_expr << " evaluates to " << abs_error << " (approx. " << storm::utility::convertNumber<double>(abs_error) << ").";
        }
    }
}
