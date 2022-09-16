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

/* Only include this header file once. */
#ifndef _GLOBALS_H_
#define _GLOBALS_H_ 1

#include <vector>
#include <cmath>
#include <iostream>
#include "versions.h"
#include "configuration.h"
#include <limits.h>

#if USE_ARBITRARY_PRECISION_INDEX
#include <gmpxx.h>
#endif

/// Globals contains several definitions global to the MADP toolbox.
namespace Globals {

#define INDEX_MAX UINT_MAX

/// A general index.
typedef unsigned int Index;
/// A long long index.
#if USE_ARBITRARY_PRECISION_INDEX
typedef mpz_class LIndex;
#else
typedef unsigned long long int LIndex;
#endif

/* constants */

/// The highest horizon we will consider.
/** When the horizon of a problem is set to this value, we consider it
 * an infinite-horizon problem. */
const unsigned int MAXHORIZON=999999;

///constant to denote *all* solutions (e.g., nrDesiredSolutions = ALL_SOLUTIONS )
const size_t ALL_SOLUTIONS=0;


/// The precision for probabilities.

/** Used to determine when two probabilities are considered equal, for
 * instance when converting full beliefs to sparse beliefs. */
const double PROB_PRECISION=1e-12;
/** Used to determine when two (immediate) rewards are considered equal */
const double REWARD_PRECISION=1e-12;

bool EqualProbability(double p1, double p2);
bool EqualReward(double r1, double r2);
Index CastLIndexToIndex(LIndex i);
double CastLIndexToDouble(LIndex i);

/// The initial (=empty) joint observation history index.
const Index INITIAL_JOHI=0;
/// The initial (=empty) joint action-observation history index.
const Index INITIAL_JAOHI=0;

/// Inherited from Tony's POMDP file format.
enum reward_t {REWARD, COST};

}

using namespace Globals;

//Frans: should not matter if NDEBUG is defined?
//http://lists.boost.org/MailArchives/ublas/2007/02/1764.php
// Tell Boost Ublas to not use exceptions for speed reasons.
//#define BOOST_UBLAS_NO_EXCEPTIONS 1


#include "PrintTools.h"
using namespace PrintTools;

#include "E.h"
#include "ENoSubScope.h"
#include "EInvalidIndex.h"

#endif /* !_GLOBALS_H_ */


// Local Variables: ***
// mode:c++ ***
// End: ***
