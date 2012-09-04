/* /////////////////////////////////////////////////////////////////////////
 * File:        inserters/pointer.cpp
 *
 * Purpose:     Implementation of the inserter classes.
 *
 * Created:     21st June 2005
 * Updated:     6th August 2012
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
#include <pantheios/internal/nox.h>
#include <pantheios/internal/safestr.h>

#if defined(STLSOFT_COMPILER_IS_MWERKS)
 /* For some to-be-determined reason, CodeWarrior can't see a whole lot of
  * things in STLSoft header files from <string.h>, even though those
  * files #include it.
  */
# include <string.h>
#endif /* compiler */

#include <pantheios/inserters/pointer.hpp>

#include <pantheios/quality/contract.h>
#include <pantheios/util/string/snprintf.h>

/* STLSoft Header Files */

#include <stlsoft/conversion/union_cast.hpp>
#include <stlsoft/util/size_traits.hpp>

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

/* /////////////////////////////////////////////////////////////////////////
 * pointer
 */

inline /* static */ int pointer::validate_width_(int minWidth)
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER((minWidth >= -20 && minWidth <= 20), "pantheios::pointer width must be in range [-20, 20]");

    return minWidth;
}

/* explicit */ pointer::pointer(void const volatile* pv, int widthAndFormat)
    : m_value(pv)
    , m_len(0)
    , m_minWidth(validate_width_(static_cast<pan_sint8_t>(widthAndFormat & 0xff)))
    , m_format((widthAndFormat & ~0xff) | (0 != (fmt::zeroXPrefix & widthAndFormat) ? fmt::zeroPad : 0))
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER((widthAndFormat >= 0), "pantheios::pointer width must be non-negative when specified in composite widthAndFormat");

    m_sz[0] = '\0';
}

/* explicit */ pointer::pointer(void const volatile* pv, int minWidth, int format)
    : m_value(pv)
    , m_len(0)
    , m_minWidth(validate_width_(minWidth))
    , m_format(format)
{
    m_sz[0] = '\0';
}

inline void pointer::construct_() const
{
    const_cast<class_type*>(this)->construct_();
}

pan_char_t const* pointer::data() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_sz;
}

pan_char_t const* pointer::c_str() const
{
    return data();
}

size_t pointer::length() const
{
    if(0 == m_sz[0])
    {
        construct_();
    }

    return m_len;
}

void pointer::construct_()
{
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == m_sz[0], "cannot construct if value is non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 == m_len, "cannot construct if length is non-0");

    if(static_cast<void const*>(0) == m_value)
    {
        static const pan_char_t  s_null[] = PANTHEIOS_LITERAL_STRING("(null)");

        STLSOFT_STATIC_ASSERT(sizeof(m_sz) >= sizeof(s_null));

        ::memcpy(&m_sz[0], &s_null[0], sizeof(s_null));

        m_len = STLSOFT_NUM_ELEMENTS(s_null) - 1;
    }
    else
    {
        pan_char_t          szFmt[101];
        int                 width;
        pan_char_t const*   zeroX;
        pan_char_t const*   leadingMinus;
        pan_char_t const*   zeroPad;

        if(m_minWidth < 0)
        {
            width           =   -m_minWidth;
            leadingMinus    =   PANTHEIOS_LITERAL_STRING("-");
        }
        else
        {
            width           =   m_minWidth;
            leadingMinus    =   PANTHEIOS_LITERAL_STRING("");
        }

        zeroX   =   (m_format & fmt::zeroXPrefix) ? PANTHEIOS_LITERAL_STRING("0x") : PANTHEIOS_LITERAL_STRING("");
        zeroPad =   (m_format & fmt::zeroPad) ? PANTHEIOS_LITERAL_STRING("0") : PANTHEIOS_LITERAL_STRING("");

        if( 0 != width &&
            fmt::zeroXPrefix == (m_format & (fmt::zeroXPrefix | fmt::zeroPad)))
        {
            // Special case

            pan_char_t szTemp[23]; // 23 is always big enough, since the width is 21

            PANTHEIOS_CONTRACT_ENFORCE_ASSUMPTION(0 == (m_format & fmt::zeroPad));

            // Determine ptr-sized integer type
#if defined(STLSOFT_COMPILER_IS_GCC)
            typedef unsigned long                                             intptr_t_;
#else /* ? compiler */
            typedef stlsoft::int_size_traits<sizeof(void*)>::unsigned_type    intptr_t_;
#endif /* compiler */

            int r = pantheios_util_snprintf(
                        &szTemp[0]
                    ,   STLSOFT_NUM_ELEMENTS(szTemp)
                    ,   PANTHEIOS_LITERAL_STRING("0x%lx")
                    ,   static_cast<intptr_t_>(stlsoft::union_cast<intptr_t_>(m_value))
                    );

            if(r < 0)
            {
                return;
            }
            else
            {
                size_t n = static_cast<size_t>(r);

                PANTHEIOS_CONTRACT_ENFORCE_POSTCONDITION_RETURN_INTERNAL((n + 2) <= STLSOFT_NUM_ELEMENTS(szTemp), "snprintf() overwrote the local buffer capacity");

                width += 2;

                if(static_cast<size_t>(width) > n)
                {
                    if(m_minWidth < 0)
                    {
                        ::memcpy(&m_sz[0], szTemp, n * sizeof(pan_char_t));
                        std::fill_n(&m_sz[0] + n, size_t(width - n), ' ');
                        m_sz[width] = '\0';
                    }
                    else
                    {
                        std::fill_n(&m_sz[0], size_t(width - n), ' ');
                        ::memcpy(&m_sz[0] + (size_t(width) - n), szTemp, (n + 1) * sizeof(pan_char_t));
                    }

                    m_len = static_cast<size_t>(width);
                }
                else
                {
                    ::memcpy(&m_sz[0], szTemp, (n + 1) * sizeof(pan_char_t));

                    m_len = n;
                }
            }
        }
        else
        {
            pantheios_util_snprintf(&szFmt[0]
                                ,   STLSOFT_NUM_ELEMENTS(szFmt)
                                ,   PANTHEIOS_LITERAL_STRING("%s%%%s%s%dx")
                                ,   zeroX
                                ,   leadingMinus
                                ,   zeroPad
                                ,   width);

            m_len = static_cast<size_t>(pantheios_util_snprintf(&m_sz[0], STLSOFT_NUM_ELEMENTS(m_sz), szFmt, m_value));
        }
    }

    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(0 != m_sz[0], "failed to set value to non-empty");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_len > 0, "failed to set length");
    PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(m_len < 23, "pantheios::pointer string too long");
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
