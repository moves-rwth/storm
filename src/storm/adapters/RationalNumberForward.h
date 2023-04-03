#pragma once

#include "storm-config.h"

#if defined(STORM_HAVE_GMP)
// The small gmp.h header is always included.
#include <gmp.h>
template<typename U, typename V>
class __gmp_expr;
#endif

#if defined(STORM_HAVE_CLN)
namespace cln {
class cl_RA;
class cl_I;
}  // namespace cln
#endif

namespace storm {
#if defined(STORM_HAVE_CLN)
typedef cln::cl_RA ClnRationalNumber;
typedef cln::cl_I ClnIntegerNumber;
#endif
#if defined(STORM_HAVE_GMP)
typedef __gmp_expr<mpq_t, mpq_t> GmpRationalNumber;
typedef __gmp_expr<mpz_t, mpz_t> GmpIntegerNumber;
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
