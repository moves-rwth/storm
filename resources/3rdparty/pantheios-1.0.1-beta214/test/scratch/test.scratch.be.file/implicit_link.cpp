/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.file/implicit_link.cpp
 *
 * Purpose:     Implicit link file for the test.scratch.be.N.fail project.
 *
 * Created:     19th December 2008
 * Updated:     8th July 2009
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2009, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <pantheios/implicit_link/core.h>
#include <pantheios/implicit_link/fe.simple.h>
#include <pantheios/implicit_link/be.lrsplit.h>
#include <pantheios/implicit_link/bel.file.h>
#include <pantheios/implicit_link/ber.file.h>

#include <platformstl/platformstl.h>
#if defined(PLATFORMSTL_OS_IS_UNIX) && \
    defined(_WIN32)
# include <unixem/implicit_link.h>
#endif

/* ///////////////////////////// end of file //////////////////////////// */
