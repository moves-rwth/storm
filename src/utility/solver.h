#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/LinearEquationSolver.h"
#include "src/solver/NondeterministicLinearEquationSolver.h"
#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name);
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> getLinearEquationSolver();

            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver();

        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
