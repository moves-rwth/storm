#include "src/solver/SolverSelectionOptions.h"

namespace storm {
    namespace solver {
        std::string toString(MinMaxTechnique m) {
            switch(m) {
                case MinMaxTechnique::PolicyIteration:
                    return "policy";
                case MinMaxTechnique::ValueIteration:
                    return "value";
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
                case EquationSolverType::Topological:
                    return "Topological";
            }
        }
    }
}
