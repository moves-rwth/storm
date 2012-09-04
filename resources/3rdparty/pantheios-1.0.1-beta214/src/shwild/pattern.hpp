/* /////////////////////////////////////////////////////////////////////////
 * File:    src/pattern.hpp
 *
 * Purpose: C string object for shwild implementation
 *
 * Created: 17th June 2005
 * Updated: 20th December 2011
 *
 * Home:    http://shwild.org/
 *
 * Copyright (c) 2005-2011, Sean Kelly and Matthew Wilson
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


#ifndef SHWILD_INCL_HPP_PATTERN
#define SHWILD_INCL_HPP_PATTERN

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

/* shwild Header Files */
#include <shwild/shwild.h>

/* STLSoft Header Files */
#include "shwild_stlsoft.h"

#if defined(STLSOFT_COMPILER_IS_MSVC) && \
    _MSC_VER >= 1400
/* For some weird reason, when used with VC++ 8, pattern.cpp ends up with a
 * definition of std::allocator<>::allocate() and 
 * std::allocator<>::deallocate(), which breaks the linker with LNK2005
 * (multiple definitions).
 *
 * So we disable the use of std::allocator, and tell it to use
 * stlsoft::malloc_allocator instead.
 */
# define STLSOFT_ALLOCATOR_SELECTOR_USE_STLSOFT_MALLOC_ALLOCATOR
# define STLSOFT_ALLOCATOR_SELECTOR_NO_USE_STD_ALLOCATOR
#endif /* compiler */

#include <stlsoft/memory/auto_buffer.hpp>

/* Standard C Header Files */
#include <limits.h>

/* /////////////////////////////////////////////////////////////////////////
 * Typedefs
 */

/** \brief Types of pattern tokens */
enum token_type
{
        TOK_INVALID     =   CHAR_MIN - 1
    ,   TOK_END         =   0
    ,   TOK_WILD_1      =   CHAR_MAX + 1
    ,   TOK_WILD_N      =   CHAR_MAX + 2
    ,   TOK_RANGE_BEG   =   CHAR_MAX + 3
    ,   TOK_RANGE_END   =   CHAR_MAX + 4
    ,   TOK_ENOMEM      =   CHAR_MAX + 5
};

/** \brief Types of pattern nodes */
enum node_type
{
        NODE_NOTHING
    ,   NODE_WILD_1
    ,   NODE_WILD_N
    ,   NODE_RANGE
    ,   NODE_NOT_RANGE
    ,   NODE_LITERAL
    ,   NODE_END
};

/** \brief Node; INTERNAL CLASS. */
struct node_t
{
    node_type       type;   /*!< The type of the node */
    shwild_slice_t  data;   /*!< Indicates the contents */
};

/** \brief Buffer used when necessary. */
typedef stlsoft::auto_buffer<   char
                            ,   1024
//                          ,   ss_typename_type_def_k allocator_selector<T>::allocator_type
                            >    node_buffer_t;

/* /////////////////////////////////////////////////////////////////////////
 * API
 */

/** \brief Initialises a node. */
void node_init( node_t* node );

/** \brief Uninitialises a node, releasing any associated resources. */
void node_reset( node_t* node );

/** \brief Parses the next node */
int get_node( node_t* node, node_buffer_t &buffer, char const* buf, size_t* len, unsigned flags );

/* ////////////////////////////////////////////////////////////////////// */

#endif /* !SHWILD_INCL_HPP_PATTERN */

/* ///////////////////////////// end of file //////////////////////////// */
