#include "src/utility/solver.h"

#include <vector>

#include "src/solver/SymbolicLinearEquationSolver.h"
#include "src/solver/SymbolicMinMaxLinearEquationSolver.h"
#include "src/solver/SymbolicGameSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/EigenLinearEquationSolver.h"
#include "src/solver/EliminationLinearEquationSolver.h"
#include "src/solver/GameSolver.h"

#include "src/solver/NativeMinMaxLinearEquationSolver.h"
#include "src/solver/GmmxxMinMaxLinearEquationSolver.h"
#include "src/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

#include "src/solver/Z3SmtSolver.h"
#include "src/solver/MathsatSmtSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/MarkovChainSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<Type, ValueType>> SymbolicLinearEquationSolverFactory<Type, ValueType>::create(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
                return std::unique_ptr<storm::solver::SymbolicLinearEquationSolver<Type, ValueType>>(new storm::solver::SymbolicLinearEquationSolver<Type, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs));
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>> SymbolicMinMaxLinearEquationSolverFactory<Type, ValueType>::create(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
                return std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>>(new storm::solver::SymbolicMinMaxLinearEquationSolver<Type, ValueType>(A, allRows, illegalMask, rowMetaVariables, columnMetaVariables, choiceVariables, rowColumnMetaVariablePairs));
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> SymbolicGameSolverFactory<Type, ValueType>::create(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) const {
                return std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>>(new storm::solver::SymbolicGameSolver<Type, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables, player2Variables));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> LinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                storm::solver::EquationSolverType equationSolver = storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().getEquationSolver();
                switch (equationSolver) {
                    case storm::solver::EquationSolverType::Gmmxx: return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(matrix);
                    case storm::solver::EquationSolverType::Native: return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(matrix);
                    case storm::solver::EquationSolverType::Eigen: return std::make_unique<storm::solver::EigenLinearEquationSolver<ValueType>>(matrix);
                    case storm::solver::EquationSolverType::Elimination: return std::make_unique<storm::solver::EliminationLinearEquationSolver<ValueType>>(matrix);
                    default: return std::make_unique<storm::solver::GmmxxLinearEquationSolver<ValueType>>(matrix);
                }
            }
            
            std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> LinearEquationSolverFactory<storm::RationalFunction>::create(storm::storage::SparseMatrix<storm::RationalFunction> const& matrix) const {
                return std::make_unique<storm::solver::EigenLinearEquationSolver<storm::RationalFunction>>(matrix);
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> GmmxxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::GmmxxLinearEquationSolver<ValueType>(matrix));
            }

            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> EigenLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::EigenLinearEquationSolver<ValueType>(matrix));
            }
            
            template<typename ValueType>
            NativeLinearEquationSolverFactory<ValueType>::NativeLinearEquationSolverFactory() {
                switch (storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getLinearEquationSystemMethod()) {
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi:
                        this->method = storm::solver::NativeLinearEquationSolverSolutionMethod::Jacobi;
                        break;
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel:
                        this->method = storm::solver::NativeLinearEquationSolverSolutionMethod::GaussSeidel;
                        break;
                    case settings::modules::NativeEquationSolverSettings::LinearEquationMethod::SOR:
                        this->method = storm::solver::NativeLinearEquationSolverSolutionMethod::SOR;
                        break;
                }
                omega = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getOmega();
            }
            
            template<typename ValueType>
            NativeLinearEquationSolverFactory<ValueType>::NativeLinearEquationSolverFactory(typename storm::solver::NativeLinearEquationSolverSolutionMethod method, ValueType omega) : method(method), omega(omega) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
                return std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>>(new storm::solver::NativeLinearEquationSolver<ValueType>(matrix, method));
            }
            
            template<typename ValueType>
            MinMaxLinearEquationSolverFactory<ValueType>::MinMaxLinearEquationSolverFactory(storm::solver::EquationSolverTypeSelection solver)
            {
                prefTech = storm::solver::MinMaxTechniqueSelection::FROMSETTINGS;
                setSolverType(solver);
            }
            
            template<typename ValueType>
            MinMaxLinearEquationSolverFactory<ValueType>& MinMaxLinearEquationSolverFactory<ValueType>::setSolverType(storm::solver::EquationSolverTypeSelection solverTypeSel) {
                if(solverTypeSel == storm::solver::EquationSolverTypeSelection::FROMSETTINGS) {
                    this->solverType = storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().getEquationSolver();
                } else {
                    this->solverType = storm::solver::convert(solverTypeSel);
                }
                return *this;
            }
            
            template<typename ValueType>
            MinMaxLinearEquationSolverFactory<ValueType>& MinMaxLinearEquationSolverFactory<ValueType>::setPreferredTechnique(storm::solver::MinMaxTechniqueSelection preferredTech) {
                this->prefTech = preferredTech;
                return *this;
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix, bool trackScheduler) const {
                std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> p1;
                
                switch (solverType) {
                    case storm::solver::EquationSolverType::Gmmxx:
                    {
                        p1.reset(new storm::solver::GmmxxMinMaxLinearEquationSolver<ValueType>(matrix, this->prefTech));
                        break;
                    }
                    case storm::solver::EquationSolverType::Native:
                    {
                        p1.reset(new storm::solver::NativeMinMaxLinearEquationSolver<ValueType>(matrix, this->prefTech));
                        break;
                    }
                    case storm::solver::EquationSolverType::Topological:
                    {
                        STORM_LOG_THROW(prefTech != storm::solver::MinMaxTechniqueSelection::PolicyIteration, storm::exceptions::NotImplementedException, "Policy iteration for topological solver is not supported.");
                        p1.reset(new storm::solver::TopologicalMinMaxLinearEquationSolver<ValueType>(matrix));
                        break;
                    }
                }
                p1->setTrackScheduler(trackScheduler);
                return p1;
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::GameSolver<ValueType>> GameSolverFactory<ValueType>::create(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) const {
                return std::unique_ptr<storm::solver::GameSolver<ValueType>>(new storm::solver::GameSolver<ValueType>(player1Matrix, player2Matrix));
            }
            
            std::unique_ptr<storm::solver::LpSolver> LpSolverFactory::create(std::string const& name, storm::solver::LpSolverTypeSelection solvT) const {
                storm::solver::LpSolverType t;
                if(solvT == storm::solver::LpSolverTypeSelection::FROMSETTINGS) {
                    t = storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().getLpSolver();
                } else {
                    t = convert(solvT);
                }
                switch (t) {
                    case storm::solver::LpSolverType::Gurobi: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GurobiLpSolver(name));
                    case storm::solver::LpSolverType::Glpk: return std::unique_ptr<storm::solver::LpSolver>(new storm::solver::GlpkLpSolver(name));
                }
            }
            
            std::unique_ptr<storm::solver::LpSolver> LpSolverFactory::create(std::string const& name) const {
                return LpSolverFactory::create(name, storm::solver::LpSolverTypeSelection::FROMSETTINGS);
            }
            
            std::unique_ptr<storm::solver::LpSolver> GlpkLpSolverFactory::create(std::string const& name) const {
                return LpSolverFactory::create(name, storm::solver::LpSolverTypeSelection::Glpk);
            }
            
            std::unique_ptr<storm::solver::LpSolver> GurobiLpSolverFactory::create(std::string const& name) const {
                return LpSolverFactory::create(name, storm::solver::LpSolverTypeSelection::Gurobi);
            }
            
            std::unique_ptr<storm::solver::LpSolver> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType) {
                std::unique_ptr<storm::utility::solver::LpSolverFactory> factory(new LpSolverFactory());
                return factory->create(name, solvType);
            }
            
            std::unique_ptr<storm::solver::SmtSolver> SmtSolverFactory::create(storm::expressions::ExpressionManager& manager) const {
                storm::solver::SmtSolverType smtSolverType = storm::settings::getModule<storm::settings::modules::MarkovChainSettings>().getSmtSolver();
                switch (smtSolverType) {
                    case storm::solver::SmtSolverType::Z3: return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::Z3SmtSolver(manager));
                    case storm::solver::SmtSolverType::Mathsat: return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::MathsatSmtSolver(manager));
                }
            }
            
            std::unique_ptr<storm::solver::SmtSolver> Z3SmtSolverFactory::create(storm::expressions::ExpressionManager& manager) const {
                return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::Z3SmtSolver(manager));
            }
            
            std::unique_ptr<storm::solver::SmtSolver> MathsatSmtSolverFactory::create(storm::expressions::ExpressionManager& manager) const {
                return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::MathsatSmtSolver(manager));
            }
            
            std::unique_ptr<storm::solver::SmtSolver> getSmtSolver(storm::expressions::ExpressionManager& manager) {
                std::unique_ptr<storm::utility::solver::SmtSolverFactory> factory(new MathsatSmtSolverFactory());
                return factory->create(manager);
            }
            
            template class SymbolicLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>;
            template class SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>;
            template class SymbolicGameSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicGameSolverFactory<storm::dd::DdType::Sylvan, double>;
            template class LinearEquationSolverFactory<double>;
            template class GmmxxLinearEquationSolverFactory<double>;
            template class EigenLinearEquationSolverFactory<double>;
            template class NativeLinearEquationSolverFactory<double>;
            template class MinMaxLinearEquationSolverFactory<double>;
            template class GameSolverFactory<double>;

            template class EigenLinearEquationSolverFactory<storm::RationalFunction>;
        }
    }
}
