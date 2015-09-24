#ifndef STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_
#define STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_

#include <memory>
#include <vector>
#include <set>
#include <map>

#include "src/storage/dd/DdType.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class DdManager;

        template <storm::dd::DdType DdType>
        class Bdd;
    }
    
    namespace expressions {
        class Expression;
    }

    namespace prism {
        namespace menu_games {
            
            template <storm::dd::DdType DdType, typename ValueType>
            struct AbstractionDdInformation {
            public:
                /*!
                 * Creates a new DdInformation that uses the given manager.
                 *
                 * @param manager The manager to use.
                 * @param initialPredicates The initially considered predicates.
                 */
                AbstractionDdInformation(std::shared_ptr<storm::dd::DdManager<DdType>> const& manager, std::vector<storm::expressions::Expression> const& initialPredicates = std::vector<storm::expressions::Expression>());
                
                /*!
                 * Encodes the given distribution index by using the given number of variables from the optionDdVariables
                 * vector.
                 *
                 * @param numberOfVariables The number of variables to use.
                 * @param distributionIndex The distribution index to encode.
                 * @return The encoded distribution index.
                 */
                storm::dd::Bdd<DdType> encodeDistributionIndex(uint_fast64_t numberOfVariables, uint_fast64_t distributionIndex) const;
                
                /*!
                 * Adds the given predicate and creates all associated ressources.
                 *
                 * @param predicate The predicate to add.
                 */
                void addPredicate(storm::expressions::Expression const& predicate);
                
                /*!
                 * Retrieves the cube of option variables in the range [begin, end) the given indices.
                 *
                 * @param begin The first variable of the range to return.
                 * @param end One past the last variable of the range to return.
                 * @return The cube of variables in the given range.
                 */
                storm::dd::Bdd<DdType> getMissingOptionVariableCube(uint_fast64_t begin, uint_fast64_t end) const;
                
                /*!
                 * Examines the old and new relevant predicates and declares decision variables for the missing relevant
                 * predicates.
                 *
                 * @param manager The manager in which to declare the decision variable.
                 * @param oldRelevantPredicates The previously relevant predicates.
                 * @param newRelevantPredicates The new relevant predicates.
                 * @return Pairs of decision variables and their index for the missing predicates.
                 */
                static std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> declareNewVariables(storm::expressions::ExpressionManager& manager, std::vector<std::pair<storm::expressions::Variable, uint_fast64_t>> const& oldRelevantPredicates, std::set<uint_fast64_t> const& newRelevantPredicates);
                
                // The manager responsible for the DDs.
                std::shared_ptr<storm::dd::DdManager<DdType>> manager;
                
                // The DD variables corresponding to the predicates.
                std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> predicateDdVariables;
                
                // The set of all source variables.
                std::set<storm::expressions::Variable> sourceVariables;

                // The set of all source variables.
                std::set<storm::expressions::Variable> successorVariables;

                // The BDDs corresponding to the predicates.
                std::vector<std::pair<storm::dd::Bdd<DdType>, storm::dd::Bdd<DdType>>> predicateBdds;
                
                // The BDDs representing the predicate identities (i.e. source and successor variable have the same truth value).
                std::vector<storm::dd::Bdd<DdType>> predicateIdentities;
                
                // A BDD that represents the identity of all predicate variables.
                storm::dd::Bdd<DdType> allPredicateIdentities;
                
                // The DD variable encoding the command (i.e., the nondeterministic choices of player 1).
                storm::expressions::Variable commandDdVariable;
                
                // The DD variable encoding the update IDs for all actions.
                storm::expressions::Variable updateDdVariable;
                
                // The DD variables encoding the nondeterministic choices of player 2.
                std::vector<std::pair<storm::expressions::Variable, storm::dd::Bdd<DdType>>> optionDdVariables;
                
                // A mapping from the predicates to the BDDs.
                std::map<storm::expressions::Expression, storm::dd::Bdd<DdType>> expressionToBddMap;
            };
            
        }
    }
}

#endif /* STORM_STORAGE_PRISM_MENU_GAMES_ABSTRACTIONDDINFORMATION_H_ */
