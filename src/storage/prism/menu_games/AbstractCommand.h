#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_

#include <memory>
#include <set>

#include "src/storage/prism/menu_games/VariablePartition.h"

#include "src/storage/dd/DdType.h"
#include "src/storage/expressions/Expression.h"

#include "src/solver/SmtSolver.h"
#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        // Forward-declare concrete command and assignment classes.
        class Command;
        class Assignment;
        
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractionDdInformation;
            
            class AbstractionExpressionInformation;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractCommand {
            public:
                /*!
                 * Constructs an abstract command from the given command and the initial predicates.
                 *
                 * @param command The concrete command for which to build the abstraction.
                 * @param expressionInformation The expression-related information including the manager and the predicates.
                 * @param ddInformation The DD-related information including the manager.
                 * @param smtSolverFactory A factory that is to be used for creating new SMT solvers.
                 */
                AbstractCommand(storm::prism::Command const& command, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
            private:
                /*!
                 * Determines the relevant predicates for source as well as target states.
                 *
                 * @param assignments The assignments that are to be considered.
                 * @return A pair whose first component represents the relevant source predicates and whose second
                 * component represents the relevant target state predicates.
                 */
                std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> computeRelevantPredicates(std::vector<storm::prism::Assignment> const& assignments) const;
                
                // An SMT responsible for this abstract command.
                std::unique_ptr<storm::solver::SmtSolver> smtSolver;

                // The expression-related information.
                AbstractionExpressionInformation const& expressionInformation;
                
                // The DD-related information.
                AbstractionDdInformation<DdType, ValueType> const& ddInformation;
                
                // The concrete command this abstract command refers to.
                std::reference_wrapper<Command const> command;
                
                // The partition of variables and expressions.
                VariablePartition variablePartition;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_ */