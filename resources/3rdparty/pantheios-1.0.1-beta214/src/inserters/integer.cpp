/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/integer.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     21st June 2005
 * Updated:     7th August 2012
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2012, Matthew Wilson and Synesis Software
 * Copyright (c) 1999-2005, Synesis Software and Matthew Wilson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the
 *   names of any contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


#define PANTHEIOS_NO_INCLUDE_STLSOFT_STRING_ACCESS

/* Pantheios Header Files */
#include <pantheios/pantheios.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
 /* For some to-be-determined reason, CodeWarrior can't see a whole lot of
  * things in STLSoft header files from <string.h>, even though those
  * files #include it.
  */
# include <string.h>
#endif /* compiler */

#include <pantheios/inserters/integer.hpp>
#include <pantheios/quality/contract.h>
#if 0
# include <pantheios/quality/cover.h>
#else
# ifndef PANTHEIOS_COVER_MARK_ENTRY
#  define PANTHEIOS_COVER_MARK_ENTRY()  static_cast<void>(0)
# endif
#endif
#include <pantheios/util/string/snprintf.h>
#include <pantheios/internal/safestr.h>

#include <stlsoft/conversion/integer_to_string.hpp>
#include <stlsoft/util/integral_printf_traits.hpp>

//#include <stlsoft/meta/yesno.hpp> // TODO: Use this to remove "runtime" constant tests in integer::init_()

/* Standard C++ Header Files */
#include <algorithm>

/* Standard C Header Files */
#if defined(STLSOFT_COMPILER_IS_BORLAND)
# include <memory.h>
#endif /* compiler */
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Warning suppression
 */

#if defined(STLSOFT_COMPILER_IS_BORLAND)
# pragma warn -8008
# pragma warn -8066
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Compatibility
 */

#ifdef PANTHEIOS_USING_SAFE_STR_FUNCTIONS
# include <stlsoft/algorithms/std/alt.hpp>
namespace std
{
    using stlsoft::std_fill_n;

# define fill_n std_fill_n

} // namespace std
#endif /* PANTHEIOS_USING_SAFE_STR_FUNCTIONS */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

namespace
{

    enum IntegerSize
    {
            typeIsUnknown   =   0
        ,   typeIsS8        =   -5
        ,   typeIsU8        =   -6
        ,   typeIsS16       =   -7
        ,   typeIsU16       =   -8
        ,   typeIsS32       =   -1
        ,   typeIsU32       =   -2
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        ,   typeIsS64       =   -3
        ,   typeIsU64       =   -4
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    };

    template <ss_typename_param_k I>
    inline size_t sprint_(I const& i, int minWidth, int format, pan_char_t buffer[], size_t cchBuffer)
    {
        if( 0 == minWidth &&
            0 == format)
        {
            // No special formatting here, so we can use STLSoft's hyper-fast
            // integer_to_string() function suite.

            size_t      n /* = 0 */;
            pan_char_t* s = const_cast<pan_char_t*>(stlsoft::integer_to_string(&buffer[0], cchBuffer, i, &n));

            if(s != &buffer[0])
            {
                ::memmove(&buffer[0], s, sizeof(pan_char_t) * (n + 1));
            }

            return n;
        }
        else
        {
            // These static initialisations are subject to races
            // in a multi-threaded process, but it is entirely benign

# ifdef PANTHEIOS_USE_WIDE_STRINGS
            static pan_char_t const* const  s_decFmt    =   ::stlsoft::integral_printf_traits<I>::decimal_format_w();
            static pan_char_t const* const  s_hexFmt    =   ::stlsoft::integral_printf_traits<I>::hexadecimal_format_w(false);
# else /* ? PANTHEIOS_USE_WIDE_STRINGS */
            static pan_char_t const* const  s_decFmt    =   ::stlsoft::integral_printf_traits<I>::decimal_format_a();
            static pan_char_t const* const  s_hexFmt    =   ::stlsoft::integral_printf_traits<I>::hexadecimal_format_a(false);
# endif /* PANTHEIOS_USE_WIDE_STRINGS */

            pan_char_t          szFmt[101];
            int                 width;
            pan_char_t const*   zeroX;
            pan_char_t const*   leadingMinus;
            pan_char_t const*   zeroPad;

            if(minWidth < 0)
            {
                width           =   -minWidth;
                leadingMinus    =   PANTHEIOS_LITERAL_STRING("-");
            }
            else
            {
                width           =   minWidth;
                leadingMinus    =   PANTHEIOS_LITERAL_STRING("");
            }

            zeroX   =   ((fmt::hex | fmt::zeroXPrefix) == (format & (fmt::hex | fmt::zeroXPrefix))) ? PANTHEIOS_LITERAL_STRING("0x") : PANTHEIOS_LITERAL_STRING("");
            zeroPad =   (format & fmt::zeroPad) ? PANTHEIOS_LITERAL_STRING("0") : PANTHEIOS_LITERAL_STRING("");

            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(width >= 0);

            // Detect the special case to avoid printing strings such as "0x   1"
            if( 0 != width &&
                (fmt::hex | fmt::zeroXPrefix) == (format & (fmt::hex | fmt::zeroXPrefix | fmt::zeroPad)))
            {
                // Special case

                pan_char_t szTemp[23]; // 23 is always big enough for any number (up to and incl. 64-bit) and the 0x prefix

                PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION((fmt::hex | fmt::zeroXPrefix) == (format & (fmt::hex | fmt::zeroXPrefix)));
                PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(0 == (format & fmt::zeroPad));

                pantheios_util_snprintf(&szFmt[0]
                                    ,   STLSOFT_NUM_ELEMENTS(szFmt)
                                    ,   PANTHEIOS_LITERAL_STRING("0x%s")
                                    ,   s_hexFmt);

                int n = pantheios_util_snprintf(&szTemp[0], STLSOFT_NUM_ELEMENTS(szTemp), szFmt, i);

                if(n < 0)
                {
                    return 0;
                }

                PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(static_cast<size_t>(n + 2) <= STLSOFT_NUM_ELEMENTS(szTemp), "snprintf() overwrote the local buffer capacity");
                PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL(static_cast<size_t>(n) + 2 <= cchBuffer, "snprintf() overwrote the destination buffer capacity");

                if(width > n)
                {
                    PANTHEIOS_COVER_MARK_ENTRY();

                    if(minWidth < 0)
                    {
                        PANTHEIOS_COVER_MARK_ENTRY();

                        ::memcpy(&buffer[0], szTemp, static_cast<size_t>(n) * sizeof(pan_char_t));
                        std::fill_n(&buffer[0] + n, static_cast<size_t>(width - n), ' ');
                        buffer[width] = '\0';

                        return static_cast<size_t>(width);
                    }
                    else
                    {
                        PANTHEIOS_COVER_MARK_ENTRY();

                        std::fill_n(&buffer[0], static_cast<size_t>(width - n), ' ');
                        ::memcpy(&buffer[0] + (size_t(width) - n), szTemp, static_cast<size_t>(n + 1) * sizeof(pan_char_t));

                        return static_cast<size_t>(width);
                    }
                }
                else
                {
                    PANTHEIOS_COVER_MARK_ENTRY();

                    ::memcpy(&buffer[0], szTemp, static_cast<size_t>(n + 1) * sizeof(pan_char_t));

                    return static_cast<size_t>(n);
                }
            }
            else
            {
                // General case

                pantheios_util_snprintf(&szFmt[0]
                                    ,   STLSOFT_NUM_ELEMENTS(szFmt)
                                    ,   PANTHEIOS_LITERAL_STRING("%s%%%s%s%d%s")
                                    ,   zeroX
                                    ,   leadingMinus
                                    ,   zeroPad
                                    ,   width
                                    ,   ((format & fmt::hex) ? s_hexFmt : s_decFmt) + 1);

                return static_cast<size_t>(pantheios_util_snprintf(buffer, cchBuffer, szFmt, i));
            }
        }

#if defined(STLSOFT_COMPILER_IS_GCC) && \
    __GNUC__ == 4
        // For some reason, GCC thinks that there're code paths without
        // a return in this function, so we add one here.
        return 0;
#endif /* compiler */
    }

    int evaluate_format_(int widthAndFormat)
    {
        if(widthAndFormat < 0)
        {
            return 0;
        }
        else
        {
            return (widthAndFormat & ~0xff) /* | (0 != (fmt::zeroXPrefix & widthAndFormat) ? fmt::zeroPad : 0) */;
        }
    }

} // anonymous namespace

/* /////////////////////////////////////////////////////////////////////////
 * integer
 */

inline /* static */ int integer::validate_width_(int minWidth)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER((minWidth >= -128 && minWidth <= 127), "pantheios::integer width must be in range [-128, 127]");

    return minWidth;
}

size_t integer::init_(::stlsoft::sint8_t i)
{
    m_sz[0]     = '\0';
    m_value.s32 =   i;

    return static_cast<size_t>(typeIsS8);
}
size_t integer::init_(::stlsoft::uint8_t i)
{
    m_sz[0]     = '\0';
    m_value.u32 =   i;

    return static_cast<size_t>(typeIsU8);
}

size_t integer::init_(::stlsoft::sint16_t i)
{
    m_sz[0]     = '\0';
    m_value.s32 =   i;

    return static_cast<size_t>(typeIsS16);
}
size_t integer::init_(::stlsoft::uint16_t i)
{
    m_sz[0]     = '\0';
    m_value.u32 =   i;

    return static_cast<size_t>(typeIsU16);
}

size_t integer::init_(::stlsoft::sint32_t i)
{
    m_sz[0]     = '\0';
    m_value.s32 =   i;

    return static_cast<size_t>(typeIsS32);
}
size_t integer::init_(::stlsoft::uint32_t i)
{
    m_sz[0]     = '\0';
    m_value.u32 =   i;

    return static_cast<size_t>(typeIsU32);
}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
size_t integer::init_(::stlsoft::sint64_t i)
{
    m_sz[0]     = '\0';
    m_value.s64 =   i;

    return static_cast<size_t>(typeIsS64);
}
size_t integer::init_(::stlsoft::uint64_t i)
{
    m_sz[0]     = '\0';
    m_value.u64 =   i;

    return static_cast<size_t>(typeIsU64);
}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
size_t integer::init_(int i)
{
    m_sz[0]     = '\0';

    if(sizeof(int) <= sizeof(::stlsoft::sint32_t))
    {
        return (m_value.s32 = i, static_cast<size_t>(typeIsS32));
    }
    else
    {
        return (m_value.s64 = i, static_cast<size_t>(typeIsS64));
    }
}
size_t integer::init_(unsigned int i)
{
    m_sz[0]     = '\0';

    if(sizeof(unsigned int) <= sizeof(::stlsoft::uint32_t))
    {
        return (m_value.u32 = i, static_cast<size_t>(typeIsU32));
    }
    else
    {
        return (m_value.u64 = i, static_cast<size_t>(typeIsU64));
    }
}
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
size_t integer::init_(long i)
{
    m_sz[0]     = '\0';

    if(sizeof(long) <= sizeof(::stlsoft::sint32_t))
    {
        return (m_value.s32 = i, static_cast<size_t>(typeIsS32));
    }
    else
    {
        return (m_value.s64 = i, static_cast<size_t>(typeIsS64));
    }
}
size_t integer::init_(unsigned long i)
{
    m_sz[0]     = '\0';

    if(sizeof(unsigned long) <= sizeof(::stlsoft::uint32_t))
    {
        return (m_value.u32 = i, static_cast<size_t>(typeIsU32));
    }
    else
    {
        return (m_value.u64 = i, static_cast<size_t>(typeIsU64));
    }
}
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */


/* explicit */ integer::integer(::stlsoft::sint8_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(::stlsoft::uint8_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}

/* explicit */ integer::integer(::stlsoft::sint16_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(::stlsoft::uint16_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}

/* explicit */ integer::integer(::stlsoft::sint32_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(::stlsoft::uint32_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
/* explicit */ integer::integer(::stlsoft::sint64_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(::stlsoft::uint64_t i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
/* explicit */ integer::integer(int i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(unsigned int i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
/* explicit */ integer::integer(long i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
/* explicit */ integer::integer(unsigned long i, int widthAndFormat /* = 0 */)
    : m_len(init_(i))
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format(evaluate_format_(widthAndFormat))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");

    m_sz[0] = '\0';
}
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */

/* explicit */ integer::integer(::stlsoft::sint8_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(::stlsoft::uint8_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}

/* explicit */ integer::integer(::stlsoft::sint16_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(::stlsoft::uint16_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}

/* explicit */ integer::integer(::stlsoft::sint32_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(::stlsoft::uint32_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
/* explicit */ integer::integer(::stlsoft::sint64_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(::stlsoft::uint64_t i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
/* explicit */ integer::integer(int i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(unsigned int i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
/* explicit */ integer::integer(long i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
/* explicit */ integer::integer(unsigned long i)
    : m_len(init_(i))
    , m_minWidth(0)
    , m_format(0)
{}
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */

/* explicit */ integer::integer(::stlsoft::sint8_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(::stlsoft::uint8_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}

/* explicit */ integer::integer(::stlsoft::sint16_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(::stlsoft::uint16_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}

/* explicit */ integer::integer(::stlsoft::sint32_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(::stlsoft::uint32_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}

#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
/* explicit */ integer::integer(::stlsoft::sint64_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(::stlsoft::uint64_t i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */

#ifdef STLSOFT_CF_INT_DISTINCT_INT_TYPE
/* explicit */ integer::integer(int i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(unsigned int i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
#endif /* STLSOFT_CF_INT_DISTINCT_INT_TYPE */
#ifdef STLSOFT_CF_LONG_DISTINCT_INT_TYPE
/* explicit */ integer::integer(long i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
/* explicit */ integer::integer(unsigned long i, int minWidth, int format)
    : m_len(init_(i))
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_minWidth >= -128 && m_minWidth <= 127, "width must be in range [-128, 127]");
}
#endif /* STLSOFT_CF_LONG_DISTINCT_INT_TYPE */

inline void integer::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

pan_char_t const* integer::data() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_sz;
}

pan_char_t const* integer::c_str() const
{
    return data();
}

size_t integer::length() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_len;
}

void integer::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == m_sz[0], "cannot construct if value is non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(static_cast<int>(m_len) < 0, "cannot construct if length is not a type marker");

    IntegerSize size = static_cast<IntegerSize>(m_len);

    int minWidth = m_minWidth;

    if(0 != minWidth)
    {
        if(fmt::hex & m_format)
        {
            if(fmt::zeroXPrefix & m_format)
            {
                if(fmt::zeroPad & m_format)
                {
                    if(minWidth < -1)
                    {
                        minWidth += 2;
                    }
                    else if(minWidth > 1)
                    {
                        minWidth -= 2;
                    }
                }
            }
        }
    }

    switch(size)
    {
        case    typeIsS8:
        case    typeIsS16:
        case    typeIsS32:
            m_len = sprint_(m_value.s32, minWidth, m_format, m_sz, STLSOFT_NUM_ELEMENTS(m_sz));
            break;
        default:
            PANTHEIOS_CONTRACT_ENFORCE_UNEXPECTED_CONDITION_APPL_LAYER("unexpected type marker");
            // Fall through
        case    typeIsU8:
        case    typeIsU16:
        case    typeIsU32:
            m_len = sprint_(m_value.u32, minWidth, m_format, m_sz, STLSOFT_NUM_ELEMENTS(m_sz));
            break;
#ifdef STLSOFT_CF_64BIT_INT_SUPPORT
        case    typeIsS64:
            m_len = sprint_(m_value.s64, minWidth, m_format, m_sz, STLSOFT_NUM_ELEMENTS(m_sz));
            break;
        case    typeIsU64:
            m_len = sprint_(m_value.u64, minWidth, m_format, m_sz, STLSOFT_NUM_ELEMENTS(m_sz));
            break;
#endif /* STLSOFT_CF_64BIT_INT_SUPPORT */
    }

    // Deal with f-prefixed negative (signed) numbers in hex that are
    // smaller than int
    if(fmt::hex & m_format)
    {
        size_t
#if !defined(STLSOFT_COMPILER_IS_BORLAND)
               const
#endif
                        intWidth    =   2 * sizeof(int);
        size_t          l           =   0;

        switch(size)
        {
            case    typeIsS16:
                l += 2;
                // Fall through
            case    typeIsS8:
                l += 2;
                if( m_value.s32 < 0 &&
                    1)
                {
                    //pan_char_t* p0  =   &m_sz[0];
                    pan_char_t* p3  =   &m_sz[0] + m_len;
                    pan_char_t* p1  =   p3 - intWidth;
                    pan_char_t* p2  =   p3 - l;

                    if(p1 != p2)
                    {
                        if(size_t(abs(m_minWidth)) < m_len)
                        {
                            ::memmove(p1, p2, sizeof(pan_char_t) * (1 + (p3 - p2)));
                            m_len -= intWidth - l;
                        }
                        else
                        {
                            if(fmt::zeroPad & m_format)
                            {
                                std::fill_n(p1, intWidth - l, '0');
                            }
                            else
                            {
                                std::fill_n(p1, intWidth - l, ' ');
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

#if 0
    if(fmt::hex & m_format)
    {
        if(0 == m_minWidth)
        {
            if(0 == (fmt::zeroPad & m_format))
            {
                pan_char_t* p0  =   &m_sz[0];
                pan_char_t* p2  =   &m_sz[0] + m_len;

                if(fmt::zeroXPrefix & m_format)
                {
                    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION('0' == 0[p0]);
                    PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION('x' == 1[p0] || 'X' == 1[p0]);

                    p0 += 2;
                }

                pan_char_t* p1  =   p0;

                for(; '0' == *p1 && p1 != p2 - 1; ++p1)
                {
                    --m_len;
                }

                if(p1 != p0)
                {
                    ::memmove(p0, p1, sizeof(pan_char_t) * (1 + (p2 - p1)));
                }
            }
        }
    }
#endif /* 0 */

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 != m_sz[0], "failed to set value to non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(static_cast<int>(m_len) > 0, "failed to set length");
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
