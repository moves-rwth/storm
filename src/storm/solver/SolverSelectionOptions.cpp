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
                case MinMaxMethod::Acyclic:
                    return "acyclic";
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
    }
}
