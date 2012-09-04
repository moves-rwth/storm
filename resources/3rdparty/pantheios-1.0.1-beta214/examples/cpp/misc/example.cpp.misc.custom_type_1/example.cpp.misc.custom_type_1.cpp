/* /////////////////////////////////////////////////////////////////////////
 * File:        examples/cpp/misc/example.cpp.misc.custom_type_1/example.cpp.misc.custom_type_1.cpp
 *
 * Purpose:     C++ example program for Pantheios. Demonstrates:
 *
 *                - use of explicit conversion code for custom type
 *                - definition of specific conversion function for custom type
 *                - definition of string access shims for custom type
 *                - definition of inserter class for custom type
 *                - use of pantheios::logputs() in bail-out conditions
 *
 * Created:     31st August 2006
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


// NOTE: There is a bug in the two-phase lookup (at least I think it's
//  there) of GCC (except, afaict, the Mac OS-X version, which works fine),
//  which results in the custom string access shims for Person not being
//  detected unless they're defined or, as in this case, declared _before_
//  the inclusion of pantheios/pantheios.hpp.
//
//  In normal applications, this is going to be easy to handle, since your
//  custom types are likely to be defined (and their shims
//  defined/declared) in a header file. Since good practice dictates that
//  application-specific headers should be included before library headers
//  you'd probably see something like the following:
//
//    #include "myappcomponent.h"
//    #include "person.h"
//
//    #include <pantheios/pantheios/hpp>
//
//    #include <vector>
//
//  or similar.
//
//  In this case, however, we must synthesise this situation by doing the
//  forward declarations in this file. We only do this for GCC (and not on
//  Mac) since the other compilers all do the right thing.


#include <stlsoft/stlsoft.h>
#include <platformstl/platformstl.h>    // Need to detect comp+OS: see above
#if defined(STLSOFT_COMPILER_IS_GCC) && \
    (   !defined(UNIXSTL_OS_IS_MACOSX) || \
        __GNUC__ < 4)
// There's something wrong with GCC, so we have to fwd declare these things
# include <stlsoft/string/shim_string.hpp>
class Person;
class Person_inserter;
namespace stlsoft
{
# ifdef PANTHEIOS_USE_WIDE_STRINGS
  PAN_CHAR_T const* c_str_data_w(Person_inserter const& pi);
  size_t c_str_len_w(Person_inserter const& pi);
  stlsoft::basic_shim_string<PAN_CHAR_T> c_str_data_w(Person const& person);
  size_t c_str_len_w(Person const& pi);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
  char const* c_str_data_a(Person_inserter const& pi);
  size_t c_str_len_a(Person_inserter const& pi);
  stlsoft::basic_shim_string<char> c_str_data_a(Person const& person);
  size_t c_str_len_a(Person const& pi);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */

} // namespace stlsoft
#endif /* compiler */


/* Pantheios Header Files */
#include <pantheios/pantheios.hpp>                          // Pantheios C++ main header
#include <pantheios/inserters/integer.hpp>                  // for pantheios::integer
#include <pantheios/inserters/w2m.hpp>                      // for pantheios::w2m
#include <pantheios/quality/contract.h>
#include <pantheios/internal/safestr.h>

/* STLSoft Header Files */
#include <stlsoft/conversion/char_conversions.hpp>          // for stlsoft::w2m
#include <stlsoft/conversion/integer_to_string.hpp>         // for stlsoft::integer_to_string
#include <pantheios/util/memory/auto_buffer_selector.hpp>   // for stlsoft::auto_buffer
#include <stlsoft/string/shim_string.hpp>                   // for stlsoft::shim_string

/* Standard C/C++ Header Files */
#include <exception>                                        // for std::exception
#include <new>                                              // for std::bad_alloc
#include <string>                                           // for std::string
#include <stdlib.h>                                         // for exit codes

/* ////////////////////////////////////////////////////////////////////// */

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    defined(PANTHEIOS_USING_SAFE_STR_FUNCTIONS)
# pragma warning(disable : 4996)
#endif /* VC++ && Safe String */

/* ////////////////////////////////////////////////////////////////////// */

#define PSTR(x)         PANTHEIOS_LITERAL_STRING(x)

typedef std::basic_string<char>         string_m_t;
typedef std::basic_string<wchar_t>      string_w_t;
typedef std::basic_string<PAN_CHAR_T>   string_t;

/* ////////////////////////////////////////////////////////////////////// */

class Person
{
private:
  typedef string_w_t  string_type;

public:
  Person(wchar_t const* forename, wchar_t const* surname, unsigned age)
    : m_forename(forename)
    , m_surname(surname)
    , m_age(age)
  {}

public:
  string_type const& forename() const
  {
    return m_forename;
  }
  string_type const& surname() const
  {
    return m_surname;
  }
  unsigned age() const
  {
    return m_age;
  }

private:
  const string_type m_forename;
  const string_type m_surname;
  const unsigned    m_age;

private:
  Person &operator =(Person const&);
};

/* ////////////////////////////////////////////////////////////////////// */

/* Define the stock front-end process identity, so that it links when using
 * fe.N, fe.simple, etc. */
PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.misc.custom_type_1");

/* ////////////////////////////////////////////////////////////////////// */

static void log_with_explicit_conversion_code(Person const& person);
static void log_with_conversion_function(Person const& person);
static void log_with_string_access_shims(Person const& person);
static void log_with_inserter_class(Person const& person);

/* ////////////////////////////////////////////////////////////////////// */

int main()
{
  try
  {
    Person  person(L"Dr", L"Proctor", 38);

    // log with explicit conversion

    log_with_explicit_conversion_code(person);

    // log with conversion function

    log_with_conversion_function(person);

    // log with string access shims

    log_with_string_access_shims(person);

    // log with inserter class

    log_with_inserter_class(person);


    return EXIT_SUCCESS;
  }
  catch(std::bad_alloc&)
  {
    pantheios::log(pantheios::alert, PSTR("out of memory"));
  }
  catch(std::exception& x)
  {
    pantheios::log_CRITICAL(PSTR("Exception: "), x);
  }
  catch(...)
  {
    pantheios::logputs(pantheios::emergency, PSTR("Unexpected unknown error"));
  }

  return EXIT_FAILURE;
}

/* ////////////////////////////////////////////////////////////////////// */

static void log_with_explicit_conversion_code(Person const& person)
{
  if(pantheios::isSeverityLogged(pantheios::notice))
  {
    // Since Person's names are wide, we need to convert

    // NOTE: this code does not check for stlsoft::w2m() failure (returns size_t(-1))

    // 1. Create buffers for conversion. (auto_buffer used to minimise heap
    //     when not necessary.)

    pantheios::log_NOTICE(PSTR("Person: "), pantheios::w2m(person.forename().c_str()), PSTR(" "), pantheios::w2m(person.surname().c_str()), PSTR(", aged "), pantheios::integer(person.age()));
  }
}

/* ////////////////////////////////////////////////////////////////////// */

static string_t Person_to_string(Person const& person)
{
  // Since Person's names are wide, we need to convert

  // NOTE: this code does not check for wcstombs() failure (returns size_t(-1))

  // 1. Create buffers for conversion. (auto_buffer used to minimise heap
  //     when not necessary.)

  string_w_t  result;
  wchar_t     sz[21];

  result += person.forename();
  result += L' ';
  result += person.surname();
  result += L", aged ";
  result += stlsoft::integer_to_string(sz, STLSOFT_NUM_ELEMENTS(sz), person.age());

# ifdef PANTHEIOS_USE_WIDE_STRINGS

  return result;

# else /* ? PANTHEIOS_USE_WIDE_STRINGS */

  // This using declaration is necessary to workaround a flaw in Digital
  // Mars, which otherwise can't distinguish between pantheios::w2m and
  // stlsoft::w2m, despite their being explicitly qualified.
  using ::stlsoft::w2m;

  return string_t(stlsoft::w2m(result));

# endif /* PANTHEIOS_USE_WIDE_STRINGS */
}

static void log_with_conversion_function(Person const& person)
{
  pantheios::log_NOTICE(PSTR("Person: "), Person_to_string(person));
}

/* ////////////////////////////////////////////////////////////////////// */

namespace stlsoft
{

# ifdef PANTHEIOS_USE_WIDE_STRINGS
  inline stlsoft::basic_shim_string<PAN_CHAR_T> c_str_data_w(Person const& person)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
  inline stlsoft::basic_shim_string<PAN_CHAR_T> c_str_data_a(Person const& person)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
  {

    string_w_t  result;
    wchar_t     sz[21];

    result += person.forename();
    result += L' ';
    result += person.surname();
    result += L", aged ";
    result += stlsoft::integer_to_string(sz, STLSOFT_NUM_ELEMENTS(sz), person.age());

# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return stlsoft::basic_shim_string<PAN_CHAR_T>(result.c_str());
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return stlsoft::basic_shim_string<PAN_CHAR_T>(stlsoft::w2m(result.c_str()));
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
  }

# ifdef PANTHEIOS_USE_WIDE_STRINGS
  inline size_t c_str_len_w(Person const& person)
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
  inline size_t c_str_len_a(Person const& person)
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
  {
# ifdef PANTHEIOS_USE_WIDE_STRINGS
    return c_str_data_w(person);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
    return c_str_data_a(person);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */
  }

} // namespace stlsoft

static void log_with_string_access_shims(Person const& person)
{
  pantheios::log_NOTICE(PSTR("Person: "), person);
}

/* ////////////////////////////////////////////////////////////////////// */

class Person_inserter
{
public:
  typedef Person_inserter class_type;

public:
  Person_inserter(Person const& person)
    : m_person(person)
    , m_value()
  {}

#if defined(STLSOFT_COMPILER_IS_GCC)
  Person_inserter(class_type const& rhs)
    : m_person(rhs.m_person)
    , m_value(rhs.m_value)
  {}
#endif /* compiler */
private:
  class_type& operator =(class_type const&);

public:
  PAN_CHAR_T const* data() const
  {
    if(m_value.empty())
    {
      construct_();
    }

    return m_value.data();
  }
  size_t size() const
  {
    if(m_value.empty())
    {
      construct_();
    }

    return m_value.size();
  }

public:
  void construct_() const
  {
    const_cast<class_type*>(this)->construct_();
  }
  void construct_()
  {
    wchar_t         sz[21];
    size_t          cchNumber;
    wchar_t const*  num = stlsoft::integer_to_string(sz, STLSOFT_NUM_ELEMENTS(sz), m_person.age(), cchNumber);
    string_w_t      value;

    value.reserve(m_person.forename().size() + 1 + m_person.surname().size() + 7 + cchNumber);

    value += m_person.forename().c_str();
    value += L' ';
    value += m_person.surname().c_str();
    value += L", aged ";
    value += num;

# ifdef PANTHEIOS_USE_WIDE_STRINGS

    m_value.swap(value);

# else /* ? PANTHEIOS_USE_WIDE_STRINGS */

    string_t t(stlsoft::w2m(value).c_str());

    m_value.swap(t);

# endif /* PANTHEIOS_USE_WIDE_STRINGS */
  }

private:
  Person const& m_person;
  string_t      m_value;
};

namespace stlsoft
{

#ifdef PANTHEIOS_USE_WIDE_STRINGS
  inline PAN_CHAR_T const *c_str_data_w(Person_inserter const& pi)
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
  inline char const *c_str_data_a(Person_inserter const& pi)
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
  {
    return pi.data();
  }

#ifdef PANTHEIOS_USE_WIDE_STRINGS
  inline size_t c_str_len_w(Person_inserter const& pi)
#else /* ? PANTHEIOS_USE_WIDE_STRINGS */
  inline size_t c_str_len_a(Person_inserter const& pi)
#endif /* PANTHEIOS_USE_WIDE_STRINGS */
  {
    return pi.size();
  }

} // namespace stlsoft

static void log_with_inserter_class(Person const& person)
{
  pantheios::log_NOTICE(PSTR("Person: "), Person_inserter(person));
}

/* ///////////////////////////// end of file //////////////////////////// */
