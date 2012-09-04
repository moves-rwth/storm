/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.be.N/test.scratch.be.N.cpp
 *
 * Purpose:     Implementation file for the test.scratch.be.N project.
 *
 * Created:     18th October 2006
 * Updated:     20th December 2010
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2006-2010, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/frontends/fe.N.h>
#include <pantheios/backend.h>
#include <pantheios/backends/be.N.h>
#include <pantheios/backends/bec.file.h>
#include <pantheios/backends/bec.fprintf.h>

#if defined(PLATFORMSTL_OS_IS_UNIX)
# include <pantheios/backends/bec.syslog.h>
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
# include <pantheios/backends/bec.WindowsDebugger.h>
# include <pantheios/backends/bec.WindowsSyslog.h>
#endif /* OS */

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.be.N");

PANTHEIOS_EXTERN_C pan_fe_N_t PAN_FE_N_SEVERITY_CEILINGS[] =
{
    { 1,  PANTHEIOS_SEV_NOTICE }            /* Filters out everything below 'notice' */
  , { 2,  PANTHEIOS_SEV_INFORMATIONAL  }    /* Filters out everything below 'informational' */
  , { 3,  PANTHEIOS_SEV_ERROR  }            /* Allows only 'error', 'critical', 'alert', 'emergency' */
  , { 4,  PANTHEIOS_SEV_WARNING }           /* Allows only 'warning', 'error', 'critical', 'alert', 'emergency' */
  , { 5,  PANTHEIOS_SEV_DEBUG  }            /* Allows all stock severities */
  , { 0,  PANTHEIOS_SEV_NOTICE }            /* Terminates the array; sets the default ceiling to 'notice' */
};

PANTHEIOS_EXTERN_C pan_be_N_t PAN_BE_N_BACKEND_LIST[] =
{
    PANTHEIOS_BE_N_STDFORM_ENTRY(1, pantheios_be_file, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(2, pantheios_be_fprintf, PANTHEIOS_BE_N_F_ID_MUST_MATCH_CUSTOM28)
#if defined(PLATFORMSTL_OS_IS_UNIX)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_syslog, PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE)
#elif defined(PLATFORMSTL_OS_IS_WINDOWS)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(3, pantheios_be_WindowsDebugger, 0)
  , PANTHEIOS_BE_N_STDFORM_ENTRY(4, pantheios_be_WindowsSyslog, PANTHEIOS_BE_N_F_IGNORE_INIT_FAILURE)
#endif /* OS */
  , PANTHEIOS_BE_N_STDFORM_ENTRY(5, pantheios_be_file, PANTHEIOS_BE_N_F_IGNORE_NONMATCHED_CUSTOM28_ID)

  , PANTHEIOS_BE_N_TERMINATOR_ENTRY
};

#if defined(PLATFORMSTL_OS_IS_WINDOWS)
PANTHEIOS_CALL(void) pantheios_be_WindowsDebugger_getAppInit(
  int                             backEndId
, pan_be_WindowsDebugger_init_t*  init) /* throw() */
{
  init->flags |= PANTHEIOS_BE_WINDOWSDEBUGGER_F_DETAILS_AT_START;
}
#endif /* PLATFORMSTL_OS_IS_WIN?? */

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char ** /*argv*/)
{
#ifdef PANTHEIOS_USES_VARIADIC_MACROS
  PANTHEIOS_TRACE_PRINTF(PANTHEIOS_SEV_ERROR, "Something went wrong here: %s %d", "abc", 10);
#endif /* PANTHEIOS_USES_VARIADIC_MACROS */

#ifdef PANTHEIOS_USES_VARIADIC_MACROS
  PANTHEIOS_TRACE_NOTICE("notice");
#endif /* PANTHEIOS_USES_VARIADIC_MACROS */

  pantheios::log_DEBUG("debug");
  pantheios::log_INFORMATIONAL("info");

  pantheios_be_file_setFilePath("file.log", PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BE_FILE_F_TRUNCATE, 1);

  pantheios::log_NOTICE("notice");
  pantheios::log_WARNING("warn");
  pantheios::log_ERROR("error");
  pantheios::log_CRITICAL("critical");

  pantheios_be_file_setFilePath("file-5.log", PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS, PANTHEIOS_BE_FILE_F_DISCARD_CACHED_CONTENTS, 5);

  pantheios::log_ALERT("alert");
  pantheios::log_EMERGENCY("emergency");

  pantheios::log(pantheios::warning(2), "this is targeted to go to the console (2)");

  pantheios::log(pantheios::error(4), "this is targeted to go to the syslog (4)");

  pantheios::log(pantheios::error(5), "this is targeted to go to the file (5)");

  return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
  try
  {
    return main_(argc, argv);
  }
  catch(std::exception &x)
  {
    fprintf(stderr, "Unhandled error: %s\n", x.what());
  }
  catch(...)
  {
    fprintf(stderr, "Unhandled unknown error\n");
  }

  return EXIT_FAILURE;
}

/* ///////////////////////////// end of file //////////////////////////// */
