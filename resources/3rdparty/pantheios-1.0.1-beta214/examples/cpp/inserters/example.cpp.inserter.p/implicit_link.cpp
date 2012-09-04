/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/inserters/example.cpp.inserter.p/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the example.cpp.inserter.p project.
 *
 * Created:     31st October 2010
 * Updated:     6th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/implicit_link/be.fprintf.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/implicit_link/be.WindowsConsole.h>
#else /* ? OS */
# error Platform not discriminated
#endif /* OS */

/* UNIXem Header Files */
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif /* _WIN32 || _WIN64 */

/* ///////////////////////////// end of file //////////////////////////// */
