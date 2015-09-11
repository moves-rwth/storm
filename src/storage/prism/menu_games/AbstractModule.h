#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTMODULE_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTMODULE_H_

#include "src/storage/dd/DdType.h"

#include "src/storage/prism/menu_games/AbstractCommand.h"

#include "src/storage/expressions/Expression.h"

#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        // Forward-declare concrete module class.
        class Module;
        
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractionDdInformation;
            
            class AbstractionExpressionInformation;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractModule {
            public:
                /*!
                 * Constructs an abstract module from the given module and the initial predicates.
                 *
                 * @param module The concrete module for which to build the abstraction.
                 * @param expressionInformation The expression-related information including the manager and the predicates.
                 * @param ddInformation The DD-related information including the manager.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 */
                AbstractModule(storm::prism::Module const& module, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
            private:
                // A factory that can be used to create new SMT solvers.
                storm::utility::solver::SmtSolverFactory const& smtSolverFactory;
                
                // The current set of predicates used in the abstraction.
                std::vector<storm::expressions::Expression> predicates;
                
                // The abstract commands of the abstract module.
                std::vector<AbstractCommand<DdType, ValueType>> commands;
                
                // The concrete module this abstract module refers to.
                std::reference_wrapper<Module const> module;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTMODULE_H_ */