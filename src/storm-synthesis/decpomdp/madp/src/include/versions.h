/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */

#include "boost/version.hpp"

#if BOOST_VERSION < 103300
#define BOOST_1_32_OR_LOWER 1
#else
#define BOOST_1_32_OR_LOWER 0
#endif

#if BOOST_VERSION >= 103600
#define BOOST_1_36_OR_HIGHER 1
#else
#define BOOST_1_36_OR_HIGHER 0
#endif

#if BOOST_VERSION >= 103800
// Spirit V2 has been merged in the Boost, so now we need to use the
// "classic" version
#define USE_BOOST_SPIRIT_CLASSIC 1
#else
#define USE_BOOST_SPIRIT_CLASSIC 0
#endif

