#include "src/models/symbolic/StochasticTwoPlayerGame.h"


#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

namespace storm {
    namespace models {
        namespace symbolic {
            
            template<storm::dd::DdType Type>
            StochasticTwoPlayerGame<Type>::StochasticTwoPlayerGame(std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                                                   storm::dd::Bdd<Type> reachableStates,
                                                                   storm::dd::Bdd<Type> initialStates,
                                                                   storm::dd::Add<Type> transitionMatrix,
                                                                   std::set<storm::expressions::Variable> const& rowVariables,
                                                                   std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> rowExpressionAdapter,
                                                                   std::set<storm::expressions::Variable> const& columnVariables,
                                                                   std::shared_ptr<storm::adapters::AddExpressionAdapter<Type>> columnExpressionAdapter,
                                                                   std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                                                   std::set<storm::expressions::Variable> const& player1Variables,
                                                                   std::set<storm::expressions::Variable> const& player2Variables,
                                                                   std::set<storm::expressions::Variable> const& nondeterminismVariables,
                                                                   std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                                                   std::unordered_map<std::string, RewardModelType> const& rewardModels)
                : NondeterministicModel<Type>(storm::models::ModelType::S2pg, manager, reachableStates, initialStates, transitionMatrix, rowVariables, rowExpressionAdapter, columnVariables, columnExpressionAdapter, rowColumnMetaVariablePairs, nondeterminismVariables, labelToExpressionMap, rewardModels), player1Variables(player1Variables), player2Variables(player2Variables) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType Type>
            std::set<storm::expressions::Variable> const& StochasticTwoPlayerGame<Type>::getPlayer1Variables() const {
                return player1Variables;
            }
            
            template<storm::dd::DdType Type>
            std::set<storm::expressions::Variable> const& StochasticTwoPlayerGame<Type>::getPlayer2Variables() const {
                return player2Variables;
            }
            
            // Explicitly instantiate the template class.
            template class StochasticTwoPlayerGame<storm::dd::DdType::CUDD>;
            
        } // namespace symbolic
    } // namespace models
} // namespace storm