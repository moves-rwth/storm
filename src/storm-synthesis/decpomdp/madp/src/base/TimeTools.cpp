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

#include "TimeTools.h"

namespace TimeTools{

double GetDeltaTimeDouble(timeval start_time, timeval cur_time)
{
    if(gettimeofday(&cur_time, NULL) != 0)
        throw "Error with gettimeofday";

    time_t delta_sec = cur_time.tv_sec - start_time.tv_sec;
    suseconds_t delta_usec = cur_time.tv_usec - start_time.tv_usec;
    double delta = 1000000.0 * delta_sec + delta_usec; //in microsecond
    return delta;
}

}



