#include "src/storage/prism/menu_games/StateSetAbstractor.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/dd/DdManager.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            StateSetAbstractor<DdType, ValueType>::StateSetAbstractor(AbstractionExpressionInformation& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, std::vector<storm::expressions::Expression> const& statePredicates, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolver(smtSolverFactory.create(expressionInformation.getManager())), expressionInformation(expressionInformation), ddInformation(ddInformation), variablePartition(expressionInformation.getVariables()), relevantPredicatesAndVariables(), concretePredicateVariables(), needsRecomputation(false), cachedBdd(ddInformation.manager->getBddZero()), constraint(ddInformation.manager->getBddOne()) {
                
                // Assert all range expressions to enforce legal variable values.
                for (auto const& rangeExpression : expressionInformation.getRangeExpressions()) {
                    smtSolver->add(rangeExpression);
                }
                
                // Assert all state predicates.
                for (auto const& predicate : statePredicates) {
                    smtSolver->add(predicate);
                    
                    // Extract the variables of the predicate, so we know which variables were used when abstracting.
                    std::set<storm::expressions::Variable> usedVariables = predicate.getVariables();
                    concretePredicateVariables.insert(usedVariables.begin(), usedVariables.end());
                    variablePartition.relate(usedVariables);
                }
                
                // Refine the command based on all initial predicates.
                std::vector<uint_fast64_t> allPredicateIndices(expressionInformation.getPredicates().size());
                for (auto index = 0; index < expressionInformation.getPredicates().size(); ++index) {
                    allPredicateIndices[index] = index;
                }
                this->refine(allPredicateIndices);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::addMissingPredicates(std::set<uint_fast64_t> const& newRelevantPredicateIndices) {
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newPredicateVariables = AbstractionDdInformation<DdType, ValueType>::declareNewVariables(expressionInformation.getManager(), relevantPredicatesAndVariables, newRelevantPredicateIndices);
                
                for (auto const& element : newPredicateVariables) {
                    smtSolver->add(storm::expressions::iff(element.first, expressionInformation.getPredicates()[element.second]));
                    decisionVariables.push_back(element.first);
                }
                
                relevantPredicatesAndVariables.insert(relevantPredicatesAndVariables.end(), newPredicateVariables.begin(), newPredicateVariables.end());
                std::sort(relevantPredicatesAndVariables.begin(), relevantPredicatesAndVariables.end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& firstPair, std::pair<storm::expressions::Variable, uint_fast64_t> const& secondPair) { return firstPair.second < secondPair.second; } );
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::refine(std::vector<uint_fast64_t> const& newPredicates) {
                // Make the partition aware of the new predicates, which may make more predicates relevant to the abstraction.
                for (auto const& predicateIndex : newPredicates) {
                    variablePartition.addExpression(expressionInformation.getPredicates()[predicateIndex]);
                }
                needsRecomputation = true;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::constrain(storm::dd::Bdd<DdType> const& newConstraint) {
                // If the constraint is different from the last one, we add it to the solver.
                if (newConstraint != this->constraint) {
                    constraint = newConstraint;
                    this->pushConstraintBdd();
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> StateSetAbstractor<DdType, ValueType>::getStateBdd(storm::solver::SmtSolver::ModelReference const& model) const {
                STORM_LOG_TRACE("Building source state BDD.");
                storm::dd::Bdd<DdType> result = ddInformation.manager->getBddOne();
                for (auto const& variableIndexPair : relevantPredicatesAndVariables) {
                    if (model.getBooleanValue(variableIndexPair.first)) {
                        result &= ddInformation.predicateBdds[variableIndexPair.second].first;
                    } else {
                        result &= !ddInformation.predicateBdds[variableIndexPair.second].first;
                    }
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::recomputeCachedBdd() {
                // Now check whether we need to recompute the cached BDD.
                std::set<uint_fast64_t> newRelevantPredicateIndices = variablePartition.getRelatedExpressions(concretePredicateVariables);
                STORM_LOG_TRACE("Found " << newRelevantPredicateIndices.size() << " relevant predicates in abstractor.");
                
                // Since the number of relevant predicates is monotonic, we can simply check for the size here.
                STORM_LOG_ASSERT(newRelevantPredicateIndices.size() >= relevantPredicatesAndVariables.size(), "Illegal size of relevant predicates.");
                bool recomputeBdd = newRelevantPredicateIndices.size() > relevantPredicatesAndVariables.size();
                
                if (!recomputeBdd) {
                    return;
                }
                
                // Before adding the missing predicates, we need to remove the constraint BDD.
                this->popConstraintBdd();
                
                // If we need to recompute the BDD, we start by introducing decision variables and the corresponding
                // constraints in the SMT problem.
                addMissingPredicates(newRelevantPredicateIndices);
                
                // Then re-add the constraint BDD.
                this->pushConstraintBdd();
                
                STORM_LOG_TRACE("Recomputing BDD for state set abstraction.");
                
                storm::dd::Bdd<DdType> result = ddInformation.manager->getBddZero();
                uint_fast64_t modelCounter = 0;
                smtSolver->allSat(decisionVariables, [&result,this,&modelCounter] (storm::solver::SmtSolver::ModelReference const& model) { result |= getStateBdd(model); return true; } );
                
                cachedBdd = result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::popConstraintBdd() {
                // If the last constraint was not the constant one BDD, we need to pop the constraint from the solver.
                if (this->constraint.isOne()) {
                    return;
                }
                smtSolver->pop();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::pushConstraintBdd() {
                if (this->constraint.isOne()) {
                    return;
                }
                
                // Create a new backtracking point before adding the constraint.
                smtSolver->push();
                
                // Then add the constraint.
                std::pair<std::vector<storm::expressions::Expression>, std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, storm::expressions::Variable>> result = constraint.toExpression(expressionInformation.getManager(), ddInformation.bddVariableIndexToPredicateMap);
                
                for (auto const& expression : result.first) {
                    smtSolver->add(expression);
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> StateSetAbstractor<DdType, ValueType>::getAbstractStates() {
                if (needsRecomputation) {
                    this->recomputeCachedBdd();
                }
                return cachedBdd;
            }
            
            template class StateSetAbstractor<storm::dd::DdType::CUDD, double>;
        }
    }
}
