#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/models/symbolic/StandardRewardModel.h"
#include "src/models/symbolic/Dtmc.h"
#include "src/models/symbolic/Mdp.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/dd/DdManager.h"

#include "src/abstraction/prism/PrismMenuGameAbstractor.h"

#include "src/logic/FragmentSpecification.h"

#include "src/solver/SymbolicGameSolver.h"

#include "src/utility/solver.h"
#include "src/utility/dd.h"
#include "src/utility/prism.h"
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
        
        template<storm::dd::DdType Type, typename ModelType>
        GameBasedMdpModelChecker<Type, ModelType>::GameBasedMdpModelChecker(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : originalProgram(program), smtSolverFactory(smtSolverFactory) {
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::DTMC || program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only DTMCs/MDPs are supported by the game-based model checker.");
            storm::utility::prism::requireNoUndefinedConstants(originalProgram);
            
            // Start by preparing the program. That is, we flatten the modules if there is more than one.
            if (originalProgram.getNumberOfModules() > 1) {
                preprocessedProgram = originalProgram.flattenModules(this->smtSolverFactory);
            } else {
                preprocessedProgram = originalProgram;
            }
        }

        template<storm::dd::DdType Type, typename ModelType>
        GameBasedMdpModelChecker<Type, ModelType>::~GameBasedMdpModelChecker() {
            // Intentionally left empty.
        }

        template<storm::dd::DdType Type, typename ModelType>
        bool GameBasedMdpModelChecker<Type, ModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::reachability();
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
                
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping = preprocessedProgram.getLabelToExpressionMapping();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), pathFormula.getLeftSubformula().toExpression(preprocessedProgram.getManager(), labelToExpressionMapping), pathFormula.getRightSubformula().toExpression(preprocessedProgram.getManager(), labelToExpressionMapping));
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping = preprocessedProgram.getLabelToExpressionMapping();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), originalProgram.getManager().boolean(true), pathFormula.getSubformula().toExpression(preprocessedProgram.getManager(), labelToExpressionMapping));
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
        std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& statesMin, storm::dd::Bdd<Type> const& statesMax, bool prob0) {
            std::unique_ptr<CheckResult> result;
            if ((initialStates && statesMin && statesMax) == initialStates) {
                result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), prob0 ? storm::utility::zero<ValueType>() : storm::utility::one<ValueType>());
            }
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::OptimizationDirection player2Direction, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& prob0, storm::dd::Bdd<Type> const& prob1) {
            std::unique_ptr<CheckResult> result;
            
            if (checkTask.isBoundSet()) {
                if (player2Direction == storm::OptimizationDirection::Minimize && storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                    if ((prob1 && initialStates) == initialStates) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::one<ValueType>());
                    } else if (checkTask.isQualitativeSet()) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), ValueType(0.5));
                    }
                } else if (player2Direction == storm::OptimizationDirection::Maximize && !storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                    if ((prob0 && initialStates) == initialStates) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::zero<ValueType>());
                    } else if (checkTask.isQualitativeSet()) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), ValueType(0.5));
                    }
                }
            }
            
            return result;
        }

        template<typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQuantitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::OptimizationDirection const& player2Direction, ValueType const& value) {
            std::unique_ptr<CheckResult> result;
            
            // If the minimum value exceeds an upper threshold or the maximum value is below a lower threshold, we can
            // return the value because the property will definitely hold. Vice versa, if the minimum value exceeds an
            // upper bound or the maximum value is below a lower bound, the property will definitely not hold and we can
            // return the value.
            if (checkTask.isBoundSet() && storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                if (player2Direction == storm::OptimizationDirection::Minimize) {
                    if ((storm::logic::isStrict(checkTask.getBoundComparisonType()) && value > checkTask.getBoundThreshold())
                        || (!storm::logic::isStrict(checkTask.getBoundComparisonType()) && value >= checkTask.getBoundThreshold())) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                } else {
                    if ((storm::logic::isStrict(checkTask.getBoundComparisonType()) && value <= checkTask.getBoundThreshold())
                        || (!storm::logic::isStrict(checkTask.getBoundComparisonType()) && value < checkTask.getBoundThreshold())) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                }
            } else if (checkTask.isBoundSet() && !storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                if (player2Direction == storm::OptimizationDirection::Maximize) {
                    if ((storm::logic::isStrict(checkTask.getBoundComparisonType()) && value < checkTask.getBoundThreshold()) ||
                        (!storm::logic::isStrict(checkTask.getBoundComparisonType()) && value <= checkTask.getBoundThreshold())) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                } else {
                    if ((storm::logic::isStrict(checkTask.getBoundComparisonType()) && value >= checkTask.getBoundThreshold()) ||
                        (!storm::logic::isStrict(checkTask.getBoundComparisonType()) && value > checkTask.getBoundThreshold())) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                }
            }
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQuantitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, ValueType const& minValue, ValueType const& maxValue) {
            std::unique_ptr<CheckResult> result;

            // If the lower and upper bounds are close enough, we can return the result.
            if (maxValue - minValue < storm::utility::convertNumber<ValueType>(1e-3)) {
                result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), (minValue + maxValue) / ValueType(2));
            }

            return result;
        }
        
        template<storm::dd::DdType Type>
        storm::dd::Bdd<Type> pickPivotState(storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& transitions, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables, storm::dd::Bdd<Type> const& pivotStates) {
            
            // Perform a BFS and pick the first pivot state we encounter.
            storm::dd::Bdd<Type> pivotState;
            storm::dd::Bdd<Type> frontier = initialStates;
            storm::dd::Bdd<Type> frontierPivotStates = frontier && pivotStates;

            uint64_t level = 0;
            bool foundPivotState = !frontierPivotStates.isZero();
            if (foundPivotState) {
                pivotState = frontierPivotStates.existsAbstractRepresentative(rowVariables);
                STORM_LOG_TRACE("Picked pivot state from " << frontierPivotStates.getNonZeroCount() << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
            } else {
                while (!foundPivotState) {
                    frontier = frontier.relationalProduct(transitions, rowVariables, columnVariables);
                    frontierPivotStates = frontier && pivotStates;
                
                    if (!frontierPivotStates.isZero()) {
                        STORM_LOG_TRACE("Picked pivot state from " << frontierPivotStates.getNonZeroCount() << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                        pivotState = frontierPivotStates.existsAbstractRepresentative(rowVariables);
                        foundPivotState = true;
                    }
                    ++level;
                }
            }
            
            return pivotState;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool refineAfterQualitativeCheck(storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game, detail::GameProb01Result<Type> const& prob01, storm::dd::Bdd<Type> const& transitionMatrixBdd) {
            STORM_LOG_TRACE("Trying refinement after qualitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = prob01.min.first.getPlayer1Strategy();
            storm::dd::Bdd<Type> minPlayer2Strategy = prob01.min.first.getPlayer2Strategy();
            storm::dd::Bdd<Type> maxPlayer1Strategy = prob01.max.second.getPlayer1Strategy();
            storm::dd::Bdd<Type> maxPlayer2Strategy = prob01.max.second.getPlayer2Strategy();
            
            // Redirect all player 1 choices of the min strategy to that of the max strategy if this leads to a player 2
            // state that is also a prob 0 state.
            minPlayer1Strategy = (maxPlayer1Strategy && prob01.min.first.getPlayer2States()).existsAbstract(game.getPlayer1Variables()).ite(maxPlayer1Strategy, minPlayer1Strategy);
            
            // Build the fragment of transitions that is reachable by both the min and the max strategies.
            storm::dd::Bdd<Type> reachableTransitions = transitionMatrixBdd && minPlayer1Strategy && minPlayer2Strategy && maxPlayer1Strategy && maxPlayer2Strategy;
            reachableTransitions = reachableTransitions.existsAbstract(game.getNondeterminismVariables());
            storm::dd::Bdd<Type> pivotStates = storm::utility::dd::computeReachableStates(game.getInitialStates(), reachableTransitions, game.getRowVariables(), game.getColumnVariables());
            
            // Require the pivot state to be a [0, 1] state.
            // TODO: is this restriction necessary or is it already implied?
//            pivotStates &= prob01.min.first.getPlayer1States() && prob01.max.second.getPlayer1States();

            // Then constrain these states by the requirement that for either the lower or upper player 1 choice the player 2 choices must be different and
            // that the difference is not because of a missing strategy in either case.
            
            // Start with constructing the player 2 states that have a prob 0 (min) and prob 1 (max) strategy.
            storm::dd::Bdd<Type> constraint = minPlayer2Strategy.existsAbstract(game.getPlayer2Variables()) && maxPlayer2Strategy.existsAbstract(game.getPlayer2Variables());
            
            // Now construct all player 2 choices that actually exist and differ in the min and max case.
            constraint &= minPlayer2Strategy.exclusiveOr(maxPlayer2Strategy);
            
            // Then restrict the pivot states by requiring existing and different player 2 choices.
            pivotStates &= ((minPlayer1Strategy || maxPlayer1Strategy) && constraint).existsAbstract(game.getNondeterminismVariables());
            
            // We can only refine in case we have a reachable player 1 state with a player 2 successor (under either
            // player 1's min or max strategy) such that from this player 2 state, both prob0 min and prob0 max define
            // strategies and they differ. Hence, it is possible that we arrive at a point where no suitable pivot state
            // is found. In this case, we abort the qualitative refinement here.
            if (pivotStates.isZero()) {
                return false;
            }
            
            STORM_LOG_ASSERT(!pivotStates.isZero(), "Unable to proceed without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            storm::dd::Bdd<Type> pivotState = pickPivotState<Type>(game.getInitialStates(), reachableTransitions, game.getRowVariables(), game.getColumnVariables(), pivotStates);
            
            // Compute the lower and the upper choice for the pivot state.
            std::set<storm::expressions::Variable> variablesToAbstract = game.getNondeterminismVariables();
            variablesToAbstract.insert(game.getRowVariables().begin(), game.getRowVariables().end());
            storm::dd::Bdd<Type> lowerChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && minPlayer1Strategy;
            storm::dd::Bdd<Type> lowerChoice1 = (lowerChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
            storm::dd::Bdd<Type> lowerChoice2 = (lowerChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
            
            bool lowerChoicesDifferent = !lowerChoice1.exclusiveOr(lowerChoice2).isZero();
            if (lowerChoicesDifferent) {
                STORM_LOG_TRACE("Refining based on lower choice.");
                abstractor.refine(pivotState, (pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
                return true;
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);

                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Refining based on upper choice.");
                    abstractor.refine(pivotState, (pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                    return true;
                } else {
                    STORM_LOG_ASSERT(false, "Did not find choices from which to derive predicates.");
                }
            }
            return false;
        }

        template<storm::dd::DdType Type, typename ValueType>
        void refineAfterQuantitativeCheck(storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Add<Type, ValueType> const& minResult, storm::dd::Add<Type, ValueType> const& maxResult, detail::GameProb01Result<Type> const& prob01, std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> const& minStrategyPair, std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> const& maxStrategyPair, storm::dd::Bdd<Type> const& transitionMatrixBdd) {
            STORM_LOG_TRACE("Refining after quantitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = minStrategyPair.first;
            storm::dd::Bdd<Type> minPlayer2Strategy = minStrategyPair.second;
            storm::dd::Bdd<Type> maxPlayer1Strategy = maxStrategyPair.first;
            storm::dd::Bdd<Type> maxPlayer2Strategy = maxStrategyPair.second;

            // TODO: fix min strategies to take the max strategies if possible.
            
            // Build the fragment of transitions that is reachable by both the min and the max strategies.
            storm::dd::Bdd<Type> reachableTransitions = transitionMatrixBdd && minPlayer1Strategy && minPlayer2Strategy && maxPlayer1Strategy && maxPlayer2Strategy;
            reachableTransitions = reachableTransitions.existsAbstract(game.getNondeterminismVariables());
            storm::dd::Bdd<Type> pivotStates = storm::utility::dd::computeReachableStates(game.getInitialStates(), reachableTransitions, game.getRowVariables(), game.getColumnVariables());
            
            // Require the pivot state to be a state with a lower bound strictly smaller than the upper bound.
            pivotStates &= minResult.less(maxResult);
            
            STORM_LOG_ASSERT(!pivotStates.isZero(), "Unable to refine without pivot state candidates.");

            // Then constrain these states by the requirement that for either the lower or upper player 1 choice the player 2 choices must be different and
            // that the difference is not because of a missing strategy in either case.
            
            // Start with constructing the player 2 states that have a (min) and a (max) strategy.
            // TODO: necessary?
            storm::dd::Bdd<Type> constraint = minStrategyPair.second.existsAbstract(game.getPlayer2Variables()) && maxStrategyPair.second.existsAbstract(game.getPlayer2Variables());

//            (minStrategyPair.first && pivotStates).template toAdd<ValueType>().exportToDot("piv_min_pl1.dot");
//            (maxStrategyPair.first && pivotStates).template toAdd<ValueType>().exportToDot("piv_max_pl1.dot");
//            (minStrategyPair.second && pivotStates).template toAdd<ValueType>().exportToDot("piv_min_pl2.dot");
//            (maxStrategyPair.second && pivotStates).template toAdd<ValueType>().exportToDot("piv_max_pl2.dot");
            
            // Now construct all player 2 choices that actually exist and differ in the min and max case.
            constraint &= minPlayer2Strategy.exclusiveOr(maxPlayer2Strategy);

            // Then restrict the pivot states by requiring existing and different player 2 choices.
            pivotStates &= ((minPlayer1Strategy || maxPlayer1Strategy) && constraint).existsAbstract(game.getNondeterminismVariables());
            
            STORM_LOG_ASSERT(!pivotStates.isZero(), "Unable to refine without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            storm::dd::Bdd<Type> pivotState = pickPivotState<Type>(game.getInitialStates(), reachableTransitions, game.getRowVariables(), game.getColumnVariables(), pivotStates);
            
//            storm::dd::Add<Type, ValueType> pivotStateLower = pivotState.template toAdd<ValueType>() * minResult;
//            storm::dd::Add<Type, ValueType> pivotStateUpper = pivotState.template toAdd<ValueType>() * maxResult;
//            storm::dd::Bdd<Type> pivotStateIsMinProb0 = pivotState && prob01.min.first.getPlayer1States();
//            storm::dd::Bdd<Type> pivotStateIsMaxProb0 = pivotState && prob01.max.first.getPlayer1States();
//            storm::dd::Bdd<Type> pivotStateLowerStrategies = pivotState && prob01.min.first.getPlayer1Strategy() && prob01.min.first.getPlayer2Strategy();
//            storm::dd::Bdd<Type> pivotStateUpperStrategies = pivotState && prob01.max.first.getPlayer1Strategy() && prob01.max.first.getPlayer2Strategy();
//            storm::dd::Bdd<Type> pivotStateLowerPl1Strategy = pivotState && prob01.min.first.getPlayer1Strategy();
//            storm::dd::Bdd<Type> pivotStateUpperPl1Strategy = pivotState && prob01.max.first.getPlayer1Strategy();
//            storm::dd::Bdd<Type> pivotStateLowerPl2Strategy = pivotState && prob01.min.first.getPlayer2Strategy();
//            storm::dd::Bdd<Type> pivotStateUpperPl2Strategy = pivotState && prob01.max.first.getPlayer2Strategy();
//            
//            minResult.exportToDot("minresult.dot");
//            maxResult.exportToDot("maxresult.dot");
            pivotState.template toAdd<ValueType>().exportToDot("pivot.dot");
//            pivotStateLower.exportToDot("pivot_lower.dot");
//            pivotStateUpper.exportToDot("pivot_upper.dot");
//            pivotStateIsMinProb0.template toAdd<ValueType>().exportToDot("pivot_is_minprob0.dot");
//            pivotStateIsMaxProb0.template toAdd<ValueType>().exportToDot("pivot_is_maxprob0.dot");
//            pivotStateLowerStrategies.template toAdd<ValueType>().exportToDot("pivot_lower_strats.dot");
//            pivotStateUpperStrategies.template toAdd<ValueType>().exportToDot("pivot_upper_strats.dot");
//            pivotStateLowerPl1Strategy.template toAdd<ValueType>().exportToDot("pivot_lower_pl1_strat.dot");
//            pivotStateUpperPl1Strategy.template toAdd<ValueType>().exportToDot("pivot_upper_pl1_strat.dot");
//            pivotStateLowerPl2Strategy.template toAdd<ValueType>().exportToDot("pivot_lower_pl2_strat.dot");
//            pivotStateUpperPl2Strategy.template toAdd<ValueType>().exportToDot("pivot_upper_pl2_strat.dot");
            
            // Compute the lower and the upper choice for the pivot state.
            std::set<storm::expressions::Variable> variablesToAbstract = game.getNondeterminismVariables();
            variablesToAbstract.insert(game.getRowVariables().begin(), game.getRowVariables().end());
            storm::dd::Bdd<Type> lowerChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && minPlayer1Strategy;
//            (pivotState && minStrategyPair.first).template toAdd<ValueType>().exportToDot("pl1_choice_in_pivot.dot");
//            (pivotState && minStrategyPair.first && maxStrategyPair.second).template toAdd<ValueType>().exportToDot("pl1and2_choice_in_pivot.dot");
//            (pivotState && maxStrategyPair.second).template toAdd<ValueType>().exportToDot("pl2_choice_in_pivot.dot");
            storm::dd::Bdd<Type> lowerChoice1 = (lowerChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
//            lowerChoice.template toAdd<ValueType>().exportToDot("lower.dot");
//            maxStrategyPair.second.template toAdd<ValueType>().exportToDot("max_strat.dot");
            storm::dd::Bdd<Type> lowerChoice2 = (lowerChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
//            lowerChoice2.template toAdd<ValueType>().exportToDot("tmp_lower.dot");
//            lowerChoice2 = lowerChoice2.existsAbstract(variablesToAbstract);
            
            lowerChoice.template toAdd<ValueType>().exportToDot("ref_lower.dot");
            lowerChoice1.template toAdd<ValueType>().exportToDot("ref_lower1.dot");
            lowerChoice2.template toAdd<ValueType>().exportToDot("ref_lower2.dot");
            
            bool lowerChoicesDifferent = !lowerChoice1.exclusiveOr(lowerChoice2).isZero();
            if (lowerChoicesDifferent) {
                STORM_LOG_TRACE("Refining based on lower choice.");
                abstractor.refine(pivotState, (pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
                
                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Refining based on upper choice.");
                    abstractor.refine(pivotState, (pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                } else {
                    STORM_LOG_ASSERT(false, "Did not find choices from which to derive predicates.");
                }
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        struct MaybeStateResult {
            MaybeStateResult() = default;
            
            MaybeStateResult(storm::dd::Add<Type, ValueType> const& values, storm::dd::Bdd<Type> const& player1Strategy, storm::dd::Bdd<Type> const& player2Strategy) : values(values), player1Strategy(player1Strategy), player2Strategy(player2Strategy) {
                // Intentionally left empty.
            }
            
            storm::dd::Add<Type, ValueType> values;
            storm::dd::Bdd<Type> player1Strategy;
            storm::dd::Bdd<Type> player2Strategy;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        MaybeStateResult<Type, ValueType> solveMaybeStates(storm::OptimizationDirection const& player1Direction, storm::OptimizationDirection const& player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& maybeStates, storm::dd::Bdd<Type> const& prob1States, boost::optional<MaybeStateResult<Type, ValueType>> startInfo = boost::none) {
            
            STORM_LOG_TRACE("Performing quantative solution step. Player 1: " << player1Direction << ", player 2: " << player2Direction << ".");
            
            // Compute the ingredients of the equation system.
            storm::dd::Add<Type, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
			storm::dd::Bdd<Type> allStates = game.getReachableStates();
			storm::dd::Add<Type, ValueType> allStatesAdd = allStates.template toAdd<ValueType>();
            storm::dd::Add<Type, ValueType> submatrix = allStatesAdd * game.getTransitionMatrix();
            storm::dd::Add<Type, ValueType> prob1StatesAsColumn = prob1States.template toAdd<ValueType>().swapVariables(game.getRowColumnMetaVariablePairs());
            storm::dd::Add<Type, ValueType> subvector = submatrix * prob1StatesAsColumn;
            subvector = subvector.sumAbstract(game.getColumnVariables());

            // Cut away all columns targeting non-maybe states.
            submatrix *= allStatesAdd.swapVariables(game.getRowColumnMetaVariablePairs());

            // Cut the starting vector to the maybe states of this query.
            storm::dd::Add<Type, ValueType> startVector;
            if (startInfo) {
                startVector = startInfo.get().values * allStatesAdd;
            } else {
                startVector = game.getManager().template getAddZero<ValueType>();
            }
            
            // Create the solver and solve the equation system.
            storm::utility::solver::SymbolicGameSolverFactory<Type, ValueType> solverFactory;
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> solver = solverFactory.create(submatrix, allStates, game.getIllegalPlayer1Mask(), game.getIllegalPlayer2Mask(), game.getRowVariables(), game.getColumnVariables(), game.getRowColumnMetaVariablePairs(), game.getPlayer1Variables(), game.getPlayer2Variables());
            solver->setGeneratePlayersStrategies(true);
            auto values = solver->solveGame(player1Direction, player2Direction, startVector, subvector, startInfo ? boost::make_optional(startInfo.get().player1Strategy) : boost::none, startInfo ? boost::make_optional(startInfo.get().player2Strategy) : boost::none);
            return MaybeStateResult<Type, ValueType>(values, solver->getPlayer1Strategy(), solver->getPlayer2Strategy());
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
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
            if (originalProgram.isDeterministicModel()) {
                player1Direction = storm::OptimizationDirection::Maximize;
            } else if (checkTask.isOptimizationDirectionSet()) {
                player1Direction = checkTask.getOptimizationDirection();
            } else if (checkTask.isBoundSet() && !originalProgram.isDeterministicModel()) {
                player1Direction = storm::logic::isLowerBound(checkTask.getBoundComparisonType()) ? storm::OptimizationDirection::Minimize : storm::OptimizationDirection::Maximize;
            } else {
                STORM_LOG_THROW(originalProgram.isDeterministicModel(), storm::exceptions::InvalidPropertyException, "Requiring either min or max annotation in property for nondeterministic models.");
                player1Direction = storm::OptimizationDirection::Maximize;
            }
            
            storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType> abstractor(preprocessedProgram, initialPredicates, smtSolverFactory);
            for (uint_fast64_t iterations = 0; iterations < 10000; ++iterations) {
                STORM_LOG_TRACE("Starting iteration " << iterations << ".");
                abstractor.exportToDot("game" + std::to_string(iterations) + ".dot");

                // 1. build initial abstraction based on the the constraint expression (if not 'true') and the target state expression.
                storm::abstraction::MenuGame<Type, ValueType> game = abstractor.abstract();
                STORM_LOG_DEBUG("Abstraction in iteration " << iterations << " has " << game.getNumberOfStates() << " (player 1) states and " << game.getNumberOfTransitions() << " transitions.");
                STORM_LOG_THROW(game.getInitialStates().getNonZeroCount(), storm::exceptions::InvalidModelException, "Cannot treat models with more than one (abstract) initial state.");
                
                // 1.5 build a BDD from the transition matrix for various later uses.
                storm::dd::Bdd<Type> transitionMatrixBdd = game.getTransitionMatrix().toBdd();
                
                // 2. compute all states with probability 0/1 wrt. to the two different player 2 goals (min/max).
                detail::GameProb01Result<Type> prob01;
                storm::dd::Bdd<Type> targetStates = game.getStates(targetStateExpression);
                if (player1Direction == storm::OptimizationDirection::Minimize) {
                    targetStates |= game.getBottomStates();
                }
                prob01.min = computeProb01States(player1Direction, storm::OptimizationDirection::Minimize, game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates);
                std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Minimize, game.getInitialStates(), prob01.min.first.getPlayer1States(), prob01.min.second.getPlayer1States());
                if (result) {
                    return result;
                }
                if (iterations == 18) {
                    exit(-1);
                }
                prob01.max = computeProb01States(player1Direction, storm::OptimizationDirection::Maximize, game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates);
                result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Maximize, game.getInitialStates(), prob01.max.first.getPlayer1States(), prob01.max.second.getPlayer1States());
                if (result) {
                    return result;
                }

                // 3. compute the states for which we know the result/for which we know there is more work to be done.
                storm::dd::Bdd<Type> maybeMin = !(prob01.min.first.getPlayer1States() || prob01.min.second.getPlayer1States()) && game.getReachableStates();
                storm::dd::Bdd<Type> maybeMax = !(prob01.max.first.getPlayer1States() || prob01.max.second.getPlayer1States()) && game.getReachableStates();
                
                // 4. if the initial states are not maybe states, then we can refine at this point.
                storm::dd::Bdd<Type> initialMaybeStates = (game.getInitialStates() && maybeMin) || (game.getInitialStates() && maybeMax);
                bool qualitativeRefinement = false;
                if (initialMaybeStates.isZero()) {
                    // In this case, we know the result for the initial states for both player 2 minimizing and maximizing.
                    STORM_LOG_TRACE("No initial state is a 'maybe' state. Refining abstraction based on qualitative check.");
                    
                    // Check whether we can already give the answer based on the current information.
                    result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getInitialStates(), prob01.min.first.getPlayer1States(), prob01.max.first.getPlayer1States(), true);
                    if (result) {
                        return result;
                    }
                    result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getInitialStates(), prob01.min.second.getPlayer1States(), prob01.max.second.getPlayer1States(), false);
                    if (result) {
                        return result;
                    }
                    
                    STORM_LOG_DEBUG("Obtained qualitative bounds [0, 1] on the actual value for the initial states.");

                    // If we get here, the initial states were all identified as prob0/1 states, but the value (0 or 1)
                    // depends on whether player 2 is minimizing or maximizing. Therefore, we need to find a place to refine.
                    qualitativeRefinement = refineAfterQualitativeCheck(abstractor, game, prob01, transitionMatrixBdd);
                }
                
                if (!qualitativeRefinement) {
                    // At this point, we know that we cannot answer the query without further numeric computation.
                    STORM_LOG_TRACE("Starting numerical solution step.");
                    
                    // Prepare some storage that we use on-demand during the quantitative solving step.
                    storm::dd::Add<Type, ValueType> minResult = prob01.min.second.getPlayer1States().template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> maxResult = prob01.max.second.getPlayer1States().template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> initialStatesAdd = game.getInitialStates().template toAdd<ValueType>();

                    // The minimal value after qualitative checking can only be zero. It it was 1, we could have given
                    // the result right away.
                    ValueType minValue = storm::utility::zero<ValueType>();
                    MaybeStateResult<Type, ValueType> minMaybeStateResult(game.getManager().template getAddZero<ValueType>(), game.getManager().getBddZero(), game.getManager().getBddZero());
                    if (!maybeMin.isZero()) {
                        minMaybeStateResult = solveMaybeStates(player1Direction, storm::OptimizationDirection::Minimize, game, maybeMin, prob01.min.second.getPlayer1States());
                        minResult = minMaybeStateResult.values;
                        storm::dd::Add<Type, ValueType> initialStateMin = initialStatesAdd * minResult;
                        // Here we can only require a non-zero count of *at most* one, because the result may actually be 0.
                        STORM_LOG_ASSERT(initialStateMin.getNonZeroCount() <= 1, "Wrong number of results for initial states. Expected <= 1, but got " << initialStateMin.getNonZeroCount() << ".");
                        minValue = initialStateMin.getMax();
                    }
                    STORM_LOG_TRACE("Obtained quantitative lower bound " << minValue << ".");
                    
                    // Check whether we can abort the computation because of the lower value.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Minimize, minValue);
                    if (result) {
                        return result;
                    }
                    
                    // Likewise, the maximal value after qualitative checking can only be 1. If it was 0, we could have
                    // given the result right awy.
                    ValueType maxValue = storm::utility::one<ValueType>();
                    MaybeStateResult<Type, ValueType> maxMaybeStateResult(game.getManager().template getAddZero<ValueType>(), game.getManager().getBddZero(), game.getManager().getBddZero());
                    if (!maybeMax.isZero()) {
                        maxMaybeStateResult = solveMaybeStates(player1Direction, storm::OptimizationDirection::Maximize, game, maybeMax, prob01.max.second.getPlayer1States(), boost::make_optional(minMaybeStateResult));
                        maxResult = maxMaybeStateResult.values;
                        storm::dd::Add<Type, ValueType> initialStateMax = (initialStatesAdd * maxResult);
                        // Unlike in the min-case, we can require a non-zero count of 1 here, because if the max was in
                        // fact 0, the result would be 0, which would have been detected earlier by the graph algorithms.
                        STORM_LOG_ASSERT(initialStateMax.getNonZeroCount() == 1, "Wrong number of results for initial states. Expected 1, but got " << initialStateMax.getNonZeroCount() << ".");
                        maxValue = initialStateMax.getMax();
                    }
                    STORM_LOG_TRACE("Obtained quantitative upper bound " << maxValue << ".");

                    // Check whether we can abort the computation because of the upper value.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Maximize, maxValue);
                    if (result) {
                        return result;
                    }
                    
                    STORM_LOG_DEBUG("Obtained quantitative bounds [" << minValue << ", " << maxValue << "] on the actual value for the initial states.");

                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, minValue, maxValue);
                    if (result) {
                        return result;
                    }

                    // If we arrived at this point, it means that we have all qualitative and quantitative information
                    // about the game, but we could not yet answer the query. In this case, we need to refine.
                    
                    // Redirect all player 1 choices of the min strategy to that of the max strategy if this leads to a player 2 state that has the same prob.
					// Get all relevant strategies.
					storm::dd::Bdd<Type> minPlayer1Strategy = minMaybeStateResult.player1Strategy;
					storm::dd::Bdd<Type> minPlayer2Strategy = minMaybeStateResult.player2Strategy;
					storm::dd::Bdd<Type> maxPlayer1Strategy = maxMaybeStateResult.player1Strategy;
					storm::dd::Bdd<Type> maxPlayer2Strategy = maxMaybeStateResult.player2Strategy;
					
					storm::dd::Add<Type, ValueType> matrix = game.getTransitionMatrix();
					
					storm::dd::Add<Type, ValueType> minValuesForPlayer1UnderMinP1Strategy = matrix * minPlayer1Strategy.template toAdd<ValueType>() * minPlayer2Strategy.template toAdd<ValueType>();
					storm::dd::Add<Type, ValueType> minValuesForPlayer1UnderMaxP1Strategy = matrix * maxPlayer1Strategy.template toAdd<ValueType>() * minPlayer2Strategy.template toAdd<ValueType>();
					// This BDD has a 1 for every state (s) that can switch the strategy.
					storm::dd::Bdd<Type> minIsGreaterOrEqual = minValuesForPlayer1UnderMinP1Strategy.greaterOrEqual(minValuesForPlayer1UnderMaxP1Strategy);
					
					minPlayer1Strategy = minIsGreaterOrEqual.existsAbstract(game.getPlayer1Variables()).ite(maxPlayer1Strategy, minPlayer1Strategy);

                    storm::dd::Bdd<Type> tmp = (prob01.min.first.getPlayer2Strategy().existsAbstract(game.getPlayer2Variables()) && prob01.min.second.getPlayer2Strategy().existsAbstract(game.getPlayer2Variables()));
                    STORM_LOG_ASSERT(tmp.isZero(), "wth?");
                    tmp = prob01.min.first.getPlayer2Strategy().existsAbstract(game.getPlayer2Variables()) && minPlayer2Strategy.existsAbstract(game.getPlayer2Variables());
                    if (!tmp.isZero()) {
                        tmp = tmp && prob01.min.first.getPlayer2Strategy().exclusiveOr(minPlayer2Strategy).existsAbstract(game.getPlayer2Variables());
                        (tmp && prob01.min.first.getPlayer2Strategy()).template toAdd<ValueType>().exportToDot("prob0_strat.dot");
                        (tmp && minPlayer2Strategy).template toAdd<ValueType>().exportToDot("maybe_strat.dot");
                        if (!tmp.isZero()) {
                            storm::dd::Add<Type, ValueType> values = (tmp.template toAdd<ValueType>() * game.getTransitionMatrix() * minResult.swapVariables(game.getRowColumnMetaVariablePairs())).sumAbstract(game.getColumnVariables());
                            tmp.template toAdd<ValueType>().exportToDot("illegal.dot");
                            minResult.exportToDot("vals.dot");
                        }
                        STORM_LOG_ASSERT(tmp.isZero(), "ddduuuudde?");
                    }
                    STORM_LOG_ASSERT(tmp.isZero(), "wth2?");
                    //tmp = prob01.min.second.getPlayer2Strategy().existsAbstract(game.getPlayer2Variables()) && minPlayer2Strategy;
                    
                    //(minMaybeStateResult.player2Strategy && (prob01.min.first.getPlayer2Strategy() || prob01.min.second.getPlayer2Strategy())).template toAdd<ValueType>().exportToDot("strat_overlap.dot");
                    //minMaybeStateResult.player2Strategy |= prob01.min.first.getPlayer2Strategy() || prob01.min.second.getPlayer2Strategy();
                    //maxMaybeStateResult.player1Strategy |= prob01.max.first.getPlayer1Strategy() || prob01.max.second.getPlayer1Strategy();
                    //maxMaybeStateResult.player2Strategy |= prob01.max.first.getPlayer2Strategy() || prob01.max.second.getPlayer2Strategy();
                    
                    // Make sure that all strategies are still valid strategies.
                    STORM_LOG_ASSERT(minPlayer1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for min is illegal.");
                    STORM_LOG_ASSERT(maxPlayer1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for max is illegal.");
                    STORM_LOG_ASSERT(minPlayer2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for min is illegal.");
                    STORM_LOG_ASSERT(maxPlayer2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for max is illegal.");
                    
                    refineAfterQuantitativeCheck(abstractor, game, minResult, maxResult, prob01, std::make_pair(minPlayer1Strategy, minPlayer2Strategy), std::make_pair(maxPlayer1Strategy, maxPlayer2Strategy), transitionMatrixBdd);
                }
            }

            STORM_LOG_ASSERT(false, "This point must not be reached.");
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::pair<storm::utility::graph::GameProb01Result<Type>, storm::utility::graph::GameProb01Result<Type>> GameBasedMdpModelChecker<Type, ModelType>::computeProb01States(storm::OptimizationDirection player1Direction, storm::OptimizationDirection player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates) {
            // Compute the states with probability 0/1.
//            storm::utility::graph::GameProb01Result<Type> prob0 = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, player2Direction == storm::OptimizationDirection::Minimize, player2Direction == storm::OptimizationDirection::Minimize);
//            storm::utility::graph::GameProb01Result<Type> prob1 = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, player2Direction == storm::OptimizationDirection::Maximize, player2Direction == storm::OptimizationDirection::Maximize);
            storm::utility::graph::GameProb01Result<Type> prob0 = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, true, true);
            storm::utility::graph::GameProb01Result<Type> prob1 = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, true, true);
            
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Minimize || (prob0.hasPlayer1Strategy() && (prob0.getPlayer1States().isZero() || !prob0.getPlayer1Strategy().isZero())), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Minimize || (prob0.hasPlayer2Strategy() && (prob0.getPlayer1States().isZero() || !prob0.getPlayer2Strategy().isZero())), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Maximize || (prob1.hasPlayer1Strategy() && (prob1.getPlayer1States().isZero() || !prob1.getPlayer1Strategy().isZero())), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Maximize || (prob1.hasPlayer2Strategy() && (prob1.getPlayer1States().isZero() || !prob1.getPlayer2Strategy().isZero())), "Unable to proceed without strategy.");

            STORM_LOG_TRACE("Player 1: " << player1Direction << ", player 2: " << player2Direction << ": " << prob0.getPlayer1States().getNonZeroCount() << " 'no' states, " << prob1.getPlayer1States().getNonZeroCount() << " 'yes' states.");
            
            return std::make_pair(prob0, prob1);
        }

        template<storm::dd::DdType Type, typename ModelType>
        storm::expressions::Expression GameBasedMdpModelChecker<Type, ModelType>::getExpression(storm::logic::Formula const& formula) {
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

        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;

    }
}
