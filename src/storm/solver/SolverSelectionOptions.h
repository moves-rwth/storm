#ifndef SOLVERSELECTIONOPTIONS_H
#define SOLVERSELECTIONOPTIONS_H

#include "storm/utility/ExtendSettingEnumWithSelectionField.h"

namespace storm {
namespace solver {
ExtendEnumsWithSelectionField(MinMaxMethod, ValueIteration, PolicyIteration, LinearProgramming, Topological, RationalSearch, IntervalIteration,
                              SoundValueIteration, OptimisticValueIteration, TopologicalCuda, ViToPi, Acyclic)
    ExtendEnumsWithSelectionField(MultiplierType, Native, Gmmxx) ExtendEnumsWithSelectionField(GameMethod, PolicyIteration, ValueIteration)
        ExtendEnumsWithSelectionField(LraMethod, LinearProgramming, ValueIteration, GainBiasEquations, LraDistributionEquations)
            ExtendEnumsWithSelectionField(MaBoundedReachabilityMethod, Imca, UnifPlus)

                ExtendEnumsWithSelectionField(LpSolverType, Gurobi, Glpk, Z3, Soplex)
                    ExtendEnumsWithSelectionField(EquationSolverType, Native, Gmmxx, Eigen, Elimination, Topological, Acyclic)
                        ExtendEnumsWithSelectionField(SmtSolverType, Z3, Mathsat)

                            ExtendEnumsWithSelectionField(NativeLinearEquationSolverMethod, Jacobi, GaussSeidel, SOR, WalkerChae, Power, SoundValueIteration,
                                                          OptimisticValueIteration, IntervalIteration, RationalSearch)
                                ExtendEnumsWithSelectionField(GmmxxLinearEquationSolverMethod, Bicgstab, Qmr, Gmres)
                                    ExtendEnumsWithSelectionField(GmmxxLinearEquationSolverPreconditioner, Ilu, Diagonal, None)
                                        ExtendEnumsWithSelectionField(EigenLinearEquationSolverMethod, SparseLU, Bicgstab, DGmres, Gmres)
                                            ExtendEnumsWithSelectionField(EigenLinearEquationSolverPreconditioner, Ilu, Diagonal, None)
}
}  // namespace storm

#endif
