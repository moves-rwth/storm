/* 
 * File:   RegionBoundary.h
 * Author: Tim Quatmann
 *
 * Created on October 29, 2015, 2:57 PM
 */

#ifndef STORM_MODELCHECKER_REGION_REGIONBOUNDARY_H
#define	STORM_MODELCHECKER_REGION_REGIONBOUNDARY_H

#include <ostream>

namespace storm {
    namespace modelchecker{
        namespace region {
            //This enum helps to store how a parameter will be substituted.
            enum class RegionBoundary { 
                LOWER,
                UPPER,
                UNSPECIFIED
            }; 
            
            std::ostream& operator<<(std::ostream& os, RegionBoundary const& regionBoundary);
        }
    }
}

#endif	/* STORM_MODELCHECKER_REGION_REGIONBOUNDARY_H */

