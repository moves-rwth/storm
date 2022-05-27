#include "storm/utility/solver.h"

#include <vector>

#include "storm/solver/SymbolicEliminationLinearEquationSolver.h"
#include "storm/solver/SymbolicGameSolver.h"
#include "storm/solver/SymbolicNativeLinearEquationSolver.h"

#include "storm/solver/GlpkLpSolver.h"
#include "storm/solver/GurobiLpSolver.h"
#include "storm/solver/Z3LpSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/solver/MathsatSmtSolver.h"
#include "storm/solver/Z3SmtSolver.h"
#include "storm/utility/NumberTraits.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
namespace utility {
namespace solver {

template<typename ValueType>
std::unique_ptr<storm::solver::LpSolver<ValueType>> GlpkLpSolverFactory<ValueType>::create(std::string const& name) const {
    return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::GlpkLpSolver<ValueType>(name));
}

template<typename ValueType>
std::unique_ptr<LpSolverFactory<ValueType>> GlpkLpSolverFactory<ValueType>::clone() const {
    return std::make_unique<GlpkLpSolverFactory<ValueType>>(*this);
}

template<typename ValueType>
std::unique_ptr<storm::solver::LpSolver<ValueType>> GurobiLpSolverFactory<ValueType>::create(std::string const& name) const {
    return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::GurobiLpSolver<ValueType>(name));
}

template<typename ValueType>
std::unique_ptr<LpSolverFactory<ValueType>> GurobiLpSolverFactory<ValueType>::clone() const {
    return std::make_unique<GurobiLpSolverFactory<ValueType>>(*this);
}

template<typename ValueType>
std::unique_ptr<storm::solver::LpSolver<ValueType>> Z3LpSolverFactory<ValueType>::create(std::string const& name) const {
    return std::unique_ptr<storm::solver::LpSolver<ValueType>>(new storm::solver::Z3LpSolver<ValueType>(name));
}

template<typename ValueType>
std::unique_ptr<LpSolverFactory<ValueType>> Z3LpSolverFactory<ValueType>::clone() const {
    return std::make_unique<Z3LpSolverFactory<ValueType>>(*this);
}

template<typename ValueType>
std::unique_ptr<LpSolverFactory<ValueType>> getLpSolverFactory(storm::solver::LpSolverTypeSelection solvType) {
    storm::solver::LpSolverType t;
    if (solvType == storm::solver::LpSolverTypeSelection::FROMSETTINGS) {
        t = storm::settings::getModule<storm::settings::modules::CoreSettings>().getLpSolver();
        bool useExact =
            storm::NumberTraits<ValueType>::IsExact || storm::settings::getModule<storm::settings::modules::GeneralSettings>().isExactFinitePrecisionSet();
        if (useExact && t != storm::solver::LpSolverType::Z3 &&
            storm::settings::getModule<storm::settings::modules::CoreSettings>().isLpSolverSetFromDefaultValue()) {
            t = storm::solver::LpSolverType::Z3;
        }
    } else {
        t = convert(solvType);
    }
    switch (t) {
        case storm::solver::LpSolverType::Gurobi:
            return std::unique_ptr<LpSolverFactory<ValueType>>(new GurobiLpSolverFactory<ValueType>());
        case storm::solver::LpSolverType::Glpk:
            return std::unique_ptr<LpSolverFactory<ValueType>>(new GlpkLpSolverFactory<ValueType>());
        case storm::solver::LpSolverType::Z3:
            return std::unique_ptr<LpSolverFactory<ValueType>>(new Z3LpSolverFactory<ValueType>());
    }
    return nullptr;

}

template<typename ValueType>
std::unique_ptr<storm::solver::LpSolver<ValueType>> getLpSolver(std::string const& name, storm::solver::LpSolverTypeSelection solvType) {
    std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>> factory = getLpSolverFactory<ValueType>(solvType);
    return factory->create(name);
}

std::unique_ptr<storm::solver::SmtSolver> SmtSolverFactory::create(storm::expressions::ExpressionManager& manager) const {
    storm::solver::SmtSolverType smtSolverType;
    if (storm::settings::hasModule<storm::settings::modules::CoreSettings>()) {
        smtSolverType = storm::settings::getModule<storm::settings::modules::CoreSettings>().getSmtSolver();
    } else {
#ifdef STORM_HAVE_Z3
        smtSolverType = storm::solver::SmtSolverType::Z3;
#elif STORM_HAVE_MSAT
        smtSolverType = storm::solver::SmtSolverType::Mathsat;
#else
        STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Requested an SMT solver but none was installed.");
#endif
    }
    switch (smtSolverType) {
        case storm::solver::SmtSolverType::Z3:
            return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::Z3SmtSolver(manager));
        case storm::solver::SmtSolverType::Mathsat:
            return std::unique_ptr<storm::solver::SmtSolver>(new storm::solver::MathsatSmtSolver(manager));
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
    std::unique_ptr<storm::utility::solver::SmtSolverFactory> factory(new SmtSolverFactory());
    return factory->create(manager);
}

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
}  // namespace solver
}  // namespace utility
}  // namespace storm
