/* /////////////////////////////////////////////////////////////////////////
 * File:        test/performance/test.performance.inserters.w2m/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the test.performance.inserters.w2m project.
 *
 * Created:     2nd September 2008
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <platformstl/platformstl.h>
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.null.h>

/* UNIXEm Header Files */
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif /* operating system */

/* ////////////////////////////////////////////////////////////////////// */
