#include "src/utility/solver.h"

#include "src/settings/SettingsManager.h"

#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxLinearEquationSolver.h"

#include "src/solver/NativeNondeterministicLinearEquationSolver.h"
#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/solver/TopologicalNondeterministicLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

namespace storm {
    namespace utility {
        namespace solver {
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>(matrix));
                }
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> NondeterministicLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>(matrix));
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::NativeNondeterministicLinearEquationSolver<ValueType>(matrix));
                }
            }

            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> GmmxxNondeterministicLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::GmmxxNondeterministicLinearEquationSolver<ValueType>(matrix));
            }

            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> NativeNondeterministicLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::NativeNondeterministicLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>> TopologicalNondeterministicLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::TopologicalNondeterministicLinearEquationSolver<ValueType>(matrix));
            }

            std::unique_ptr<storm::solver::LpSolver> LpSolverFactory::create(std::string const& name) const {
                storm::settings::modules::GeneralSettings::LpSolver lpSolver = storm::settings::generalSettings().getLpSolver();
                switch (lpSolver) {
                    case storm::settings::modules::GeneralSettings::LpSolver::Gurobi: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                    case storm::settings::modules::GeneralSettings::LpSolver::glpk: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                }
            }
            
            std::unique_ptr<storm::solver::LpSolver> GlpkLpSolverFactory::create(std::string const& name) const {
                return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
            }

            std::unique_ptr<storm::solver::LpSolver> GurobiLpSolverFactory::create(std::string const& name) const {
                return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
            }
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name) {
                std::unique_ptr<storm::utility::solver::LpSolverFactory> factory(new LpSolverFactory());
                return factory->create(name);
            }
            
            template class LinearEquationSolverFactory<double>;
            template class GmmxxLinearEquationSolverFactory<double>;
            template class NativeLinearEquationSolverFactory<double>;
            template class NondeterministicLinearEquationSolverFactory<double>;
            template class GmmxxNondeterministicLinearEquationSolverFactory<double>;
            template class NativeNondeterministicLinearEquationSolverFactory<double>;
            template class TopologicalNondeterministicLinearEquationSolverFactory<double>;
        }
    }
}