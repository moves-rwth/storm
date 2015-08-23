#include "src/utility/solver.h"

#include "src/settings/SettingsManager.h"

#include "src/solver/SymbolicGameSolver.h"

#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxLinearEquationSolver.h"

#include "src/solver/NativeMinMaxLinearEquationSolver.h"
#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"
#include "src/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace utility {
        namespace solver {
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<Type, ValueType>> SymbolicLinearEquationSolverFactory<Type, ValueType>::create(storm::dd::Add<Type> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
                return std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<Type, ValueType>>(new storm::solver::SymbolicLinearEquationSolver<Type, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs));
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>> SymbolicMinMaxLinearEquationSolverFactory<Type, ValueType>::create(storm::dd::Add<Type> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
                return std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>>(new storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>(A, allRows, illegalMask, rowMetaVariables, columnMetaVariables, choiceVariables, rowColumnMetaVariablePairs));
            }
            
            template<storm::dd::DdType Type>
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type>> SymbolicGameSolverFactory<Type>::create(storm::dd::Add<Type> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) const {
                return std::unique_ptr<storm::solver::SymbolicGameSolver<Type>>(new storm::solver::SymbolicGameSolver<Type>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables, player2Variables));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>(matrix));
                    default: return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
                }
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            NativeLinearEquationSolverFactory<ValueType>::NativeLinearEquationSolverFactory() {
                switch (storm::settings::nativeEquationSolverSettings().getLinearEquationSystemMethod()) {
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi:
                    this->method = storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod::Jacobi;
                    break;
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel:
                    this->method = storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod::GaussSeidel;
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::SOR:
                    this->method = storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod::SOR;
                }
                omega = storm::settings::nativeEquationSolverSettings().getOmega();
            }
            
            template<typename ValueType>
            NativeLinearEquationSolverFactory<ValueType>::NativeLinearEquationSolverFactory(typename storm::solver::NativeLinearEquationSolver<ValueType>::SolutionMethod method, ValueType omega) : method(method), omega(omega) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>(matrix, method));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                storm::settings::modules::GeneralSettings::EquationSolver equationSolver = storm::settings::generalSettings().getEquationSolver();
                switch (equationSolver) {
                    case storm::settings::modules::GeneralSettings::EquationSolver::Gmmxx: return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::GmmxxMinMaxLinearEquationSolver<ValueType>(matrix));
                    case storm::settings::modules::GeneralSettings::EquationSolver::Native: return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::NativeMinMaxLinearEquationSolver<ValueType>(matrix));
                    default: return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::GmmxxMinMaxLinearEquationSolver<ValueType>(matrix));
                }
            }

            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> GmmxxMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::GmmxxMinMaxLinearEquationSolver<ValueType>(matrix));
            }

            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> NativeMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::NativeMinMaxLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> TopologicalMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>>(new storm::solver::TopologicalMinMaxLinearEquationSolver<ValueType>(matrix));
            }

            std::unique_ptr<storm::solver::LpSolver> LpSolverFactory::create(std::string const& name) const {
                storm::settings::modules::GeneralSettings::LpSolver lpSolver = storm::settings::generalSettings().getLpSolver();
                switch (lpSolver) {
                    case storm::settings::modules::GeneralSettings::LpSolver::Gurobi: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                    case storm::settings::modules::GeneralSettings::LpSolver::glpk: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                    default: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
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
            
            template class SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicGameSolverFactory<storm::dd::DdType::CUDD>;
            template class LinearEquationSolverFactory<double>;
            template class GmmxxLinearEquationSolverFactory<double>;
            template class NativeLinearEquationSolverFactory<double>;
            template class MinMaxLinearEquationSolverFactory<double>;
            template class GmmxxMinMaxLinearEquationSolverFactory<double>;
            template class NativeMinMaxLinearEquationSolverFactory<double>;
            template class TopologicalMinMaxLinearEquationSolverFactory<double>;
        }
    }
}