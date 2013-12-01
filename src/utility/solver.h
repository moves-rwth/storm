#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/AbstractNondeterministicLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver() {
                return std::unique_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>>(new storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>());
//                return std::unique_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>());
            }
        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
