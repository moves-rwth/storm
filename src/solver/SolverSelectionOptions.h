
#ifndef SOLVERSELECTIONOPTIONS_H
#define	SOLVERSELECTIONOPTIONS_H


#include "src/utility/ExtendSettingEnumWithSelectionField.h"

namespace storm {
    namespace solver {
         ExtendEnumsWithSelectionField(MinMaxTechnique, PolicyIteration, ValueIteration)
        
                 
        ExtendEnumsWithSelectionField(LpSolverType, Gurobi, Glpk)
        ExtendEnumsWithSelectionField(EquationSolverType, Native, Gmmxx)
        
    }
} 

#endif	

