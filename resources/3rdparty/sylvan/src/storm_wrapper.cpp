#include "storm_wrapper.h"

#include <cstring>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <mutex>

#include "storm/adapters/CarlAdapter.h"
#include "storm/utility/constants.h"
#include "storm/exceptions/InvalidOperationException.h"

#include <sylvan_config.h>
#include <sylvan.h>
#include <sylvan_common.h>
#include <sylvan_mtbdd.h>

// TODO: remove and replace by proper detection in cmake
#define RATIONAL_NUMBER_THREAD_SAFE

// A mutex that is used to lock all operations accessing rational numbers as they are not necessarily thread-safe.
#ifndef RATIONAL_NUMBER_THREAD_SAFE
std::mutex rationalNumberMutex;
#endif

// A mutex that is used to lock all operations accessing rational functions as carl is not necessarily thread-safe.
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
std::mutex rationalFunctionMutex;
#endif

/***************************************************
 Function-wrappers for storm::RationalNumber
 ****************************************************/

void storm_rational_number_init(storm_rational_number_ptr* a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm_rational_number_ptr srn_ptr = new storm::RationalNumber(*((storm::RationalNumber*)(*a)));
    *a = srn_ptr;
}

void storm_rational_number_destroy(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalFunction* srn_ptr = (storm::RationalFunction*)a;
    delete srn_ptr;
}

int storm_rational_number_equals(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber*)b;
    
    return (srn_a == srn_b) ? 1 : 0;
}

char* storm_rational_number_to_str(storm_rational_number_ptr val, char *buf, size_t buflen) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    std::stringstream ss;
    storm::RationalNumber const& srn_a = *(storm::RationalNumber*)val;
    ss << srn_a;
    std::string s = ss.str();
    if (s.size() < buflen + 1) {
        std::memcpy(buf, s.c_str(), s.size() + 1);
        return buf;
    } else {
        char* result = (char*)malloc(s.size() + 1);
        std::memcpy(result, s.c_str(), s.size() + 1);
        return result;
    }
}

storm_rational_number_ptr storm_rational_number_clone(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(*((storm::RationalNumber const*)a));
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_get_zero() {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(storm::utility::zero<storm::RationalNumber>());
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_get_one() {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(storm::utility::one<storm::RationalNumber>());
    return (storm_rational_number_ptr)result_srn;
}

int storm_rational_number_is_zero(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    return storm::utility::isZero(*(storm::RationalNumber const*)a) ? 1 : 0;
}

uint64_t storm_rational_number_hash(storm_rational_number_ptr const a, uint64_t const seed) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    
    // Taken from boost::hash_combine that we do not call here for the lack of boost headers.
    return seed ^ (std::hash<storm::RationalNumber>()(srn_a) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

double storm_rational_number_get_value_double(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    return storm::utility::convertNumber<double>(srn_a);
}

storm_rational_number_ptr storm_rational_number_plus(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(srn_a + srn_b);
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_minus(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(srn_a - srn_b);
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_times(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(srn_a * srn_b);
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_divide(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    storm::RationalNumber* result_srn = new storm::RationalNumber(srn_a / srn_b);
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_pow(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    uint64_t exponentAsInteger = carl::toInt<unsigned long>(srn_b);
    storm::RationalNumber* result_srn = new storm::RationalNumber(carl::pow(srn_a, exponentAsInteger));
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_mod(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    throw storm::exceptions::InvalidOperationException() << "Modulo not supported for rational numbers.";
}

storm_rational_number_ptr storm_rational_number_min(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    return storm_rational_number_less_or_equal(a, b) ? storm_rational_number_clone(a) : storm_rational_number_clone(b);
}

storm_rational_number_ptr storm_rational_number_max(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    return storm_rational_number_less(a, b) ? storm_rational_number_clone(b) : storm_rational_number_clone(a);
}




int storm_rational_number_less(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    return srn_a < srn_b ? 1 : 0;
}

int storm_rational_number_less_or_equal(storm_rational_number_ptr a, storm_rational_number_ptr b) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber const& srn_b = *(storm::RationalNumber const*)b;
    
    return srn_a <= srn_b ? 1 : 0;
}

storm_rational_number_ptr storm_rational_number_negate(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber* result_srn = new storm::RationalNumber(-srn_a);
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_floor(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber* result_srn = new storm::RationalNumber(carl::floor(srn_a));
    return (storm_rational_number_ptr)result_srn;
}

storm_rational_number_ptr storm_rational_number_ceil(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    storm::RationalNumber* result_srn = new storm::RationalNumber(carl::ceil(srn_a));
    return (storm_rational_number_ptr)result_srn;
}


void print_storm_rational_number(storm_rational_number_ptr a) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    std::cout << srn_a << std::flush;
}

void print_storm_rational_number_to_file(storm_rational_number_ptr a, FILE* out) {
#ifndef RATIONAL_NUMBER_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalNumberMutex);
#endif
    
    std::stringstream ss;
    storm::RationalNumber const& srn_a = *(storm::RationalNumber const*)a;
    ss << srn_a;
    std::string s = ss.str();
    fprintf(out, "%s", s.c_str());
}

/***************************************************
 Function-wrappers for storm::RationalFunction
 ****************************************************/

void storm_rational_function_init(storm_rational_function_ptr* a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm_rational_function_ptr srf_ptr = new storm::RationalFunction(*((storm::RationalFunction*)(*a)));
    *a = srf_ptr;
}

void storm_rational_function_destroy(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction* srf = (storm::RationalFunction*)a;
    delete srf;
}

int storm_rational_function_equals(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction*)b;
    
    return (srf_a == srf_b) ? 1 : 0;
}

char* storm_rational_function_to_str(storm_rational_function_ptr val, char *buf, size_t buflen) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    std::stringstream ss;
    storm::RationalFunction const& srf_a = *(storm::RationalFunction*)val;
    ss << srf_a;
    std::string s = ss.str();
    if (s.size() < buflen + 1) {
        std::memcpy(buf, s.c_str(), s.size() + 1);
        return buf;
    } else {
        char* result = (char*)malloc(s.size() + 1);
        std::memcpy(result, s.c_str(), s.size() + 1);
        return result;
    }
}

storm_rational_function_ptr storm_rational_function_clone(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(*((storm::RationalFunction const*)a));
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_get_zero() {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(storm::utility::zero<storm::RationalFunction>());
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_get_one() {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(storm::utility::one<storm::RationalFunction>());
    return (storm_rational_function_ptr)result_srf;
}

int storm_rational_function_is_zero(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    return storm::utility::isZero(*(storm::RationalFunction const*)a) ? 1 : 0;
}

uint64_t storm_rational_function_hash(storm_rational_function_ptr const a, uint64_t const seed) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    
    // Taken from boost::hash_combine that we do not call here for the lack of boost headers.
    return seed ^ (carl::hash_value(srf_a) + 0x9e3779b9 + (seed<<6) + (seed>>2));
}

double storm_rational_function_get_value_double(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    if (srf_a.isConstant()) {
        return storm::utility::convertNumber<double>(srf_a);
    } else {
        throw storm::exceptions::InvalidOperationException() << "Cannot evaluate rational function as it's not constant.";
    }
}

storm_rational_function_ptr storm_rational_function_plus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
    *result_srf += srf_b;
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_minus(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
    *result_srf -= srf_b;
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_times(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
    *result_srf *= srf_b;
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_divide(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    storm::RationalFunction* result_srf = new storm::RationalFunction(srf_a);
    *result_srf *= srf_b;
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_pow(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    uint64_t exponentAsInteger = carl::toInt<unsigned long>(srf_b.nominatorAsNumber());
    storm::RationalFunction* result_srf = new storm::RationalFunction(carl::pow(srf_a, exponentAsInteger));
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_mod(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    
    if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
        throw storm::exceptions::InvalidOperationException() << "Operands of mod must not be non-constant rational functions.";
    }
    throw storm::exceptions::InvalidOperationException() << "Modulo not supported for rational functions.";
}

storm_rational_function_ptr storm_rational_function_min(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    if (storm_rational_function_less_or_equal(a, b)) {
        return storm_rational_function_clone(a);
    } else {
        return storm_rational_function_clone(b);
    }
}

storm_rational_function_ptr storm_rational_function_max(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    if (storm_rational_function_less(a, b)) {
        return storm_rational_function_clone(b);
    } else {
        return storm_rational_function_clone(a);
    }
}

int storm_rational_function_less(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
        throw storm::exceptions::InvalidOperationException() << "Operands of less must not be non-constant rational functions.";
    }
    
    if (srf_a.nominatorAsNumber() < srf_b.nominatorAsNumber()) {
        return 1;
    } else {
        return 0;
    }
    return -1;
}

int storm_rational_function_less_or_equal(storm_rational_function_ptr a, storm_rational_function_ptr b) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction const& srf_b = *(storm::RationalFunction const*)b;
    if (!storm::utility::isInteger(srf_a) || !storm::utility::isInteger(srf_b)) {
        throw storm::exceptions::InvalidOperationException() << "Operands of less-or-equal must not be non-constant rational functions.";
    }
    
    if (srf_a.nominatorAsNumber() <= srf_b.nominatorAsNumber()) {
        return 1;
    } else {
        return 0;
    }
    return -1;
}

storm_rational_function_ptr storm_rational_function_negate(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    storm::RationalFunction* result_srf = new storm::RationalFunction(-srf_a);
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_floor(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    if (!storm::utility::isInteger(srf_a)) {
        throw storm::exceptions::InvalidOperationException() << "Operand of floor must not be non-constant rational function.";
    }
    storm::RationalFunction* result_srf = new storm::RationalFunction(carl::floor(srf_a.nominatorAsNumber()));
    return (storm_rational_function_ptr)result_srf;
}

storm_rational_function_ptr storm_rational_function_ceil(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    if (!storm::utility::isInteger(srf_a)) {
        throw storm::exceptions::InvalidOperationException() << "Operand of ceil must not be non-constant rational function.";
    }
    storm::RationalFunction* result_srf = new storm::RationalFunction(carl::ceil(srf_a.nominatorAsNumber()));
    return (storm_rational_function_ptr)result_srf;
}


void print_storm_rational_function(storm_rational_function_ptr a) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    std::cout << srf_a << std::flush;
}

void print_storm_rational_function_to_file(storm_rational_function_ptr a, FILE* out) {
#ifndef RATIONAL_FUNCTION_THREAD_SAFE
    std::lock_guard<std::mutex> lock(rationalFunctionMutex);
#endif
    
    std::stringstream ss;
    storm::RationalFunction const& srf_a = *(storm::RationalFunction const*)a;
    ss << srf_a;
    std::string s = ss.str();
    fprintf(out, "%s", s.c_str());
}
