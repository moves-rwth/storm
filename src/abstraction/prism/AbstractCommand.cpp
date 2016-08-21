#include "src/abstraction/prism/AbstractCommand.h"

#include <boost/iterator/transform_iterator.hpp>

#include "src/abstraction/AbstractionInformation.h"
#include "src/abstraction/BottomStateResult.h"

#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Add.h"

#include "src/storage/prism/Command.h"
#include "src/storage/prism/Update.h"

#include "src/utility/solver.h"
#include "src/utility/macros.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractCommand<DdType, ValueType>::AbstractCommand(storm::prism::Command const& command, AbstractionInformation<DdType>& abstractionInformation, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory, bool guardIsPredicate) : smtSolver(smtSolverFactory->create(abstractionInformation.getExpressionManager())), abstractionInformation(abstractionInformation), command(command), localExpressionInformation(abstractionInformation.getVariables()), evaluator(abstractionInformation.getExpressionManager()), relevantPredicatesAndVariables(), cachedDd(abstractionInformation.getDdManager().getBddZero(), 0, 0), decisionVariables(), guardIsPredicate(guardIsPredicate), abstractGuard(abstractionInformation.getDdManager().getBddZero()), bottomStateAbstractor(abstractionInformation, abstractionInformation.getExpressionVariables(), {!command.getGuardExpression()}, smtSolverFactory) {

                // Make the second component of relevant predicates have the right size.
                relevantPredicatesAndVariables.second.resize(command.getNumberOfUpdates());
                
                // Assert all constraints to enforce legal variable values.
                for (auto const& constraint : abstractionInformation.getConstraints()) {
                    smtSolver->add(constraint);
                    bottomStateAbstractor.constrain(constraint);
                }
                
                // Assert the guard of the command.
                smtSolver->add(command.getGuardExpression());

                // Refine the command based on all initial predicates.
                std::vector<uint_fast64_t> allPredicateIndices(abstractionInformation.getNumberOfPredicates());
                for (uint_fast64_t index = 0; index < abstractionInformation.getNumberOfPredicates(); ++index) {
                    allPredicateIndices[index] = index;
                }
                this->refine(allPredicateIndices);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractCommand<DdType, ValueType>::refine(std::vector<uint_fast64_t> const& predicates) {
                // Add all predicates to the variable partition.
                for (auto predicateIndex : predicates) {
                    localExpressionInformation.addExpression(this->getAbstractionInformation().getPredicateByIndex(predicateIndex), predicateIndex);
                }
                
                STORM_LOG_TRACE("Current variable partition is: " << localExpressionInformation);
                
                // Next, we check whether there is work to be done by recomputing the relevant predicates and checking
                // whether they changed.
                std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> newRelevantPredicates = this->computeRelevantPredicates();
                
                // If the DD does not need recomputation, we can return the cached result.
                bool recomputeDd = this->relevantPredicatesChanged(newRelevantPredicates);
                if (!recomputeDd) {
                    // If the new predicates are unrelated to the BDD of this command, we need to multiply their identities.
                    cachedDd.bdd &= computeMissingGlobalIdentities();
                } else {
                    // If the DD needs recomputation, it is because of new relevant predicates, so we need to assert the appropriate clauses in the solver.
                    addMissingPredicates(newRelevantPredicates);
                    
                    // Finally recompute the cached BDD.
                    this->recomputeCachedBdd();
                }
                
                // Refine bottom state abstractor.
                // FIXME: Should this only be done if the other BDD needs recomputation? 
                bottomStateAbstractor.refine(predicates);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractCommand<DdType, ValueType>::recomputeCachedBdd() {
                STORM_LOG_TRACE("Recomputing BDD for command " << command.get());
                
                // Create a mapping from source state DDs to their distributions.
                std::unordered_map<storm::dd::Bdd<DdType>, std::vector<storm::dd::Bdd<DdType>>> sourceToDistributionsMap;
                smtSolver->allSat(decisionVariables, [&sourceToDistributionsMap,this] (storm::solver::SmtSolver::ModelReference const& model) {
                    sourceToDistributionsMap[getSourceStateBdd(model)].push_back(getDistributionBdd(model));
                    return true;
                });
                
                // Now we search for the maximal number of choices of player 2 to determine how many DD variables we
                // need to encode the nondeterminism.
                uint_fast64_t maximalNumberOfChoices = 0;
                for (auto const& sourceDistributionsPair : sourceToDistributionsMap) {
                    maximalNumberOfChoices = std::max(maximalNumberOfChoices, static_cast<uint_fast64_t>(sourceDistributionsPair.second.size()));
                }
                
                // We now compute how many variables we need to encode the choices. We add one to the maximal number of
                // choices to account for a possible transition to a bottom state.
                uint_fast64_t numberOfVariablesNeeded = static_cast<uint_fast64_t>(std::ceil(std::log2(maximalNumberOfChoices + 1)));
                
                // Finally, build overall result.
                storm::dd::Bdd<DdType> resultBdd = this->getAbstractionInformation().getDdManager().getBddZero();
                if (!guardIsPredicate) {
                    abstractGuard = this->getAbstractionInformation().getDdManager().getBddZero();
                }
                uint_fast64_t sourceStateIndex = 0;
                for (auto const& sourceDistributionsPair : sourceToDistributionsMap) {
                    if (!guardIsPredicate) {
                        abstractGuard |= sourceDistributionsPair.first;
                    }
                    
                    STORM_LOG_ASSERT(!sourceDistributionsPair.first.isZero(), "The source BDD must not be empty.");
                    STORM_LOG_ASSERT(!sourceDistributionsPair.second.empty(), "The distributions must not be empty.");
                    storm::dd::Bdd<DdType> allDistributions = this->getAbstractionInformation().getDdManager().getBddZero();
                    uint_fast64_t distributionIndex = 0;
                    for (auto const& distribution : sourceDistributionsPair.second) {
                        allDistributions |= distribution && this->getAbstractionInformation().encodePlayer2Choice(distributionIndex, numberOfVariablesNeeded);
                        ++distributionIndex;
                        STORM_LOG_ASSERT(!allDistributions.isZero(), "The BDD must not be empty.");
                    }
                    resultBdd |= sourceDistributionsPair.first && allDistributions;
                    ++sourceStateIndex;
                    STORM_LOG_ASSERT(!resultBdd.isZero(), "The BDD must not be empty.");
                }
                
                resultBdd &= computeMissingIdentities();
                resultBdd &= this->getAbstractionInformation().encodePlayer1Choice(command.get().getGlobalIndex(), this->getAbstractionInformation().getPlayer1VariableCount());
                STORM_LOG_ASSERT(sourceToDistributionsMap.empty() || !resultBdd.isZero(), "The BDD must not be empty, if there were distributions.");
                
                // Cache the result.
                cachedDd = GameBddResult<DdType>(resultBdd, numberOfVariablesNeeded, maximalNumberOfChoices);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> AbstractCommand<DdType, ValueType>::computeRelevantPredicates(std::vector<storm::prism::Assignment> const& assignments) const {
                std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> result;
                
                std::set<storm::expressions::Variable> assignedVariables;
                for (auto const& assignment : assignments) {
                    // Also, variables appearing on the right-hand side of an assignment are relevant for source state.
                    auto const& rightHandSidePredicates = localExpressionInformation.getExpressionsUsingVariables(assignment.getExpression().getVariables());
                    result.first.insert(rightHandSidePredicates.begin(), rightHandSidePredicates.end());
                    
                    // Variables that are being assigned are relevant for the successor state.
                    storm::expressions::Variable const& assignedVariable = assignment.getVariable();
                    auto const& leftHandSidePredicates = localExpressionInformation.getExpressionsUsingVariable(assignedVariable);
                    result.second.insert(leftHandSidePredicates.begin(), leftHandSidePredicates.end());
                    
                    // Keep track of all assigned variables, so we can find the related predicates later.
                    assignedVariables.insert(assignedVariable);
                }
                
                auto const& predicatesRelatedToAssignedVariable = localExpressionInformation.getRelatedExpressions(assignedVariables);
                
                result.first.insert(predicatesRelatedToAssignedVariable.begin(), predicatesRelatedToAssignedVariable.end());
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> AbstractCommand<DdType, ValueType>::computeRelevantPredicates() const {
                std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> result;

                // To start with, all predicates related to the guard are relevant source predicates.
                result.first = localExpressionInformation.getExpressionsUsingVariables(command.get().getGuardExpression().getVariables());
                
                // Then, we add the predicates that become relevant, because of some update.
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
            void AbstractCommand<DdType, ValueType>::addMissingPredicates(std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> const& newRelevantPredicates) {
                // Determine and add new relevant source predicates.
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newSourceVariables = this->getAbstractionInformation().declareNewVariables(relevantPredicatesAndVariables.first, newRelevantPredicates.first);
                for (auto const& element : newSourceVariables) {
                    smtSolver->add(storm::expressions::iff(element.first, this->getAbstractionInformation().getPredicateByIndex(element.second)));
                    decisionVariables.push_back(element.first);
                }
                
                // Insert the new variables into the record of relevant source variables.
                relevantPredicatesAndVariables.first.insert(relevantPredicatesAndVariables.first.end(), newSourceVariables.begin(), newSourceVariables.end());
                std::sort(relevantPredicatesAndVariables.first.begin(), relevantPredicatesAndVariables.first.end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& first, std::pair<storm::expressions::Variable, uint_fast64_t> const& second) { return first.second < second.second; } );
                
                // Do the same for every update.
                for (uint_fast64_t index = 0; index < command.get().getNumberOfUpdates(); ++index) {
                    std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newSuccessorVariables = this->getAbstractionInformation().declareNewVariables(relevantPredicatesAndVariables.second[index], newRelevantPredicates.second[index]);
                    for (auto const& element : newSuccessorVariables) {
                        smtSolver->add(storm::expressions::iff(element.first, this->getAbstractionInformation().getPredicateByIndex(element.second).substitute(command.get().getUpdate(index).getAsVariableToExpressionMap())));
                        decisionVariables.push_back(element.first);
                    }
                    
                    relevantPredicatesAndVariables.second[index].insert(relevantPredicatesAndVariables.second[index].end(), newSuccessorVariables.begin(), newSuccessorVariables.end());
                    std::sort(relevantPredicatesAndVariables.second[index].begin(), relevantPredicatesAndVariables.second[index].end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& first, std::pair<storm::expressions::Variable, uint_fast64_t> const& second) { return first.second < second.second; } );
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractCommand<DdType, ValueType>::getSourceStateBdd(storm::solver::SmtSolver::ModelReference const& model) const {
                STORM_LOG_TRACE("Building source state BDD.");
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddOne();
                for (auto const& variableIndexPair : relevantPredicatesAndVariables.first) {
                    if (model.getBooleanValue(variableIndexPair.first)) {
                        result &= this->getAbstractionInformation().encodePredicateAsSource(variableIndexPair.second);
                    } else {
                        result &= !this->getAbstractionInformation().encodePredicateAsSource(variableIndexPair.second);
                    }
                }
                
                STORM_LOG_ASSERT(!result.isZero(), "Source must not be empty.");
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractCommand<DdType, ValueType>::getDistributionBdd(storm::solver::SmtSolver::ModelReference const& model) const {
                STORM_LOG_TRACE("Building distribution BDD.");
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddZero();
                
                for (uint_fast64_t updateIndex = 0; updateIndex < command.get().getNumberOfUpdates(); ++updateIndex) {
                    storm::dd::Bdd<DdType> updateBdd = this->getAbstractionInformation().getDdManager().getBddOne();

                    // Translate block variables for this update into a successor block.
                    for (auto const& variableIndexPair : relevantPredicatesAndVariables.second[updateIndex]) {
                        if (model.getBooleanValue(variableIndexPair.first)) {
                            updateBdd &= this->getAbstractionInformation().encodePredicateAsSuccessor(variableIndexPair.second);
                        } else {
                            updateBdd &= !this->getAbstractionInformation().encodePredicateAsSuccessor(variableIndexPair.second);
                        }
                        updateBdd &= this->getAbstractionInformation().encodeAux(updateIndex, 0, this->getAbstractionInformation().getAuxVariableCount());
                    }
                    
                    result |= updateBdd;
                }
                
                STORM_LOG_ASSERT(!result.isZero(), "Distribution must not be empty.");
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractCommand<DdType, ValueType>::computeMissingIdentities() const {
                storm::dd::Bdd<DdType> identities = computeMissingGlobalIdentities();
                identities &= computeMissingUpdateIdentities();
                return identities;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractCommand<DdType, ValueType>::computeMissingUpdateIdentities() const {
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddZero();
                for (uint_fast64_t updateIndex = 0; updateIndex < command.get().getNumberOfUpdates(); ++updateIndex) {
                    // Compute the identities that are missing for this update.
                    auto firstIt = relevantPredicatesAndVariables.first.begin();
                    auto firstIte = relevantPredicatesAndVariables.first.end();
                    auto secondIt = relevantPredicatesAndVariables.second[updateIndex].begin();
                    auto secondIte = relevantPredicatesAndVariables.second[updateIndex].end();
                    
                    // Go through all relevant source predicates. This is guaranteed to be a superset of the set of
                    // relevant successor predicates for any update.
                    storm::dd::Bdd<DdType> updateIdentity = this->getAbstractionInformation().getDdManager().getBddOne();
                    for (; firstIt != firstIte; ++firstIt) {
                        // If the predicates do not match, there is a predicate missing, so we need to add its identity.
                        if (secondIt == secondIte || firstIt->second != secondIt->second) {
                            updateIdentity &= this->getAbstractionInformation().getPredicateIdentity(firstIt->second);
                        } else if (secondIt != secondIte) {
                            ++secondIt;
                        }
                    }
                    
                    result |= updateIdentity && this->getAbstractionInformation().encodeAux(updateIndex, 0, this->getAbstractionInformation().getAuxVariableCount());
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> AbstractCommand<DdType, ValueType>::computeMissingGlobalIdentities() const {
                auto relevantIt = relevantPredicatesAndVariables.first.begin();
                auto relevantIte = relevantPredicatesAndVariables.first.end();
                
                storm::dd::Bdd<DdType> result = this->getAbstractionInformation().getDdManager().getBddOne();
                for (uint_fast64_t predicateIndex = 0; predicateIndex < this->getAbstractionInformation().getNumberOfPredicates(); ++predicateIndex) {
                    if (relevantIt == relevantIte || relevantIt->second != predicateIndex) {
                        result &= this->getAbstractionInformation().getPredicateIdentity(predicateIndex);
                    } else {
                        ++relevantIt;
                    }
                }
                return result;
            }

            template <storm::dd::DdType DdType, typename ValueType>
            GameBddResult<DdType> AbstractCommand<DdType, ValueType>::getAbstractBdd() {
                return cachedDd;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            BottomStateResult<DdType> AbstractCommand<DdType, ValueType>::getBottomStateTransitions(storm::dd::Bdd<DdType> const& reachableStates, uint_fast64_t numberOfPlayer2Variables) {
                BottomStateResult<DdType> result(this->getAbstractionInformation().getDdManager().getBddZero(), this->getAbstractionInformation().getDdManager().getBddZero());

                // Use the state abstractor to compute the set of abstract states that has this command enabled but
                // still has a transition to a bottom state.
                bottomStateAbstractor.constrain(reachableStates && abstractGuard);
                result.states = bottomStateAbstractor.getAbstractStates();
                
                // Now equip all these states with an actual transition to a bottom state.
                result.transitions = result.states && this->getAbstractionInformation().getAllPredicateIdentities() && this->getAbstractionInformation().getBottomStateBdd(false, false);
                
                // Mark the states as bottom states.
                result.states &= this->getAbstractionInformation().getBottomStateBdd(true, false);
                
                // Add the command encoding and the next free player 2 encoding.
                result.transitions &= this->getAbstractionInformation().encodePlayer1Choice(command.get().getGlobalIndex(), this->getAbstractionInformation().getPlayer1VariableCount()) && this->getAbstractionInformation().encodePlayer2Choice(cachedDd.nextFreePlayer2Index, cachedDd.numberOfPlayer2Variables) && this->getAbstractionInformation().encodeAux(0, 0, this->getAbstractionInformation().getAuxVariableCount());
                
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Add<DdType, ValueType> AbstractCommand<DdType, ValueType>::getCommandUpdateProbabilitiesAdd() const {
                storm::dd::Add<DdType, ValueType> result = this->getAbstractionInformation().getDdManager().template getAddZero<ValueType>();
                for (uint_fast64_t updateIndex = 0; updateIndex < command.get().getNumberOfUpdates(); ++updateIndex) {
                    result += this->getAbstractionInformation().encodeAux(updateIndex, 0, this->getAbstractionInformation().getAuxVariableCount()).template toAdd<ValueType>() * this->getAbstractionInformation().getDdManager().getConstant(evaluator.asRational(command.get().getUpdate(updateIndex).getLikelihoodExpression()));
                }
                result *= this->getAbstractionInformation().encodePlayer1Choice(command.get().getGlobalIndex(), this->getAbstractionInformation().getPlayer1VariableCount()).template toAdd<ValueType>();
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::prism::Command const& AbstractCommand<DdType, ValueType>::getConcreteCommand() const {
                return command.get();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void AbstractCommand<DdType, ValueType>::notifyGuardIsPredicate() {
                guardIsPredicate = true;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& AbstractCommand<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation.get();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType>& AbstractCommand<DdType, ValueType>::getAbstractionInformation() {
                return abstractionInformation.get();
            }
            
            template class AbstractCommand<storm::dd::DdType::CUDD, double>;
            template class AbstractCommand<storm::dd::DdType::Sylvan, double>;
        }
    }
}