#pragma once

// To detect whether the usage of TBB is possible, this include is neccessary
#include "storm-config.h"

/* On macOS, TBB includes the header
 * /Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk/usr/include/mach/boolean.h
 * which #defines macros TRUE and FALSE. Since these also occur as identifiers, we #undef them after including TBB.
 * We still issue a warning in case these macros have been defined before.
 */

#ifdef TRUE
#warning "Undefining previously defined macro 'TRUE'."
#endif

#ifdef FALSE
#warning "Undefining previously defined macro 'FALSE'."
#endif

#ifdef STORM_HAVE_INTELTBB
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/tbb_stddef.h"
#endif

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif
