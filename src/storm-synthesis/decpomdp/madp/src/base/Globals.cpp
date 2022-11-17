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

#include "Globals.h"
#include "EOverflow.h"
#include <sstream>

using namespace std;

bool Globals::EqualProbability(double p1, double p2)
{
    return ( abs(p1-p2) < PROB_PRECISION ) ;
}

bool Globals::EqualReward(double r1, double r2)
{
    return ( abs(r1-r2) < REWARD_PRECISION ) ;
}

#if USE_ARBITRARY_PRECISION_INDEX
Index Globals::CastLIndexToIndex(LIndex i)
{
    Index j=0;
    if(i.fits_ulong_p())
        j=i.get_ui();
    else
    {
        stringstream ss;
        ss << "LIndex with value "
           << i
           << " does not fit in an Index";
        throw(EOverflow(ss));
    }
    return(j);
}
#else
Index Globals::CastLIndexToIndex(LIndex i) {  return(i); }
#endif

#if USE_ARBITRARY_PRECISION_INDEX
double Globals::CastLIndexToDouble(LIndex i)
{
    mpf_t y;
    mpf_init(y);
    mpf_set_z(y,i.get_mpz_t());
    return mpf_get_d(y);
}
#else
double Globals::CastLIndexToDouble(LIndex i) {  return(static_cast<double>(i)); }
#endif
