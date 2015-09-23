#include "src/storage/prism/menu_games/StateSetAbstractor.h"

#include "src/storage/prism/menu_games/AbstractionExpressionInformation.h"
#include "src/storage/prism/menu_games/AbstractionDdInformation.h"

#include "src/storage/dd/CuddDdManager.h"

#include "src/utility/macros.h"
#include "src/utility/solver.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            StateSetAbstractor<DdType, ValueType>::StateSetAbstractor(AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory) : smtSolver(smtSolverFactory.create(expressionInformation.manager)), expressionInformation(expressionInformation), ddInformation(ddInformation), variablePartition(expressionInformation.variables), relevantPredicatesAndVariables(), concretePredicateVariables(), needsRecomputation(false), cachedBdd(ddInformation.manager->getBddZero()) {
                
                // Assert all range expressions to enforce legal variable values.
                for (auto const& rangeExpression : expressionInformation.rangeExpressions) {
                    smtSolver->add(rangeExpression);
                }
                
                // Refine the command based on all initial predicates.
                std::vector<uint_fast64_t> allPredicateIndices(expressionInformation.predicates.size());
                for (auto index = 0; index < expressionInformation.predicates.size(); ++index) {
                    allPredicateIndices[index] = index;
                }
                this->refine(allPredicateIndices);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::addPredicate(storm::expressions::Expression const& predicate) {
                smtSolver->add(predicate);
                
                // Extract the variables of the predicate, so we know which variables were used when abstracting.
                std::set<storm::expressions::Variable> usedVariables = predicate.getVariables();
                concretePredicateVariables.insert(usedVariables.begin(), usedVariables.end());
                variablePartition.relate(usedVariables);

                // Remember that we have to recompute the BDD.
                this->needsRecomputation = true;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::addMissingPredicates(std::set<uint_fast64_t> const& newRelevantPredicateIndices) {
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> newPredicateVariables = AbstractionDdInformation<DdType, ValueType>::declareNewVariables(expressionInformation.manager, relevantPredicatesAndVariables, newRelevantPredicateIndices);
                
                for (auto const& element : newPredicateVariables) {
                    smtSolver->add(storm::expressions::iff(element.first, expressionInformation.predicates[element.second]));
                    decisionVariables.push_back(element.first);
                }
                
                relevantPredicatesAndVariables.insert(relevantPredicatesAndVariables.end(), newPredicateVariables.begin(), newPredicateVariables.end());
                std::sort(relevantPredicatesAndVariables.begin(), relevantPredicatesAndVariables.end(), [] (std::pair<storm::expressions::Variable, uint_fast64_t> const& firstPair, std::pair<storm::expressions::Variable, uint_fast64_t> const& secondPair) { return firstPair.second < secondPair.second; } );
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::refine(std::vector<uint_fast64_t> const& newPredicates) {
                // Make the partition aware of the new predicates, which may make more predicates relevant to the abstraction.
                for (auto const& predicateIndex : newPredicates) {
                    variablePartition.addExpression(expressionInformation.predicates[predicateIndex]);
                }
                
                // Now check whether we need to recompute the cached BDD.
                std::set<uint_fast64_t> newRelevantPredicateIndices = variablePartition.getRelatedExpressions(concretePredicateVariables);
                STORM_LOG_TRACE("Found " << newRelevantPredicateIndices.size() << " relevant predicates in abstractor.");
                
                // Since the number of relevant predicates is monotonic, we can simply check for the size here.
                STORM_LOG_ASSERT(newRelevantPredicateIndices.size() >= relevantPredicatesAndVariables.size(), "Illegal size of relevant predicates.");
                bool recomputeDd = newRelevantPredicateIndices.size() > relevantPredicatesAndVariables.size();
                
                if (recomputeDd) {
                    // If we need to recompute the BDD, we start by introducing decision variables and the corresponding
                    // constraints in the SMT problem.
                    addMissingPredicates(newRelevantPredicateIndices);
                    
                    // Finally recompute the cached BDD.
                    this->recomputeCachedBdd();
                }
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> StateSetAbstractor<DdType, ValueType>::getStateBdd(storm::solver::SmtSolver::ModelReference const& model) const {
                STORM_LOG_TRACE("Building source state BDD.");
                std::cout << "new model: " << std::endl;
                storm::dd::Bdd<DdType> result = ddInformation.manager->getBddOne();
                for (auto const& variableIndexPair : relevantPredicatesAndVariables) {
                    if (model.getBooleanValue(variableIndexPair.first)) {
                        std::cout << expressionInformation.predicates[variableIndexPair.second] << " is true" << std::endl;
                        result &= ddInformation.predicateBdds[variableIndexPair.second].first;
                    } else {
                        std::cout << expressionInformation.predicates[variableIndexPair.second] << " is false" << std::endl;
                        result &= !ddInformation.predicateBdds[variableIndexPair.second].first;
                    }
                }
                return result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void StateSetAbstractor<DdType, ValueType>::recomputeCachedBdd() {
                STORM_LOG_TRACE("Recomputing BDD for state set abstraction.");
                
                storm::dd::Bdd<DdType> result = ddInformation.manager->getBddZero();
                uint_fast64_t modelCounter = 0;
                smtSolver->allSat(decisionVariables, [&result,this,&modelCounter] (storm::solver::SmtSolver::ModelReference const& model) { result |= getStateBdd(model); ++modelCounter; std::cout << "found " << modelCounter << " models" << std::endl; return modelCounter < 10000 ? true : false; } );
                
                cachedBdd = result;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> StateSetAbstractor<DdType, ValueType>::getAbstractStates() {
                if (needsRecomputation) {
                    this->refine();
                }
                return cachedBdd;
            }
            
            template class StateSetAbstractor<storm::dd::DdType::CUDD, double>;
        }
    }
}
