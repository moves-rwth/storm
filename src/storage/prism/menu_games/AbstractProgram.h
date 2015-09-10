#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_

#include "src/storage/dd/DdType.h"

#include "src/storage/prism/menu_games/AbstractModule.h"

#include "src/storage/expressions/Expression.h"

#include "src/utility/solver.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class DdManager;
    }
    
    namespace prism {
        // Forward-declare concrete Program class.
        class Program;
        
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractProgram {
            public:
                /*!
                 * Constructs an abstract program from the given program and the initial predicates.
                 *
                 * @param expressionManager The manager responsible for the expressions of the program.
                 * @param program The concrete program for which to build the abstraction.
                 * @param initialPredicates The initial set of predicates.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 * @param addAllGuards A flag that indicates whether all guards of the program should be added to the initial set of predicates.
                 */
                AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory = std::unique_ptr<storm::utility::solver::SmtSolverFactory>(new storm::utility::solver::SmtSolverFactory()), bool addAllGuards = false);
                
            private:
                // The manager responsible for the expressions of the program and the SMT solvers.
                storm::expressions::ExpressionManager& expressionManager;
                
                // A factory that can be used to create new SMT solvers.
                std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                
                // The current set of predicates used in the abstraction.
                std::vector<storm::expressions::Expression> predicates;
                
                // The manager responsible for the DDs.
                std::shared_ptr<storm::dd::DdManager<DdType>> ddManager;
                
                // The DD variables corresponding to the predicates.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> predicateDdVariables;
                
                // The DD variable encoding the command (i.e., the nondeterministic choices of player 1).
                std::pair<storm::expressions::Variable, storm::expressions::Variable> commandDdVariable;
                
                // The DD variables encoding the nondeterministic choices of player 2.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> optionDdVariables;
                
                // The abstract modules of the abstract program.
                std::vector<AbstractModule<DdType, ValueType>> modules;
                
                // The concrete program this abstract program refers to.
                std::reference_wrapper<Program const> program;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_ */