/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.fe/test.scratch.fe.cpp
 *
 * Purpose:     Implementation file for the test.scratch.fe project.
 *
 * Created:     21st June 2005
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              www:        http://www.synesis.com.au/software
 *
 *              This source code is placed into the public domain 2005
 *              by Synesis Software Pty Ltd. There are no restrictions
 *              whatsoever to your use of the software.
 *
 *              This source code is provided by Synesis Software Pty Ltd "as is"
 *              and any warranties, whether expressed or implied, including, but
 *              not limited to, the implied warranties of merchantability and
 *              fitness for a particular purpose are disclaimed. In no event
 *              shall the Synesis Software Pty Ltd be liable for any direct,
 *              indirect, incidental, special, exemplary, or consequential
 *              damages (including, but not limited to, procurement of
 *              substitute goods or services; loss of use, data, or profits; or
 *              business interruption) however caused and on any theory of
 *              liability, whether in contract, strict liability, or tort
 *              (including negligence or otherwise) arising in any way out of
 *              the use of this software, even if advised of the possibility of
 *              such damage.
 *
 *              Neither the name of Synesis Software Pty Ltd nor the names of
 *              any subdivisions, employees or agents of Synesis Software Pty
 *              Ltd, nor the names of any other contributors to this software
 *              may be used to endorse or promote products derived from this
 *              software without specific prior written permission.
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* This inclusion required for suppressing warnings during NoX (No eXception-support) configurations. */
#include <pantheios/util/test/compiler_warnings_suppression.first_include.h>

/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/integer.hpp>
#include <pantheios/inserters/pointer.hpp>
#include <pantheios/inserters/real.hpp>
#include <pantheios/backend.h>
#include <pantheios/frontend.h>
#include <pantheios/quality/contract.h>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>

/* Standard C++ Header Files */
#include <exception>
#include <iostream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

/* Standard C Header Files */
#include <stdio.h>

#include <pantheios/util/test/compiler_warnings_suppression.last_include.h>

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

#if (   defined(_WIN32) || \
        defined(_WIN64)) && \
    !defined(_INC_WINDOWS)
extern "C" void __stdcall Sleep(unsigned long);
#endif /* WIN32 */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

static int  severities[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

/* ////////////////////////////////////////////////////////////////////// */

#ifdef PANTHEIOS_USE_WIDE_STRINGS

int main()
{
        return 0;
}

#else /* ? PANTHEIOS_USE_WIDE_STRINGS */

/* ////////////////////////////////////////////////////////////////////// */

//static void call_all_severities();

static void call_all_severities()
{
    pantheios::log_DEBUG("I'm so gorgeous, there's a six month waiting list for birds to suddenly appear, every time I am near!");
    pantheios::log_INFORMATIONAL("Your nickname was never Ace. Maybe Ace Hole.");
    pantheios::log_NOTICE("Please rush me my portable walrus polishing kit. Four super brushes that will clean even the trickiest of seabound mammals.");
    pantheios::log_WARNING("Now kindly cluck off, before I extract your gibblets, and shove a large seasoned onion between the lips you never kiss with.");
    pantheios::log_ERROR("So this is really me? A no-style gimbo with teeth druids could use as a place of worship?");
    pantheios::log_CRITICAL("Well, the thing about a black hole - it's main distinguishing feature - is it's black. And the thing about space, your basic space colour is black. So how are you supposed to see them?");
    pantheios::log_ALERT("You actually expect something to go right for me? Arnold schmucko Rimmer? Tosspot by royal appointment?");
    pantheios::log_EMERGENCY("Emergency, there's an emergency going on, it's still going on, it's still an emergency");
}

static int main_(int /* argc */, char ** /*argv*/)
{
#if defined(_DEBUG)
    {
        using namespace pantheios;

        short                   s     = 123;
        int                     i     = 456;
        long                    l     = 789;
        float                   f     = static_cast<float>(0.123);
        double                  d     = 0.456;
        long double             ld    = 0.789;
        void                    *p    = &l;
#if 0
        char const              *lstr = "{a pointer to a C-style string}";
        std::string             str   = "{an instance of std::string}";
        stlsoft::simple_string  sstr  = "{an instance of stlsoft::simple_string}";
        #if defined(PLATFORMSTL_OS_IS_WINDOWS)
        SYSTEMTIME              tm    = GetCurrentTime_SYSTEMTIME();
        #else /* ? PLATFORMSTL_OS_IS_???? */
        struct tm const         *tm   = GetCurrentTime_tm();
        #endif /* PLATFORMSTL_OS_IS_???? */
#endif /* 0 */

#if 0
        log_INFORMATIONAL("This is a (hopefully) typical error string, containing: "
                        , "some integers (", integer(s), ", ", integer(i), ", ", integer(l), "); "
                        , "some real numbers (", real(f), ", ", real(d), ", ", real(ld), "); "
                        , "a pointer (", pointer(p, fmt::hex), "); "
                        , "some strings (", lstr, ", ", str, ", ", sstr, "); "
                        , "and a converted time value (", tm, ")"
                        );
#else /* ? 0 */
        log_INFORMATIONAL(
                          "Integers (", integer(s, 4 | fmt::zeroPad), ", ", integer(i), ", ", integer(l), "); "
                        , "floating points (", real(f), ", ", real(d), ", ", real(ld), "); "
                        , "and a pointer (", pointer(p, fmt::hex | fmt::zeroXPrefix), ")");

#endif /* 0 */
    }

# if defined(PLATFORMSTL_OS_IS_WINDOWS)
    {
        VARIANT var;

        ::VariantInit(&var);
        var.vt = VT_I4;
        var.lVal = 12345678;

        HWND    hwnd    =   ::GetDesktopWindow();

        pantheios::log_INFORMATIONAL("v=", var, "; h=", hwnd);
    }
# endif /* PLATFORMSTL_OS_IS_WIN?? */

#endif


    cout << endl << "Output with all severity levels on:" << endl;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_CRITICAL:" << endl;
    severities[PANTHEIOS_SEV_CRITICAL] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_DEBUG:" << endl;
    severities[PANTHEIOS_SEV_DEBUG] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_WARNING:" << endl;
    severities[PANTHEIOS_SEV_WARNING] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_INFORMATIONAL:" << endl;
    severities[PANTHEIOS_SEV_INFORMATIONAL] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_ERROR:" << endl;
    severities[PANTHEIOS_SEV_ERROR] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_EMERGENCY:" << endl;
    severities[PANTHEIOS_SEV_EMERGENCY] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_ALERT:" << endl;
    severities[PANTHEIOS_SEV_ALERT] = 0;
    call_all_severities();

    cout << endl << "Output after switching off PANTHEIOS_SEV_NOTICE:" << endl;
    severities[PANTHEIOS_SEV_NOTICE] = 0;
    call_all_severities();

    cout << endl << "Output complete" << endl;

    return 0;
}

int main(int argc, char *argv[])
{
#if 0
    for(;;) {}
#endif /* 0 */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        return main_(argc, argv);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::exception &x)
    {
        fprintf(stderr, "Unhandled error: %s\n", x.what());
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, "Unhandled unknown error");
    }

    return EXIT_FAILURE;
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

/* ////////////////////////////////////////////////////////////////////// */

#endif /* PANTHEIOS_USE_WIDE_STRINGS */

/* ////////////////////////////////////////////////////////////////////// */

void* const  BACKEND_TOKEN_VALUE    =   NULL;

PANTHEIOS_CALL(int) pantheios_be_init(
    PAN_CHAR_T const*   processIdentity
,   void*               reserved
,   void**              ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(reserved);
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != ptoken, "token pointer may not be null");

    *ptoken = BACKEND_TOKEN_VALUE;

    // Normally one would likely not seek to do anything at this point, since
    // it's *very* early in the process initialisation, and the raison d'etre
    // of Pantheios is to be the output library! ;-)
    //
    // However, for pedagogical purposes in this case, we seek to print out the
    // initialisation criteria.
    //
    // But this illustrates an important point: we can't use use IOStreams,
    // because some libraries are not initialised by this point and the process
    // actually crashes.
    //
    // So, _if_ you seek to do some output you should bear in mind the very
    // early nature of this call and, at most, use the C streams. (Even better
    // would be to use underlying OS calls, e.g. ::write() or
    // ::OutputDebugString().

    ::fprintf(stdout, "Pantheios initialised (%s, %p)\n", processIdentity, reserved);
//  cout << "Pantheios initialised (" << processIdentity << ", " << reserved << ")" << endl;

    return 0;
}

PANTHEIOS_CALL(void) pantheios_be_uninit(void *token)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(BACKEND_TOKEN_VALUE == token, "back-end token is not expected value");

    ::fprintf(stdout, "Pantheios uninitialised (%p)\n", token);
//  cout << "Pantheios uninitialised (" << token << ")" << endl;
}

PANTHEIOS_CALL(int) pantheios_be_logEntry(
    void*               feToken
,   void*               beToken
,   int                 severity
,   PAN_CHAR_T const*   entry
,   size_t              /* cchEntry */
)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(BACKEND_TOKEN_VALUE == beToken, "back-end token is not expected value");
    STLSOFT_SUPPRESS_UNUSED(feToken);
    STLSOFT_SUPPRESS_UNUSED(beToken);

    cout << "Pantheios log entry [" << severity << ": " << pantheios::pantheios_getSeverityString(static_cast<pantheios::pan_sev_t>(severity)) << "]: " << entry << endl;

    return 0;
}

/* ////////////////////////////////////////////////////////////////////// */

PANTHEIOS_CALL(int) pantheios_fe_init(
    void*   reserved
,   void**  ptoken
)
{
    STLSOFT_SUPPRESS_UNUSED(reserved);
    STLSOFT_SUPPRESS_UNUSED(ptoken);

    return 0;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void *token)
{
    STLSOFT_SUPPRESS_UNUSED(token);
}

PANTHEIOS_CALL(PAN_CHAR_T const*) pantheios_fe_getProcessIdentity(void *token)
{
    STLSOFT_SUPPRESS_UNUSED(token);

    return PANTHEIOS_LITERAL_STRING("panetheios::test.scratch.fe");
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(void *token, int severity, int backEndId)
{
    STLSOFT_SUPPRESS_UNUSED(token);
    STLSOFT_SUPPRESS_UNUSED(backEndId);

    return severities[severity];
}

/* ///////////////////////////// end of file //////////////////////////// */
