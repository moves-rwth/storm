/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.extras.com.invoke_nothrow_method/test.scratch.extras.com.invoke_nothrow_method.cpp
 *
 * Purpose:     Implementation file for the test.scratch.extras.com.invoke_nothrow_method project.
 *
 * Created:     3rd November 2008
 * Updated:     6th August 2012
 *
 * Status:      Wizard-generated
 *
 * License:     (Licensed under the Synesis Software Open License)
 *
 *              Copyright (c) 2008-2012, Synesis Software Pty Ltd.
 *              All rights reserved.
 *
 *              www:        http://www.synesis.com.au/software
 *
 * ////////////////////////////////////////////////////////////////////// */


/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>
#include <pantheios/extras/com/exception_helpers.hpp>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.hpp>

/* Standard C++ Header Files */
#include <exception>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Macros and definitions
 */


/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.extras.com.invoke_nothrow_method");

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

class Class1
{
public:
    STDMETHOD(Method00)()
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method00_, "Method00");
    }
    STDMETHOD(Method01)(short* arg0)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method01_, arg0, "Method01");
    }
    STDMETHOD(Method02)(short* arg0, unsigned short* arg1)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method02_, arg0, arg1, "Method02");
    }
    STDMETHOD(Method03)(short* arg0, unsigned short* arg1, int* arg2)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method03_, arg0, arg1, arg2, "Method03");
    }
    STDMETHOD(Method04)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method04_, arg0, arg1, arg2, arg3, "Method04");
    }
    STDMETHOD(Method05)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method05_, arg0, arg1, arg2, arg3, arg4, "Method05");
    }
    STDMETHOD(Method06)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method06_, arg0, arg1, arg2, arg3, arg4, arg5, "Method06");
    }
    STDMETHOD(Method07)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method07_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, "Method07");
    }
    STDMETHOD(Method08)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method08_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, "Method08");
    }
    STDMETHOD(Method09)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method09_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, "Method09");
    }
    STDMETHOD(Method10)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8, unsigned char* arg9)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class1::Method10_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, "Method10");
    }

private:
    HRESULT Method00_()
    {
        return E_NOTIMPL;
    }
    HRESULT Method01_(short* arg0)
    {
        return E_NOTIMPL;
    }
    HRESULT Method02_(short* arg0, unsigned short* arg1)
    {
        return E_NOTIMPL;
    }
    HRESULT Method03_(short* arg0, unsigned short* arg1, int* arg2)
    {
        return E_NOTIMPL;
    }
    HRESULT Method04_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3)
    {
        return E_NOTIMPL;
    }
    HRESULT Method05_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4)
    {
        return E_NOTIMPL;
    }
    HRESULT Method06_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5)
    {
        return E_NOTIMPL;
    }
    HRESULT Method07_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6)
    {
        return E_NOTIMPL;
    }
    HRESULT Method08_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7)
    {
        return E_NOTIMPL;
    }
    HRESULT Method09_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8)
    {
        return E_NOTIMPL;
    }
    HRESULT Method10_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8, unsigned char* arg9)
    {
        return E_NOTIMPL;
    }
};

class Class2
{
public:
    STDMETHOD(Method00)()
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method00_, "Method00");
    }
    STDMETHOD(Method01)(short* arg0)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method01_, arg0, "Method01");
    }
    STDMETHOD(Method02)(short* arg0, unsigned short* arg1)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method02_, arg0, arg1, "Method02");
    }
    STDMETHOD(Method03)(short* arg0, unsigned short* arg1, int* arg2)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method03_, arg0, arg1, arg2, "Method03");
    }
    STDMETHOD(Method04)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method04_, arg0, arg1, arg2, arg3, "Method04");
    }
    STDMETHOD(Method05)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method05_, arg0, arg1, arg2, arg3, arg4, "Method05");
    }
    STDMETHOD(Method06)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method06_, arg0, arg1, arg2, arg3, arg4, arg5, "Method06");
    }
    STDMETHOD(Method07)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method07_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, "Method07");
    }
    STDMETHOD(Method08)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method08_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, "Method08");
    }
    STDMETHOD(Method09)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method09_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, "Method09");
    }
    STDMETHOD(Method10)(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8, unsigned char* arg9)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &Class2::Method10_, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, "Method10");
    }

private:
    HRESULT STDAPICALLTYPE Method00_()
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method01_(short* arg0)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method02_(short* arg0, unsigned short* arg1)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method03_(short* arg0, unsigned short* arg1, int* arg2)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method04_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method05_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method06_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method07_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method08_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method09_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8)
    {
        return E_NOTIMPL;
    }
    HRESULT STDAPICALLTYPE Method10_(short* arg0, unsigned short* arg1, int* arg2, unsigned int* arg3, long* arg4, unsigned long* arg5, float* arg6, double* arg7, char* arg8, unsigned char* arg9)
    {
        return E_NOTIMPL;
    }
};

/* /////////////////////////////////////////////////////////////////////////
 * Forward declarations
 */

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char** /*argv*/)
{
    Class1  instance1;

    instance1.Method00();
    instance1.Method01(NULL);
    instance1.Method02(NULL, NULL);
    instance1.Method03(NULL, NULL, NULL);
    instance1.Method04(NULL, NULL, NULL, NULL);
    instance1.Method05(NULL, NULL, NULL, NULL, NULL);
    instance1.Method06(NULL, NULL, NULL, NULL, NULL, NULL);
    instance1.Method07(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance1.Method08(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance1.Method09(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance1.Method10(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    Class2  instance2;

    instance2.Method00();
    instance2.Method01(NULL);
    instance2.Method02(NULL, NULL);
    instance2.Method03(NULL, NULL, NULL);
    instance2.Method04(NULL, NULL, NULL, NULL);
    instance2.Method05(NULL, NULL, NULL, NULL, NULL);
    instance2.Method06(NULL, NULL, NULL, NULL, NULL, NULL);
    instance2.Method07(NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance2.Method08(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance2.Method09(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    instance2.Method10(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int             res;

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemState    memState;
#endif /* _MSC_VER && _MSC_VER */

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemCheckpoint(&memState);
#endif /* _MSC_VER && _MSC_VER */

    try
    {
        res = main_(argc, argv);
    }
    catch(std::exception& x)
    {
        pantheios::log_ALERT("Unexpected general error: ", x, ". Application terminating");

        res = EXIT_FAILURE;
    }
    catch(...)
    {
        pantheios::logputs(pantheios::emergency, "Unhandled unknown error");

        res = EXIT_FAILURE;
    }

#if defined(_MSC_VER) && \
    defined(_DEBUG)
    _CrtMemDumpAllObjectsSince(&memState);
#endif /* _MSC_VER) && _DEBUG */

    return res;
}

/* ///////////////////////////// end of file //////////////////////////// */
