#include "src/storage/prism/menu_games/AbstractCommand.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/prism/Command.h"
#include "src/storage/prism/Update.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractCommand<DdType, ValueType>::AbstractCommand(storm::prism::Command const& command, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolver(smtSolverFactory.create(expressionInformation.expressionManager)), expressionInformation(expressionInformation), ddInformation(ddInformation), command(command) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> AbstractCommand<DdType, ValueType>::computeRelevantPredicates(std::vector<storm::prism::Assignment> const& assignments) const {
                std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> result;
                
                // To start with, all predicates related to the guard are relevant source predicates.
                result.first = variablePartition.getExpressionsUsingVariables(command.get().getGuardExpression().getVariables());
                
                std::set<storm::expressions::Variable> assignedVariables;
                for (auto const& assignment : assignments) {
                    // Also, variables appearing on the right-hand side of an assignment are relevant for source state.
                    auto const& rightHandSidePredicates = variablePartition.getExpressionsUsingVariables(assignment.getExpression().getVariables());
                    result.first.insert(rightHandSidePredicates.begin(), rightHandSidePredicates.end());
                    
                    // Variables that are being assigned are relevant for the target state.
                    storm::expressions::Variable const& assignedVariable = assignment.getVariable();
                    auto const& leftHandSidePredicates = variablePartition.getExpressionsUsingVariable(assignedVariable);
                    result.second.insert(leftHandSidePredicates.begin(), leftHandSidePredicates.end());
                    
                    // Keep track of all assigned variables, so we can find the related predicates later.
                    assignedVariables.insert(assignedVariable);
                }
                
                auto const& predicatesRelatedToAssignedVariable = variablePartition.getRelatedExpressions(assignedVariables);
                result.first.insert(predicatesRelatedToAssignedVariable.begin(), predicatesRelatedToAssignedVariable.end());
                
                return result;
            }
            
            template class AbstractCommand<storm::dd::DdType::CUDD, double>;
        }
    }
}