#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/GurobiLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
            }
        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
