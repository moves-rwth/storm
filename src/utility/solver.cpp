#include "src/utility/solver.h"

#include "src/settings/SettingsManager.h"

#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxLinearEquationSolver.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                storm::settings::modules::GeneralSettings::LpSolver lpSolver = storm::settings::generalSettings().getLpSolver();
                switch (lpSolver) {
                    case storm::settings::modules::GeneralSettings::LpSolver::Gurobi: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                    case storm::settings::modules::GeneralSettings::LpSolver::glpk: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                }
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> getLinearEquationSolver() {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>());
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>());
                }
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> getNondeterministicLinearEquationSolver() {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>());
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::NativeNondeterministicLinearEquationSolver<ValueType>());
                }
            }
            
            template std::unique_ptr<storm::solver::LinearEquationSolver<double>> getLinearEquationSolver();

            template std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<double>> getNondeterministicLinearEquationSolver();
        }
    }
}