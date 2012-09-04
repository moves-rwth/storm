/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/severity/levels.hpp
 *
 * Purpose:     Definition of the pantheios::level class template, which may
 *              be used to generate levels pseudo-constants.
 *
 * Created:     22nd July 2006
 * Updated:     23rd July 2010
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


/** \file pantheios/severity/levels.hpp
 *
 * [C++ only] Definition of the pantheios::level class template, which can
 *   be used to generate levels pseudo-constants.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_SEVERITY_HPP_LEVELS
#define PANTHEIOS_INCL_PANTHEIOS_SEVERITY_HPP_LEVELS

/* /////////////////////////////////////////////////////////////////////////
 * Version information
 */

#ifndef PANTHEIOS_DOCUMENTATION_SKIP_SECTION
# define PANTHEIOS_VER_PANTHEIOS_SEVERITY_HPP_LEVELS_MAJOR      2
# define PANTHEIOS_VER_PANTHEIOS_SEVERITY_HPP_LEVELS_MINOR      0
# define PANTHEIOS_VER_PANTHEIOS_SEVERITY_HPP_LEVELS_REVISION   2
# define PANTHEIOS_VER_PANTHEIOS_SEVERITY_HPP_LEVELS_EDIT       23
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

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
namespace pantheios
{
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Class that acts as an integer value - indicating a severity
 *   level - but which also facilitates the provision of 28-bits of
 *   extended severity information.
 *
 * In normal use, the levels instances - pantheios::debug,
 * pantheios::informational ... pantheios::emergency - can be used in the
 * place of the pan_severity_t enumerators.
 *
 * \warning The levels values must not be used in the definition of a
 *   front-end or back-end. This is because the levels values are static
 *   instances, and will likely be in a race with the API Schwarz counter
 *   (static) initialiser instances. It is likely that using one of the
 *   levels values in that case will yield the value 0.
 */
template <int Level>
class level
{
/// \name Member Types
/// @{
public:
    typedef level<Level>    class_type;
/// @}

/// \name Construction
/// @{
public:
    level()
    {}
/// @}

/// \name Operators
/// @{
public:
    /// Implicit conversion to the underlying level value
    operator pan_sev_t () const
    {
        return Level;
    }

    /// Function-call operator that allows extended severity information
    /// to be provided to the front-/back-ends
    ///
    /// \param extendedSeverityInformation28 28-bits of extended
    ///   information to be included in the
    ///
    /// \pre 0 == (extendedSeverityInformation28 & ~0xf0000000)
    pan_sev_t operator ()(pan_sev_t extendedSeverityInformation28) const
    {
        PANTHEIOS_CONTRACT_ENFORCE_PRECONDITION_PARAMS_APPL_LAYER(extendedSeverityInformation28 < 0x10000000, "Extended severity information is limited to 28-bits");

        return (Level & 0x0f) | ((extendedSeverityInformation28 << 4) & ~0x0f);
    }
/// @}

/// \name Not to be implemented
/// @{
private:
    class_type& operator =(class_type const&);
/// @}
};

/* /////////////////////////////////////////////////////////////////////////
 * Namespace
 */

#if !defined(PANTHEIOS_NO_NAMESPACE)
} /* namespace pantheios */
#endif /* !PANTHEIOS_NO_NAMESPACE */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_SEVERITY_HPP_LEVELS */

/* ///////////////////////////// end of file //////////////////////////// */
