/* /////////////////////////////////////////////////////////////////////////
 * File:        test/scratch/test.scratch.extras.com_exception_helpers/test.scratch.extras.com_exception_helpers.cpp
 *
 * Purpose:     Implementation file for the test.scratch.extras.com_exception_helpers project.
 *
 * Created:     19th December 2008
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


#define PANTHEIOS_NO_INCLUDE_OS_AND_3PTYLIB_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/extras/com/exception_helpers.hpp>
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters/args.hpp>

/* STLSoft Header Files */
#include <stlsoft/stlsoft.h>
#include <stlsoft/smartptr/ref_ptr.hpp>

#include <comstl/comstl.h>
#include <comstl/error/errorinfo_desc.hpp>
#include <comstl/util/creation_functions.hpp>
#include <comstl/util/initialisers.hpp>

#include <winstl/winstl.h>
#include <winstl/error/error_desc.hpp>

/* Standard C++ Header Files */
#include <exception>
#include <stdexcept>

/* Standard C Header Files */
#include <stdio.h>
#include <stdlib.h>

#if defined(_MSC_VER) && \
    defined(_DEBUG)
# include <crtdbg.h>
#endif /* _MSC_VER) && _DEBUG */

/* /////////////////////////////////////////////////////////////////////////
 * Globals
 */

PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("test.scratch.extras.com_exception_helpers");

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

class TestClass
{
public:
    TestClass()
        : ReturnCode(S_OK)
    {}

public:
    HRESULT ReturnCode;

public:
    HRESULT f1()
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &TestClass::f1_, "f1");
    }
    HRESULT f2(int i)
    {
        return pantheios::extras::com::invoke_nothrow_method(this, &TestClass::f2_, i, "f2");
    }

    static HRESULT f3(int i, char const* s, HRESULT hr)
    {
#if 0
        PANTHEIOS_EXTRAS_COM_EXCEPTION_PROTECTION_PREFIX

        HRESULT hri = f3_(i, s, hr);

        PANTHEIOS_EXTRAS_COM_EXCEPTION_PROTECTION_TEST_OUTOFMEMORY(hri)

        return hri;

        PANTHEIOS_EXTRAS_COM_EXCEPTION_PROTECTION_SUFFIX
#else /* ? 0 */
        return pantheios::extras::com::invoke_nothrow(f3_, i, s, hr, "f3");
#endif /* 0 */
    }

private:
    HRESULT f1_()
    {
        if(E_UNEXPECTED == ReturnCode)
        {
            throw std::runtime_error("Houston, we have a problem");
        }

        return ReturnCode;
    }
    HRESULT f2_(int i)
    {
        if(E_UNEXPECTED == ReturnCode)
        {
            throw std::runtime_error("Meh!");
        }

        return ReturnCode;
    }

    static HRESULT STDAPICALLTYPE f3_(int i, char const* s, HRESULT hr)
    {
        if(E_UNEXPECTED == hr)
        {
            throw std::runtime_error("Boiing!");
        }

        return hr;
    }
};

/* ////////////////////////////////////////////////////////////////////// */

static int main_(int /* argc */, char** /*argv*/)
{
    TestClass   instance;

    instance.ReturnCode =   S_OK;

    instance.f1();
    instance.f2(10);
    instance.f3(-10, "stuff", instance.ReturnCode);

    instance.ReturnCode =   E_INVALIDARG;

    instance.f1();
    instance.f2(10);
    instance.f3(-10, "stuff", instance.ReturnCode);

    instance.ReturnCode =   E_OUTOFMEMORY;

    instance.f1();
    instance.f2(10);
    instance.f3(-10, "stuff", instance.ReturnCode);

    instance.ReturnCode =   E_UNEXPECTED;

    instance.f1();
    instance.f2(10);
    instance.f3(-10, "stuff", instance.ReturnCode);

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

#if 0
    { for(size_t i = 0; i < 0xffffffff; ++i){} }
#endif /* 0 */

    try
    {
#if defined(_DEBUG) || \
    defined(__SYNSOFT_DBS_DEBUG)
        puts("test.scratch.extras.com_exception_helpers: " __STLSOFT_COMPILER_LABEL_STRING);
#endif /* debug */

        comstl::com_initialiser coinit;

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
