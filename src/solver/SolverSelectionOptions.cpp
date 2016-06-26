#include "src/solver/SolverSelectionOptions.h"

namespace storm {
    namespace solver {
        std::string toString(MinMaxTechnique m) {
            switch(m) {
                case MinMaxTechnique::PolicyIteration:
                    return "policy";
                case MinMaxTechnique::ValueIteration:
                    return "value";
                case MinMaxTechnique::Topological:
                    return "topological";

            }
        }
        
        std::string toString(LpSolverType t) {
            switch(t) {
                case LpSolverType::Gurobi:
                    return "Gurobi";
                case LpSolverType::Glpk:
                    return "Glpk";
            }
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
        }
        
        std::string toString(SmtSolverType t) {
            switch(t) {
                case SmtSolverType::Z3:
                    return "Z3";
                case SmtSolverType::Mathsat:
                    return "Mathsat";
            }
        }
    }
}
