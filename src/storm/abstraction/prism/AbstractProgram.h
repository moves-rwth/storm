#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/AbstractionInformation.h"
#include "storm/abstraction/MenuGame.h"
#include "storm/abstraction/prism/AbstractModule.h"

#include "storm/storage/dd/Add.h"

#include "storm/storage/expressions/Expression.h"

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
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 * @param addAllGuards A flag that indicates whether all guards of the program should be added to the initial set of predicates.
                 */
                AbstractProgram(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), bool addAllGuards = false);
                
                AbstractProgram(AbstractProgram const&) = default;
                AbstractProgram& operator=(AbstractProgram const&) = default;
                AbstractProgram(AbstractProgram&&) = default;
                AbstractProgram& operator=(AbstractProgram&&) = default;
                
                /*!
                 * Uses the current set of predicates to derive the abstract menu game in the form of an ADD.
                 *
                 * @return The abstract stochastic two player game.
                 */
                MenuGame<DdType, ValueType> abstract();
                
                /*!
                 * Retrieves information about the abstraction.
                 *
                 * @return The abstraction information object.
                 */
                AbstractionInformation<DdType> const& getAbstractionInformation() const;
                
                /*!
                 * Retrieves the guard predicate of the given player 1 choice.
                 *
                 * @param player1Choice The choice for which to retrieve the guard.
                 * @return The guard of the player 1 choice.
                 */
                storm::expressions::Expression const& getGuard(uint64_t player1Choice) const;
                
                /*!
                 * Retrieves a mapping from variables to expressions that define their updates wrt. to the given player
                 * 1 choice and auxiliary choice.
                 */
                std::map<storm::expressions::Variable, storm::expressions::Expression> getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const;
                
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
                
                /*!
                 * Exports the current state of the abstraction in the dot format to the given file.
                 *
                 * @param filename The name of the file to which to write the dot output.
                 * @param highlightStates A BDD characterizing states that will be highlighted.
                 * @param filter A filter that is applied to select which part of the game to export.
                 */
                void exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const;
                
            private:                
                /*!
                 * Builds the stochastic game representing the abstraction of the program.
                 *
                 * @return The stochastic game.
                 */
                std::unique_ptr<MenuGame<DdType, ValueType>> buildGame();
                
                /*!
                 * Decodes the given choice over the auxiliary and successor variables to a mapping from update indices
                 * to bit vectors representing the successors under these updates.
                 *
                 * @param choice The choice to decode.
                 */
                std::map<uint_fast64_t, storm::storage::BitVector> decodeChoiceToUpdateSuccessorMapping(storm::dd::Bdd<DdType> const& choice) const;
                
                // The concrete program this abstract program refers to.
                std::reference_wrapper<storm::prism::Program const> program;

                // A factory that can be used to create new SMT solvers.
                std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                
                // An object containing all information about the abstraction like predicates and the corresponding DDs.
                AbstractionInformation<DdType> abstractionInformation;
                
                // The abstract modules of the abstract program.
                std::vector<AbstractModule<DdType, ValueType>> modules;
                
                // A state-set abstractor used to determine the initial states of the abstraction.
                StateSetAbstractor<DdType, ValueType> initialStateAbstractor;
                                
                // A flag that stores whether all guards were added (which is relevant for determining the bottom states).
                bool addedAllGuards;
                
                // An ADD characterizing the probabilities of commands and their updates.
                storm::dd::Add<DdType, ValueType> commandUpdateProbabilitiesAdd;
                
                // The current game-based abstraction.
                std::unique_ptr<MenuGame<DdType, ValueType>> currentGame;
                
                // A flag storing whether a refinement was performed.
                bool refinementPerformed;
            };
        }
    }
}
