#ifndef SOLVERSELECTIONOPTIONS_H
#define	SOLVERSELECTIONOPTIONS_H


#include "storm/utility/ExtendSettingEnumWithSelectionField.h"

namespace storm {
    namespace solver {
        ExtendEnumsWithSelectionField(MinMaxMethod, PolicyIteration, ValueIteration, LinearProgramming, Topological, RationalSearch)
        ExtendEnumsWithSelectionField(GameMethod, PolicyIteration, ValueIteration)
        ExtendEnumsWithSelectionField(LraMethod, LinearProgramming, ValueIteration)

        ExtendEnumsWithSelectionField(LpSolverType, Gurobi, Glpk, Z3)
        ExtendEnumsWithSelectionField(EquationSolverType, Native, Gmmxx, Eigen, Elimination, Topological)
        ExtendEnumsWithSelectionField(SmtSolverType, Z3, Mathsat)
        
        ExtendEnumsWithSelectionField(NativeLinearEquationSolverMethod, Jacobi, GaussSeidel, SOR, WalkerChae, Power, RationalSearch, QuickPower)
        ExtendEnumsWithSelectionField(GmmxxLinearEquationSolverMethod, Bicgstab, Qmr, Gmres)
        ExtendEnumsWithSelectionField(GmmxxLinearEquationSolverPreconditioner, Ilu, Diagonal, None)
        ExtendEnumsWithSelectionField(EigenLinearEquationSolverMethod, SparseLU, Bicgstab, DGmres, Gmres)
        ExtendEnumsWithSelectionField(EigenLinearEquationSolverPreconditioner, Ilu, Diagonal, None)
    }
}

#endif	

