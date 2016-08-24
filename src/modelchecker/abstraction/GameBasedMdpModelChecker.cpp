#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/dd/DdManager.h"

#include "src/abstraction/prism/PrismMenuGameAbstractor.h"

#include "src/logic/FragmentSpecification.h"

#include "src/solver/SymbolicGameSolver.h"

#include "src/utility/solver.h"
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
        storm::dd::Add<Type, ValueType> solveMaybeStates(storm::OptimizationDirection const& player1Direction, storm::OptimizationDirection const& player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& maybeStates, storm::dd::Bdd<Type> const& prob1States, boost::optional<storm::dd::Add<Type, ValueType>> startVector = boost::none) {
            
            // Compute the ingredients of the equation system.
            storm::dd::Add<Type, ValueType> maybeStatesAdd = maybeStates.template toAdd<ValueType>();
            storm::dd::Add<Type, ValueType> submatrix = maybeStatesAdd * game.getTransitionMatrix();
            storm::dd::Add<Type, ValueType> prob1StatesAsColumn = prob1States.template toAdd<ValueType>().swapVariables(game.getRowColumnMetaVariablePairs());
            storm::dd::Add<Type, ValueType> subvector = submatrix * prob1StatesAsColumn;
            subvector = subvector.sumAbstract(game.getColumnVariables());

            // Cut away all columns targeting non-maybe states.
            submatrix *= maybeStatesAdd.swapVariables(game.getRowColumnMetaVariablePairs());

            // Cut the starting vector to the maybe states of this query.
            if (startVector) {
                startVector.get() *= maybeStatesAdd;
            }
            
            // Create the solver and solve the equation system.
            storm::utility::solver::SymbolicGameSolverFactory<Type, ValueType> solverFactory;
            std::unique_ptr<storm::solver::SymbolicGameSolver<Type, ValueType>> solver = solverFactory.create(submatrix, maybeStates, game.getIllegalPlayer1Mask(), game.getIllegalPlayer2Mask(), game.getRowVariables(), game.getColumnVariables(), game.getRowColumnMetaVariablePairs(), game.getPlayer1Variables(), game.getPlayer2Variables());
            return solver->solveGame(player1Direction, player2Direction, startVector ? startVector.get() : game.getManager().template getAddZero<ValueType>(), subvector);
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
                STORM_LOG_TRACE("Starting iteration " << iterations);
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
                std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Minimize, game.getInitialStates(), prob01.min.first.states, prob01.min.second.states);
                if (result) {
                    return result;
                }
                prob01.max = computeProb01States(player1Direction, storm::OptimizationDirection::Maximize, game, transitionMatrixBdd, game.getStates(constraintExpression), targetStates);
                result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Maximize, game.getInitialStates(), prob01.max.first.states, prob01.max.second.states);
                if (result) {
                    return result;
                }
                
                // 3. compute the states for which we know the result/for which we know there is more work to be done.
                storm::dd::Bdd<Type> maybeMin = !(prob01.min.first.states || prob01.min.second.states) && game.getReachableStates();
                storm::dd::Bdd<Type> maybeMax = !(prob01.max.first.states || prob01.max.second.states) && game.getReachableStates();
                
                maybeMin.template toAdd<ValueType>().exportToDot("maybemin.dot");
                maybeMax.template toAdd<ValueType>().exportToDot("maybemax.dot");
                game.getInitialStates().template toAdd<ValueType>().exportToDot("init.dot");
                
                // 4. if the initial states are not maybe states, then we can refine at this point.
                storm::dd::Bdd<Type> initialMaybeStates = (game.getInitialStates() && maybeMin) || (game.getInitialStates() && maybeMax);
                initialMaybeStates.template toAdd<ValueType>().exportToDot("initmaybe.dot");
                if (initialMaybeStates.isZero()) {
                    // In this case, we know the result for the initial states for both player 2 minimizing and maximizing.
                    STORM_LOG_TRACE("No initial state is a 'maybe' state. Refining abstraction based on qualitative check.");
                    
                    // Check whether we can already give the answer based on the current information.
                    result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getInitialStates(), prob01.min.first.states, prob01.max.first.states, true);
                    if (result) {
                        return result;
                    }
                    result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, game.getInitialStates(), prob01.min.second.states, prob01.max.second.states, false);
                    if (result) {
                        return result;
                    }
                    
                    STORM_LOG_DEBUG("Obtained qualitative bounds [0, 1] on the actual value for the initial states.");

                    // If we get here, the initial states were all identified as prob0/1 states, but the value (0 or 1)
                    // depends on whether player 2 is minimizing or maximizing. Therefore, we need to find a place to refine.
                    refineAfterQualitativeCheck(abstractor, game, prob01, transitionMatrixBdd);
                } else {

                    // At this point, we know that we cannot answer the query without further numeric computation.
                    STORM_LOG_TRACE("Starting numerical solution step.");
                    
                    // Prepare some storage that we use on-demand during the quantitative solving step.
                    storm::dd::Add<Type, ValueType> minResult = prob01.min.second.states.template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> maxResult = prob01.max.second.states.template toAdd<ValueType>();
                    storm::dd::Add<Type, ValueType> initialStatesAdd = game.getInitialStates().template toAdd<ValueType>();

                    ValueType minValue = (prob01.min.second.states && game.getInitialStates()) == game.getInitialStates() ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>();
                    if (!maybeMin.isZero()) {
                        minResult += solveMaybeStates(player1Direction, storm::OptimizationDirection::Minimize, game, maybeMin, prob01.min.second.states);
                        storm::dd::Add<Type, ValueType> initialStateMin = initialStatesAdd * minResult;
                        STORM_LOG_ASSERT(initialStateMin.getNonZeroCount() == 1, "Wrong number of results for initial states.");
                        minValue = initialStateMin.getMax();
                    }
                    STORM_LOG_TRACE("Obtained quantitative lower bound " << minValue);
                    
                    // Check whether we can abort the computation because of the lower value.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Minimize, minValue);
                    if (result) {
                        return result;
                    }
                    
                    ValueType maxValue = (prob01.max.second.states && game.getInitialStates()) == game.getInitialStates() ? storm::utility::one<ValueType>() : storm::utility::zero<ValueType>();
                    if (!maybeMax.isZero()) {
                        maxResult += solveMaybeStates(player1Direction, storm::OptimizationDirection::Maximize, game, maybeMax, prob01.max.second.states, boost::optional<storm::dd::Add<Type, ValueType>>(minResult));
                        storm::dd::Add<Type, ValueType> initialStateMax = (initialStatesAdd * maxResult);
                        STORM_LOG_ASSERT(initialStateMax.getNonZeroCount() == 1, "Wrong number of results for initial states.");
                        maxValue = initialStateMax.getMax();
                    }
                    STORM_LOG_TRACE("Obtained quantitative upper bound " << minValue);

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
                    
                    STORM_LOG_ASSERT(false, "Quantiative refinement not yet there. :)");
                }
            }

            STORM_LOG_ASSERT(false, "This point must not be reached.");
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::pair<storm::utility::graph::GameProb01Result<Type>, storm::utility::graph::GameProb01Result<Type>> GameBasedMdpModelChecker<Type, ValueType>::computeProb01States(storm::OptimizationDirection player1Direction, storm::OptimizationDirection player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates) {
            // Compute the states with probability 0/1.
            storm::utility::graph::GameProb01Result<Type> prob0 = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, player2Direction == storm::OptimizationDirection::Minimize, player2Direction == storm::OptimizationDirection::Minimize);
            storm::utility::graph::GameProb01Result<Type> prob1 = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, player2Direction, player2Direction == storm::OptimizationDirection::Maximize, player2Direction == storm::OptimizationDirection::Maximize);
            
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Minimize || prob0.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Minimize || prob0.hasPlayer2Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Maximize || prob1.hasPlayer1Strategy(), "Unable to proceed without strategy.");
            STORM_LOG_ASSERT(player2Direction != storm::OptimizationDirection::Maximize || prob1.hasPlayer2Strategy(), "Unable to proceed without strategy.");

            STORM_LOG_TRACE("Player 1: " << player1Direction << ", player 2: " << player2Direction << ": " << prob0.states.getNonZeroCount() << " 'no' states, " << prob1.states.getNonZeroCount() << " 'yes' states.");
            return std::make_pair(prob0, prob1);
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