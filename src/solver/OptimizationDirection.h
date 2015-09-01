#ifndef OPTIMIZATIONDIRECTIONSETTING_H
#define	OPTIMIZATIONDIRECTIONSETTING_H

#include <iostream>

namespace storm {
    namespace solver {
        enum class OptimizationDirection { Minimize = 0, Maximize = 1 };
        enum class OptimizationDirectionSetting { Minimize = 0, Maximize = 1, Unset };
        
        bool isSet(OptimizationDirectionSetting s);
        
        bool minimize(OptimizationDirection d);
        
        bool maximize(OptimizationDirection d);
        
        OptimizationDirection convert(OptimizationDirectionSetting s);
        
        OptimizationDirectionSetting convert(OptimizationDirection d);
        
        OptimizationDirection invert(OptimizationDirection d);
        
        std::ostream& operator<<(std::ostream& out, OptimizationDirection d);
    }
    
    using OptimizationDirection = solver::OptimizationDirection;
}


#endif	/* OPTIMIZATIONDIRECTIONSETTING_H */

