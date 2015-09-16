#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_

#include "src/storage/dd/DdType.h"

#include "src/storage/prism/menu_games/AbstractionDdInformation.h"
#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/StateSetAbstractor.h"
#include "src/storage/prism/menu_games/AbstractModule.h"

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace utility {
        namespace solver {
            class SmtSolverFactory;
        }
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
                
                /*!
                 * Uses the current set of predicates to derive the abstract menu game in the form of an ADD.
                 *
                 * @return The ADD representing the game.
                 */
                storm::dd::Add<DdType> getAbstractAdd();
                
                /*!
                 * Refines the abstract module with the given predicates.
                 *
                 * @param predicates The new predicates.
                 */
                void refine(std::vector<storm::expressions::Expression> const& predicates);
                
            private:
                /*!
                 * Computes the reachable states of the transition relation.
                 *
                 * @param initialStates The BDD representing the initial states of the model.
                 * @param transitionRelation The BDD representing the transition relation that does only contain state
                 * and successor variables.
                 * @return The BDD representing the reachable states.
                 */
                storm::dd::Bdd<DdType> getReachableStates(storm::dd::Bdd<DdType> const& initialStates, storm::dd::Bdd<DdType> const& transitionRelation);
                
                // A factory that can be used to create new SMT solvers.
                std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                
                // A struct containing all DD-related information like variables and managers.
                AbstractionDdInformation<DdType, ValueType> ddInformation;
                
                // A struct containing all expression-related information like variables, managers and the predicates.
                AbstractionExpressionInformation expressionInformation;
                
                // The abstract modules of the abstract program.
                std::vector<AbstractModule<DdType, ValueType>> modules;
                
                // The concrete program this abstract program refers to.
                std::reference_wrapper<Program const> program;
                
                // A state-set abstractor used to determine the initial states of the abstraction.
                StateSetAbstractor<DdType, ValueType> initialStateAbstractor;
                
                // An ADD characterizing the probabilities of commands and their updates.
                storm::dd::Add<DdType> commandUpdateProbabilitiesAdd;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTPROGRAM_H_ */