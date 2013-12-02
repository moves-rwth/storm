#include "src/utility/solver.h"

namespace storm {
    namespace utility {
        namespace solver {
            std::shared_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                return std::shared_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
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