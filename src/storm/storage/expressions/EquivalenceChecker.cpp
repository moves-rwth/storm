#include "storm/storage/expressions/EquivalenceChecker.h"

#include "storm/solver/SmtSolver.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace expressions {
 
        EquivalenceChecker::EquivalenceChecker(std::unique_ptr<storm::solver::SmtSolver>&& smtSolver, storm::expressions::Expression const& constraint) : smtSolver(std::move(smtSolver)) {
            this->smtSolver->add(constraint);
        }
        
        bool EquivalenceChecker::areEquivalent(storm::expressions::Expression const& first, storm::expressions::Expression const& second) {
            this->smtSolver->push();
            this->smtSolver->add((first && !second) || (!first && second));
            bool equivalent = smtSolver->check() == storm::solver::SmtSolver::CheckResult::Unsat;
            this->smtSolver->pop();
            return equivalent;
        }
        
    }
}
