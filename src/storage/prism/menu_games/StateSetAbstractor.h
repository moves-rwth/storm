#ifndef STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_

#include <memory>
#include <set>
#include <boost/optional.hpp>

#include "src/utility/OsDetection.h"

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
        
        template <storm::dd::DdType DdType, typename ValueType>
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
                // Provide a no-op default constructor.
                StateSetAbstractor() = default;

                StateSetAbstractor(StateSetAbstractor const& other) = default;
                StateSetAbstractor& operator=(StateSetAbstractor const& other) = default;
                
#ifndef WINDOWS
                StateSetAbstractor(StateSetAbstractor&& other) = default;
                StateSetAbstractor& operator=(StateSetAbstractor&& other) = default;
#endif
                
                /*!
                 * Creates a state set abstractor.
                 *
                 * @param expressionInformation The expression-related information including the manager and the predicates.
                 * @param ddInformation The DD-related information including the manager.
                 * @param statePredicates A set of predicates that have to hold in the concrete states this abstractor is
                 * supposed to abstract.
                 * @param smtSolverFactory A factory that can create new SMT solvers.
                 */
                StateSetAbstractor(AbstractionExpressionInformation const& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, std::vector<storm::expressions::Expression> const& statePredicates, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
                /*!
                 * Refines the abstractor by making the given predicates new abstract predicates.
                 *
                 * @param newPredicateIndices The indices of the new predicates.
                 */
                void refine(std::vector<uint_fast64_t> const& newPredicateIndices);
                
                /*!
                 * Constraints the abstract states with the given BDD.
                 *
                 * @param newConstraint The BDD used as the constraint.
                 */
                void constrain(storm::dd::Bdd<DdType> const& newConstraint);
                
                /*!
                 * Retrieves the set of abstract states matching all predicates added to this abstractor.
                 *
                 * @return The set of matching abstract states in the form of a BDD
                 */
                storm::dd::Bdd<DdType> getAbstractStates();
                
            private:
                /*!
                 * Creates decision variables and the corresponding constraints for the missing predicates.
                 *
                 * @param newRelevantPredicateIndices The set of all relevant predicate indices.
                 */
                void addMissingPredicates(std::set<uint_fast64_t> const& newRelevantPredicateIndices);
                
                /*!
                 * Adds the current constraint BDD to the solver.
                 */
                void pushConstraintBdd();

                /*!
                 * Removes the current constraint BDD (if any) from the solver.
                 */
                void popConstraintBdd();
                
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
                
                // A flag indicating whether the cached BDD needs recomputation.
                bool needsRecomputation;
                
                // The cached BDD representing the abstraction. This variable is written to in refinement steps (if work
                // needed to be done).
                storm::dd::Bdd<DdType> cachedBdd;
                
                // This BDD currently constrains the search for solutions.
                storm::dd::Bdd<DdType> constraint;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_STATESETABSTRACTOR_H_ */
