/* /////////////////////////////////////////////////////////////////////////
 * File:        pantheios/internal/initialiser.hpp
 *
 * Purpose:     Automatic initialisation of Pantheios Core API
 *
 * Created:     21st June 2005
 * Updated:     2nd August 2010
 *
 * Home:        http://www.pantheios.org/
 *
 * Copyright (c) 2005-2010, Matthew Wilson and Synesis Software
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


/** \file pantheios/internal/initialiser.hpp
 *
 * [C, C++] INTERNAL FILE: Provides automatic initialisation of Pantheios
 *   Core API, via the pantheios_initialiser Schwarz counter class.
 *
 * \note Inclusion of this file, via pantheios/pantheios.hpp, declares an
 *   instance of the pantheios::pantheios_initialiser class in each
 *   compilation unit in which it is included.
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_HPP_INITIALISER
#define PANTHEIOS_INCL_PANTHEIOS_HPP_INITIALISER

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifndef PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS
# include <pantheios/pantheios.h>
#endif /* !PANTHEIOS_INCL_PANTHEIOS_H_PANTHEIOS */

/* /////////////////////////////////////////////////////////////////////////
 * Classes
 */

/** Schwarz Counter initialiser for the Pantheios library
 *
 * In "normal" use, this file is included by pantheios/pantheios.hpp, so
 * that every C++ compilation unit that may potentially be issuing
 * Pantheios calls contains, at file scope (well, actually in an
 * anonymous namespace that is itself at file scoope) an instance of the
 * initialiser. Since the Pantheios core is a reference-counted API
 * (see Chapter 11 of
 * <a href = "http://imperfectcplusplus.com" target="_blank">Imperfect C++</a>),
 * this means that the first C++ compilation linked in initialises the
 * core, and the remainder keep it initialised, until they are all
 * "completed". In other words, just including pantheios/pantheios.hpp
 * causes Pantheios to be initialised.
 *
 * In non-"normal" cases, the inclusion is not made, and the initialisation
 * is not performed. These cases are either when:
 *  - The link-unit being built is a dynamic library (as discriminated by
 *     the presence of the processor symbols <code>_USRDLL</code> or
 *     <code>_AFXDLL</code>, and the preprocessor symbol
 *     <code>PANTHEIOS_FORCE_AUTO_INIT</code> is <b>not</b> defined, or
 *  - The preprocessor symbol <code>PANTHEIOS_NO_AUTO_INIT</code> <b>is</b>
 *     defined.
 *
 * In either of these cases, you <b>must</b> explicitly initialise the
 * Pantheios core (which initialises the front-end and back-end(s))
 * explicitly, using
 * \link pantheios::pantheios_init pantheios_init()\endlink
 * and
 * \link pantheios::pantheios_uninit pantheios_uninit()\endlink.
 */
class pantheios_initialiser
{
/// \name Member Types
/// @{
public:
    typedef pantheios_initialiser   class_type;
/// @}

/// \name Construction
/// @{
public:
    ///  Initialises the Pantheios API
    ///
    /// Calls pantheios::pantheios_init(), calling exit(1) on failure
    pantheios_initialiser()
    {
#ifndef PANTHEIOS_NO_NAMESPACE
        using namespace pantheios;
        using namespace pantheios::core;
#endif /* !PANTHEIOS_NO_NAMESPACE */

        if(pantheios_init() < 0)
        {
            pantheios_exitProcess(1);
        }
    }
    ///  Uninitialises the Pantheios API
    ///
    /// Calls pantheios::pantheios_uninit()
    ~pantheios_initialiser()
    {
#ifndef PANTHEIOS_NO_NAMESPACE
        using namespace pantheios;
#endif /* !PANTHEIOS_NO_NAMESPACE */

        pantheios_uninit();
    }
/// @}

/// \name Not to be implemented
/// @{
private:
    pantheios_initialiser(class_type const&);
    class_type& operator =(class_type const&);
/// @}
};

namespace
{
    /// The per-compilation unit instance of pantheios_initialiser, which
    /// ensures that the Pantheios library is initialised prior to use.
    static pantheios_initialiser    s_pantheios_initialiser;

} /* anonymous namespace */

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !PANTHEIOS_INCL_PANTHEIOS_HPP_INITIALISER */

/* ///////////////////////////// end of file //////////////////////////// */
