/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/contract/example.cpp.contract.PANTHEIOS_ASSERT/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the example.cpp.contract.PANTHEIOS_ASSERT project.
 *
 * Created:     8th May 2009
 * Updated:     6th December 2010
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
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.console.h>

/* UNIXem Header Files */
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif /* _WIN32 || _WIN64 */

/* ///////////////////////////// end of file //////////////////////////// */
