/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/util/backends/context.hpp
 *
 * Purpose:     Implementation class to assist in the creation of back-ends.
 *
 * Created:     16th December 2006
 * Updated:     26th November 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2006-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/util/backends/context.hpp
 *
 * [C++ only] Implementation class to assist in the creation of back-ends.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT
#define PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT_MAJOR    3
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT_MINOR    3
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT_REVISION 2
# define PANTHEIOS_VER_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT_EDIT     33
#endif /* !PANTHEIOS_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */
#ifndef PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT
# include <pantheios/quality/contract.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_QUALITY_H_CONTRACT */

#ifndef __cplusplus
# error This file can only be used in C++ compilation units
#endif /* !__cplusplus */

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
namespace util
{
namespace backends
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Framework class for assisting in the writing of \ref group__backend "Back-ends".
 *
 * To use pantheios::util::backends::Context, you derive from it, and override
 * the rawLogEntry() method, as in the implementation of be.speech:
<pre>
struct be_speech_context
    : public pantheios::util::backends::Context
{
public: // Member Types
  typedef pantheios::util::backends::Context  parent_class_type;
  typedef be_speech_context                   class_type;
  typedef std::string                         string_type;
  typedef stlsoft::ref_ptr<ISpVoice>          voice_type;

public: // Construction
  be_speech_context(pan_char_t const* processIdentity, int id, pantheios::uint32_t flags, voice_type voice);
  ~be_speech_context() throw();

private: // Overrides
  virtual int rawLogEntry(int severity, const pan_slice_t (&ar)[rawLogArrayDimension], size_t cchTotal);

  . . .
};
</pre>
 *
 * An instance of the derived class is created in the back-end
 * initialisation function, and destroyed in the uninitialisation function,
 * each of which are called a maximum of one time per program execution.
 *
 * Finally, in the implementation of the back-end's log entry function, the
 * logEntry() method is called, as in:
<pre>
static int pantheios_be_speech_logEntry(  void*             // feToken
                                        , void*             beToken
                                        , int               severity
                                        , pan_char_t const* entry
                                        , size_t            cchEntry)
{
  PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_API(NULL != beToken, "back-end token may not be null");

  be_speech_context* ctxt = static_cast<be_speech_context*>(beToken);

  return ctxt->logEntry(severity, entry, cchEntry);
}
</pre>
 *
 * \ingroup group__utility__backend
 */
class Context
{
/// \name Member Types
/// @{
public:
    typedef Context     class_type;
/// @}

/// \name Member Constants
/// @{
protected:
    enum { rawLogArrayDimension = 10 };
/// @}

/// \name Construction
/// @{
protected:
    /// Constructs an instance
    ///
    /// \param processIdentity The process identity. May not be NULL or
    ///   empty
    /// \param id The back-end identifier
    /// \param flags Flags that control the elements presented in the
    ///   log statements
    /// \param severityMask Mask that prescribes the valid range of the
    ///   severity for the derived class. Must be either 0x07 or 0x0f
    Context(pan_char_t const* processIdentity, int id, pan_uint32_t flags, int severityMask);
    virtual ~Context() throw();
/// @}

/// \name Operations
/// @{
public:
    ///
    ///
    /// \note This method does not mutate any member data. It is not marked
    ///   <code>const</code> solely because it invokes rawLogEntry(), which
    ///   may need to mutate member data of the derived class (though this
    ///   will not be the dominant form).
    ///
    /// \note Because the method does not, in and of itself, mutate data,
    ///   backends using the class may only need to lock if the underlying
    ///   transmission medium requires it, and this may only need to be
    ///   inside the call to rawLogEntry(), on a back-end-specific basis.
    int logEntry(int severity, pan_char_t const* entry, size_t cchEntry);
/// @}

/// \name Overrides
/// @{
private:
    /// \param severity The severity at which the statement will be logged
    /// \param ar A reference to an array of 'rawLogArrayDimension' slices
    virtual int rawLogEntry(int severity4, int severityX, const pan_slice_t (&ar)[rawLogArrayDimension], size_t cchTotal) = 0;

    /// \param severity The severity at which the statement will be logged
    /// \param entry Pointer to the C-style string containing the entry
    /// \param cchEntry Number of characters in the entry, not including the
    ///   nul-terminator
    virtual int rawLogEntry(int severity4, int severityX, pan_char_t const* entry, size_t cchEntry);
/// @}

/// \name Utilities
/// @{
protected:
    /// Concatenates the slices into the given destination
    static size_t concatenateSlices(pan_char_t* dest, size_t cchDest, size_t numSlices, pan_slice_t const* slices);
/// @}

/// \name Accessors
/// @{
public:
    /// The process identity
    ///
    /// \note In builds where exceptions are not enabled, or with compilers
    ///   that do not throw std::bad_alloc on allocation failure, this will
    ///   return NULL to indicate that construction has failed
    pan_char_t const*   getProcessIdentity() const;
    int                 getBackEndId() const;
/// @}

/// \name Member Variables
/// @{
protected:
    pan_char_t* const   m_processIdentity;
    int const           m_id;
    pan_uint32_t const  m_flags;
    int const           m_severityMask;
private:
    // 0: "["
    // 1: process Id
    // 2: "."
    // 3: thread Id
    // 4: "; "
    // 5: time
    // 6: ";  "
    // 5: severity
    // 8: "]: "
    // 9: entry

    pan_slice_t     m_slice0;
    pan_slice_t     m_slice1;
    pan_slice_t     m_slice2;
    pan_slice_t     m_slice4;
    pan_slice_t     m_slice6;
    pan_slice_t     m_slice8;
/// @}

/// \name Not to be implemented
/// @{
private:
    Context(class_type const&);
    class_type& operator =(class_type const&);
/// @}
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} // namespace backends
} // namespace util
} // namespace pantheios
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_UTIL_BACKENDS_HPP_CONTEXT */

/* ///////////////////////////// end of file //////////////////////////// */
