#ifndef STORM_PRISM_MENU_GAMES_MENUGAME_H_
#define STORM_PRISM_MENU_GAMES_MENUGAME_H_

#include <map>

#include "src/models/symbolic/StochasticTwoPlayerGame.h"

#include "src/utility/OsDetection.h"

namespace storm {
    namespace prism {
        namespace menu_games {
            
            /*!
             * This class represents a discrete-time stochastic two-player game.
             */
            template<storm::dd::DdType Type>
            class MenuGame : public storm::models::symbolic::StochasticTwoPlayerGame<Type> {
            public:
                typedef typename storm::models::symbolic::StochasticTwoPlayerGame<Type>::RewardModelType RewardModelType;
                
                MenuGame(MenuGame<Type> const& other) = default;
                MenuGame& operator=(MenuGame<Type> const& other) = default;
                
#ifndef WINDOWS
                MenuGame(MenuGame<Type>&& other) = default;
                MenuGame& operator=(MenuGame<Type>&& other) = default;
#endif
                
                /*!
                 * Constructs a model from the given data.
                 *
                 * @param manager The manager responsible for the decision diagrams.
                 * @param reachableStates A DD representing the reachable states.
                 * @param initialStates A DD representing the initial states of the model.
                 * @param transitionMatrix The matrix representing the transitions in the model.
                 * @param rowVariables The set of row meta variables used in the DDs.
                 * @param columVariables The set of column meta variables used in the DDs.
                 * @param rowColumnMetaVariablePairs All pairs of row/column meta variables.
                 * @param player1Variables The meta variables used to encode the nondeterministic choices of player 1.
                 * @param player2Variables The meta variables used to encode the nondeterministic choices of player 2.
                 * @param allNondeterminismVariables The meta variables used to encode the nondeterminism in the model.
                 * @param updateVariable The variable used to encode the different updates of the probabilistic program.
                 * @param expressionToBddMap A mapping from expressions (used) in the abstraction to the BDDs encoding
                 * them.
                 */
                MenuGame(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                         storm::dd::Bdd<Type> reachableStates,
                         storm::dd::Bdd<Type> initialStates,
                         storm::dd::Add<Type> transitionMatrix,
                         std::set<storm::expressions::Variable> const& rowVariables,
                         std::set<storm::expressions::Variable> const& columnVariables,
                         std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                         std::set<storm::expressions::Variable> const& player1Variables,
                         std::set<storm::expressions::Variable> const& player2Variables,
                         std::set<storm::expressions::Variable> const& allNondeterminismVariables,
                         storm::expressions::Variable const& updateVariable,
                         std::map<storm::expressions::Expression, storm::dd::Bdd<Type>> const& expressionToBddMap);
                
                virtual storm::dd::Bdd<Type> getStates(std::string const& label) const override;
                
                virtual storm::dd::Bdd<Type> getStates(storm::expressions::Expression const& expression) const override;
                
                virtual bool hasLabel(std::string const& label) const override;
                
            private:
                // The meta variable used to encode the updates.
                storm::expressions::Variable updateVariable;
                
                // A mapping from expressions that were used in the abstraction process to the the BDDs representing them.
                std::map<storm::expressions::Expression, storm::dd::Bdd<Type>> expressionToBddMap;
            };
            
        } // namespace menu_games
    } // namespace prism
} // namespace storm

#endif /* STORM_PRISM_MENU_GAMES_MENUGAME_H_ */
