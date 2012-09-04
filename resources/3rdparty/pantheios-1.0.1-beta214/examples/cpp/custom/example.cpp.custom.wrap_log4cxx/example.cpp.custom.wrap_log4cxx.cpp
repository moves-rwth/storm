/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/custom/example.cpp.custom.wrap_log4cxx/example.cpp.custom.wrap_log4cxx.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of a custom back-end to wrap the log4cxx library
 *                - interaction between log4cxx constructs and Pantheios log
 *                  statements
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     16th August 2006
 * Updated:     7th December 2010
 *
 * www:         http://www.pantheios.org/
 *
 * License:     This source code is placed into the public domain 2006
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software.
 *
 *              This software is provided "as is", and any warranties,
 *              express or implied, of any kind and for any purpose, are
 *              disclaimed.
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS // Faster compilation

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>          // Pantheios C++ main header
#include <pantheios/backend.h>
#include <pantheios/init_codes.h>

/* log4cxx Header Files */
#include <log4cxx/logger.h>
#include <log4cxx/hierarchy.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/ndc.h>

/* Standard C/C++ Header Files */
#include <exception>                        // for std::exception
#include <new>                              // for std::bad_alloc
#include <string>                           // for std::string
#include <stdlib.h>                         // for exit codes

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# if defined(STLSOFT_COMPILER_IS_MSVC)
#  pragma warning(disable : 4702)
# endif /* compiler */
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    using namespace log4cxx;
    using namespace log4cxx::helpers;

    // A normal log statement

    pantheios::log_DEBUG("debug stmt");
    pantheios::log_INFORMATIONAL("informational stmt");
    pantheios::log_NOTICE("notice stmt");
    pantheios::log_WARNING("warning stmt");
    pantheios::log_ERROR("error stmt");
    pantheios::log_CRITICAL("critical stmt");
    pantheios::log_ALERT("alert stmt");
    pantheios::log_EMERGENCY("emergency stmt");


    // Now we push a context ...
    NDC::push(_T("trivial context"));

    // ... log ...
    pantheios::log_INFORMATIONAL("stmt#2");

    // ... pop ...
    NDC::pop();

    // ... and log again
    pantheios::log_INFORMATIONAL("stmt#3");



    return EXIT_SUCCESS;
  }
  catch(std::bad_alloc&)
  {
    pantheios::log(pantheios::alert, "out of memory");
  }
  catch(std::exception& x)
  {
    pantheios::log_CRITICAL("Exception: ", x);
  }
  catch(...)
  {
    pantheios::logputs(pantheios::emergency, "Unexpected unknown error");
  }

  return EXIT_FAILURE;
}

/* /////////////////////////////////////////////////////////////////////////
 * Front-end
 */

PANTHEIOS_CALL(int) pantheios_fe_init(
  int     /* reserved */
, void**  /* ptoken */
)
{
  return PANTHEIOS_INIT_RC_SUCCESS; // Successful initialisation
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void* /* token */)
{}

PANTHEIOS_CALL(char const*) pantheios_fe_getProcessIdentity(void* /* token */)
{
  return "example.cpp.custom.wrap_log4cxx";
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(
  void* /* token */
, int severity
, int /* backEndId */
)
{
  using namespace log4cxx;
  using namespace log4cxx::helpers;

  LevelPtr  level;

  switch(severity & 0x0f)
  {
    case  PANTHEIOS_SEV_EMERGENCY:
    case  PANTHEIOS_SEV_ALERT:
      level = Level::FATAL;
      break;
    case  PANTHEIOS_SEV_CRITICAL:
    case  PANTHEIOS_SEV_ERROR:
      level = Level::ERROR;
      break;
    case  PANTHEIOS_SEV_WARNING:
      level = Level::WARN;
      break;
    case  PANTHEIOS_SEV_NOTICE:
    case  PANTHEIOS_SEV_INFORMATIONAL:
      level = Level::INFO;
      break;
    case  PANTHEIOS_SEV_DEBUG:
      level = Level::DEBUG;
      break;
  }

  LoggerPtr rootLogger = Logger::getRootLogger();

  return rootLogger->isEnabledFor(level);
}

/* /////////////////////////////////////////////////////////////////////////
 * Back-end
 */

PANTHEIOS_CALL(int) pantheios_be_init(
  char const* /* processIdentity */
, void*       /* reserved */
, void**      /* ptoken */
)
{
  using namespace log4cxx;
  using namespace log4cxx::helpers;

  try
  {  
    BasicConfigurator::configure();
  }
  catch(std::bad_alloc&)
  {
    pantheios::util::onBailOut(PANTHEIOS_LOG_ALERT, "failed to initialise back-end", processIdentity, "out of memory");

    return PANTHEIOS_INIT_RC_OUT_OF_MEMORY
  }
  catch(std::exception& x)
  {
    pantheios::util::onBailOut(PANTHEIOS_LOG_ALERT, "failed to initialise back-end", processIdentity, x.what());

    return PANTHEIOS_INIT_RC_UNSPECIFIED_EXCEPTION;
  }

  return PANTHEIOS_INIT_RC_SUCCESS; // Successful initialisation
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void* /* token */)
{}

PANTHEIOS_CALL(int) pantheios_be_logEntry(
  void*       /* feToken */
, void*       /* beToken */
, int         severity
, char const* entry
, size_t      cchEntry
)
{
  using namespace log4cxx;
  using namespace log4cxx::helpers;

  severity &= 0x07;

  LoggerPtr rootLogger = Logger::getRootLogger();

  switch(severity)
  {
    case  PANTHEIOS_SEV_EMERGENCY:
    case  PANTHEIOS_SEV_ALERT:
      rootLogger->fatal(entry);
      break;
    case  PANTHEIOS_SEV_CRITICAL:
    case  PANTHEIOS_SEV_ERROR:
      rootLogger->error(entry);
      break;
    case  PANTHEIOS_SEV_WARNING:
      rootLogger->warn(entry);
      break;
    case  PANTHEIOS_SEV_NOTICE:
    case  PANTHEIOS_SEV_INFORMATIONAL:
      rootLogger->info(entry);
      break;
    case  PANTHEIOS_SEV_DEBUG:
      rootLogger->debug(entry);
      break;
  }

  return cchEntry;
}

/* ///////////////////////////// end of file //////////////////////////// */
