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
        
        namespace prism {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractModule {
            public:
                /*!
                 * Constructs an abstract module from the given module and the initial predicates.
                 *
                 * @param module The concrete module for which to build the abstraction.
                 * @param abstractionInformation An object holding information about the abstraction such as predicates and BDDs.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 */
                AbstractModule(storm::prism::Module const& module, AbstractionInformation<DdType>& abstractionInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
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
                std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> getAbstractBdd();
                
                /*!
                 * Retrieves an ADD that maps the encodings of commands and their updates to their probabilities.
                 *
                 * @return The command-update probability ADD.
                 */
                storm::dd::Add<DdType, ValueType> getCommandUpdateProbabilitiesAdd() const;
                
            private:
                // A factory that can be used to create new SMT solvers.
                storm::utility::solver::SmtSolverFactory const& smtSolverFactory;
                
                // The DD-related information.
                AbstractionInformation<DdType> const& abstractionInformation;
                
                // The abstract commands of the abstract module.
                std::vector<AbstractCommand<DdType, ValueType>> commands;
                
                // The concrete module this abstract module refers to.
                std::reference_wrapper<storm::prism::Module const> module;
            };
        }
    }
}
