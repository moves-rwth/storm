#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/dd/DdManager.h"

#include "src/abstraction/prism/PrismMenuGameAbstractor.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/dd.h"
#include "src/utility/macros.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidModelException.h"

#include "src/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        namespace detail {
            template<storm::dd::DdType DdType>
            GameProb01Result<DdType>::GameProb01Result(storm::utility::graph::GameProb01Result<DdType> const& prob0Min, storm::utility::graph::GameProb01Result<DdType> const& prob1Min, storm::utility::graph::GameProb01Result<DdType> const& prob0Max, storm::utility::graph::GameProb01Result<DdType> const& prob1Max) : min(std::make_pair(prob0Min, prob1Min)), max(std::make_pair(prob0Max, prob1Max)) {
                // Intentionally left empty.
            }
        }
        
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
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), getExpression(pathFormula.getLeftSubformula()), getExpression(pathFormula.getRightSubformula()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), originalProgram.getManager().boolean(true), getExpression(pathFormula.getSubformula()));
        }
        
        template<typename ValueType>
        bool getResultConsideringBound(ValueType const& value, storm::logic::Bound<ValueType> const& bound) {
            if (storm::logic::isLowerBound(bound.comparisonType)) {
                if (storm::logic::isStrict(bound.comparisonType)) {
                    return value > bound.threshold;
                } else {
                    return value >= bound.threshold;
                }
            } else {
                if (storm::logic::isStrict(bound.comparisonType)) {
                    return value < bound.threshold;
                } else {
                    return value <= bound.threshold;
                }
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> getResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::dd::DdManager<Type> const& ddManager, storm::dd::Bdd<Type> const& reachableStates, storm::dd::Bdd<Type> const& initialStates, bool prob0) {
            return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), prob0 ? storm::utility::zero<ValueType>() : storm::utility::one<ValueType>());
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Bdd<Type> pickPivotState(storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& transitions, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables, storm::dd::Bdd<Type> const& pivotStates) {
            // Perform a BFS and pick the first pivot state we encounter.
            storm::dd::Bdd<Type> pivotState;
            storm::dd::Bdd<Type> frontier = initialStates;
            storm::dd::Bdd<Type> frontierPivotStates = frontier && pivotStates;

            bool foundPivotState = !frontierPivotStates.isZero();
            if (foundPivotState) {
                pivotState = frontierPivotStates.existsAbstractRepresentative(rowVariables);
            } else {
                while (!foundPivotState) {
                    frontier = frontier.relationalProduct(transitions, rowVariables, columnVariables);
                    frontierPivotStates = frontier && pivotStates;
                
                    if (!frontierPivotStates.isZero()) {
                        pivotState = frontierPivotStates.existsAbstractRepresentative(rowVariables);
                        foundPivotState = true;
                    }
                }
            }
            
            return pivotState;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void refineAfterQualitativeCheck(storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game, detail::GameProb01Result<Type> const& prob01, storm::dd::Bdd<Type> const& transitionMatrixBdd) {
            storm::dd::Bdd<Type> transitionsInIntersection = transitionMatrixBdd && prob01.min.first.getPlayer1Strategy() && prob01.min.first.getPlayer2Strategy() && prob01.max.second.getPlayer1Strategy() && prob01.max.second.getPlayer2Strategy();
            
            // First, we have to find the pivot state candidates. Start by constructing the reachable fragment of the
            // state space *under both* strategy pairs.
            storm::dd::Bdd<Type> pivotStates = storm::utility::dd::computeReachableStates(game.getInitialStates(), transitionsInIntersection.existsAbstract(game.getNondeterminismVariables()), game.getRowVariables(), game.getColumnVariables());
            
            // Then constrain this set by requiring that the two stratey pairs resolve the nondeterminism differently.
            pivotStates &= (prob01.min.first.getPlayer1Strategy() && prob01.min.first.getPlayer2Strategy()).exclusiveOr(prob01.max.second.getPlayer1Strategy() && prob01.max.second.getPlayer2Strategy()).existsAbstract(game.getNondeterminismVariables());
            STORM_LOG_ASSERT(!pivotStates.isZero(), "Unable to refine without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            storm::dd::Bdd<Type> pivotState = pickPivotState<Type>(game.getInitialStates(), transitionsInIntersection, game.getRowVariables(), game.getColumnVariables(), pivotStates);
            
            // Compute the lower and the upper choice for the pivot state.
            std::set<storm::expressions::Variable> variablesToAbstract = game.getNondeterminismVariables();
            variablesToAbstract.insert(game.getRowVariables().begin(), game.getRowVariables().end());
            storm::dd::Bdd<Type> lowerChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && prob01.min.first.getPlayer1Strategy();
            storm::dd::Bdd<Type> lowerChoice1 = (lowerChoice && prob01.min.first.getPlayer2Strategy()).existsAbstract(variablesToAbstract);
            storm::dd::Bdd<Type> lowerChoice2 = (lowerChoice && prob01.max.second.getPlayer2Strategy()).existsAbstract(variablesToAbstract);
                        
            bool lowerChoicesDifferent = !lowerChoice1.exclusiveOr(lowerChoice2).isZero();
            if (lowerChoicesDifferent) {
                abstractor.refine(pivotState, (pivotState && prob01.min.first.getPlayer1Strategy()).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && prob01.max.second.getPlayer1Strategy();
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && prob01.min.first.getPlayer2Strategy()).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && prob01.max.second.getPlayer2Strategy()).existsAbstract(variablesToAbstract);
                
                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    abstractor.refine(pivotState, (pivotState && prob01.max.second.getPlayer1Strategy()).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                } else {
                    STORM_LOG_ASSERT(false, "Did not find choices from which to derive predicates.");
                }
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidPropertyException, "The game-based abstraction refinement model checker can only compute the result for the initial states.");

            // Optimization: do not compute both bounds if not necessary (e.g. if bound given and exceeded, etc.)
            
            // Set up initial predicates.
            std::vector<storm::expressions::Expression> initialPredicates;
            initialPredicates.push_back(targetStateExpression);
            if (!constraintExpression.isTrue() && !constraintExpression.isFalse()) {
                initialPredicates.push_back(constraintExpression);
            }
            
            // Derive the optimization direction for player 1 (assuming menu-game abstraction).
            storm::OptimizationDirection player1Direction;
            if (checkTask.isOptimizationDirectionSet()) {
                player1Direction = checkTask.getOptimizationDirection();
            } else if (checkTask.isBoundSet() && !originalProgram.isDeterministicModel()) {
                player1Direction = storm::logic::isLowerBound(checkTask.getBoundComparisonType()) ? storm::OptimizationDirection::Minimize : storm::OptimizationDirection::Maximize;
            } else {
                STORM_LOG_THROW(originalProgram.isDeterministicModel(), storm::exceptions::InvalidPropertyException, "Requiring either min or max annotation in property for nondeterministic models.");
                player1Direction = storm::OptimizationDirection::Maximize;
            }
            
            storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType> abstractor(preprocessedProgram, initialPredicates, smtSolverFactory);
            
            for (uint_fast64_t iterations = 0; iterations < 10000; ++iterations) {
                STORM_LOG_TRACE("Starting iteration " << iterations);
                abstractor.exportToDot("game" + std::to_string(iterations) + ".dot");

                // 1. build initial abstraction based on the the constraint expression (if not 'true') and the target state expression.
                storm::abstraction::MenuGame<Type, ValueType> game = abstractor.abstract();
                STORM_LOG_DEBUG("Abstraction in iteration " << iterations << " has " << game.getNumberOfStates() << " (player 1) states and " << game.getNumberOfTransitions() << " transitions.");
                STORM_LOG_THROW(game.getInitialStates().getNonZeroCount(), storm::exceptions::InvalidModelException, "Cannot treat models with more than one (abstract) initial state.");
                
                // 1.5 build a BDD from the transition matrix for various later uses.
                storm::dd::Bdd<Type> transitionMatrixBdd = game.getTransitionMatrix().toBdd();
                
                // 2. compute all states with probability 0/1 wrt. to the two different player 2 goals (min/max).
                detail::GameProb01Result<Type> prob01 = computeProb01States(player1Direction, game, transitionMatrixBdd, constraintExpression, targetStateExpression);
                
                // 3. compute the states for which we know the result/for which we know there is more work to be done.
                storm::dd::Bdd<Type> maybeMin = !(prob01.min.first.states || prob01.min.second.states) && game.getReachableStates();
                storm::dd::Bdd<Type> maybeMax = !(prob01.max.first.states || prob01.max.second.states) && game.getReachableStates();
                
                // 4. if the initial states are not maybe states, then we can refine at this point.
                storm::dd::Bdd<Type> initialMaybeStates = (game.getInitialStates() && maybeMin) || (game.getInitialStates() && maybeMax);
                if (initialMaybeStates.isZero()) {
                    // In this case, we know the result for the initial states for both player 2 minimizing and maximizing.
                    STORM_LOG_TRACE("No initial state is a 'maybe' state. Refining abstraction based on qualitative check.");
                    
                    // If the result is 0 independent of the player 2 strategy, then we know the final result and can return it.
                    storm::dd::Bdd<Type> tmp = game.getInitialStates() && prob01.min.first.states && prob01.max.first.states;
                    if (tmp == game.getInitialStates()) {
                        return getResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getManager(), game.getReachableStates(), game.getInitialStates(), true);
                    }
                    
                    // If the result is 1 independent of the player 2 strategy, then we know the final result and can return it.
                    tmp = game.getInitialStates() && prob01.min.second.states && prob01.max.second.states;
                    if (tmp == game.getInitialStates()) {
                        return getResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getManager(), game.getReachableStates(), game.getInitialStates(), false);
                    }
                    
                    // If we get here, the initial states were all identified as prob0/1 states, but the value (0 or 1)
                    // depends on whether player 2 is minimizing or maximizing. Therefore, we need to find a place to refine.
                    refineAfterQualitativeCheck(abstractor, game, prob01, transitionMatrixBdd);
                } else {
                    
                    // If we can already answer the question, do not solve the game numerically.
                    
                    
                    STORM_LOG_ASSERT(false, "Quantiative refinement not yet there. :)");
                }
            }

            STORM_LOG_ASSERT(false, "This point must not be reached.");
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        detail::GameProb01Result<Type> GameBasedMdpModelChecker<Type, ValueType>::computeProb01States(storm::OptimizationDirection player1Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            storm::dd::Bdd<Type> bottomStatesBdd = game.getBottomStates();
            
            storm::dd::Bdd<Type> targetStates = game.getStates(targetStateExpression);
            if (player1Direction == storm::OptimizationDirection::Minimize) {
                targetStates |= game.getBottomStates();
            }
            
            // Start by computing the states with probability 0/1 when player 2 minimizes.
            storm::utility::graph::GameProb01Result<Type> prob0Min = storm::utility::graph::performProb0(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true);
            storm::utility::graph::GameProb01Result<Type> prob1Min = storm::utility::graph::performProb1(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Minimize, false, false);
            
            // Now compute the states with probability 0/1 when player 2 maximizes.
            storm::utility::graph::GameProb01Result<Type> prob0Max = storm::utility::graph::performProb0(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Maximize, false, false);
            storm::utility::graph::GameProb01Result<Type> prob1Max = storm::utility::graph::performProb1(game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true);
            prob1Max.getPlayer1Strategy().template toAdd<ValueType>().exportToDot("prob1maxstrat.dot");
            
            STORM_LOG_ASSERT(prob0Min.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(prob0Min.hasPlayer2Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(prob1Max.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(prob1Max.hasPlayer2Strategy(), "Unable to proceed without strategy.");

            STORM_LOG_TRACE("Min: " << prob0Min.states.getNonZeroCount() << " no states, " << prob1Min.states.getNonZeroCount() << " yes states.");
            STORM_LOG_TRACE("Max: " << prob0Max.states.getNonZeroCount() << " no states, " << prob1Max.states.getNonZeroCount() << " yes states.");
            
            return detail::GameProb01Result<Type>(prob0Min, prob1Min, prob0Max, prob1Max);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::expressions::Expression GameBasedMdpModelChecker<Type, ValueType>::getExpression(storm::logic::Formula const& formula) {
            STORM_LOG_THROW(formula.isBooleanLiteralFormula() || formula.isAtomicExpressionFormula() || formula.isAtomicLabelFormula(), storm::exceptions::InvalidPropertyException, "The target states have to be given as label or an expression.");
            storm::expressions::Expression result;
            if (formula.isAtomicLabelFormula()) {
                result = preprocessedProgram.getLabelExpression(formula.asAtomicLabelFormula().getLabel());
            } else if (formula.isAtomicExpressionFormula()) {
                result = formula.asAtomicExpressionFormula().getExpression();
            } else {
                result = formula.asBooleanLiteralFormula().isTrueFormula() ? originalProgram.getManager().boolean(true) : originalProgram.getManager().boolean(false);
            }
            return result;
        }
        
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, double>;
    }
}