/* /////////////////////////////////////////////////////////////////////////
 * File:    src/matches.hpp
 *
 * Purpose: Definition of Match interface class and concrete match classes
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


#ifndef SHWILD_INCL_SRC_HPP_MATCHES
#define SHWILD_INCL_SRC_HPP_MATCHES

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#include <shwild/shwild.h>
#include "shwild_stlsoft.h"
#include "shwild_string.hpp"    // for shwild_string_t

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
 * Functions
 */

char *STLSOFT_CDECL shwild_strchr(const char *, int);
char *STLSOFT_CDECL shwild_strichr(const char *, int);
char *STLSOFT_CDECL shwild_strstr(const char *, const char *);
char *STLSOFT_CDECL shwild_stristr(const char *, const char *);
int STLSOFT_CDECL   shwild_strncmp(const char *, const char *, size_t);
int STLSOFT_CDECL   shwild_strnicmp(const char *, const char *, size_t);

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** \brief Root class of the match hierarchy; INTERNAL CLASS. */
class Match
{
public:
    virtual ~Match() = 0;

    /// \brief Sets the next Match in the series
    virtual void setNext(Match *next) = 0;

public:
    /// \brief Attempt full match of the given string range
    ///
    /// The instance matches the given string range against its criteria, and
    /// the criteria of all its down-the-line peers
    virtual bool match(char const *first, char const *last) const = 0;
    /// \brief Attempt partial match of the given string range
    ///
    /// The instance matches the given string range against its own criteria. If
    /// the match is successful, *nextLen is set to the matched length, and the
    /// return value is first + *nextLen
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const = 0;
};

/** \brief Matches 0 or more of any characters; INTERNAL CLASS. */
class MatchWild
    : public Match
{
public:
    typedef MatchWild   class_type;

public:
    explicit MatchWild(unsigned flags);
    virtual ~MatchWild();

    virtual void setNext(Match *next);
public:
    virtual bool match(char const *first, char const *last) const;
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const;
private:
    Match   *m_next;
private:
    class_type &operator =(class_type const &);
};

/** \brief Matches exactly 1 of any characters; INTERNAL CLASS. */
class MatchWild1
    : public Match
{
public:
    typedef MatchWild1  class_type;

public:
    explicit MatchWild1(unsigned flags);
    virtual ~MatchWild1();

    virtual void setNext(Match *next);
public:
    virtual bool match(char const *first, char const *last) const;
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const;
private:
    Match   *m_next;
private:
    class_type &operator =(class_type const &);
};

/** \brief Matches exactly 1 character from a given range; INTERNAL CLASS. */
class MatchRange
    : public Match
{
public:
    typedef MatchRange  class_type;

public:
    MatchRange(size_t len, char const *chars, unsigned flags);
    virtual ~MatchRange();

    virtual void setNext(Match *next);
public:
    virtual bool match(char const *first, char const *last) const;
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const;
protected:
    bool matchContents(char ch) const;
protected:
    Match                   *m_next;
    const shwild_string_t   m_chars;
    const unsigned          m_flags;
private:
    class_type &operator =(class_type const &);
};

/** \brief Matches exactly 1 character not from a given range; INTERNAL CLASS. */
class MatchNotRange
    : public MatchRange
{
public:
    typedef MatchRange      parent_class_type;
    typedef MatchNotRange   class_type;

public:
    MatchNotRange(size_t len, char const *chars, unsigned flags);
    virtual ~MatchNotRange();

public:
    virtual bool match(char const *first, char const *last) const;

private:
    class_type &operator =(class_type const &);
};

/** \brief Matches the end of a string; INTERNAL CLASS. */
class MatchEnd
    : public Match
{
public:
    typedef MatchEnd    class_type;

public:
    explicit MatchEnd(unsigned flags);
    virtual ~MatchEnd();

    virtual void setNext(Match *next);
public:
    virtual bool match(char const *first, char const *last) const;
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const;
private:
    Match   *m_next;
private:
    class_type &operator =(class_type const &);
};

/** \brief Matches a literal string; INTERNAL CLASS. */
class MatchLiteral
    : public Match
{
public:
    typedef MatchLiteral    class_type;

public:
    MatchLiteral(size_t cchContents, char const *contents, unsigned flags);
    virtual ~MatchLiteral();

    virtual void setNext(Match *next);
public:
    virtual bool match(char const *first, char const *last) const;
    virtual char const *nextSub(char const *first, char const *last, size_t *nextLen) const;
private:
    Match                   *m_next;
    const shwild_string_t   m_contents;
    const unsigned          m_flags;
private:
    class_type &operator =(class_type const &);
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(SHWILD_NO_NAMESPACE)
} // namespace impl

} // namespace shwild
#endif /* !SHWILD_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_SRC_HPP_MATCHES */

/* ////////////////////////////////////////////////////////////////////// */
