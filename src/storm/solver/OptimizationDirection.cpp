#include "OptimizationDirection.h"
#include <iostream>
#include "storm/utility/macros.h"

namespace storm {
    namespace solver {
       
        bool isSet(OptimizationDirectionSetting s) {
            return s != OptimizationDirectionSetting::Unset;
        } 
        
        bool minimize(OptimizationDirection d) {
            return d == OptimizationDirection::Minimize;
        }
        
        bool maximize(OptimizationDirection d) {
            return d == OptimizationDirection::Maximize;
        }
        
        OptimizationDirection convert(OptimizationDirectionSetting s) {
            STORM_LOG_ASSERT(isSet(s), "Setting is not set.");
            return static_cast<OptimizationDirection>(s);
        }
        
        OptimizationDirectionSetting convert(OptimizationDirection d) {
            return static_cast<OptimizationDirectionSetting>(d);
        }
        
        OptimizationDirection invert(OptimizationDirection d) {
            return d == OptimizationDirection::Minimize ? OptimizationDirection::Maximize : OptimizationDirection::Minimize;
        }
        
        std::ostream& operator<<(std::ostream& out, OptimizationDirection d) {
            return d == OptimizationDirection::Minimize ? out << "minimize" : out << "maximize";
        } 
    }
}

