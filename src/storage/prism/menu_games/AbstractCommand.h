#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_

#include <memory>
#include <vector>
#include <set>
#include <map>

#include "src/storage/prism/menu_games/LocalExpressionInformation.h"

#include "src/storage/dd/DdType.h"
#include "src/storage/expressions/Expression.h"

#include "src/solver/SmtSolver.h"

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
        // Forward-declare concrete command and assignment classes.
        class Command;
        class Assignment;
        
        namespace menu_games {
            template <storm::dd::DdType DdType, typename ValueType>
            struct AbstractionDdInformation;
            
            struct AbstractionExpressionInformation;
            
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
                AbstractCommand(storm::prism::Command const& command, AbstractionExpressionInformation& expressionInformation, AbstractionDdInformation<DdType, ValueType> const& ddInformation, storm::utility::solver::SmtSolverFactory const& smtSolverFactory);
                
                /*!
                 * Refines the abstract command with the given predicates.
                 *
                 * @param predicates The new predicates.
                 */
                void refine(std::vector<uint_fast64_t> const& predicates);
                
                /*!
                 * Computes the abstraction of the command wrt. to the current set of predicates.
                 *
                 * @return The abstraction of the command in the form of a BDD together with the number of DD variables
                 * used to encode the choices of player 2.
                 */
                std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> getAbstractBdd();
                
                /*!
                 * Retrieves an ADD that maps the encoding of the command and its updates to their probabilities.
                 *
                 * @return The command-update probability ADD.
                 */
                storm::dd::Add<DdType, ValueType> getCommandUpdateProbabilitiesAdd() const;
                
            private:
                /*!
                 * Determines the relevant predicates for source as well as successor states wrt. to the given assignments
                 * (that, for example, form an update).
                 *
                 * @param assignments The assignments that are to be considered.
                 * @return A pair whose first component represents the relevant source predicates and whose second
                 * component represents the relevant successor state predicates.
                 */
                std::pair<std::set<uint_fast64_t>, std::set<uint_fast64_t>> computeRelevantPredicates(std::vector<storm::prism::Assignment> const& assignments) const;
                
                /*!
                 * Determines the relevant predicates for source as well as successor states.
                 *
                 * @return A pair whose first component represents the relevant source predicates and whose second
                 * component represents the relevant successor state predicates.
                 */
                std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> computeRelevantPredicates() const;

                /*!
                 * Checks whether the relevant predicates changed.
                 *
                 * @param newRelevantPredicates The new relevant predicates.
                 */
                bool relevantPredicatesChanged(std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> const& newRelevantPredicates) const;
                                
                /*!
                 * Takes the new relevant predicates and creates the appropriate variables and assertions for the ones
                 * that are currently missing.
                 *
                 * @param newRelevantPredicates The new relevant predicates.
                 */
                void addMissingPredicates(std::pair<std::set<uint_fast64_t>, std::vector<std::set<uint_fast64_t>>> const& newRelevantPredicates);
                
                /*!
                 * Translates the given model to a source state DD.
                 *
                 * @param model The model to translate.
                 * @return The source state encoded as a DD.
                 */
                storm::dd::Bdd<DdType> getSourceStateBdd(storm::solver::SmtSolver::ModelReference const& model) const;

                /*!
                 * Translates the given model to a distribution over successor states.
                 *
                 * @param model The model to translate.
                 * @return The source state encoded as a DD.
                 */
                storm::dd::Bdd<DdType> getDistributionBdd(storm::solver::SmtSolver::ModelReference const& model) const;
                
                /*!
                 * Recomputes the cached BDD. This needs to be triggered if any relevant predicates change.
                 */
                void recomputeCachedBdd();

                /*!
                 * Computes the missing state identities.
                 *
                 * @return A BDD that represents the all missing state identities.
                 */
                storm::dd::Bdd<DdType> computeMissingIdentities() const;
                
                /*!
                 * Computes the missing state identities for the updates.
                 *
                 * @return A BDD that represents the state identities for predicates that are irrelevant for the
                 * successor states.
                 */
                storm::dd::Bdd<DdType> computeMissingUpdateIdentities() const;
                
                /*!
                 * Computes the globally missing state identities.
                 *
                 * @return A BDD that represents the global state identities for predicates that are irrelevant for the
                 * source and successor states.
                 */
                storm::dd::Bdd<DdType> computeMissingGlobalIdentities() const;
                
                // An SMT responsible for this abstract command.
                std::unique_ptr<storm::solver::SmtSolver> smtSolver;

                // The global expression-related information.
                AbstractionExpressionInformation& globalExpressionInformation;
                
                // The DD-related information.
                AbstractionDdInformation<DdType, ValueType> const& ddInformation;
                
                // The concrete command this abstract command refers to.
                std::reference_wrapper<Command const> command;
                
                // The local expression-related information.
                LocalExpressionInformation localExpressionInformation;
                
                // The currently relevant source/successor predicates and the corresponding variables.
                std::pair<std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>>, std::vector<std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>>>> relevantPredicatesAndVariables;
                
                // The most recent result of a call to computeDd. If nothing has changed regarding the relevant
                // predicates, this result may be reused.
                std::pair<storm::dd::Bdd<DdType>, uint_fast64_t> cachedDd;
                
                // All relevant decision variables over which to perform AllSat.
                std::vector<storm::expressions::Variable> decisionVariables;
            };
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTCOMMAND_H_ */