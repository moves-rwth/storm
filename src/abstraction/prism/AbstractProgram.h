#pragma once

#include "src/storage/dd/DdType.h"

#include "src/abstraction/AbstractionDdInformation.h"
#include "src/abstraction/AbstractionExpressionInformation.h"
#include "src/abstraction/StateSetAbstractor.h"
#include "src/abstraction/MenuGame.h"
#include "src/abstraction/prism/AbstractModule.h"

#include "src/storage/expressions/Expression.h"

namespace storm {
    namespace utility {
        namespace solver {
            class SmtSolverFactory;
        }
    }
    
    namespace models {
        namespace symbolic {
            template<storm::dd::DdType Type, typename ValueType>
            class StochasticTwoPlayerGame;
        }
    }
    
    namespace prism {
        // Forward-declare concrete Program class.
        class Program;
    }
    
    namespace abstraction {
        namespace prism {
            
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
                AbstractProgram(storm::expressions::ExpressionManager& expressionManager, storm::prism::Program const& program, std::vector<storm::expressions::Expression> const& initialPredicates, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory = std::make_unique<storm::utility::solver::SmtSolverFactory>(), bool addAllGuards = false);
                
                /*!
                 * Uses the current set of predicates to derive the abstract menu game in the form of an ADD.
                 *
                 * @return The abstract stochastic two player game.
                 */
                MenuGame<DdType, ValueType> getAbstractGame();
                
                /*!
                 * Retrieves the set of states (represented by a BDD) satisfying the given predicate, assuming that it
                 * was either given as an initial predicate or used as a refining predicate later.
                 *
                 * @param predicate The predicate for which to retrieve the states.
                 * @return The BDD representing the set of states.
                 */
                storm::dd::Bdd<DdType> getStates(storm::expressions::Expression const& predicate);
                
                /*!
                 * Refines the abstract program with the given predicates.
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
                
                /*!
                 * Builds the stochastic game representing the abstraction of the program.
                 *
                 * @return The stochastic game.
                 */
                std::unique_ptr<MenuGame<DdType, ValueType>> buildGame();
                
                // A factory that can be used to create new SMT solvers.
                std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                
                // A struct containing all DD-related information like variables and managers.
                AbstractionDdInformation<DdType, ValueType> ddInformation;
                
                // A struct containing all expression-related information like variables, managers and the predicates.
                AbstractionExpressionInformation expressionInformation;
                
                // The abstract modules of the abstract program.
                std::vector<AbstractModule<DdType, ValueType>> modules;
                
                // The concrete program this abstract program refers to.
                std::reference_wrapper<storm::prism::Program const> program;
                
                // A state-set abstractor used to determine the initial states of the abstraction.
                StateSetAbstractor<DdType, ValueType> initialStateAbstractor;
                
                // A flag that stores whether all guards were added (which is relevant for determining the bottom states).
                bool addedAllGuards;

                // A state-set abstractor used to determine the bottom states if not all guards were added.
                StateSetAbstractor<DdType, ValueType> bottomStateAbstractor;
                
                // An ADD characterizing the probabilities of commands and their updates.
                storm::dd::Add<DdType, ValueType> commandUpdateProbabilitiesAdd;
                
                // The current game-based abstraction.
                std::unique_ptr<MenuGame<DdType, ValueType>> currentGame;
            };
        }
    }
}
