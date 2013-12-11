#ifndef STORM_UTILITY_SOLVER_H_
#define STORM_UTILITY_SOLVER_H_

#include "src/solver/AbstractLinearEquationSolver.h"
#include "src/solver/AbstractNondeterministicLinearEquationSolver.h"
#include "src/solver/LpSolver.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            std::shared_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name);
            
            template<typename ValueType>
            std::shared_ptr<storm::solver::AbstractLinearEquationSolver<ValueType>> getLinearEquationSolver();

            template<typename ValueType>
            std::shared_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver();

        }
    }
}

#endif /* STORM_UTILITY_SOLVER_H_ */
