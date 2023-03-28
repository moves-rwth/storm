#pragma once

#include "storm-config.h"

#if defined(STORM_HAVE_CLN)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#include <cln/cln.h>
#pragma clang diagnostic pop
#endif

#if defined(STORM_HAVE_GMP)
#include <gmpxx.h>
#endif

#include <carl/numbers/numbers.h>

#if defined(STORM_HAVE_CLN)
namespace cln {
inline size_t hash_value(cl_RA const& n) {
    std::hash<cln::cl_RA> h;
    return h(n);
}
}  // namespace cln
#endif

#if defined(STORM_HAVE_GMP)
inline size_t hash_value(mpq_class const& q) {
    std::hash<mpq_class> h;
    return h(q);
}
#endif

namespace storm {
#if defined(STORM_HAVE_CLN)
typedef cln::cl_RA ClnRationalNumber;
#endif
#if defined(STORM_HAVE_GMP)
typedef mpq_class GmpRationalNumber;
#endif

#if defined(STORM_HAVE_CLN) && defined(STORM_USE_CLN_EA)
typedef ClnRationalNumber RationalNumber;
#elif defined(STORM_HAVE_GMP) && !defined(STORM_USE_CLN_EA)
typedef GmpRationalNumber RationalNumber;
#elif defined(STORM_USE_CLN_EA)
#error CLN is to be used, but is not available.
#else
#error GMP is to be used, but is not available.
#endif
}  // namespace storm
