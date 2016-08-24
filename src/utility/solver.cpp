#include "src/utility/solver.h"

#include <vector>

#include "src/solver/SymbolicLinearEquationSolver.h"
#include "src/solver/SymbolicMinMaxLinearEquationSolver.h"
#include "src/solver/SymbolicGameSolver.h"
#include "src/solver/GameSolver.h"

#include "src/solver/TopologicalMinMaxLinearEquationSolver.h"

#include "src/solver/GurobiLpSolver.h"
#include "src/solver/GlpkLpSolver.h"

#include "src/solver/Z3SmtSolver.h"
#include "src/solver/MathsatSmtSolver.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/CoreSettings.h"

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
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> SymbolicGameSolverFactory<Type, ValueType>::create(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalPlayer1Mask, storm::dd::Bdd<Type> const& illegalPlayer2Mask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) const {
                return std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>>(new storm::solver::SymbolicGameSolver<Type, ValueType>(A, allRows, illegalPlayer1Mask, illegalPlayer2Mask, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables, player2Variables));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::GameSolver<ValueType>> GameSolverFactory<ValueType>::create(storm::storage::SparseMatrix<storm::storage::sparse::state_type> const& player1Matrix, storm::storage::SparseMatrix<ValueType> const& player2Matrix) const {
                return std::unique_ptr<storm::solver::GameSolver<ValueType>>(new storm::solver::GameSolver<ValueType>(player1Matrix, player2Matrix));
            }
            
            std::unique_ptr<storm::solver::LpSolver> LpSolverFactory::create(std::string const& name, storm::solver::LpSolverTypeSelection solvT) const {
                storm::solver::LpSolverType t;
                if(solvT == storm::solver::LpSolverTypeSelection::FROMSETTINGS) {
                    t = storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver();
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
                storm::solver::SmtSolverType smtSolverType = storm::settings::getModule<storm::settings::modules::CoreSettings>().getSmtSolver();
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
            template class GameSolverFactory<double>;
        }
    }
}
