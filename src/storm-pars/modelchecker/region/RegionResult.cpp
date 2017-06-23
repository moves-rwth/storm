#include "storm-pars/modelchecker/region/RegionResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        std::ostream& operator<<(std::ostream& os, RegionResult const& regionResult) {
            switch (regionResult) {
                case RegionResult::Unknown:
                    os << "Unknown";
                    break;
                case RegionResult::ExistsSat:
                    os << "ExistsSat";
                    break;
                case RegionResult::ExistsViolated:
                    os << "ExistsViolated";
                    break;
                case RegionResult::CenterSat:
                    os << "CenterSat";
                    break;
                case RegionResult::CenterViolated:
                    os << "CenterViolated";
                    break;
                case RegionResult::ExistsBoth:
                    os << "ExistsBoth";
                    break;
                case RegionResult::AllSat:
                    os << "AllSat";
                    break;
                case RegionResult::AllViolated:
                    os << "AllViolated";
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Could not get a string from the region check result. The case has not been implemented");
            }
            return os;
        }
    }
}
