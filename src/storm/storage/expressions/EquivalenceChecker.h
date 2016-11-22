#pragma once

#include <memory>

#include "storm/solver/SmtSolver.h"

namespace storm {
    namespace expressions {
        class Expression;
 
        class EquivalenceChecker {
        public:
            /*!
             * Creates an equivalence checker with the given solver and constraint.
             *
             * @param smtSolver The solver to use.
             * @param constraint An additional constraint. Must be satisfiable.
             */
            EquivalenceChecker(std::unique_ptr<storm::solver::SmtSolver>&& smtSolver, storm::expressions::Expression const& constraint);
            
            bool areEquivalent(storm::expressions::Expression const& first, storm::expressions::Expression const& second);
            
        private:
            std::unique_ptr<storm::solver::SmtSolver> smtSolver;
        };
        
    }
}
