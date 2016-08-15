#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/abstraction/prism/PrismMenuGameAbstractor.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/graph.h"
#include "src/utility/macros.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidPropertyException.h"

#include "src/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType>
        GameBasedMdpModelChecker<Type, ValueType>::GameBasedMdpModelChecker(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : originalProgram(program), smtSolverFactory(smtSolverFactory) {
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only DTMCs/MDPs are supported by the game-based model checker.");
                        
            // Start by preparing the program. That is, we flatten the modules if there is more than one.
            if (originalProgram.getNumberOfModules() > 1) {
                preprocessedProgram = originalProgram.flattenModules(this->smtSolverFactory);
            } else {
                preprocessedProgram = originalProgram;
            }
        }

        template<storm::dd::DdType Type, typename ValueType>
        GameBasedMdpModelChecker<Type, ValueType>::~GameBasedMdpModelChecker() {
            // Intentionally left empty.
        }

        template<storm::dd::DdType Type, typename ValueType>
        bool GameBasedMdpModelChecker<Type, ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::reachability();
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
                
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            return performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula>(pathFormula), getExpression(pathFormula.getLeftSubformula()), getExpression(pathFormula.getRightSubformula()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), originalProgram.getManager().boolean(true), getExpression(pathFormula.getSubformula()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidPropertyException, "The game-based abstraction refinement model checker can only compute the result for the initial states.");

            // Set up initial predicates.
            std::vector<storm::expressions::Expression> initialPredicates;
            initialPredicates.push_back(targetStateExpression);
            if (!constraintExpression.isTrue() && !constraintExpression.isFalse()) {
                initialPredicates.push_back(constraintExpression);
            }
            
            // Derive the optimization direction for player 1 (assuming menu-game abstraction).
            storm::OptimizationDirection player1Direction = checkTask.isOptimizationDirectionSet() ? checkTask.getOptimizationDirection() : storm::OptimizationDirection::Maximize;
            
            storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType> abstractor(preprocessedProgram, initialPredicates, smtSolverFactory);
            
            
            // 1. build initial abstraction based on the the constraint expression (if not 'true') and the target state expression.
            storm::abstraction::MenuGame<Type, ValueType> game = abstractor.abstract();
            STORM_LOG_DEBUG("Initial abstraction has " << game.getNumberOfStates() << " (player 1) states and " << game.getNumberOfTransitions() << " transitions.");
            
            // 2. solve the game wrt. to min/max as given by checkTask and min/max for the abstraction player to obtain two bounds.
            // Note that we have to deal with bottom states if not all guards were added in the beginning.
            // Also note that it might be the case that not both bounds need to be computed if there is a bound given in checkTask.
            computeProb01States(player1Direction, game, constraintExpression, targetStateExpression);
            
//            std::unique_ptr<storm::utility::solver::SymbolicGameSolverFactory<Type, ValueType>> gameSolverFactory = std::make_unique<storm::utility::solver::SymbolicGameSolverFactory<Type, ValueType>>();
//            gameSolverFactory->create(game.getTransitionMatrix(), game.getReachableStates());
//            storm::dd::Add<Type, ValueType> const& A, storm::dd::Bdd<Type> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::set<storm::expressions::Variable> const& player1Variables, std::set<storm::expressions::Variable> const& player2Variables
            
            // 3. if the bounds suffice to complete checkTask, return result now.
            
            // 4. if the bounds do not suffice
            
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void GameBasedMdpModelChecker<Type, ValueType>::computeProb01States(storm::OptimizationDirection player1Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            storm::dd::Bdd<Type> transitionMatrixBdd = game.getTransitionMatrix().toBdd();
            storm::dd::Bdd<Type> bottomStatesBdd = game.getBottomStates();
            
            storm::dd::Bdd<Type> targetStates = game.getStates(targetStateExpression);
            if (player1Direction == storm::OptimizationDirection::Minimize) {
                targetStates |= game.getBottomStates();
            }
            
            transitionMatrixBdd.template toAdd<ValueType>().exportToDot("transbdd.dot");
            
            // Start by computing the states with probability 0/1 when player 2 minimizes.
            storm::utility::graph::GameProb01Result<Type> prob0Min = storm::utility::graph::performProb0(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Minimize, true);
            storm::utility::graph::GameProb01Result<Type> prob1Min = storm::utility::graph::performProb1(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Minimize, false);
            
            // Now compute the states with probability 0/1 when player 2 maximizes.
            storm::utility::graph::GameProb01Result<Type> prob0Max = storm::utility::graph::performProb0(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Maximize, false);
            storm::utility::graph::GameProb01Result<Type> prob1Max = storm::utility::graph::performProb1(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Maximize, true);
            
//            STORM_LOG_ASSERT(prob0Min.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(prob0Min.hasPlayer2Strategy(), "Unable to proceed without strategy.");

            STORM_LOG_ASSERT(prob1Max.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(prob1Max.hasPlayer2Strategy(), "Unable to proceed without strategy.");

//            prob0Min.getPlayer1Strategy().template toAdd<ValueType>().exportToDot("prob0_min_pl1_strat.dot");
            prob0Min.getPlayer2Strategy().template toAdd<ValueType>().exportToDot("prob0_min_pl2_strat.dot");

            prob1Max.getPlayer1Strategy().template toAdd<ValueType>().exportToDot("prob1_max_pl1_strat.dot");
            prob1Max.getPlayer2Strategy().template toAdd<ValueType>().exportToDot("prob1_max_pl2_strat.dot");

            STORM_LOG_DEBUG("Min: " << prob0Min.states.getNonZeroCount() << " no states, " << prob1Min.states.getNonZeroCount() << " yes states.");
            STORM_LOG_DEBUG("Max: " << prob0Max.states.getNonZeroCount() << " no states, " << prob1Max.states.getNonZeroCount() << " yes states.");
            
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::expressions::Expression GameBasedMdpModelChecker<Type, ValueType>::getExpression(storm::logic::Formula const& formula) {
            STORM_LOG_THROW(formula.isAtomicExpressionFormula() || formula.isAtomicLabelFormula(), storm::exceptions::InvalidPropertyException, "The target states have to be given as label or an expression.");
            storm::expressions::Expression result;
            if (formula.isAtomicLabelFormula()) {
                result = preprocessedProgram.getLabelExpression(formula.asAtomicLabelFormula().getLabel());
            } else {
                result = formula.asAtomicExpressionFormula().getExpression();
            }
            return result;
        }
        
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, double>;
    }
}