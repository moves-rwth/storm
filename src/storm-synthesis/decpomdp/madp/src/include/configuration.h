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

// This file contains some project-wide defines that indicate the
// availability of certain software libraries. Enabling something here
// usually also requires modifying Makefile.custom.

// Indicates whether we use libgmp to represent LIndex.
#define USE_ARBITRARY_PRECISION_INDEX 0

// Indicates whether to use the pomdp-solve library, or call an external program.
//  #if !DARWIN
//  #define USE_POMDPSOLVE_LIBRARY 1
//  #else // on OSX the pomdpsolve library doesn't get compiled in, so this
//        // should always be set to 0
//  #define USE_POMDPSOLVE_LIBRARY 0
//  #endif
//FAO: we don't want to use it anymore...
#define USE_POMDPSOLVE_LIBRARY 0
