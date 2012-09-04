/* /////////////////////////////////////////////////////////////////////////
 * File:    shwild/shwild.hpp
 *
 * Purpose: C++ root file for the shwild C-API
 *
 * Created: 17th June 2005
 * Updated: 19th December 2011
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


/** \file shwild/shwild.hpp [C++] C++ root file for the shwild C-API
 */

#ifndef SHWILD_INCL_SHWILD_HPP_SHWILD
#define SHWILD_INCL_SHWILD_HPP_SHWILD

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION
# define SHWILD_VER_SHWILD_HPP_SHWILD_MAJOR     1
# define SHWILD_VER_SHWILD_HPP_SHWILD_MINOR     1
# define SHWILD_VER_SHWILD_HPP_SHWILD_REVISION  4
# define SHWILD_VER_SHWILD_HPP_SHWILD_EDIT      7
#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef SHWILD_INCL_SHWILD_H_SHWILD
# include <shwild/shwild.h>
#endif /* !SHWILD_INCL_SHWILD_H_SHWILD */

#include <stdexcept>

/* /////////////////////////////////////////////////////////////////////////
 * Compiler warnings
 */

#ifdef __BORLANDC__
# pragma warn -8026 /* Suppresses "Functions with exception specifications are not expanded inline" */
#endif /* compiler */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
namespace shwild
{
#endif /* !SHWILD_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

// TODO: Flesh this out to be a full and complete exception class
/// Exception thrown by the Pattern constructor
/// \ingroup group__shwild_api__cpp_api
class PatternException
    : public std::runtime_error
{
/// \name Member Types
/// @{
public:
    typedef std::runtime_error  parent_class_type;
    typedef PatternException    class_type;
/// @}

/// \name Construction
/// @{
public:
    /// Construct from the given message and error code
    PatternException(char const* message, int shwildErrorCode)
        : parent_class_type(message)
        , m_shwildErrorCode(shwildErrorCode)
    {}
/// @}

/// \name Accessors
/// @{
public:
    /// nul-terminated C-style string describing the exception
    virtual char const* what() const throw()
    {
        return "Pattern Exception";
    }
    /// The error code associated with the exception
    int errorCode() const throw()
    {
        return m_shwildErrorCode;
    }
/// @}

/// \name Members
/// @{
private:
    int m_shwildErrorCode;
/// @}
};

/// Facade for the \ref group__shwild_api__c_api "shwild C API"
/// \ingroup group__shwild_api__cpp_api
class Pattern
{
public:
    /// Parses and precompiles the given pattern, according to the behaviour specified by the given flags
    ///
    /// \note If the parsing fails, an instance of PatternException is thrown
    explicit Pattern(char const* pattern, unsigned flags = 0);
    /// Parses and precompiles the given pattern, according to the behaviour specified by the given flags
    ///
    /// \note If the parsing fails, an instance of PatternException is thrown
    explicit Pattern(slice_t const* pattern, unsigned flags = 0);
    /// Parses and precompiles the given pattern, according to the behaviour specified by the given flags
    ///
    /// \note If the parsing fails, an instance of PatternException is thrown
    explicit Pattern(slice_t const& pattern, unsigned flags = 0);
    /// Releases any resources associated with the instance
    ~Pattern();

public:
    /// Match the given string against the precompiled pattern maintained as member state
    bool match(char const* string) const;
    /// Match the given string against the precompiled pattern maintained as member state
    bool match(slice_t const* string) const;
    /// Match the given string against the precompiled pattern maintained as member state
    bool match(slice_t const& string) const;

public:
    /** The number of potential matches (including the end marker) in
     *    the compiled pattern.
     *
     * \note For compilation without exception support, this will be <0 if a
     *    compilation error occurred
     */
    int numMatched() const
    {
        return m_numMatches;
    }

private:
    static shwild_handle_t init_(char const* pattern, unsigned flags, int &numMatches);
    static shwild_handle_t init_(slice_t const* pattern, unsigned flags, int &numMatches);

private:
    shwild_handle_t m_hCompiledPattern;
    int             m_numMatches;

private:
    Pattern(Pattern const&);
    Pattern &operator =(Pattern const&);
};

/* /////////////////////////////////////////////////////////////////////////
 * Implementation
 */

#ifndef SHWILD_DOCUMENTATION_SKIP_SECTION

inline /* static */ shwild_handle_t Pattern::init_(char const* pattern, unsigned flags, int &numMatches)
{
    shwild_handle_t hCompiledPattern;

    numMatches = shwild_compile_pattern(pattern, flags, &hCompiledPattern);

    if(numMatches < 0)
    {
        hCompiledPattern    =   NULL;

        throw PatternException("Failed to compile pattern", numMatches);
    }

    return hCompiledPattern;
}

inline /* static */ shwild_handle_t Pattern::init_(slice_t const* pattern, unsigned flags, int &numMatches)
{
    shwild_handle_t hCompiledPattern;

    numMatches = shwild_compile_pattern_s(pattern, flags, &hCompiledPattern);

    if(numMatches < 0)
    {
        hCompiledPattern    =   NULL;

        throw PatternException("Failed to compile pattern", numMatches);
    }

    return hCompiledPattern;
}

inline Pattern::Pattern(char const* pattern, unsigned flags)
    : m_hCompiledPattern(init_(pattern, flags, m_numMatches))
{}

inline Pattern::Pattern(slice_t const* pattern, unsigned flags)
    : m_hCompiledPattern(init_(pattern, flags, m_numMatches))
{}

inline Pattern::Pattern(slice_t const& pattern, unsigned flags)
    : m_hCompiledPattern(init_(&pattern, flags, m_numMatches))
{}

inline Pattern::~Pattern()
{
    shwild_destroy_pattern(m_hCompiledPattern);
}

inline bool Pattern::match(char const* string) const
{
    int r = shwild_match_pattern(m_hCompiledPattern, string);

    if(r < 0)
    {
        throw PatternException("Match failed", r);
    }

    return 0 == r;
}

inline bool Pattern::match(slice_t const* string) const
{
    int r = shwild_match_pattern_s(m_hCompiledPattern, string);

    if(r < 0)
    {
        throw PatternException("Match failed", r);
    }

    return 0 == r;
}

inline bool Pattern::match(slice_t const& string) const
{
    int r = shwild_match_pattern_s(m_hCompiledPattern, &string);

    if(r < 0)
    {
        throw PatternException("Match failed", r);
    }

    return 0 == r;
}

#endif /* !SHWILD_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} // namespace shwild

#endif /* !SHWILD_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_SHWILD_HPP_SHWILD */

/* ///////////////////////////// end of file //////////////////////////// */
