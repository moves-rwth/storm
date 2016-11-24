#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/solver/SymbolicGameSolver.h"

#include "storm/utility/solver.h"
#include "storm/utility/dd.h"
#include "storm/utility/prism.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidModelException.h"

#include "storm/modelchecker/results/CheckResult.h"

//#define LOCAL_DEBUG

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType Type, typename ModelType>
        GameBasedMdpModelChecker<Type, ModelType>::GameBasedMdpModelChecker(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : smtSolverFactory(smtSolverFactory) {
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::NotSupportedException, "Currently only PRISM models are supported by the game-based model checker.");
            storm::prism::Program const& originalProgram = model.asPrismProgram();
            STORM_LOG_THROW(originalProgram.getModelType() == storm::prism::Program::ModelType::DTMC || originalProgram.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only DTMCs/MDPs are supported by the game-based model checker.");
            storm::utility::prism::requireNoUndefinedConstants(originalProgram);
            
            // Start by preparing the program. That is, we flatten the modules if there is more than one.
            if (originalProgram.getNumberOfModules() > 1) {
                preprocessedModel = originalProgram.flattenModules(this->smtSolverFactory);
            } else {
                preprocessedModel = originalProgram;
            }
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
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping = preprocessedModel.asPrismProgram().getLabelToExpressionMapping();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), pathFormula.getLeftSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping), pathFormula.getRightSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping));
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping = preprocessedModel.asPrismProgram().getLabelToExpressionMapping();
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), preprocessedModel.getManager().boolean(true), pathFormula.getSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping));
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
            
            boost::optional<storm::logic::Bound<ValueType>> const& bound = checkTask.getOptionalBound();
            if (bound) {
                if (player2Direction == storm::OptimizationDirection::Minimize && storm::logic::isLowerBound(bound.get().comparisonType)) {
                    if ((prob1 && initialStates) == initialStates) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::one<ValueType>());
                    } else if (checkTask.isQualitativeSet()) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), ValueType(0.5));
                    }
                } else if (player2Direction == storm::OptimizationDirection::Maximize && !storm::logic::isLowerBound(bound.get().comparisonType)) {
                    if ((prob0 && initialStates) == initialStates) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::zero<ValueType>());
                    } else if (checkTask.isQualitativeSet()) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), ValueType(0.5));
                    }
                }
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<Type> const& initialStates, detail::GameProb01ResultMinMax<Type> const& qualitativeResult) {
            // Check whether we can already give the answer based on the current information.
            std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, initialStates, qualitativeResult.prob0Min.getPlayer1States(), qualitativeResult.prob0Max.getPlayer1States(), true);
            if (result) {
                return result;
            }
            result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, initialStates, qualitativeResult.prob1Min.getPlayer1States(), qualitativeResult.prob1Max.getPlayer1States(), false);
            if (result) {
                return result;
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
            boost::optional<storm::logic::Bound<ValueType>> const& bound = checkTask.getOptionalBound();
            if (!bound) {
                return result;
            }
            
            storm::logic::ComparisonType comparisonType = bound.get().comparisonType;
            ValueType threshold = bound.get().threshold;
            
            if (storm::logic::isLowerBound(comparisonType)) {
                if (player2Direction == storm::OptimizationDirection::Minimize) {
                    if ((storm::logic::isStrict(comparisonType) && value > threshold)
                        || (!storm::logic::isStrict(comparisonType) && value >= threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                } else {
                    if ((storm::logic::isStrict(comparisonType) && value <= threshold)
                        || (!storm::logic::isStrict(comparisonType) && value < threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                }
            } else if (!storm::logic::isLowerBound(comparisonType)) {
                if (player2Direction == storm::OptimizationDirection::Maximize) {
                    if ((storm::logic::isStrict(comparisonType) && value < threshold) ||
                        (!storm::logic::isStrict(comparisonType) && value <= threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), value);
                    }
                } else {
                    if ((storm::logic::isStrict(comparisonType) && value >= threshold) ||
                        (!storm::logic::isStrict(comparisonType) && value > threshold)) {
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
        bool refineAfterQualitativeCheck(storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game, detail::GameProb01ResultMinMax<Type> const& qualitativeResult, storm::dd::Bdd<Type> const& transitionMatrixBdd) {
            STORM_LOG_TRACE("Trying refinement after qualitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = qualitativeResult.prob0Min.getPlayer1Strategy();
            storm::dd::Bdd<Type> minPlayer2Strategy = qualitativeResult.prob0Min.getPlayer2Strategy();
            storm::dd::Bdd<Type> maxPlayer1Strategy = qualitativeResult.prob1Max.getPlayer1Strategy();
            storm::dd::Bdd<Type> maxPlayer2Strategy = qualitativeResult.prob1Max.getPlayer2Strategy();
            
            // Redirect all player 1 choices of the min strategy to that of the max strategy if this leads to a player 2
            // state that is also a prob 0 state.
            minPlayer1Strategy = (maxPlayer1Strategy && qualitativeResult.prob0Min.getPlayer2States()).existsAbstract(game.getPlayer1Variables()).ite(maxPlayer1Strategy, minPlayer1Strategy);
            
            // Build the fragment of transitions that is reachable by both the min and the max strategies.
            storm::dd::Bdd<Type> reachableTransitions = transitionMatrixBdd && (minPlayer1Strategy || minPlayer2Strategy) && maxPlayer1Strategy && maxPlayer2Strategy;
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
                auto refinementStart = std::chrono::high_resolution_clock::now();
                abstractor.refine(pivotState, (pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
                auto refinementEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
                return true;
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);

                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Refining based on upper choice.");
                    auto refinementStart = std::chrono::high_resolution_clock::now();
                    abstractor.refine(pivotState, (pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                    auto refinementEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
                    return true;
                } else {
                    STORM_LOG_ASSERT(false, "Did not find choices from which to derive predicates.");
                }
            }
            return false;
        }

        template<storm::dd::DdType Type, typename ValueType>
        void refineAfterQuantitativeCheck(storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Add<Type, ValueType> const& minResult, storm::dd::Add<Type, ValueType> const& maxResult, detail::GameProb01ResultMinMax<Type> const& qualitativeResult, std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> const& minStrategyPair, std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> const& maxStrategyPair, storm::dd::Bdd<Type> const& transitionMatrixBdd) {
            STORM_LOG_TRACE("Refining after quantitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = minStrategyPair.first;
            storm::dd::Bdd<Type> minPlayer2Strategy = minStrategyPair.second;
            storm::dd::Bdd<Type> maxPlayer1Strategy = maxStrategyPair.first;
            storm::dd::Bdd<Type> maxPlayer2Strategy = maxStrategyPair.second;

            // TODO: fix min strategies to take the max strategies if possible.
            
            // Build the fragment of transitions that is reachable by both the min and the max strategies.
            storm::dd::Bdd<Type> reachableTransitions = transitionMatrixBdd && (minPlayer1Strategy || maxPlayer1Strategy) && minPlayer2Strategy && maxPlayer2Strategy;
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
            
            // Now construct all player 2 choices that actually exist and differ in the min and max case.
            constraint &= minPlayer2Strategy.exclusiveOr(maxPlayer2Strategy);

            // Then restrict the pivot states by requiring existing and different player 2 choices.
            pivotStates &= ((minPlayer1Strategy || maxPlayer1Strategy) && constraint).existsAbstract(game.getNondeterminismVariables());
            
            STORM_LOG_ASSERT(!pivotStates.isZero(), "Unable to refine without pivot state candidates.");
            
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
                auto refinementStart = std::chrono::high_resolution_clock::now();
                abstractor.refine(pivotState, (pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
                auto refinementEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
                
                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Refining based on upper choice.");
                    auto refinementStart = std::chrono::high_resolution_clock::now();
                    abstractor.refine(pivotState, (pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                    auto refinementEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
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
            storm::dd::Add<Type, ValueType> submatrix = maybeStatesAdd * game.getTransitionMatrix();
            storm::dd::Add<Type, ValueType> prob1StatesAsColumn = prob1States.template toAdd<ValueType>().swapVariables(game.getRowColumnMetaVariablePairs());
            storm::dd::Add<Type, ValueType> subvector = submatrix * prob1StatesAsColumn;
            subvector = subvector.sumAbstract(game.getColumnVariables());

            // Cut away all columns targeting non-maybe states.
            submatrix *= maybeStatesAdd.swapVariables(game.getRowColumnMetaVariablePairs());

            // Cut the starting vector to the maybe states of this query.
            storm::dd::Add<Type, ValueType> startVector;
            if (startInfo) {
                startVector = startInfo.get().values * maybeStatesAdd;
            } else {
                startVector = game.getManager().template getAddZero<ValueType>();
            }
            
            // Create the solver and solve the equation system.
            storm::utility::solver::SymbolicGameSolverFactory<Type, ValueType> solverFactory;
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> solver = solverFactory.create(submatrix, maybeStates, game.getIllegalPlayer1Mask(), game.getIllegalPlayer2Mask(), game.getRowVariables(), game.getColumnVariables(), game.getRowColumnMetaVariablePairs(), game.getPlayer1Variables(), game.getPlayer2Variables());
            solver->setGeneratePlayersStrategies(true);
            auto values = solver->solveGame(player1Direction, player2Direction, startVector, subvector, startInfo ? boost::make_optional(startInfo.get().player1Strategy) : boost::none, startInfo ? boost::make_optional(startInfo.get().player2Strategy) : boost::none);
            return MaybeStateResult<Type, ValueType>(values, solver->getPlayer1Strategy(), solver->getPlayer2Strategy());
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::InvalidPropertyException, "The game-based abstraction refinement model checker can only compute the result for the initial states.");

            // Optimization: do not compute both bounds if not necessary (e.g. if bound given and exceeded, etc.)
            
            // Set up initial predicates.
            std::vector<storm::expressions::Expression> initialPredicates = getInitialPredicates(constraintExpression, targetStateExpression);
            
            // Derive the optimization direction for player 1 (assuming menu-game abstraction).
            storm::OptimizationDirection player1Direction = getPlayer1Direction(checkTask);
            
            // Create the abstractor.
            storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType> abstractor(preprocessedModel.asPrismProgram(), initialPredicates, smtSolverFactory);
            
            // TODO: create refiner and move initial predicates there.
            
            // Enter the main-loop of abstraction refinement.
            for (uint_fast64_t iterations = 0; iterations < 10000; ++iterations) {
                auto iterationStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Starting iteration " << iterations << ".");

                // (1) build initial abstraction based on the the constraint expression (if not 'true') and the target state expression.
                storm::abstraction::MenuGame<Type, ValueType> game = abstractor.abstract();
                STORM_LOG_DEBUG("Abstraction in iteration " << iterations << " has " << game.getNumberOfStates() << " (player 1) states and " << game.getNumberOfTransitions() << " transitions.");
                STORM_LOG_THROW(game.getInitialStates().getNonZeroCount(), storm::exceptions::InvalidModelException, "Cannot treat models with more than one (abstract) initial state.");
                
                // (2) Prepare transition matrix BDD and target state BDD for later use.
                storm::dd::Bdd<Type> transitionMatrixBdd = game.getTransitionMatrix().toBdd();
                storm::dd::Bdd<Type> constraintStates = game.getStates(constraintExpression);
                storm::dd::Bdd<Type> targetStates = game.getStates(targetStateExpression);
                if (player1Direction == storm::OptimizationDirection::Minimize) {
                    targetStates |= game.getBottomStates();
                }
                
                // #ifdef LOCAL_DEBUG
                // abstractor.exportToDot("game" + std::to_string(iterations) + ".dot", targetStates, game.getManager().getBddOne());
                // #endif
                
                // (3) compute all states with probability 0/1 wrt. to the two different player 2 goals (min/max).
                auto qualitativeStart = std::chrono::high_resolution_clock::now();
                detail::GameProb01ResultMinMax<Type> qualitativeResult;
                std::unique_ptr<CheckResult> result = computeProb01States(checkTask, qualitativeResult, game, player1Direction, transitionMatrixBdd, game.getInitialStates(), constraintStates, targetStates);
                if (result) {
                    return result;
                }
                auto qualitativeEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Qualitative computation completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(qualitativeEnd - qualitativeStart).count() << "ms.");
                
                // (4) compute the states for which we have to determine quantitative information.
                storm::dd::Bdd<Type> maybeMin = !(qualitativeResult.prob0Min.getPlayer1States() || qualitativeResult.prob1Min.getPlayer1States()) && game.getReachableStates();
                storm::dd::Bdd<Type> maybeMax = !(qualitativeResult.prob0Max.getPlayer1States() || qualitativeResult.prob1Max.getPlayer1States()) && game.getReachableStates();
                
                // (5) if the initial states are not maybe states, then we can refine at this point.
                storm::dd::Bdd<Type> initialMaybeStates = (game.getInitialStates() && maybeMin) || (game.getInitialStates() && maybeMax);
                bool qualitativeRefinement = false;
                if (initialMaybeStates.isZero()) {
                    // In this case, we know the result for the initial states for both player 2 minimizing and maximizing.
                    STORM_LOG_TRACE("No initial state is a 'maybe' state. Refining abstraction based on qualitative check.");
                    
                    result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getInitialStates(), qualitativeResult);
                    if (result) {
                        return result;
                    } else {
                        STORM_LOG_DEBUG("Obtained qualitative bounds [0, 1] on the actual value for the initial states.");
                        
                        // If we get here, the initial states were all identified as prob0/1 states, but the value (0 or 1)
                        // depends on whether player 2 is minimizing or maximizing. Therefore, we need to find a place to refine.
                        qualitativeRefinement = refineAfterQualitativeCheck(abstractor, game, qualitativeResult, transitionMatrixBdd);
                    }
                }
                
                if (!qualitativeRefinement) {
                    // At this point, we know that we cannot answer the query without further numeric computation.
                    STORM_LOG_TRACE("Starting numerical solution step.");
                    auto quantitativeStart = std::chrono::high_resolution_clock::now();
                    
                    // Prepare some storage that we use on-demand during the quantitative solving step.
                    storm::dd::Add<Type, ValueType> minResult = qualitativeResult.prob1Min.getPlayer1States().template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> maxResult = qualitativeResult.prob1Max.getPlayer1States().template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> initialStatesAdd = game.getInitialStates().template toAdd<ValueType>();

					storm::dd::Bdd<Type> combinedMinPlayer1QualitativeStrategies = (qualitativeResult.prob0Min.getPlayer1Strategy() || qualitativeResult.prob1Min.getPlayer1Strategy());
					storm::dd::Bdd<Type> combinedMinPlayer2QualitativeStrategies = (qualitativeResult.prob0Min.getPlayer2Strategy() || qualitativeResult.prob1Min.getPlayer2Strategy());

                    // The minimal value after qualitative checking can only be zero. It it was 1, we could have given
                    // the result right away.
                    ValueType minValue = storm::utility::zero<ValueType>();
                    MaybeStateResult<Type, ValueType> minMaybeStateResult(game.getManager().template getAddZero<ValueType>(), game.getManager().getBddZero(), game.getManager().getBddZero());
                    auto minStart = std::chrono::high_resolution_clock::now();
                    if (!maybeMin.isZero()) {
                        minMaybeStateResult = solveMaybeStates(player1Direction, storm::OptimizationDirection::Minimize, game, maybeMin, qualitativeResult.prob1Min.getPlayer1States());
						minMaybeStateResult.player1Strategy &= game.getReachableStates();
						minMaybeStateResult.player2Strategy &= game.getReachableStates();
                        minResult += minMaybeStateResult.values;
                        storm::dd::Add<Type, ValueType> initialStateMin = initialStatesAdd * minResult;
                        // Here we can only require a non-zero count of *at most* one, because the result may actually be 0.
                        STORM_LOG_ASSERT(initialStateMin.getNonZeroCount() <= 1, "Wrong number of results for initial states. Expected <= 1, but got " << initialStateMin.getNonZeroCount() << ".");
                        minValue = initialStateMin.getMax();
                    }
                    auto minEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Obtained quantitative lower bound " << minValue << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(minEnd - minStart).count() << "ms.");
                    
					minMaybeStateResult.player1Strategy = combinedMinPlayer1QualitativeStrategies.existsAbstract(game.getPlayer1Variables()).ite(combinedMinPlayer1QualitativeStrategies, minMaybeStateResult.player1Strategy);
					minMaybeStateResult.player2Strategy = combinedMinPlayer2QualitativeStrategies.existsAbstract(game.getPlayer2Variables()).ite(combinedMinPlayer2QualitativeStrategies, minMaybeStateResult.player2Strategy);
					
                    // Check whether we can abort the computation because of the lower value.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Minimize, minValue);
                    if (result) {
                        return result;
                    }

					storm::dd::Bdd<Type> combinedMaxPlayer1QualitativeStrategies = (qualitativeResult.prob0Max.getPlayer1Strategy() || qualitativeResult.prob1Max.getPlayer1Strategy());
					storm::dd::Bdd<Type> combinedMaxPlayer2QualitativeStrategies = (qualitativeResult.prob0Max.getPlayer2Strategy() || qualitativeResult.prob1Max.getPlayer2Strategy());

                    // Likewise, the maximal value after qualitative checking can only be 1. If it was 0, we could have
                    // given the result right awy.
                    ValueType maxValue = storm::utility::one<ValueType>();
                    auto maxStart = std::chrono::high_resolution_clock::now();
                    MaybeStateResult<Type, ValueType> maxMaybeStateResult(game.getManager().template getAddZero<ValueType>(), game.getManager().getBddZero(), game.getManager().getBddZero());
                    if (!maybeMax.isZero()) {
                        maxMaybeStateResult = solveMaybeStates(player1Direction, storm::OptimizationDirection::Maximize, game, maybeMax, qualitativeResult.prob1Max.getPlayer1States(), boost::make_optional(minMaybeStateResult));
						maxMaybeStateResult.player1Strategy &= game.getReachableStates();
						maxMaybeStateResult.player2Strategy &= game.getReachableStates();
                        maxResult += maxMaybeStateResult.values;
                        storm::dd::Add<Type, ValueType> initialStateMax = (initialStatesAdd * maxResult);
                        // Unlike in the min-case, we can require a non-zero count of 1 here, because if the max was in
                        // fact 0, the result would be 0, which would have been detected earlier by the graph algorithms.
                        STORM_LOG_ASSERT(initialStateMax.getNonZeroCount() == 1, "Wrong number of results for initial states. Expected 1, but got " << initialStateMax.getNonZeroCount() << ".");
                        maxValue = initialStateMax.getMax();
                    }
                    auto maxEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Obtained quantitative upper bound " << maxValue << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(maxEnd - maxStart).count() << "ms.");

					maxMaybeStateResult.player1Strategy = combinedMaxPlayer1QualitativeStrategies.existsAbstract(game.getPlayer1Variables()).ite(combinedMaxPlayer1QualitativeStrategies, maxMaybeStateResult.player1Strategy);
					maxMaybeStateResult.player2Strategy = combinedMaxPlayer2QualitativeStrategies.existsAbstract(game.getPlayer2Variables()).ite(combinedMaxPlayer2QualitativeStrategies, maxMaybeStateResult.player2Strategy);

                    // Check whether we can abort the computation because of the upper value.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Maximize, maxValue);
                    if (result) {
                        return result;
                    }
                    
                    auto quantitativeEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_DEBUG("Obtained quantitative bounds [" << minValue << ", " << maxValue << "] on the actual value for the initial states in " << std::chrono::duration_cast<std::chrono::milliseconds>(quantitativeEnd - quantitativeStart).count() << "ms.");

                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, minValue, maxValue);
                    if (result) {
                        return result;
                    }

                    // If we arrived at this point, it means that we have all qualitative and quantitative information
                    // about the game, but we could not yet answer the query. In this case, we need to refine.
                    
                    // Make sure that all strategies are still valid strategies.
                    STORM_LOG_ASSERT(minMaybeStateResult.player1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for min is illegal.");
                    STORM_LOG_ASSERT(maxMaybeStateResult.player1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for max is illegal.");
                    STORM_LOG_ASSERT(minMaybeStateResult.player2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for min is illegal.");
                    STORM_LOG_ASSERT(maxMaybeStateResult.player2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for max is illegal.");

					// Check whether the strategies coincide over the reachable parts.
					storm::dd::Bdd<Type> tmp = game.getTransitionMatrix().toBdd() && (minMaybeStateResult.player1Strategy || maxMaybeStateResult.player1Strategy) && (minMaybeStateResult.player2Strategy || maxMaybeStateResult.player2Strategy);
					storm::dd::Bdd<Type> commonReach = storm::utility::dd::computeReachableStates(game.getInitialStates(), tmp.existsAbstract(game.getNondeterminismVariables()), game.getRowVariables(), game.getColumnVariables());
					STORM_LOG_ASSERT((commonReach && minMaybeStateResult.player1Strategy) != (commonReach && maxMaybeStateResult.player1Strategy) || (commonReach && minMaybeStateResult.player2Strategy) != (commonReach && maxMaybeStateResult.player2Strategy), "The strategies fully coincide.");

                    refineAfterQuantitativeCheck(abstractor, game, minResult, maxResult, qualitativeResult, std::make_pair(minMaybeStateResult.player1Strategy, minMaybeStateResult.player2Strategy), std::make_pair(maxMaybeStateResult.player1Strategy, maxMaybeStateResult.player2Strategy), transitionMatrixBdd);
                }
                auto iterationEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Iteration " << iterations << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(iterationEnd - iterationStart).count() << "ms.");
            }

            STORM_LOG_ASSERT(false, "This point must not be reached.");
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::vector<storm::expressions::Expression> GameBasedMdpModelChecker<Type, ModelType>::getInitialPredicates(storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            std::vector<storm::expressions::Expression> initialPredicates;
            initialPredicates.push_back(targetStateExpression);
            if (!constraintExpression.isTrue() && !constraintExpression.isFalse()) {
                initialPredicates.push_back(constraintExpression);
            }
            return initialPredicates;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        storm::OptimizationDirection GameBasedMdpModelChecker<Type, ModelType>::getPlayer1Direction(CheckTask<storm::logic::Formula> const& checkTask) {
            if (preprocessedModel.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC) {
                return storm::OptimizationDirection::Maximize;
            } else if (checkTask.isOptimizationDirectionSet()) {
                return checkTask.getOptimizationDirection();
            } else if (checkTask.isBoundSet() && preprocessedModel.getModelType() != storm::storage::SymbolicModelDescription::ModelType::DTMC) {
                return storm::logic::isLowerBound(checkTask.getBoundComparisonType()) ? storm::OptimizationDirection::Minimize : storm::OptimizationDirection::Maximize;
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Could not derive player 1 optimization direction.");
            return storm::OptimizationDirection::Maximize;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::computeProb01States(CheckTask<storm::logic::Formula> const& checkTask, detail::GameProb01ResultMinMax<Type>& qualitativeResult, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::OptimizationDirection player1Direction, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates) {
            qualitativeResult.prob0Min = computeProb01States(true, player1Direction, storm::OptimizationDirection::Minimize, game, transitionMatrixBdd, constraintStates, targetStates);
            qualitativeResult.prob1Min = computeProb01States(false, player1Direction, storm::OptimizationDirection::Minimize, game, transitionMatrixBdd, constraintStates, targetStates);
            std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Minimize, initialStates, qualitativeResult.prob0Min.getPlayer1States(), qualitativeResult.prob1Min.getPlayer1States());
            if (result) {
                return result;
            }
            
            qualitativeResult.prob0Max = computeProb01States(true, player1Direction, storm::OptimizationDirection::Maximize, game, transitionMatrixBdd, constraintStates, targetStates);
            qualitativeResult.prob1Max = computeProb01States(false, player1Direction, storm::OptimizationDirection::Maximize, game, transitionMatrixBdd, constraintStates, targetStates);
            result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Maximize, initialStates, qualitativeResult.prob0Max.getPlayer1States(), qualitativeResult.prob1Max.getPlayer1States());
            if (result) {
                return result;
            }
            return result;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        storm::utility::graph::GameProb01Result<Type> GameBasedMdpModelChecker<Type, ModelType>::computeProb01States(bool prob0, storm::OptimizationDirection player1Direction, storm::OptimizationDirection player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates) {
            storm::utility::graph::GameProb01Result<Type> result;
            if (prob0) {
                result = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, true, true);
            } else {
                result = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, true, true);
            }
            
            STORM_LOG_ASSERT(result.hasPlayer1Strategy() && (result.getPlayer1States().isZero() || !result.getPlayer1Strategy().isZero()), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(result.hasPlayer2Strategy() && (result.getPlayer2States().isZero() || !result.getPlayer2Strategy().isZero()), "Unable to proceed without strategy.");

            STORM_LOG_TRACE("Computed states with probability " << (prob0 ? "0" : "1") << " (player 1: " << player1Direction << ", player 2: " << player2Direction << "): " << result.getPlayer1States().getNonZeroCount() << " '" << (prob0 ? "no" : "yes") << "' states.");
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        storm::expressions::Expression GameBasedMdpModelChecker<Type, ModelType>::getExpression(storm::logic::Formula const& formula) {
            STORM_LOG_THROW(formula.isBooleanLiteralFormula() || formula.isAtomicExpressionFormula() || formula.isAtomicLabelFormula(), storm::exceptions::InvalidPropertyException, "The target states have to be given as label or an expression.");
            storm::expressions::Expression result;
            if (formula.isAtomicLabelFormula()) {
                result = preprocessedModel.asPrismProgram().getLabelExpression(formula.asAtomicLabelFormula().getLabel());
            } else if (formula.isAtomicExpressionFormula()) {
                result = formula.asAtomicExpressionFormula().getExpression();
            } else {
                result = formula.asBooleanLiteralFormula().isTrueFormula() ? preprocessedModel.getManager().boolean(true) : preprocessedModel.getManager().boolean(false);
            }
            return result;
        }
        
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Dtmc<storm::dd::DdType::CUDD, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, storm::models::symbolic::Mdp<storm::dd::DdType::CUDD, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Dtmc<storm::dd::DdType::Sylvan, double>>;
        template class GameBasedMdpModelChecker<storm::dd::DdType::Sylvan, storm::models::symbolic::Mdp<storm::dd::DdType::Sylvan, double>>;
    }
}
