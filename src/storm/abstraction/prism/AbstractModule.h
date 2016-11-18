#pragma once

#include "src/storage/dd/DdType.h"

#include "src/abstraction/prism/AbstractCommand.h"

#include "src/storage/expressions/Expression.h"

#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        // Forward-declare concrete module class.
        class Module;
    }
    
    namespace abstraction {
        template <storm::dd::DdType DdType>
        class AbstractionInformation;

        template<storm::dd::DdType DdType>
        struct BottomStateResult;
        
        namespace prism {
            template<storm::dd::DdType DdType>
            struct GameBddResult;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractModule {
            public:
                /*!
                 * Constructs an abstract module from the given module and the initial predicates.
                 *
                 * @param module The concrete module for which to build the abstraction.
                 * @param abstractionInformation An object holding information about the abstraction such as predicates and BDDs.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 * @param allGuardsAdded A flag indicating whether all guards of the program were added.
                 */
                AbstractModule(storm::prism::Module const& module, AbstractionInformation<DdType>& abstractionInformation, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>(), bool allGuardsAdded = false);
                
                AbstractModule(AbstractModule const&) = default;
                AbstractModule& operator=(AbstractModule const&) = default;
                AbstractModule(AbstractModule&&) = default;
                AbstractModule& operator=(AbstractModule&&) = default;
                
                /*!
                 * Refines the abstract module with the given predicates.
                 *
                 * @param predicates The new predicate indices.
                 */
                void refine(std::vector<uint_fast64_t> const& predicates);
                
                /*!
                 * Computes the abstraction of the module wrt. to the current set of predicates.
                 *
                 * @return The abstraction of the module in the form of a BDD together with how many option variables were used.
                 */
                GameBddResult<DdType> getAbstractBdd();
                
                /*!
                 * Retrieves the transitions to bottom states of this module.
                 *
                 * @param reachableStates A BDD representing the reachable states.
                 * @param numberOfPlayer2Variables The number of variables used to encode the choices of player 2.
                 * @return The bottom states and the necessary transitions.
                 */
                BottomStateResult<DdType> getBottomStateTransitions(storm::dd::Bdd<DdType> const& reachableStates, uint_fast64_t numberOfPlayer2Variables);
                
                /*!
                 * Retrieves an ADD that maps the encodings of commands and their updates to their probabilities.
                 *
                 * @return The command-update probability ADD.
                 */
                storm::dd::Add<DdType, ValueType> getCommandUpdateProbabilitiesAdd() const;
                
                /*!
                 * Retrieves the abstract commands of this abstract module.
                 *
                 * @return The abstract commands.
                 */
                std::vector<AbstractCommand<DdType, ValueType>> const& getCommands() const;

                /*!
                 * Retrieves the abstract commands of this abstract module.
                 *
                 * @return The abstract commands.
                 */
                std::vector<AbstractCommand<DdType, ValueType>>& getCommands();

            private:
                /*!
                 * Retrieves the abstraction information.
                 *
                 * @return The abstraction information.
                 */
                AbstractionInformation<DdType> const& getAbstractionInformation() const;
                
                // A factory that can be used to create new SMT solvers.
                std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
                
                // The DD-related information.
                std::reference_wrapper<AbstractionInformation<DdType> const> abstractionInformation;
                
                // The abstract commands of the abstract module.
                std::vector<AbstractCommand<DdType, ValueType>> commands;
                
                // The concrete module this abstract module refers to.
                std::reference_wrapper<storm::prism::Module const> module;
            };
        }
    }
}
