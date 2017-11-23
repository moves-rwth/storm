#include "storm/solver/LinearEquationSolverTask.h"

namespace storm {
    namespace solver {
        
        std::ostream& operator<<(std::ostream& out, LinearEquationSolverTask const& task) {
            switch (task) {
                case LinearEquationSolverTask::Unspecified: out << "unspecified"; break;
                case LinearEquationSolverTask::SolveEquations: out << "solve equations"; break;
                case LinearEquationSolverTask::Multiply: out << "multiply"; break;
            }
            return out;
        }
        
    }
}
