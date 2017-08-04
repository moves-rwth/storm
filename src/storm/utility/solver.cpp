#include "storm/utility/solver.h"

#include <vector>

#include "storm/solver/SymbolicNativeLinearEquationSolver.h"
#include "storm/solver/SymbolicEliminationLinearEquationSolver.h"
#include "storm/solver/SymbolicGameSolver.h"


#include "storm/solver/GurobiLpSolver.h"
#include "storm/solver/Z3LpSolver.h"
#include "storm/solver/GlpkLpSolver.h"

#include "storm/solver/Z3SmtSolver.h"
#include "storm/solver/MathsatSmtSolver.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace utility {
        namespace solver {
            
            template<storm::dd::DdType Type, typename ValueType>
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> SymbolicGameSolverFactory<Type, ValueType>::create(storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, storm::dd::Bdd<Type> const& illegalPlayer1Mask, storm::dd::Bdd<Type> const& illegalPlayer2Mask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables) const {
                return std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>>(new storm::solver::SymbolicGameSolver<Type, ValueType>(A, allRows, illegalPlayer1Mask, illegalPlayer2Mask, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables, player2Variables));
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> LpSolverFactory<ValueType>::create(std::string const& name, storm::solver::LpSolverTypeSelection solvT) const {
                storm::solver::LpSolverType t;
                if(solvT == storm::solver::LpSolverTypeSelection::FROMSETTINGS) {
                    t = storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver();
                } else {
                    t = convert(solvT);
                }
                switch (t) {
                    case storm::solver::LpSolverType::Gurobi: return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::GurobiLpSolver<ValueType>(name));
                    case storm::solver::LpSolverType::Glpk: return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::GlpkLpSolver<ValueType>(name));
                    case storm::solver::LpSolverType::Z3: return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::Z3LpSolver<ValueType>(name));
                }
                return nullptr;
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> LpSolverFactory<ValueType>::create(std::string const& name) const {
                return LpSolverFactory<ValueType>::create(name, storm::solver::LpSolverTypeSelection::FROMSETTINGS);
            }
            
            template<typename ValueType>
            std::unique_ptr<LpSolverFactory<ValueType>> LpSolverFactory<ValueType>::clone() const {
                return std::make_unique<LpSolverFactory<ValueType>>(*this);
            }
  
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> GlpkLpSolverFactory<ValueType>::create(std::string const& name) const {
                return LpSolverFactory<ValueType>::create(name, storm::solver::LpSolverTypeSelection::Glpk);
            }
            
            template<typename ValueType>
            std::unique_ptr<LpSolverFactory<ValueType>> GlpkLpSolverFactory<ValueType>::clone() const {
                return std::make_unique<GlpkLpSolverFactory<ValueType>>(*this);
            }
            
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> GurobiLpSolverFactory<ValueType>::create(std::string const& name) const {
                return LpSolverFactory<ValueType>::create(name, storm::solver::LpSolverTypeSelection::Gurobi);
            }
     
            template<typename ValueType>
            std::unique_ptr<LpSolverFactory<ValueType>> GurobiLpSolverFactory<ValueType>::clone() const {
                return std::make_unique<GurobiLpSolverFactory<ValueType>>(*this);
            }
    
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> Z3LpSolverFactory<ValueType>::create(std::string const& name) const {
                return LpSolverFactory<ValueType>::create(name, storm::solver::LpSolverTypeSelection::Z3);
            }
            
            template<typename ValueType>
            std::unique_ptr<LpSolverFactory<ValueType>> Z3LpSolverFactory<ValueType>::clone() const {
                return std::make_unique<Z3LpSolverFactory<ValueType>>(*this);
            }
    
            template<typename ValueType>
            std::unique_ptr<storm::solver::LpSolver<ValueType>> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType) {
                std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> factory(new LpSolverFactory<ValueType>());
                return factory->create(name, solvType);
            }
            
            std::unique_ptr<storm::solver::SmtSolver> SmtSolverFactory::create(storm::expressions::ExpressionManager& manager) const {
                storm::solver::SmtSolverType smtSolverType = storm::settings::getModule<storm::settings::modules::CoreSettings>().getSmtSolver();
                switch (smtSolverType) {
                    case storm::solver::SmtSolverType::Z3: return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::Z3SmtSolver(manager));
                    case storm::solver::SmtSolverType::Mathsat: return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::MathsatSmtSolver(manager));
                }
                return nullptr;
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
            
            template class SymbolicGameSolverFactory<storm::dd::DdType::CUDD, double>;
            template class SymbolicGameSolverFactory<storm::dd::DdType::Sylvan, double>;
            template class LpSolverFactory<double>;
            template class LpSolverFactory<storm::RationalNumber>;
            template class GlpkLpSolverFactory<double>;
            template class GlpkLpSolverFactory<storm::RationalNumber>;
            template class GurobiLpSolverFactory<double>;
            template class GurobiLpSolverFactory<storm::RationalNumber>;
            template class Z3LpSolverFactory<double>;
            template class Z3LpSolverFactory<storm::RationalNumber>;
            template std::unique_ptr<storm::solver::LpSolver<double>> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType);
            template std::unique_ptr<storm::solver::LpSolver<storm::RationalNumber>> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType);
        }
    }
}
