/* /////////////////////////////////////////////////////////////////////////
 * File:    src/api.cpp
 *
 * Purpose: Implementation of the shwild API
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
#include "shwild_vector.hpp"
#include "pattern.hpp"

#include <stlsoft/smartptr/shared_ptr.hpp>
#define SHWILD_ASSERT   stlsoft_assert

#include <algorithm>

#include <string.h>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler compatiblity
 */

#if defined(STLSOFT_COMPILER_IS_INTEL)
# pragma warning(disable : 444)
#endif /* compiler */

#if !defined(STLSOFT_COMPILER_IS_WATCOM)
# define SHWILD_API_USE_ANONYMOUS_NAMESPACE
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

struct shwild_handle_t_
{};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifdef SHWILD_API_USE_ANONYMOUS_NAMESPACE
namespace
{
#endif /* SHWILD_API_USE_ANONYMOUS_NAMESPACE */

#ifndef SHWILD_NO_NAMESPACE
    using shwild::slice_t;
    using shwild::impl::Match;
    using shwild::impl::MatchWild;
    using shwild::impl::MatchWild1;
    using shwild::impl::MatchRange;
    using shwild::impl::MatchNotRange;
    using shwild::impl::MatchLiteral;
    using shwild::impl::MatchEnd;
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

typedef stlsoft::shared_ptr<Match>                      Match_ptr;
typedef ::shwild::impl::vector_maker<Match_ptr>::type   Matches;

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

// Returns the number of matches, or <0 on failure
static int  shwild_parseMatches_(Matches &matches, slice_t const* pattern, unsigned flags);
static void shwild_tieMatches_(Matches &matches);
static bool shwild_match_(Matches const &matches, slice_t const* string);

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/// \brief Maintains the state of a compiled pattern; INTERNAL CLASS.
class PatternMatcher
    : public shwild_handle_t_
{
public:
    PatternMatcher();
    ~PatternMatcher();

public:
    int compile(slice_t const* pattern, unsigned flags);
    int match(slice_t const* string) const;

private:
    Matches     m_matches;
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifdef SHWILD_API_USE_ANONYMOUS_NAMESPACE
} // anonymous namespace
#endif /* SHWILD_API_USE_ANONYMOUS_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION

int shwild_init(void)
{
    return 0;
}

void shwild_uninit(void)
{
}


int shwild_match(char const* pattern, char const* string, unsigned flags)
{
    SHWILD_ASSERT(NULL != pattern);
    SHWILD_ASSERT(NULL != string);

    const shwild_slice_t    pattern_slice(::strlen(pattern), pattern);
    const shwild_slice_t    string_slice(::strlen(string), string);

    return shwild_match_s(&pattern_slice, &string_slice, flags);
}

int shwild_match_s(shwild_slice_t const* pattern, shwild_slice_t const* string, unsigned flags)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        SHWILD_ASSERT(NULL != pattern);
        SHWILD_ASSERT(NULL != string);

        // Create a local pattern object list and match pattern against it

        // First, deal with two special cases:

        // 1. Is the pattern empty? If so, it can only match an empty string
        if(0 == pattern->len)
        {
            return 0 == string->len ? 0 : 1; // An empty pattern only matches an empty string
        }
        // 2. Is the pattern entirely composed of *? If so, it matches anything
        else
        {
            char const* b;
            char const* e;

            for(b  = pattern->ptr, e = pattern->ptr + pattern->len; b != e; ++b)
            {
                if('*' != *b)
                {
                    break;
                }
            }
            if(b == e)
            {
                return 0; // * or ** or ... ********** matches anything
            }
        }

        Matches matches;
        int     nMatches    =   shwild_parseMatches_(matches, pattern, flags);

        if(nMatches < 0)
        {
            return nMatches;
        }
        else
        {
            shwild_tieMatches_(matches);

            return shwild_match_(matches, string) ? 0 : 1;
        }
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc &)
    {
        return SHWILD_RC_ALLOC_ERROR;
    }
    catch(std::exception &)
    {
        return SHWILD_RC_UNSPECIFIED;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

int shwild_compile_pattern(char const* pattern, unsigned flags, shwild_handle_t *phCompiledPattern)
{
    SHWILD_ASSERT(NULL != pattern);

    shwild_slice_t  pattern_slice(::strlen(pattern), pattern);

    return shwild_compile_pattern_s(&pattern_slice, flags, phCompiledPattern);
}

int shwild_compile_pattern_s(shwild_slice_t const* pattern, unsigned flags, shwild_handle_t *phCompiledPattern)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        SHWILD_ASSERT(NULL != pattern);
        SHWILD_ASSERT(NULL != phCompiledPattern);

        PatternMatcher  *pm;

        *phCompiledPattern = NULL;

#if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
        try
        {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
            pm = new PatternMatcher();
#if defined(STLSOFT_CF_EXCEPTION_SUPPORT)
        }
# if defined(STLSOFT_CF_THROW_BAD_ALLOC)
        catch(std::bad_alloc &)
        {
            pm = NULL;
        }
# endif /* STLSOFT_CF_THROW_BAD_ALLOC */
        catch(std::exception &)
        {
            pm = NULL;
        }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        if(NULL == pm)
        {
            return -2;  // TODO: organise return codes
        }
        else
        {
            int r   =   pm->compile(pattern, flags);

            if(r < 0)
            {
                delete pm;
            }
            else
            {
                *phCompiledPattern = pm;
            }

            return r;
        }
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc &)
    {
        return SHWILD_RC_ALLOC_ERROR;
    }
    catch(std::exception &)
    {
        return SHWILD_RC_UNSPECIFIED;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

int shwild_match_pattern(shwild_handle_t hCompiledPattern, char const* string)
{
    SHWILD_ASSERT(NULL != hCompiledPattern);
    SHWILD_ASSERT(NULL != string);

    shwild_slice_t  string_slice(::strlen(string), string);

    return shwild_match_pattern_s(hCompiledPattern, &string_slice);
}

int shwild_match_pattern_s(shwild_handle_t hCompiledPattern, shwild_slice_t const* string)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    try
    {
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

        SHWILD_ASSERT(NULL != hCompiledPattern);
        SHWILD_ASSERT(NULL != string);

        PatternMatcher const* pm = static_cast<PatternMatcher const*>(hCompiledPattern);

        return pm->match(string);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    }
    catch(std::bad_alloc &)
    {
        return SHWILD_RC_ALLOC_ERROR;
    }
    catch(std::exception &)
    {
        return SHWILD_RC_UNSPECIFIED;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */
}

void shwild_destroy_pattern(shwild_handle_t hCompiledPattern)
{
    PatternMatcher  *pm =   static_cast<PatternMatcher*>(hCompiledPattern);

    delete pm;
}

#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* ////////////////////////////////////////////////////////////////////// */

#ifdef SHWILD_API_USE_ANONYMOUS_NAMESPACE
namespace
{
#endif /* SHWILD_API_USE_ANONYMOUS_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

static int shwild_parseMatches_(Matches &matches, slice_t const* pattern, unsigned flags)
{
    node_t          node;
    int             nMatches    =   0;
    char const*     pos;
    int             res;
    size_t          len;
    node_type       prevType    =   NODE_NOTHING;
    node_buffer_t   buffer(1);

    node_init(&node);

    for(pos = pattern->ptr; pos != pattern->ptr + pattern->len + 1; ++nMatches)
    {
        res = get_node(&node, buffer, pos, &len, flags);

        if(0 != res)
        {
            nMatches = static_cast<int>(res);

            break;
        }
        else
        {
            switch(node.type)
            {
                case    NODE_NOTHING:
                    break;
                case    NODE_WILD_1:
                    matches.push_back(Match_ptr(new MatchWild1(flags)));
                    break;
                case    NODE_WILD_N:
                    // *** === *, so coalesce. (Impl. of MatchWild() depends on this anyway)
                    if(NODE_WILD_N != prevType)
                    {
                        matches.push_back(Match_ptr(new MatchWild(flags)));
                    }
                    break;
                case    NODE_RANGE:
                    if(node.data.len > 0)
                    {
                        matches.push_back(Match_ptr(new MatchRange(node.data.len, node.data.ptr, flags)));
                    }
                    break;
                case    NODE_NOT_RANGE:
                    if(node.data.len > 0)
                    {
                        matches.push_back(Match_ptr(new MatchNotRange(node.data.len, node.data.ptr, flags)));
                    }
                    break;
                case    NODE_LITERAL:
#if !defined(STLSOFT_COMPILER_IS_BORLAND) && \
    !defined(STLSOFT_COMPILER_IS_WATCOM)
                    SHWILD_ASSERT(  NULL != ::strstr(pattern->ptr, "\\?") ||
                                    NULL != ::strstr(pattern->ptr, "\\*") ||
                                    NULL != ::strstr(pattern->ptr, "\\\\") ||
                                    NULL != ::strstr(pattern->ptr, "\\[") ||
                                    NULL != ::strstr(pattern->ptr, "\\]") ||
                                    NULL != ::strstr(pattern->ptr, "\\^") ||
                                    pattern->ptr + pattern->len != std::search( pattern->ptr, pattern->ptr + pattern->len
                                                                            ,   node.data.ptr, node.data.ptr + node.data.len));
#endif /* compiler */
                    matches.push_back(Match_ptr(new MatchLiteral(node.data.len, node.data.ptr, flags)));
                    break;
                case    NODE_END:
                    matches.push_back(Match_ptr(new MatchEnd(flags)));
                    break;
            }

            prevType    =   node.type;
            pos         +=  len;
        }
    }

    node_reset(&node);

    STLSOFT_SUPPRESS_UNUSED(flags);

    return nMatches;
}

static void shwild_tieMatches_(Matches &matches)
{
    for(size_t i = 1; i < matches.size(); ++i)
    {
        matches[i-1]->setNext(&*matches[i]);
    }
}

static bool shwild_match_(Matches const &matches, slice_t const* string)
{
    return matches[0]->match(&string->ptr[0], &string->ptr[0] + string->len);
}

/* ////////////////////////////////////////////////////////////////////// */

PatternMatcher::PatternMatcher()
{}

PatternMatcher::~PatternMatcher()
{}

int PatternMatcher::compile(slice_t const* pattern, unsigned flags)
{
    int nMatches = shwild_parseMatches_(m_matches, pattern, flags);

    if(nMatches >= 0)
    {
        shwild_tieMatches_(m_matches);
    }

    return nMatches;
}

int PatternMatcher::match(slice_t const* string) const
{
    return shwild_match_(m_matches, string) ? 0 : 1;
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifdef SHWILD_API_USE_ANONYMOUS_NAMESPACE
} // anonymous namespace
#endif /* SHWILD_API_USE_ANONYMOUS_NAMESPACE */

/* ///////////////////////////// end of file //////////////////////////// */
