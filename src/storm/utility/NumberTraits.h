#pragma once

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
template<typename ValueType>
struct NumberTraits {
    static const bool SupportsExponential = false;
    static const bool IsExact = false;
};

template<>
struct NumberTraits<double> {
    static const bool SupportsExponential = true;
    static const bool IsExact = false;

    typedef uint64_t IntegerType;
};

#if defined(STORM_HAVE_CLN)
template<>
struct NumberTraits<storm::ClnRationalNumber> {
    static const bool SupportsExponential = false;
    static const bool IsExact = true;

    typedef cln::cl_I IntegerType;
};
#endif

#if defined(STORM_HAVE_GMP)
template<>
struct NumberTraits<storm::GmpRationalNumber> {
    static const bool SupportsExponential = false;
    static const bool IsExact = true;

    typedef mpz_class IntegerType;
};
#endif

template<>
struct NumberTraits<storm::RationalFunction> {
    static const bool SupportsExponential = false;
    static const bool IsExact = true;
};
}  // namespace storm
