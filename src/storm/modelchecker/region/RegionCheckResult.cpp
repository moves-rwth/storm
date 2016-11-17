/* 
 * File:   RegionCheckResult.cpp
 * Author: tim
 * 
 * Created on September 9, 2015, 1:56 PM
 */

#include "RegionCheckResult.h"
#include "src/utility/macros.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            std::ostream& operator<<(std::ostream& os, RegionCheckResult const& regionCheckResult) {
                switch (regionCheckResult) {
                    case RegionCheckResult::UNKNOWN:
                        os << "Unknown";
                        break;
                    case RegionCheckResult::EXISTSSAT:
                        os << "ExistsSat";
                        break;
                    case RegionCheckResult::EXISTSVIOLATED:
                        os << "ExistsViolated";
                        break;
                    case RegionCheckResult::EXISTSBOTH:
                        os << "ExistsBoth";
                        break;
                    case RegionCheckResult::ALLSAT:
                        os << "AllSat";
                        break;
                    case RegionCheckResult::ALLVIOLATED:
                        os << "AllViolated";
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Could not get a string from the region check result. The case has not been implemented");
                }
                return os;
            }
        }
    }
}