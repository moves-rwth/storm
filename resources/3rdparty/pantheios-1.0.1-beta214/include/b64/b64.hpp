/* /////////////////////////////////////////////////////////////////////////
 * File:        b64/b64.hpp
 *
 * Purpose:     Header file for the b64 C++-API.
 *
 * Created:     18th October 2004
 * Updated:     4th February 2012
 *
 * Home:        http://synesis.com.au/software/
 *
 * Copyright 2004-2012, Matthew Wilson and Synesis Software
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer. 
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name(s) of Matthew Wilson and Synesis Software nor the names of
 *   any contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ////////////////////////////////////////////////////////////////////// */


/** \file b64/b64.hpp [C++ only] Header file for the b64 C++-API
 *
 * This header file contains the b64 C++-API types and functions. There are
 * no associated implementation files; in other words, the b64 C++-API is
 * header only.
 *
 * The b64/C++ is dependent on several components from the
 * <a href = "http://stlsoft.org/">STLSoft</a> libraries (also 100% header-only)
 * so you will need to have them available on your include path when using this
 * header.
 *
 * \htmlonly
 * <hr>
 * \endhtmlonly
 */

#ifndef B64_INCL_B64_HPP_B64
#define B64_INCL_B64_HPP_B64

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef B64_DOCUMENTATION_SKIP_SECTION
# define B64_VER_B64_HPP_B64_MAJOR      2
# define B64_VER_B64_HPP_B64_MINOR      1
# define B64_VER_B64_HPP_B64_REVISION   12
# define B64_VER_B64_HPP_B64_EDIT       38
#endif /* !B64_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef B64_INCL_B64_H_B64
# include <b64/b64.h>
#endif /* !B64_INCL_B64_H_B64 */

/* If the compiler cannot find the following include, you may have not
 * upgraded to the latest version of STLSoft: 1.9.1. Go to
 *  http://stlsoft.org/ and download this version or later.
 */
#ifndef STLSOFT_INCL_STLSOFT_H_STLSOFT
# include <stlsoft/stlsoft.h>
#endif /* !STLSOFT_INCL_STLSOFT_H_STLSOFT */

#if !defined(_STLSOFT_VER) || \
    _STLSOFT_VER < 0x01096fff
# error Requires STLSoft 1.9.111, or later. (www.stlsoft.org/downloads.html)
#endif /* STLSoft version */

#ifdef STLSOFT_CF_std_NAMESPACE

# if defined(B64_USE_CUSTOM_STRING)
#  include B64_CUSTOM_STRING_INCLUDE
# else /* B64_USE_CUSTOM_STRING */
#  include <string>
# endif /* !B64_USE_CUSTOM_STRING */

# if defined(B64_USE_CUSTOM_VECTOR)
#  include B64_CUSTOM_VECTOR_INCLUDE
# else /* B64_USE_CUSTOM_VECTOR */
#  include <vector>
# endif /* !B64_USE_CUSTOM_VECTOR */

 /* We'll now have a go at checking whether the string type is 
  * known to be contiguous
  */
# ifndef STLSOFT_INCL_STLSOFT_UTIL_STD_LIBRARY_DISCRIMINATOR
#  include <stlsoft/util/std/library_discriminator.hpp>
# endif /* !STLSOFT_INCL_STLSOFT_UTIL_STD_LIBRARY_DISCRIMINATOR */
# ifdef STLSOFT_CF_STD_LIBRARY_IS_DINKUMWARE_VC
#  if STLSOFT_CF_STD_LIBRARY_DINKUMWARE_VC_VERSION <= STLSOFT_CF_DINKUMWARE_VC_VERSION_7_1
#   define B64_STRING_TYPE_IS_CONTIGUOUS
#  endif /* STLSOFT_CF_STD_LIBRARY_DINKUMWARE_VC_VERSION */
# endif /* STLSOFT_CF_STD_LIBRARY_IS_DINKUMWARE_VC */

#else /* ? STLSOFT_CF_std_NAMESPACE */

# if defined(STLSOFT_COMPILER_IS_WATCOM)
#  include <string.hpp>
#  include <wcvector.h>
#  define   B64_USE_CUSTOM_STRING
#  define   B64_CUSTOM_STRING_TYPE      watcom_string_for_b64
#  define   B64_USE_CUSTOM_VECTOR
#  define   B64_CUSTOM_BLOB_TYPE        watcom_vector_for_b64
#  define   B64_STRING_TYPE_IS_CONTIGUOUS
# else /* ? compiler */
#  error No other non-std compiler is known
# endif /* ? compiler */

#endif /* STLSOFT_CF_std_NAMESPACE */

#ifdef B64_NO_CONTIGUOUS_STRING_TYPE
# ifdef B64_STRING_TYPE_IS_CONTIGUOUS
#  undef B64_STRING_TYPE_IS_CONTIGUOUS
# endif /* B64_STRING_TYPE_IS_CONTIGUOUS */
#endif /* B64_NO_CONTIGUOUS_STRING_TYPE */

#if !defined(B64_STRING_TYPE_IS_CONTIGUOUS)
# ifndef STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER
#  include <stlsoft/memory/auto_buffer.hpp>
# endif /* !STLSOFT_INCL_STLSOFT_MEMORY_HPP_AUTO_BUFFER */
#endif /* !B64_STRING_TYPE_IS_CONTIGUOUS */

#ifndef STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_HPP_STRING
# include <stlsoft/shims/access/string.hpp>
#endif /* !STLSOFT_INCL_STLSOFT_SHIMS_ACCESS_HPP_STRING */

/* #ifdef B64_ */

#define B64_DECLARE_SHIM_PAIR_()		stlsoft_ns_using(c_str_data_a); stlsoft_ns_using(c_str_len_a)
#define B64_INVOKE_SHIM_data_(s)		c_str_data_a(s)
#define B64_INVOKE_SHIM_len_(s)			c_str_len_a(s)

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
# if defined(STLSOFT_COMPILER_IS_WATCOM)
#  include <stdexcep.h>
# else /* ? compiler */
#  include <stdexcept>
# endif /* compiler */
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#ifndef B64_NO_NAMESPACE
namespace B64_NAMESPACE
{
#endif /* !B64_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

#if defined(STLSOFT_COMPILER_IS_WATCOM)
class watcom_vector_for_b64
    : public WCValVector<unsigned char>
{
private:
    typedef WCValVector<unsigned char>  parent_class_type;
    typedef watcom_vector_for_b64       class_type;
public:
    watcom_vector_for_b64()
    {}
    watcom_vector_for_b64(size_t n)
        : parent_class_type(n)
    {}
    watcom_vector_for_b64(class_type const &rhs)
        : parent_class_type(rhs)
    {}

public:
    size_t size() const
    {
        return parent_class_type::length();
    }
};

class watcom_string_for_b64
    : public String
{
public:
    watcom_string_for_b64(size_t n, char ch)
        : String(ch, n)
    {}

public:
    void resize(size_t n)
    {
        String& this_ = *this;

        this_ = String(*this, n);
    }
};
#endif /* STLSOFT_COMPILER_IS_WATCOM */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
/** \brief Exception thrown during encoding/decoding
 */
class coding_exception
    : public stlsoft_ns_qual_std(runtime_error)
{
public:
    typedef stlsoft_ns_qual_std(runtime_error)  parent_class_type;
    typedef coding_exception                    class_type;
private:
#if defined(B64_USE_CUSTOM_STRING)
    typedef B64_CUSTOM_STRING_TYPE              string_type;
#else /* B64_USE_CUSTOM_STRING */
    typedef std::string                         string_type;
#endif /* !B64_USE_CUSTOM_STRING */

public:
    /** Constructs an exception based on the error code, and a pointer to the bad character. */
    coding_exception(B64_RC rc, char const* badChar)
        : parent_class_type(make_message_(rc))
        , m_rc(rc)
        , m_badChar(badChar)
    {}

public:
    /** The error code associated with the exception. */
    B64_RC get_rc() const
    {
        return m_rc;
    }
    /** The return code associated with the exception.
     *
     * \note May be NULL, if the error is not associated with an invalid character encountered during decoding.
     */
    char const* get_badChar() const
    {
        return m_badChar;
    }

private:
    static string_type make_message_(B64_RC rc)
    {
        return string_type("Decoding error: ") + b64_getStatusCodeString(rc);
    }

private:
    B64_RC      m_rc;
    char const* m_badChar;
};
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** The string type for the b64 namespace
 *
 * \note This defaults to <tt>::std::string</tt>. It is possible to override this, using
 * preprocessor symbol definitions. To do this, you must define
 * the symbol \c B64_USE_CUSTOM_STRING to instruct the compiler that the <b>b64</b>
 * library is to use a custom string type. In that case you <b>must also</b>
 * define the \c B64_CUSTOM_STRING_INCLUDE and \c B64_CUSTOM_STRING_TYPE and symbols.
 *
 * \c B64_CUSTOM_STRING_INCLUDE specifies the \c <> or \c "" surrounded include file name, e.g.
 *
 * <tt>&nbsp;&nbsp;\#define B64_CUSTOM_STRING_INCLUDE <stlsoft/simple_string.hpp></tt>
 *
 * \c B64_CUSTOM_STRING_TYPE specifies the string type, e.g.
 *
 * <tt>&nbsp;&nbsp;\#define B64_CUSTOM_STRING_TYPE&nbsp;&nbsp;&nbsp;&nbsp;::stlsoft::basic_simple_string<char></tt>
 *
 * \note For Open Watcom compilation, this type is actually the class 
 * class <code>b64::watcom_string_for_b64</code>, a class adaptor for
 * the Open Watcom <code>String</code> class. The definition
 * of this class can be found in b64/b64.hpp.
 */
#if defined(B64_USE_CUSTOM_STRING)
typedef B64_CUSTOM_STRING_TYPE          string_t;
#else /* B64_USE_CUSTOM_STRING */
typedef std::string                     string_t;
#endif /* !B64_USE_CUSTOM_STRING */

/** The blob type for the b64 namespace 
 *
 * \note This defaults to <tt>::std::vector< ::stlsoft::byte_t></tt>. It is possible to
 * override this, using
 * preprocessor symbol definitions. To do this, you must define
 * the symbol \c B64_USE_CUSTOM_VECTOR to instruct the compiler that the <b>b64</b>
 * library is to use a custom string type. In that case you <b>must also</b>
 * define the \c B64_CUSTOM_VECTOR_INCLUDE and \c B64_CUSTOM_BLOB_TYPE and symbols.
 *
 * \c B64_CUSTOM_VECTOR_INCLUDE specifies the \c <> or \c "" surrounded include file name, e.g.
 *
 * <tt>&nbsp;&nbsp;\#define B64_CUSTOM_VECTOR_INCLUDE <stlsoft/pod_vector.hpp></tt>
 *
 * \c B64_CUSTOM_BLOB_TYPE specifies the string type, e.g.
 *
 * <tt>&nbsp;&nbsp;\#define B64_CUSTOM_BLOB_TYPE&nbsp;&nbsp;&nbsp;&nbsp;::stlsoft::pod_vector<unsigned char></tt>
 *
 * \note For Open Watcom compilation, this type is actually the class 
 * class <code>b64::watcom_vector_for_b64</code>, a class adaptor for
 * the <code>WCValVector&lt;unsigned char&gt;</code> specialisation. The definition
 * of this class can be found in b64/b64.hpp.
 */
#if defined(B64_USE_CUSTOM_VECTOR)
typedef B64_CUSTOM_BLOB_TYPE            blob_t;
#else /* B64_USE_CUSTOM_VECTOR */
# ifndef B64_DOCUMENTATION_SKIP_SECTION
typedef ::stlsoft::byte_t               byte_t_;
typedef std::vector<byte_t_>            blob_t;
# else /* !B64_DOCUMENTATION_SKIP_SECTION */
typedef std::vector< ::stlsoft::byte_t> blob_t;
# endif /* !B64_DOCUMENTATION_SKIP_SECTION */
#endif /* !B64_USE_CUSTOM_VECTOR */

/* /////////////////////////////////////////////////////////////////////////
 * Functions
 */

/** \brief Encodes the given block of memory into base-64.
 *
 * This function takes a pointer to a memory block to be encoded, and a number of
 * bytes to be encoded, and carries out a base-64 encoding on it, returning the
 * results in an instance of the string type \link #string_t string_t \endlink. See the 
 * \ref section__cpp_api "example" from the main page
 *
 * \param src Pointer to the block to be encoded
 * \param srcSize Number of bytes in the block to be encoded
 * \param flags A combination of the B64_FLAGS enumeration, that moderate the
 *   behaviour of the function
 * \param lineLen If the flags parameter contains B64_F_LINE_LEN_USE_PARAM, then
 *   this parameter represents the length of the lines into which the encoded form is split,
 *   with a hard line break ('\\r\\n'). If this value is 0, then the line is not
 *   split. If it is <0, then the RFC-1113 recommended line length of 64 is used
 * \param rc The return code representing the status of the operation. May be NULL.
 *
 * \return The string form of the block
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
inline string_t encode(void const* src, size_t srcSize, int flags, int lineLen = 0, B64_RC* rc = NULL)
{
    B64_RC  rc_;

    // Make sure rc is non-NULL, since we will need to get the RC in order to
    // throw exception later.
    if(NULL == rc)
    {
        rc = &rc_;
    }

    size_t      n   =   B64_NAMESPACE_QUALIFIER::b64_encode2(src, srcSize, NULL, 0u, static_cast<unsigned>(flags), lineLen, rc);

#ifdef B64_STRING_TYPE_IS_CONTIGUOUS

    // If the string type is known to have contiguous storage we can avoid
    // any intermediate memory, and decode directly into its internal
    // buffer.

    string_t    s(n, '~'); // ~ is used for an invalid / eyecatcher

    STLSOFT_MESSAGE_ASSERT("assumed contiguous string type is not so. Please report this error. To effect fix now, #define B64_NO_CONTIGUOUS_STRING_TYPE", 0 == n || &s[n - 1] == &s[0] + (n - 1));

    size_t      n2  =   B64_NAMESPACE_QUALIFIER::b64_encode2(src, srcSize, &s[0], s.length(), static_cast<unsigned>(flags), lineLen, rc);

    s.resize(n2);

#else /* ? B64_STRING_TYPE_IS_CONTIGUOUS */

    // If the string type is not known to be contiguous, then we must use
    // intermediate storage. Here we use a 1KB auto_buffer, so that only
    // data in excess of that will incur an additional (over the string's)
    // heap allocation.

    typedef stlsoft::auto_buffer<b64_char_t, 1024>  buffer_t;

    buffer_t    buffer(n);
    size_t      n2  =   B64_NAMESPACE_QUALIFIER::b64_encode2(src, srcSize, &buffer[0], buffer.size(), static_cast<unsigned>(flags), lineLen, rc);

    string_t    s(&buffer[0], n2);
#endif /* B64_STRING_TYPE_IS_CONTIGUOUS */

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    if( 0 != srcSize &&
        0 == n2 &&
        rc == &rc_)
    {
        throw coding_exception(*rc, NULL);
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    return s;
}

/** \brief Encodes the given block of memory into base-64.
 *
 * This function takes a pointer to a memory block to be encoded, and a number of
 * bytes to be encoded, and carries out a base-64 encoding on it, returning the
 * results in an instance of the string type \link #string_t string_t \endlink. See the 
 * \ref section__cpp_api "example" from the main page
 *
 * \param src Pointer to the block to be encoded
 * \param srcSize Number of bytes in the block to be encoded
 *
 * \return The string form of the block
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
inline string_t encode(void const* src, size_t srcSize)
{
    return encode(src, srcSize, 0, 0, NULL);
}

#ifdef STLSOFT_CF_STATIC_ARRAY_SIZE_DETERMINATION_SUPPORT
/** \brief Encodes the given array into base-64
 *
 * \param ar The array whose contents are to be encoded
 *
 * \return The string form of the block
 *
 * \note This function is only defined for compilers that are able to discriminate
 * between pointers and arrays. See Chapter 14 of <a href = "http://imperfectcplusplus.com/">Imperfect C++</a>
 * for details about this facility, and consult your <a href = "http://stlsoft.org/">STLSoft</a> header files
 * to find out to which compilers this applies.
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
template <typename T, size_t N>
inline string_t encode(T (&ar)[N])
{
    return encode(&ar[0], sizeof(T) * N);
}
#endif /* STLSOFT_CF_STATIC_ARRAY_SIZE_DETERMINATION_SUPPORT */

/** \brief Encodes the given blob into base-64
 *
 * \param blob The blob whose contents are to be encoded
 *
 * \return The string form of the block
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
inline string_t encode(blob_t const &blob)
{
    return encode(blob.empty() ? NULL : &blob[0], blob.size());
}

/** \brief Encodes the given blob into base-64
 *
 * \param blob The blob whose contents are to be encoded
 * \param flags A combination of the B64_FLAGS enumeration, that moderate the
 *   behaviour of the function
 * \param lineLen If the flags parameter contains B64_F_LINE_LEN_USE_PARAM, then
 *   this parameter represents the length of the lines into which the encoded form is split,
 *   with a hard line break ('\\r\\n'). If this value is 0, then the line is not
 *   split. If it is <0, then the RFC-1113 recommended line length of 64 is used
 * \param rc The return code representing the status of the operation. May be NULL.
 *
 * \return The string form of the block
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
inline string_t encode(blob_t const &blob, int flags, int lineLen = 0, B64_RC* rc = NULL)
{
    return encode(blob.empty() ? NULL : &blob[0], blob.size(), flags, lineLen, rc);
}

/** \brief Decodes the given base-64 block into binary
 *
 * \param src Pointer to the block to be decoded
 * \param srcLen Number of characters in the block to be decoded
 * \param flags A combination of the B64_FLAGS enumeration, that moderate the
 *   behaviour of the function.
 * \param rc The return code representing the status of the operation. May be NULL.
 * \param badChar If the flags parameter does not contain B64_F_STOP_ON_NOTHING, this
 *   parameter specifies the address of a pointer that will be set to point to the 
 *   character in the sequence that stops the parsing, as dictated by the flags 
 *   parameter. May be NULL.
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 * \exception b64::coding_exception If a bad character is encountered in the encoded block (as moderated by the flags parameter; see \link b64::B64_FLAGS B64_FLAGS\endlink)
 */
inline blob_t decode(b64_char_t const* src, size_t srcLen, int flags, b64_char_t const** badChar = NULL, B64_RC* rc = NULL)
{
#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    B64_RC              rc_;
    b64_char_t const*   badChar_;

    if(NULL == rc)
    {
        rc = &rc_;
    }
    if(NULL == badChar)
    {
        badChar = &badChar_;
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    size_t  n   =   B64_NAMESPACE_QUALIFIER::b64_decode2(src, srcLen, NULL, 0, static_cast<unsigned>(flags), badChar, rc);
    blob_t  v(n);
    size_t  n2  =   v.empty() ? 0 : B64_NAMESPACE_QUALIFIER::b64_decode2(src, srcLen, &v[0], v.size(), static_cast<unsigned>(flags), badChar, rc);

    v.resize(n2);

#ifdef STLSOFT_CF_EXCEPTION_SUPPORT
    if( 0 != srcLen &&
        0 == n2 &&
        rc == &rc_)
    {
        if(B64_RC_OK == *rc)
        {
            *rc = B64_RC_TRUNCATED_INPUT;
        }

        throw coding_exception(*rc, (badChar == &badChar_) ? *badChar : NULL);
    }
#endif /* STLSOFT_CF_EXCEPTION_SUPPORT */

    return v;
}

/** \brief Decodes the given base-64 block into binary
 *
 * \param src Pointer to the block to be decoded
 * \param srcLen Number of characters in the block to be decoded
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
inline blob_t decode(b64_char_t const* src, size_t srcLen)
{
    return decode(src, srcLen, B64_F_STOP_ON_BAD_CHAR, NULL, NULL);
}

#ifndef B64_DOCUMENTATION_SKIP_SECTION
STLSOFT_OPEN_WORKER_NS_(impl)

/** \brief Function template that decodes an instance of an arbitrary string type from base-64 into binary
 *
 * \param str The string whose contents are to be decoded
 * \param flags Flags that moderate the decoding
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
template <class S>
inline blob_t b64_impl_decode_(int flags, S const &str)
{
    B64_DECLARE_SHIM_PAIR_();

    b64_char_t const* dummy; // Cannot rely on badChar being available in str

    return decode(
        B64_INVOKE_SHIM_data_(str)
    ,   B64_INVOKE_SHIM_len_(str)
    ,   static_cast<B64_FLAGS>(flags)
    ,   &dummy
    ,   NULL
    );
}

STLSOFT_CLOSE_WORKER_NS_(ns)
#endif /* !B64_DOCUMENTATION_SKIP_SECTION */


/** \brief Function template that decodes an instance of an arbitrary string type from base-64 into binary
 *
 * \param str The string whose contents are to be decoded
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
template <class S>
inline blob_t decode(S const &str)
{
    return STLSOFT_WORKER_NS_QUAL_(impl, b64_impl_decode_)(B64_F_STOP_ON_BAD_CHAR, str);
}

/** \brief Function template that decodes an instance of an arbitrary string type from base-64 into binary
 *
 * \param str The string whose contents are to be decoded
 * \param flags Flags that moderate the decoding
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 */
template <class S>
inline blob_t decode(int flags, S const &str) // NOTE: This has to be overloaded, rather than use default arguments, otherwise VC has a spit
{
    return STLSOFT_WORKER_NS_QUAL_(impl, b64_impl_decode_)(flags, str);
}

/** \brief Decodes the given string from base-64 into binary
 *
 * \param str The string whose contents are to be decoded
 * \param flags A combination of the B64_FLAGS enumeration, that moderate the
 *   behaviour of the function.
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 * \exception b64::coding_exception If a bad character is encountered in the encoded block (as moderated by the flags parameter; see \link b64::B64_FLAGS B64_FLAGS\endlink)
 */
inline blob_t decode(string_t const &str, int flags = B64_F_STOP_ON_BAD_CHAR)
{
    B64_DECLARE_SHIM_PAIR_();

    return decode(
        B64_INVOKE_SHIM_data_(str)
    ,   B64_INVOKE_SHIM_len_(str)
    ,   flags
    ,   NULL
    ,   NULL
    );
}

/** \brief Decodes the given string from base-64 into binary
 *
 * \param str The string whose contents are to be decoded
 * \param flags A combination of the B64_FLAGS enumeration, that moderate the
 *   behaviour of the function.
 * \param rc The return code representing the status of the operation. May be NULL.
 * \param badChar If the flags parameter does not contain B64_F_STOP_ON_NOTHING, this
 *   parameter specifies the address of a pointer that will be set to point to the 
 *   character in the sequence that stops the parsing, as dictated by the flags 
 *   parameter. May be NULL.
 *
 * \return The binary form of the block, as a \c blob_t
 *
 * \note There is no error return. If insufficient memory can be allocated, an
 * instance of \c std::bad_alloc will be thrown
 *
 * \note Threading: The function is fully re-entrant, assuming that the heap for the
 * system is re-entrant.
 *
 * \note Exceptions: Provides the strong guarantee, assuming that the constructor for
 * the string type (\c string_t) does so.
 *
 * \exception std::bad_alloc If insufficient memory is available to complete the operation
 * \exception b64::coding_exception If a bad character is encountered in the encoded block (as moderated by the flags parameter; see \link b64::B64_FLAGS B64_FLAGS\endlink)
 */
inline blob_t decode(string_t const &str, int flags, b64_char_t const** badChar, B64_RC* rc = NULL)
{
    B64_DECLARE_SHIM_PAIR_();

    return decode(
        B64_INVOKE_SHIM_data_(str)
    ,   B64_INVOKE_SHIM_len_(str)
    ,   flags
    ,   badChar
    ,   rc
    );
}

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

/** \brief [C++ only] The <code>b64::cpp</code> namespace, within which the C++
 *    types and functions <b><i>used</i></b> to reside; they now reside
 *    within the <code>b64</code> namespace.
 *
 * \deprecated All the constructs that were formerly in the
 *    <code>b64::cpp</code> namespace now reside within the <code>b64</code>
 *    namespace.
 */
namespace cpp
{

    using B64_NAMESPACE::coding_exception;

    using B64_NAMESPACE::blob_t;
    using B64_NAMESPACE::string_t;

    using B64_NAMESPACE::decode;
    using B64_NAMESPACE::encode;

} /* namespace cpp */

#ifndef B64_NO_NAMESPACE
} /* namespace B64_NAMESPACE */
#endif /* !B64_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* B64_INCL_B64_HPP_B64 */

/* ///////////////////////////// end of file //////////////////////////// */
