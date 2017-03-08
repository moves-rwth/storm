#include "storm/modelchecker/parametric/RegionCheckResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            std::ostream& operator<<(std::ostream& os, RegionCheckResult const& regionCheckResult) {
                switch (regionCheckResult) {
                    case RegionCheckResult::Unknown:
                        os << "Unknown";
                        break;
                    case RegionCheckResult::ExistsSat:
                        os << "ExistsSat";
                        break;
                    case RegionCheckResult::ExistsViolated:
                        os << "ExistsViolated";
                        break;
                    case RegionCheckResult::CenterSat:
                        os << "CenterSat";
                        break;
                    case RegionCheckResult::CenterViolated:
                        os << "CenterViolated";
                        break;
                    case RegionCheckResult::ExistsBoth:
                        os << "ExistsBoth";
                        break;
                    case RegionCheckResult::AllSat:
                        os << "AllSat";
                        break;
                    case RegionCheckResult::AllViolated:
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
