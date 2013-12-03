#include "src/utility/solver.h"

#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            std::shared_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                std::string const& lpSolver = storm::settings::Settings::getInstance()->getOptionByLongName("lpSolver").getArgument(0).getValueAsString();
                if (lpSolver == "gurobi") {
                    return std::shared_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                } else if (lpSolver == "glpk") {
                    return std::shared_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                }
                
                throw storm::exceptions::InvalidSettingsException() << "No suitable LP-solver selected.";
            }
            
            template<typename ValueType>
            std::shared_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver() {
                std::string const& matrixLibrary = storm::settings::Settings::getInstance()->getOptionByLongName("matrixLibrary").getArgument(0).getValueAsString();
                if (matrixLibrary == "gmm++") {
                    return std::shared_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>());
                } else if (matrixLibrary == "native") {
                    return std::shared_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>>(new storm::solver::AbstractNondeterministicLinearEquationSolver<ValueType>());
                }
                
                throw storm::exceptions::InvalidSettingsException() << "No suitable nondeterministic linear equation solver selected.";
            }
            
            template std::shared_ptr<storm::solver::AbstractNondeterministicLinearEquationSolver<double>> getNondeterministicLinearEquationSolver();
        }
    }
}