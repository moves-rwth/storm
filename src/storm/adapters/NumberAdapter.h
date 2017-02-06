#pragma once

#include "storm-config.h"

#ifdef STORM_HAVE_CARL

#include <carl/numbers/numbers.h>
namespace storm {
#if defined STORM_HAVE_CLN && defined STORM_USE_CLN_NUMBERS
    typedef cln::cl_RA RationalNumber;
#else
    typedef mpq_class RationalNumber;
#endif
}
#endif
