#include "src/storage/prism/menu_games/AbstractCommand.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"

#include "src/storage/prism/Command.h"
#include "src/storage/prism/Update.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractCommand<DdType, ValueType>::AbstractCommand(storm::prism::Command const& command, AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolver(smtSolverFactory.create(expressionInformation.expressionManager)), expressionInformation(expressionInformation), ddInformation(ddInformation), command(command), variablePartition(expressionInformation.variables), relevantPredicatesAndVariables(), cachedDd(std::make_pair(ddInformation.ddManager->getAddZero(), 0)) {

                // Make the second component of relevant predicates have the right size.
                relevantPredicatesAndVariables.second.resize(command.getNumberOfUpdates());
                
                // Assert all range expressions to enforce legal variable values.
                for (auto const& rangeExpression : expressionInformation.rangeExpressions) {
                    smtSolver->add(rangeExpression);
                }
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
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> AbstractCommand<DdType, ValueType>::computeRelevantPredicates() const {
                std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> result;

                for (auto const& update : command.get().getUpdates()) {
                    std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> relevantUpdatePredicates = computeRelevantPredicates(update.getAssignments());
                    result.first.insert(relevantUpdatePredicates.first.begin(), relevantUpdatePredicates.first.end());
                    result.second.push_back(relevantUpdatePredicates.second);
                }
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            bool AbstractCommand<DdType, ValueType>::relevantPredicatesChanged(std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> const& newRelevantPredicates) const {
                if (newRelevantPredicates.first.size() > relevantPredicatesAndVariables.first.size()) {
                    return true;
                }
                
                for (uint_fast64_t index = 0; index < command.get().getNumberOfUpdates(); ++index) {
                    if (newRelevantPredicates.second[index].size() > relevantPredicatesAndVariables.second[index].size()) {
                        return true;
                    }
                }
                
                return false;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> AbstractCommand<DdType, ValueType>::declareNewVariables(std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> const& oldRelevantPredicates, std::set<uint_fast64_t> const& newRelevantPredicates) {
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> result;
                
                auto oldIt = oldRelevantPredicates.begin();
                auto oldIte = oldRelevantPredicates.end();
                for (auto newIt = newRelevantPredicates.begin(), newIte = newRelevantPredicates.end(); newIt != newIte; ++newIt) {
                    // If the new variable does not yet exist as a source variable, we create it now.
                    if (oldIt == oldIte || oldIt->second != *newIt) {
                        result.push_back(std::make_pair(expressionInformation.expressionManager.declareFreshBooleanVariable(), *newIt));
                    }
                }
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractCommand<DdType, ValueType>::addMissingPredicates(std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> const& newRelevantPredicates) {
                // Determine and add new relevant source predicates.
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newSourceVariables = declareNewVariables(relevantPredicatesAndVariables.first, newRelevantPredicates.first);
                for (auto const& element : newSourceVariables) {
                    smtSolver->add(storm::expressions::iff(element.first, expressionInformation.predicates[element.second]));
                }
                
                // Insert the new variables into the record of relevant source variables.
                relevantPredicatesAndVariables.first.insert(relevantPredicatesAndVariables.first.end(), newSourceVariables.begin(), newSourceVariables.end());
                std::sort(relevantPredicatesAndVariables.first.begin(), relevantPredicatesAndVariables.first.end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& first, std::pair<storm::expressions::Variable, uint_fast64_t> const& second) { return first.second < second.second; } );
                
                // Do the same for every update.
                for (uint_fast64_t index = 0; index < command.get().getNumberOfUpdates(); ++index) {
                    std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newTargetVariables = declareNewVariables(relevantPredicatesAndVariables.second[index], newRelevantPredicates.second[index]);
                    for (auto const& element : newSourceVariables) {
                        smtSolver->add(storm::expressions::iff(element.first, expressionInformation.predicates[element.second].substitute(command.get().getUpdate(index).getAsVariableToExpressionMap())));
                    }
                    
                    relevantPredicatesAndVariables.second[index].insert(relevantPredicatesAndVariables.second[index].end(), newTargetVariables.begin(), newTargetVariables.end());
                    std::sort(relevantPredicatesAndVariables.second[index].begin(), relevantPredicatesAndVariables.second[index].end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& first, std::pair<storm::expressions::Variable, uint_fast64_t> const& second) { return first.second < second.second; } );
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<storm::dd::Add<DdType>, uint_fast64_t> AbstractCommand<DdType, ValueType>::computeDd() {
                // First, we check whether there is work to be done by recomputing the relevant predicates and checking
                // whether they changed.
                std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> newRelevantPredicates;
                bool recomputeDd = this->relevantPredicatesChanged(newRelevantPredicates);
                
                if (recomputeDd) {
                    relevantPredicates = std::move(newRelevantPredicates);
                    
                    
                    
                    storm::dd::Add<DdType> result;
                    
                    return result;
                } else {
                    return cachedDd;
                }
            }
            
            template class AbstractCommand<storm::dd::DdType::CUDD, double>;
        }
    }
}