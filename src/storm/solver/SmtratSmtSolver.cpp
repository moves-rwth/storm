#include "storm/solver/SmtratSmtSolver.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

#ifdef STORM_HAVE_SMTRAT

#ifdef SMTRATDOESNTWORK  // Does not compile with current version of smtrat.
#include "lib/smtrat.h"

namespace storm {
namespace solver {

SmtratSmtSolver::SmtratSmtSolver(storm::expressions::ExpressionManager& manager) : SmtSolver(manager) {
    // Construct the settingsManager.
    // smtrat::RuntimeSettingsManager settingsManager;

    // Construct solver.
    smtrat::RatOne* solver = new smtrat::RatOne();

    // std::list<std::pair<std::string, smtrat::RuntimeSettings*> > settingsObjects =
    smtrat::addModules(solver);

    // Introduce the settingsObjects from the modules to the manager.
    // settingsManager.addSettingsObject( settingsObjects );
    // settingsObjects.clear();
}

SmtratSmtSolver::~SmtratSmtSolver() {
    delete solver;
}

void SmtratSmtSolver::push() {
    this->solver->push();
}

void SmtratSmtSolver::pop() {
    this->solver->pop();
}

void SmtratSmtSolver::pop(uint_fast64_t n) {
    this->solver->pop(static_cast<unsigned int>(n));
}

SmtSolver::CheckResult SmtratSmtSolver::check() {
    switch (this->solver->check()) {
        case smtrat::Answer::True:
            this->lastResult = SmtSolver::CheckResult::Sat;
            break;
        case smtrat::Answer::False:
            this->lastResult = SmtSolver::CheckResult::Unsat;
            break;
        case smtrat::Answer::Unknown:
            this->lastResult = SmtSolver::CheckResult::Unknown;
            break;
        default:
            // maybe exit
            this->lastResult = SmtSolver::CheckResult::Unknown;
            break;
    }
    return this->lastResult;
}
void SmtratSmtSolver::add(const storm::RawPolynomial& pol, storm::CompareRelation cr) {
    this->solver->add(smtrat::FormulaT(pol, cr));
}

template<>
smtrat::Model SmtratSmtSolver::getModel() const {
    return this->solver->model();
}

std::vector<smtrat::FormulasT> const& SmtratSmtSolver::getUnsatisfiableCores() const {
    return this->solver->infeasibleSubsets();
}

}  // namespace solver
}  // namespace storm
#endif
#endif
