/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.N.with.custom.fe/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the test.scratch.be.N.with.custom.fe project.
 *
 * Created:     23rd December 2010
 * Updated:     10th January 2011
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2010-2011, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <platformstl/platformstl.h>
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.N.h>
#include <pantheios/implicit_link/bec.file.h>
#include <pantheios/implicit_link/bec.console.h>

/* UNIXem Header Files */
#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif /* operating system */

/* ///////////////////////////// end of file //////////////////////////// */
