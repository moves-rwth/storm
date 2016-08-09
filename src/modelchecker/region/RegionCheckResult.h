/* 
 * File:   RegionCheckResult.h
 * Author: tim
 *
 * Created on September 9, 2015, 1:56 PM
 */

#ifndef STORM_MODELCHECKER_REGION_REGIONCHECKRESULT_H
#define	STORM_MODELCHECKER_REGION_REGIONCHECKRESULT_H

#include <ostream>

namespace storm {
    namespace modelchecker {
        namespace region {
            /*!
             * The possible results for a single Parameter region
             */
            enum class RegionCheckResult { 
                UNKNOWN, /*!< the result is unknown */
                EXISTSSAT, /*!< the formula is satisfied for at least one parameter evaluation that lies in the given region */
                EXISTSVIOLATED, /*!< the formula is violated for at least one parameter evaluation that lies in the given region */
                EXISTSBOTH, /*!< the formula is satisfied for some parameters but also violated for others */
                ALLSAT, /*!< the formula is satisfied for all parameters in the given region */
                ALLVIOLATED /*!< the formula is violated for all parameters in the given region */
            };
            
            std::ostream& operator<<(std::ostream& os, RegionCheckResult const& regionCheckResult);
        }
    }
}

#endif	/* STORM_MODELCHECKER_REGION_REGIONCHECKRESULT_H */

