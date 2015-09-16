#ifndef STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_

#include <memory>
#include <set>

#include "src/storage/dd/DdType.h"

#include "src/solver/SmtSolver.h"

#include "src/storage/prism/menu_games/VariablePartition.h"

namespace storm {
    namespace utility {
        namespace solver {
            class SmtSolverFactory;
        }
    }

    namespace dd {
        template <storm::dd::DdType DdType>
        class Bdd;
        
        template <storm::dd::DdType DdType>
        class Add;
    }
    
    namespace prism {
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            class AbstractionDdInformation;
            
            class AbstractionExpressionInformation;
            
            template <storm::dd::DdType DdType, typename ValueType>
            class StateSetAbstractor {
            public:
                /*!
                 * Creates a state set abstractor.
                 *
                 * @param expressionInformation The expression-related information including the manager and the predicates.
                 * @param ddInformation The DD-related information including the manager.
                 * @param smtSolverFactory A factory that can create new SMT solvers.
                 */
                StateSetAbstractor(AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
                /*!
                 * Adds the given (concrete) predicate to the abstractor and therefore restricts the abstraction to
                 * abstract states that contain at least some states satisfying the predicate.
                 */
                void addPredicate(storm::expressions::Expression const& predicate);
                
                /*!
                 * Refines the abstractor by making the given predicates new abstract predicates.
                 *
                 * @param newPredicateIndices The indices of the new predicates.
                 */
                void refine(std::vector<uint_fast64_t> const& newPredicateIndices = std::vector<uint_fast64_t>());
                
                /*!
                 * Retrieves the set of abstract states matching all predicates added to this abstractor.
                 *
                 * @return The set of matching abstract states in the form of a BDD
                 */
                storm::dd::Bdd<DdType> getAbstractStates() const;
                
            private:
                /*!
                 * Creates decision variables and the corresponding constraints for the missing predicates.
                 *
                 * @param newRelevantPredicateIndices The set of all relevant predicate indices.
                 */
                void addMissingPredicates(std::set<uint_fast64_t> const& newRelevantPredicateIndices);
                
                /*!
                 * Recomputes the cached BDD. This needs to be triggered if any relevant predicates change.
                 */
                void recomputeCachedBdd();
                
                /*!
                 * Translates the given model to a state DD.
                 *
                 * @param model The model to translate.
                 * @return The state encoded as a DD.
                 */
                storm::dd::Bdd<DdType> getStateBdd(storm::solver::SmtSolver::ModelReference const& model) const;
                
                // The SMT solver used for abstracting the set of states.
                std::unique_ptr<storm::solver::SmtSolver> smtSolver;
                
                // The expression-related information.
                AbstractionExpressionInformation const& expressionInformation;
                
                // The DD-related information.
                AbstractionDdInformation<DdType, ValueType> const& ddInformation;
                
                // The partition of the variables.
                VariablePartition variablePartition;
                
                // The set of relevant predicates and the corresponding decision variables.
                std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> relevantPredicatesAndVariables;
                
                // The set of all variables appearing in the concrete predicates.
                std::set<storm::expressions::Variable> concretePredicateVariables;
                
                // The set of all decision variables over which to perform the all-sat enumeration.
                std::vector<storm::expressions::Variable> decisionVariables;
                
                // The cached BDD representing the abstraction. This variable is written to in refinement steps (if work
                // needed to be done).
                storm::dd::Bdd<DdType> cachedBdd;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_ */
