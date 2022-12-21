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


#ifndef  TIMETOOLS_INC
#define  TIMETOOLS_INC

#include <sys/time.h>
#include <time.h>

namespace TimeTools{
    ///Returns the difference between start time and current time
    /**Returns a double, time is in microseconds
     */
    double GetDeltaTimeDouble(timeval start_time, timeval cur_time);

}

#endif   /* ----- #ifndef TIMETOOLS_INC  ----- */

