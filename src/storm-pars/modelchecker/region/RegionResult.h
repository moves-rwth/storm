#pragma once

#include <ostream>

namespace storm {
    namespace modelchecker {
        /*!
         * The results for a single Parameter Region
         */
        enum class RegionResult {
            Unknown, /*!< the result is unknown */
            ExistsSat, /*!< the formula is satisfied for at least one parameter evaluation that lies in the given region */
            ExistsViolated, /*!< the formula is violated for at least one parameter evaluation that lies in the given region */
            CenterSat, /*!< the formula is satisfied for the parameter Valuation that corresponds to the center point of the region */
            CenterViolated, /*!< the formula is violated for the parameter Valuation that corresponds to the center point of the region */
            ExistsBoth, /*!< the formula is satisfied for some parameters but also violated for others */
            AllSat, /*!< the formula is satisfied for all parameters in the given region */
            AllViolated /*!< the formula is violated for all parameters in the given region */
        };
        
        std::ostream& operator<<(std::ostream& os, RegionResult const& regionCheckResult);
    }
}

