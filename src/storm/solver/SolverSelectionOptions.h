#ifndef SOLVERSELECTIONOPTIONS_H
#define	SOLVERSELECTIONOPTIONS_H


#include "storm/utility/ExtendSettingEnumWithSelectionField.h"

namespace storm {
    namespace solver {
        ExtendEnumsWithSelectionField(MinMaxMethod, PolicyIteration, ValueIteration, LinearProgramming, Topological, RationalSearch)
        ExtendEnumsWithSelectionField(GameMethod, PolicyIteration, ValueIteration)
        ExtendEnumsWithSelectionField(LraMethod, LinearProgramming, ValueIteration)

        ExtendEnumsWithSelectionField(LpSolverType, Gurobi, Glpk, Z3)
        ExtendEnumsWithSelectionField(EquationSolverType, Native, Gmmxx, Eigen, Elimination)
        ExtendEnumsWithSelectionField(SmtSolverType, Z3, Mathsat)
    }
} 

#endif	

