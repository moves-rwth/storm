#pragma once
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundef"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#if __GNUC__ > 8

#endif
#endif

#include "gtest/gtest.h"

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <boost/optional/optional_io.hpp>

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/utility/constants.h"
#include "storm/utility/initialize.h"

namespace testing {
namespace internal {

inline GTEST_API_ AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, storm::RationalNumber val1,
                                                       storm::RationalNumber val2, storm::RationalNumber abs_error) {
    const storm::RationalNumber diff = storm::utility::abs<storm::RationalNumber>(val1 - val2);
    if (diff <= abs_error)
        return AssertionSuccess();
    return AssertionFailure() << "The difference between " << expr1 << " and " << expr2 << " is " << diff << " (approx. "
                              << storm::utility::convertNumber<double>(diff) << "), which exceeds " << abs_error_expr << ", where\n"
                              << expr1 << " evaluates to " << val1 << " (approx. " << storm::utility::convertNumber<double>(val1) << "),\n"
                              << expr2 << " evaluates to " << val2 << " (approx. " << storm::utility::convertNumber<double>(val2) << "),\n"
                              << abs_error_expr << " evaluates to " << abs_error << " (approx. " << storm::utility::convertNumber<double>(abs_error) << ").";
}
}  // namespace internal
}  // namespace testing

namespace storm {
namespace test {
extern bool noGurobi;

bool testGurobiLicense();

inline void initialize(int* argc, char** argv) {
    // GoogleTest-specific commandline arguments should already be processed before and removed from argc/argv
    storm::utility::initializeLogger();
    // Only enable error output by default.
    storm::utility::setLogLevel(l3pp::LogLevel::ERR);
    noGurobi = !storm::test::testGurobiLicense();
    for (int i = 1; i < *argc; ++i) {
        if (std::string(argv[i]) == "--nogurobi") {
            noGurobi = true;
        } else {
            STORM_LOG_WARN("Unknown argument: " << argv[i]);
        }
    }
}

inline void enableErrorOutput() {
    // Only decrease the log level
    if (storm::utility::getLogLevel() > l3pp::LogLevel::ERR) {
        storm::utility::setLogLevel(l3pp::LogLevel::ERR);
    }
}

inline void disableOutput() {
    storm::utility::setLogLevel(l3pp::LogLevel::OFF);
}
}  // namespace test
}  // namespace storm

// Some tests have to be skipped for specific z3 versions because of a bug that was present in z3.
#ifdef STORM_HAVE_Z3
#include <z3.h>
namespace storm {
namespace test {
inline bool z3AtLeastVersion(unsigned expectedMajor, unsigned expectedMinor, unsigned expectedBuildNumber) {
    std::vector<unsigned> actual(4), expected({expectedMajor, expectedMinor, expectedBuildNumber, 0u});
    Z3_get_version(&actual[0], &actual[1], &actual[2], &actual[3]);
    for (uint64_t i = 0; i < 4; ++i) {
        if (actual[i] > expected[i]) {
            return true;
        }
        if (actual[i] < expected[i]) {
            return false;
        }
    }
    return true;  // Equal versions
}
}  // namespace test
}  // namespace storm
#endif

#define STORM_SILENT_ASSERT_THROW(statement, expected_exception) \
    storm::test::disableOutput();                                \
    ASSERT_THROW(statement, expected_exception);                 \
    storm::test::enableErrorOutput()

#define STORM_SILENT_EXPECT_THROW(statement, expected_exception) \
    storm::test::disableOutput();                                \
    EXPECT_THROW(statement, expected_exception);                 \
    storm::test::enableErrorOutput()
