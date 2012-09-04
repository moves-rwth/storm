/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.N/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the test.scratch.be.N.fail project.
 *
 * Created:     19th December 2008
 * Updated:     20th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.N.h>
#include <pantheios/implicit_link/be.N.h>
#include <pantheios/implicit_link/bec.file.h>
#include <pantheios/implicit_link/bec.fprintf.h>
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/implicit_link/bec.syslog.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/implicit_link/bec.WindowsDebugger.WithCallback.h>
# include <pantheios/implicit_link/bec.WindowsSyslog.h>
#endif /* OS */

/* UNIXem Header Files */
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif

/* ///////////////////////////// end of file //////////////////////////// */
