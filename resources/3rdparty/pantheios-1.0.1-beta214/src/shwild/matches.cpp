/* /////////////////////////////////////////////////////////////////////////
 * File:    src/matches.cpp
 *
 * Purpose: Implementation of Match interface class and concrete match classes
 *
 * Created: 17th June 2005
 * Updated: 20th December 2011
 *
 * Home:    http://shwild.org/
 *
 * Copyright (c) 2005-2011, Matthew Wilson and Sean Kelly
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
 * - Neither the names of Matthew Wilson and Sean Kelly nor the names of
 *   any contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
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


/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>
#include "shwild_stlsoft.h"

#if !defined(STLSOFT_CF_EXCEPTION_SUPPORT) && \
    defined(STLSOFT_COMPILER_IS_MSVC)
# pragma warning(disable : 4530) // Suppress: "C++ exception handler used, but unwind semantics are not enabled."
#endif /* NoX && VC++ */

#include "matches.hpp"
#include "shwild_assert.h"

#include <ctype.h>
#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler warnings
 */

#if defined(STLSOFT_COMPILER_IS_WATCOM)
# pragma warning(disable : 656)
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
namespace shwild
{
namespace impl
{
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/* /////////////////////////////////////////////////////////////////////////
 * Match
 */

/* virtual */ Match::~Match()
{}

/* /////////////////////////////////////////////////////////////////////////
 * MatchWild
 */

MatchWild::MatchWild(unsigned /* flags */)
    : m_next(NULL)
{}

MatchWild::~MatchWild()
{}

void MatchWild::setNext(Match *next)
{
    SHWILD_ASSERT(NULL == m_next);
    m_next = next;
}

bool MatchWild::match(char const* first, char const* last) const
{
    SHWILD_ASSERT(NULL != m_next);

    if(m_next->match(last, last))
    {
        return true;
    }
    else
    {
        char const* next;
        size_t      nextLen;

        for(next = first; last != (next = m_next->nextSub(next, last, &nextLen)); next += nextLen)
        {
            if(m_next->match(next, last))
            {
                return true;
            }
        }
    }

    return false;
}

char const* MatchWild::nextSub(char const*  /* first */, char const*  /* last */, size_t * /* nextLen */) const
{
    SHWILD_MESSAGE_ASSERT("nextSub() not callable on MatchWild", 0);

    return NULL;
}

/* /////////////////////////////////////////////////////////////////////////
 * MatchWild1
 */

MatchWild1::MatchWild1(unsigned /* flags */)
    : m_next(NULL)
{}

MatchWild1::~MatchWild1()
{}

void MatchWild1::setNext(Match *next)
{
    SHWILD_ASSERT(NULL == m_next);
    m_next = next;
}

bool MatchWild1::match(char const* first, char const* last) const
{
    SHWILD_ASSERT(NULL != m_next);

    if(0 == (last - first))
    {
        return false;
    }
    else if(m_next->match(first + 1, last))
    {
        return true;
    }

    return false;
}

char const* MatchWild1::nextSub(char const* first, char const*  /* last */, size_t *nextLen) const
{
    *nextLen = 1;   // If the contract for nextSub was that *nextLen was valid when return != last, then we'd
                    // have to set to 0 if first == last

    return first;
}

/* /////////////////////////////////////////////////////////////////////////
 * MatchRange
 */

MatchRange::MatchRange(size_t len, char const* chars, unsigned flags)
    : m_next(NULL)
    , m_chars(chars, len)
    , m_flags(flags)
{}

MatchRange::~MatchRange()
{}

void MatchRange::setNext(Match *next)
{
    SHWILD_ASSERT(NULL == m_next);
    m_next = next;
}

bool MatchRange::matchContents(char ch) const
{
    typedef char *(STLSOFT_CDECL *pfn_t)(char const*, int);

    pfn_t   pfn =   (m_flags & SHWILD_F_IGNORE_CASE)
                        ? shwild_strichr
                        : shwild_strchr;

    return NULL != (*pfn)(m_chars.c_str(), ch);
}

bool MatchRange::match(char const* first, char const* last) const
{
    SHWILD_ASSERT(NULL != m_next);

    if(0 == (last - first))
    {
        return false;
    }
    else if(!this->matchContents(*first))
    {
        return false;
    }
    else if(m_next->match(first + 1, last))
    {
        return true;
    }

    return false;
}

char const* MatchRange::nextSub(char const* first, char const*  /* last */, size_t *nextLen) const
{
    *nextLen = 1;   // If the contract for nextSub was that *nextLen was valid when return != last, then we'd
                    // have to set to 0 if first == last

    return first;
}

/* /////////////////////////////////////////////////////////////////////////
 * MatchNotRange
 */

MatchNotRange::MatchNotRange(size_t len, char const* chars, unsigned flags)
    : parent_class_type(len, chars, flags)
{}

MatchNotRange::~MatchNotRange()
{}

bool MatchNotRange::match(char const* first, char const* last) const
{
    SHWILD_ASSERT(NULL != m_next);

    if(0 == (last - first))
    {
        return false;
    }
    else if(this->matchContents(*first))
    {
        return false;
    }
    else if(m_next->match(first + 1, last))
    {
        return true;
    }

    return false;
}


/* /////////////////////////////////////////////////////////////////////////
 * MatchEnd
 */

MatchEnd::MatchEnd(unsigned /* flags */)
    : m_next(NULL)
{}

MatchEnd::~MatchEnd()
{}

void MatchEnd::setNext(Match *next)
{
    SHWILD_ASSERT(NULL == next);
    SHWILD_MESSAGE_ASSERT("WildEnd must always be the end", 0);

    STLSOFT_SUPPRESS_UNUSED(next);
}

bool MatchEnd::match(char const* first, char const* last) const
{
    return first == last;
}

char const* MatchEnd::nextSub(char const*  /* first */, char const* last, size_t *nextLen) const
{
    *nextLen = 0;

    return last;
}

/* /////////////////////////////////////////////////////////////////////////
 * MatchLiteral
 */

MatchLiteral::MatchLiteral(size_t cchContents, char const* contents, unsigned flags)
    : m_next(NULL)
    , m_contents(contents, cchContents)
    , m_flags(flags)
{}

MatchLiteral::~MatchLiteral()
{}

void MatchLiteral::setNext(Match *next)
{
    SHWILD_ASSERT(NULL == m_next);
    m_next = next;
}

bool MatchLiteral::match(char const* first, char const* last) const
{
    SHWILD_ASSERT(first <= last);

    typedef int (STLSOFT_CDECL *pfn_t)(char const*, char const*, size_t);

    const size_t    len =   m_contents.length();

    pfn_t pfn = (m_flags & SHWILD_F_IGNORE_CASE)
                    ? shwild_strnicmp
                    : shwild_strncmp;

    if(static_cast<size_t>(last - first) < len)
    {
        return false;
    }
    else if(0 == (*pfn)(first, m_contents.data(), len))
    {
        return m_next->match(first + len, last);
    }
    else
    {
        return false;
    }
}

char const* MatchLiteral::nextSub(char const* first, char const* last, size_t *nextLen) const
{
#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    !defined(_DEBUG) && \
    _MSC_VER == 1300
//# error This assumes *last == '\0', which is NOT VALID!
#endif

    typedef char *(STLSOFT_CDECL *pfn_t)(char const*, char const*);

    char const* next;
    pfn_t       pfn =   (m_flags & SHWILD_F_IGNORE_CASE)
                            ? shwild_stristr
                            : shwild_strstr;

    if(NULL == (next = (*pfn)(first, m_contents.c_str())))
    {
        return last;
    }
    else
    {
        *nextLen = m_contents.length();

        return next;
    }
}

/* /////////////////////////////////////////////////////////////////////////
 * Utility functions
 */

char *STLSOFT_CDECL shwild_strchr(const char *s, int ch)
{
    return ::strchr(const_cast<char*>(s), ch);
}

char *STLSOFT_CDECL shwild_strichr(const char *s, int ch)
{
    for(; '\0' != *s; ++s)
    {
        if(toupper(*s) == toupper(ch))
        {
            return const_cast<char*>(s);
        }
    }

    return NULL;
}

char *STLSOFT_CDECL shwild_strstr(const char *s1, const char *s2)
{
    return ::strstr(const_cast<char*>(s1), s2);
}

char *STLSOFT_CDECL shwild_stristr(const char *s1, const char *s2)
{
    const size_t    len2    =   ::strlen(s2);

    for(; '\0' != *s1; ++s1)
    {
        if(0 == shwild_strnicmp(s1, s2, len2))
        {
            return const_cast<char*>(s1);
        }
    }

    return NULL;
}

int STLSOFT_CDECL shwild_strncmp(const char *s1, const char *s2, size_t n)
{
    return ::strncmp(s1, s2, n);
}

int STLSOFT_CDECL shwild_strnicmp(const char *s1, const char *s2, size_t n)
{
#if defined(STLSOFT_COMPILER_IS_DMC)
    return ::strnicmp(s1, s2, n);
#elif (   defined(STLSOFT_COMPILER_IS_INTEL) && \
          defined(WIN32)) || \
      defined(STLSOFT_COMPILER_IS_MSVC)
    return ::_strnicmp(s1, s2, n);
#else /* ? compiler */

    { for(size_t i = 0; i < n; ++i)
    {
        char    ch1 =   (char)tolower(*s1++);
        char    ch2 =   (char)tolower(*s2++);

        if(ch1 < ch2)
        {
            return -1;
        }
        else if(ch1 > ch2)
        {
            return 1;
        }
        else if(0 == ch1)
        {
            break;
        }
    }}
    return 0;

#endif /* compiler */
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} // namespace impl

} // namespace shwild

#endif /* !SHWILD_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */
