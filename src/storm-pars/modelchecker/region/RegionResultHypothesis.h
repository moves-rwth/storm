#pragma once

#include <ostream>

namespace storm {
    namespace modelchecker {
        /*!
         * hypothesis for the result for a single Parameter Region
         */
        enum class RegionResultHypothesis {
            Unknown,
            AllSat,
            AllViolated
        };
        
        std::ostream& operator<<(std::ostream& os, RegionResultHypothesis const& regionResultHypothesis);
    }
}

