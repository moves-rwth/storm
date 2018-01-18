#include "storm/solver/SolverSelectionOptions.h"

namespace storm {
    namespace solver {
        std::string toString(MinMaxMethod m) {
            switch(m) {
                case MinMaxMethod::PolicyIteration:
                    return "policy";
                case MinMaxMethod::ValueIteration:
                    return "value";
                case MinMaxMethod::LinearProgramming:
                    return "linearprogramming";
                case MinMaxMethod::Topological:
                    return "topological";
                case MinMaxMethod::RationalSearch:
                    return "ratsearch";
                case MinMaxMethod::QuickValueIteration:
                    return "QuickValueIteration";
                case MinMaxMethod::TopologicalCuda:
                    return "topologicalcuda";
            }
            return "invalid";
        }
        
        std::string toString(LraMethod m) {
            switch(m) {
                case LraMethod::LinearProgramming:
                    return "linearprogramming";
                case LraMethod::ValueIteration:
                    return "valueiteration";
            }
            return "invalid";
        }
        
        std::string toString(LpSolverType t) {
            switch(t) {
                case LpSolverType::Gurobi:
                    return "Gurobi";
                case LpSolverType::Glpk:
                    return "Glpk";
                case LpSolverType::Z3:
                    return "Z3";
            }
            return "invalid";
        }
        
        std::string toString(EquationSolverType t) {
            switch(t) {
                case EquationSolverType::Native:
                    return "Native";
                case EquationSolverType::Gmmxx:
                    return "Gmmxx";
                case EquationSolverType::Eigen:
                    return "Eigen";
                case EquationSolverType::Elimination:
                    return "Elimination";
                case EquationSolverType::Topological:
                    return "Topological";
            }
            return "invalid";
        }
        
        std::string toString(SmtSolverType t) {
            switch(t) {
                case SmtSolverType::Z3:
                    return "Z3";
                case SmtSolverType::Mathsat:
                    return "Mathsat";
            }
            return "invalid";
        }
        
        std::string toString(NativeLinearEquationSolverMethod t) {
            switch( t) {
                case NativeLinearEquationSolverMethod::Jacobi:
                    return "Jacobi";
                case NativeLinearEquationSolverMethod::GaussSeidel:
                    return "GaussSeidel";
                case NativeLinearEquationSolverMethod::SOR:
                    return "SOR";
                case NativeLinearEquationSolverMethod::WalkerChae:
                    return "WalkerChae";
                case NativeLinearEquationSolverMethod::Power:
                    return "Power";
                case NativeLinearEquationSolverMethod::RationalSearch:
                    return "RationalSearch";
                case NativeLinearEquationSolverMethod::QuickPower:
                    return "QuickPower";
            }
            return "invalid";
        }
        
        std::string toString(GmmxxLinearEquationSolverMethod t) {
            switch (t) {
                case GmmxxLinearEquationSolverMethod::Bicgstab:
                    return "BiCGSTAB";
                case GmmxxLinearEquationSolverMethod::Qmr:
                    return "QMR";
                case GmmxxLinearEquationSolverMethod::Gmres:
                    return "GMRES";
            }
            return "invalid";
        }
        
        std::string toString(GmmxxLinearEquationSolverPreconditioner t) {
            switch (t) {
                case GmmxxLinearEquationSolverPreconditioner::Diagonal:
                    return "diagonal";
                case GmmxxLinearEquationSolverPreconditioner::Ilu:
                    return "ilu";
                case GmmxxLinearEquationSolverPreconditioner::None:
                    return "none";
            }
            return "invalid";
        }
        
        std::string toString(EigenLinearEquationSolverMethod t) {
            switch (t) {
                case EigenLinearEquationSolverMethod::SparseLU:
                    return "SparseLU";
                case EigenLinearEquationSolverMethod::Bicgstab:
                    return "BiCGSTAB";
                case EigenLinearEquationSolverMethod::DGmres:
                    return "DGMRES";
                case EigenLinearEquationSolverMethod::Gmres:
                    return "GMRES";
            }
            return "invalid";
        }
        
        std::string toString(EigenLinearEquationSolverPreconditioner t) {
            switch (t) {
                case EigenLinearEquationSolverPreconditioner::Diagonal:
                    return "diagonal";
                case EigenLinearEquationSolverPreconditioner::Ilu:
                    return "ilu";
                case EigenLinearEquationSolverPreconditioner::None:
                    return "none";
            }
            return "invalid";
        }
    }
}
