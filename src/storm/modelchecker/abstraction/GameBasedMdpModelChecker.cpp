#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/models/symbolic/StandardRewardModel.h"
#include "storm/models/symbolic/Dtmc.h"
#include "storm/models/symbolic/Mdp.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/VariableSetPredicateSplitter.h"


#include "storm/storage/jani/Edge.h"
#include "storm/storage/jani/EdgeDestination.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Automaton.h"
#include "storm/storage/jani/Location.h"
#include "storm/storage/jani/AutomatonComposition.h"
#include "storm/storage/jani/ParallelComposition.h"
#include "storm/storage/jani/CompositionInformationVisitor.h"

#include "storm/storage/dd/DdManager.h"

#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"
#include "storm/abstraction/jani/JaniMenuGameAbstractor.h"
#include "storm/abstraction/MenuGameRefiner.h"

#include "storm/logic/FragmentSpecification.h"

#include "storm/solver/SymbolicGameSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/utility/solver.h"
#include "storm/utility/prism.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidModelException.h"

#include "storm/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        
        using storm::abstraction::QuantitativeResult;
        using storm::abstraction::QuantitativeResultMinMax;
        
        template<storm::dd::DdType Type, typename ModelType>
        GameBasedMdpModelChecker<Type, ModelType>::GameBasedMdpModelChecker(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory) : smtSolverFactory(smtSolverFactory), comparator(storm::settings::getModule<storm::settings::modules::AbstractionSettings>().getPrecision()), reuseQualitativeResults(false), reuseQuantitativeResults(false) {
            model.requireNoUndefinedConstants();
            if (model.isPrismProgram()) {
                storm::prism::Program const& originalProgram = model.asPrismProgram();
                STORM_LOG_THROW(originalProgram.getModelType() == storm::prism::Program::ModelType::DTMC || originalProgram.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only DTMCs/MDPs are supported by the game-based model checker.");
                
                // Flatten the modules if there is more than one.
                if (originalProgram.getNumberOfModules() > 1) {
                    preprocessedModel = originalProgram.flattenModules(this->smtSolverFactory);
                } else {
                    preprocessedModel = originalProgram;
                }
                
                STORM_LOG_TRACE("Game-based model checker got program " << preprocessedModel.asPrismProgram());
            } else {
                storm::jani::Model const& originalModel = model.asJaniModel();
                STORM_LOG_THROW(originalModel.getModelType() == storm::jani::ModelType::DTMC || originalModel.getModelType() == storm::jani::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only DTMCs/MDPs are supported by the game-based model checker.");
                
                // Flatten the parallel composition.
                preprocessedModel = model.asJaniModel().flattenComposition();
            }
            
            storm::settings::modules::AbstractionSettings::ReuseMode reuseMode = storm::settings::getModule<storm::settings::modules::AbstractionSettings>().getReuseMode();
            reuseQualitativeResults = reuseMode == storm::settings::modules::AbstractionSettings::ReuseMode::All || reuseMode == storm::settings::modules::AbstractionSettings::ReuseMode::Qualitative;
            reuseQuantitativeResults = reuseMode == storm::settings::modules::AbstractionSettings::ReuseMode::All || reuseMode == storm::settings::modules::AbstractionSettings::ReuseMode::Quantitative;
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
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping;
            if (preprocessedModel.isPrismProgram()) {
                labelToExpressionMapping = preprocessedModel.asPrismProgram().getLabelToExpressionMapping();
            } else {
                storm::jani::Model const& janiModel = preprocessedModel.asJaniModel();
                for (auto const& variable : janiModel.getGlobalVariables().getBooleanVariables()) {
                    if (variable.isTransient()) {
                        labelToExpressionMapping[variable.getName()] = janiModel.getLabelExpression(variable.asBooleanVariable());
                    }
                }
            }
            
            storm::expressions::Expression constraintExpression = pathFormula.getLeftSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping);
            storm::expressions::Expression targetStateExpression = pathFormula.getRightSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping);
            
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), constraintExpression, targetStateExpression);
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ModelType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            std::map<std::string, storm::expressions::Expression> labelToExpressionMapping;
            if (preprocessedModel.isPrismProgram()) {
                labelToExpressionMapping = preprocessedModel.asPrismProgram().getLabelToExpressionMapping();
            } else {
                storm::jani::Model const& janiModel = preprocessedModel.asJaniModel();
                for (auto const& variable : janiModel.getGlobalVariables().getBooleanVariables()) {
                    if (variable.isTransient()) {
                        labelToExpressionMapping[variable.getName()] = janiModel.getLabelExpression(variable.asBooleanVariable());
                    }
                }
            }
            
            storm::expressions::Expression constraintExpression = preprocessedModel.getManager().boolean(true);
            storm::expressions::Expression targetStateExpression = pathFormula.getSubformula().toExpression(preprocessedModel.getManager(), labelToExpressionMapping);
            
            return performGameBasedAbstractionRefinement(checkTask.substituteFormula<storm::logic::Formula>(pathFormula), constraintExpression, targetStateExpression);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::OptimizationDirection player2Direction, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& prob0, storm::dd::Bdd<Type> const& prob1) {
            std::unique_ptr<CheckResult> result;
            
            if (checkTask.isBoundSet()) {
                // Despite having a bound, we create a quanitative result so that the next layer can perform the comparison.
                
                if (player2Direction == storm::OptimizationDirection::Minimize) {
                    if (storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                        if ((prob1 && initialStates) == initialStates) {
                            result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::one<ValueType>());
                        }
                    } else {
                        if (!(prob1 && initialStates).isZero()) {
                            result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::one<ValueType>());
                        }
                    }
                } else if (player2Direction == storm::OptimizationDirection::Maximize) {
                    if (!storm::logic::isLowerBound(checkTask.getBoundComparisonType())) {
                        if ((prob0 && initialStates) == initialStates) {
                            result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::zero<ValueType>());
                        }
                    } else {
                        if (!(prob0 && initialStates).isZero()) {
                            result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), storm::utility::zero<ValueType>());
                        }
                    }
                }
            }
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQualitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::dd::Bdd<Type> const& initialStates, QualitativeResultMinMax<Type> const& qualitativeResult) {
            // Check whether we can already give the answer based on the current information.
            std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Minimize, initialStates, qualitativeResult.prob0Min.getPlayer1States(), qualitativeResult.prob1Min.getPlayer1States());
            if (result) {
                return result;
            }
            result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, storm::OptimizationDirection::Maximize, initialStates, qualitativeResult.prob0Max.getPlayer1States(), qualitativeResult.prob1Max.getPlayer1States());
            if (result) {
                return result;
            }
            return result;
        }

        template<typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQuantitativeCheck(CheckTask<storm::logic::Formula> const& checkTask, storm::OptimizationDirection const& player2Direction, std::pair<ValueType, ValueType> const& initialValueRange) {
            std::unique_ptr<CheckResult> result;
            
            // If the minimum value exceeds an upper threshold or the maximum value is below a lower threshold, we can
            // return the value because the property will definitely hold. Vice versa, if the minimum value exceeds an
            // upper bound or the maximum value is below a lower bound, the property will definitely not hold and we can
            // return the value.
            if (!checkTask.isBoundSet()) {
                return result;
            }
            
            ValueType const& lowerValue = initialValueRange.first;
            ValueType const& upperValue = initialValueRange.second;
            
            storm::logic::ComparisonType comparisonType = checkTask.getBoundComparisonType();
            ValueType threshold = checkTask.getBoundThreshold();
            
            if (storm::logic::isLowerBound(comparisonType)) {
                if (player2Direction == storm::OptimizationDirection::Minimize) {
                    if ((storm::logic::isStrict(comparisonType) && lowerValue > threshold)
                        || (!storm::logic::isStrict(comparisonType) && lowerValue >= threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), lowerValue);
                    }
                } else {
                    if ((storm::logic::isStrict(comparisonType) && upperValue <= threshold)
                        || (!storm::logic::isStrict(comparisonType) && upperValue < threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), upperValue);
                    }
                }
            } else {
                if (player2Direction == storm::OptimizationDirection::Maximize) {
                    if ((storm::logic::isStrict(comparisonType) && upperValue < threshold) ||
                        (!storm::logic::isStrict(comparisonType) && upperValue <= threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), upperValue);
                    }
                } else {
                    if ((storm::logic::isStrict(comparisonType) && lowerValue >= threshold) ||
                        (!storm::logic::isStrict(comparisonType) && lowerValue > threshold)) {
                        result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), lowerValue);
                    }
                }
            }
            
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> checkForResultAfterQuantitativeCheck(ValueType const& minValue, ValueType const& maxValue, storm::utility::ConstantsComparator<ValueType> const& comparator) {
            std::unique_ptr<CheckResult> result;

            // If the lower and upper bounds are close enough, we can return the result.
            if (comparator.isEqual(minValue, maxValue)) {
                result = std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), (minValue + maxValue) / ValueType(2));
            }

            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        QuantitativeResult<Type, ValueType> solveMaybeStates(storm::OptimizationDirection const& player1Direction, storm::OptimizationDirection const& player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& maybeStates, storm::dd::Bdd<Type> const& prob1States, boost::optional<QuantitativeResult<Type, ValueType>> const& startInfo = boost::none) {
            
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
            return QuantitativeResult<Type, ValueType>(std::make_pair(storm::utility::zero<ValueType>(), storm::utility::one<ValueType>()), values, solver->getPlayer1Strategy(), solver->getPlayer2Strategy());
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        QuantitativeResult<Type, ValueType> computeQuantitativeResult(storm::OptimizationDirection player1Direction, storm::OptimizationDirection player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, QualitativeResultMinMax<Type> const& qualitativeResult, storm::dd::Add<Type, ValueType> const& initialStatesAdd, storm::dd::Bdd<Type> const& maybeStates, boost::optional<QuantitativeResult<Type, ValueType>> const& startInfo = boost::none) {
            
            bool min = player2Direction == storm::OptimizationDirection::Minimize;
            QuantitativeResult<Type, ValueType> result;
            
            // We fix the strategies. That is, we take the decisions of the strategies obtained in the qualitiative
            // preprocessing if possible.
            storm::dd::Bdd<Type> combinedPlayer1QualitativeStrategies;
            storm::dd::Bdd<Type> combinedPlayer2QualitativeStrategies;
            if (min) {
                combinedPlayer1QualitativeStrategies = (qualitativeResult.prob0Min.getPlayer1Strategy() || qualitativeResult.prob1Min.getPlayer1Strategy());
                combinedPlayer2QualitativeStrategies = (qualitativeResult.prob0Min.getPlayer2Strategy() || qualitativeResult.prob1Min.getPlayer2Strategy());
            } else {
                combinedPlayer1QualitativeStrategies = (qualitativeResult.prob0Max.getPlayer1Strategy() || qualitativeResult.prob1Max.getPlayer1Strategy());
                combinedPlayer2QualitativeStrategies = (qualitativeResult.prob0Max.getPlayer2Strategy() || qualitativeResult.prob1Max.getPlayer2Strategy());
            }
            
            result.player1Strategy = combinedPlayer1QualitativeStrategies;
            result.player2Strategy = combinedPlayer2QualitativeStrategies;
            result.values = game.getManager().template getAddZero<ValueType>();
            
            auto start = std::chrono::high_resolution_clock::now();
            if (!maybeStates.isZero()) {
                STORM_LOG_TRACE("Solving " << maybeStates.getNonZeroCount() << " maybe states.");
                
                // Solve the quantitative values of maybe states.
                result = solveMaybeStates(player1Direction, player2Direction, game, maybeStates, min ? qualitativeResult.prob1Min.getPlayer1States() : qualitativeResult.prob1Max.getPlayer1States(), startInfo);
                
                // Cut the obtained strategies to the reachable states of the game.
                result.player1Strategy &= game.getReachableStates();
                result.player2Strategy &= game.getReachableStates();
                
                // Extend the values of the maybe states by the qualitative values.
                result.values += min ? qualitativeResult.prob1Min.getPlayer1States().template toAdd<ValueType>() : qualitativeResult.prob1Max.getPlayer1States().template toAdd<ValueType>();
            } else {
                STORM_LOG_TRACE("No maybe states.");

                // Extend the values of the maybe states by the qualitative values.
                result.values += min ? qualitativeResult.prob1Min.getPlayer1States().template toAdd<ValueType>() : qualitativeResult.prob1Max.getPlayer1States().template toAdd<ValueType>();
            }
            
            // Construct an ADD holding the initial values of initial states and extract the bound on the initial states.
            storm::dd::Add<Type, ValueType> initialStateValueAdd = initialStatesAdd * result.values;
            
            ValueType maxValueOverInitialStates = initialStateValueAdd.getMax();
            initialStateValueAdd += (!game.getInitialStates()).template toAdd<ValueType>();
            ValueType minValueOverInitialStates = initialStateValueAdd.getMin();
            
            result.initialStatesRange = std::make_pair(minValueOverInitialStates, maxValueOverInitialStates);
            
            result.player1Strategy = combinedPlayer1QualitativeStrategies.existsAbstract(game.getPlayer1Variables()).ite(combinedPlayer1QualitativeStrategies, result.player1Strategy);
            result.player2Strategy = combinedPlayer2QualitativeStrategies.existsAbstract(game.getPlayer2Variables()).ite(combinedPlayer2QualitativeStrategies, result.player2Strategy);
            
            auto end = std::chrono::high_resolution_clock::now();
            STORM_LOG_TRACE("Obtained quantitative " << (min ? "lower" : "upper") << " bound " << (min ? result.initialStatesRange.first : result.initialStatesRange.second) << " in " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms.");
            
            return result;
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
            std::shared_ptr<storm::abstraction::MenuGameAbstractor<Type, ValueType>> abstractor;
            if (preprocessedModel.isPrismProgram()) {
                abstractor = std::make_shared<storm::abstraction::prism::PrismMenuGameAbstractor<Type, ValueType>>(preprocessedModel.asPrismProgram(), smtSolverFactory);
            } else {
                abstractor = std::make_shared<storm::abstraction::jani::JaniMenuGameAbstractor<Type, ValueType>>(preprocessedModel.asJaniModel(), smtSolverFactory);
            }
            
            // Create a refiner that can be used to refine the abstraction when needed.
            storm::abstraction::MenuGameRefiner<Type, ValueType> refiner(*abstractor, smtSolverFactory->create(preprocessedModel.getManager()));
            refiner.refine(initialPredicates);
            
            storm::dd::Bdd<Type> globalConstraintStates = abstractor->getStates(constraintExpression);
            storm::dd::Bdd<Type> globalTargetStates = abstractor->getStates(targetStateExpression);
            
            // Enter the main-loop of abstraction refinement.
            boost::optional<QualitativeResultMinMax<Type>> previousQualitativeResult = boost::none;
            boost::optional<QuantitativeResult<Type, ValueType>> previousMinQuantitativeResult = boost::none;
            for (uint_fast64_t iterations = 0; iterations < 10000; ++iterations) {
                auto iterationStart = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Starting iteration " << iterations << ".");

                // (1) build the abstraction.
                auto abstractionStart = std::chrono::high_resolution_clock::now();
                storm::abstraction::MenuGame<Type, ValueType> game = abstractor->abstract();
                auto abstractionEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Abstraction in iteration " << iterations << " has " << game.getNumberOfStates() << " (player 1) states and " << game.getNumberOfTransitions() << " transitions (computed in " << std::chrono::duration_cast<std::chrono::milliseconds>(abstractionEnd - abstractionStart).count() << "ms).");
                
                // (2) Prepare transition matrix BDD and target state BDD for later use.
                storm::dd::Bdd<Type> transitionMatrixBdd = game.getTransitionMatrix().toBdd();
                storm::dd::Bdd<Type> initialStates = game.getInitialStates();
                STORM_LOG_THROW(initialStates.getNonZeroCount() == 1 || checkTask.isBoundSet(), storm::exceptions::InvalidPropertyException, "Game-based abstraction refinement requires a bound on the formula for model with " << initialStates.getNonZeroCount() << " initial states.");
                storm::dd::Bdd<Type> constraintStates = globalConstraintStates && game.getReachableStates();
                storm::dd::Bdd<Type> targetStates = globalTargetStates && game.getReachableStates();
                if (player1Direction == storm::OptimizationDirection::Minimize) {
                    targetStates |= game.getBottomStates();
                }
                
                // #ifdef LOCAL_DEBUG
                // targetStates.template toAdd<ValueType>().exportToDot("target.dot");
                // abstractor->exportToDot("game" + std::to_string(iterations) + ".dot", targetStates, game.getManager().getBddOne());
                // game.getReachableStates().template toAdd<ValueType>().exportToDot("reach.dot");
                // #endif
                
                // (3) compute all states with probability 0/1 wrt. to the two different player 2 goals (min/max).
                auto qualitativeStart = std::chrono::high_resolution_clock::now();
                QualitativeResultMinMax<Type> qualitativeResult = computeProb01States(previousQualitativeResult, game, player1Direction, transitionMatrixBdd, constraintStates, targetStates);
                std::unique_ptr<CheckResult> result = checkForResultAfterQualitativeCheck<Type, ValueType>(checkTask, initialStates, qualitativeResult);
                if (result) {
                    printStatistics(*abstractor, game);
                    return result;
                }
                previousQualitativeResult = qualitativeResult;
                auto qualitativeEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_DEBUG("Qualitative computation completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(qualitativeEnd - qualitativeStart).count() << "ms.");
                
                // (4) compute the states for which we have to determine quantitative information.
                storm::dd::Bdd<Type> maybeMin = !(qualitativeResult.prob0Min.getPlayer1States() || qualitativeResult.prob1Min.getPlayer1States()) && game.getReachableStates();
                storm::dd::Bdd<Type> maybeMax = !(qualitativeResult.prob0Max.getPlayer1States() || qualitativeResult.prob1Max.getPlayer1States()) && game.getReachableStates();
                
                // (5) if the initial states are not maybe states, then we can refine at this point.
                storm::dd::Bdd<Type> initialMaybeStates = (initialStates && maybeMin) || (initialStates && maybeMax);
                bool qualitativeRefinement = false;
                if (initialMaybeStates.isZero()) {
                    // In this case, we know the result for the initial states for both player 2 minimizing and maximizing.
                    STORM_LOG_TRACE("No initial state is a 'maybe' state.");
                    
                    STORM_LOG_DEBUG("Obtained qualitative bounds [0, 1] on the actual value for the initial states. Refining abstraction based on qualitative check.");
                    
                    // If we get here, the initial states were all identified as prob0/1 states, but the value (0 or 1)
                    // depends on whether player 2 is minimizing or maximizing. Therefore, we need to find a place to refine.
                    auto qualitativeRefinementStart = std::chrono::high_resolution_clock::now();
                    qualitativeRefinement = refiner.refine(game, transitionMatrixBdd, qualitativeResult);
                    auto qualitativeRefinementEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_DEBUG("Qualitative refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(qualitativeRefinementEnd - qualitativeRefinementStart).count() << "ms.");
                } else if (initialMaybeStates == initialStates && checkTask.isQualitativeSet()) {
                    // If all initial states are 'maybe' states and the property we needed to check is a qualitative one,
                    // we can return the result here.
                    return std::make_unique<ExplicitQuantitativeCheckResult<ValueType>>(storm::storage::sparse::state_type(0), ValueType(0.5));
                }
                
                // (6) if we arrived at this point and no refinement was made, we need to compute the quantitative solution.
                if (!qualitativeRefinement) {
                    // At this point, we know that we cannot answer the query without further numeric computation.

                    storm::dd::Add<Type, ValueType> initialStatesAdd = initialStates.template toAdd<ValueType>();
                    
                    STORM_LOG_TRACE("Starting numerical solution step.");
                    auto quantitativeStart = std::chrono::high_resolution_clock::now();

                    QuantitativeResultMinMax<Type, ValueType> quantitativeResult;
                    
                    // (7) Solve the min values and check whether we can give the answer already.
                    quantitativeResult.min = computeQuantitativeResult(player1Direction, storm::OptimizationDirection::Minimize, game, qualitativeResult, initialStatesAdd, maybeMin, reuseQuantitativeResults ? previousMinQuantitativeResult : boost::none);
                    previousMinQuantitativeResult = quantitativeResult.min;
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Minimize, quantitativeResult.min.initialStatesRange);
                    if (result) {
                        printStatistics(*abstractor, game);
                        return result;
                    }
                    
                    // (8) Solve the max values and check whether we can give the answer already.
                    quantitativeResult.max = computeQuantitativeResult(player1Direction, storm::OptimizationDirection::Maximize, game, qualitativeResult, initialStatesAdd, maybeMax, boost::make_optional(quantitativeResult.min));
                    result = checkForResultAfterQuantitativeCheck<ValueType>(checkTask, storm::OptimizationDirection::Maximize, quantitativeResult.max.initialStatesRange);
                    if (result) {
                        printStatistics(*abstractor, game);
                        return result;
                    }

                    auto quantitativeEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_DEBUG("Obtained quantitative bounds [" << quantitativeResult.min.initialStatesRange.first << ", " << quantitativeResult.max.initialStatesRange.second << "] on the actual value for the initial states in " << std::chrono::duration_cast<std::chrono::milliseconds>(quantitativeEnd - quantitativeStart).count() << "ms.");
                    
                    // (9) Check whether the lower and upper bounds are close enough to terminate with an answer.
                    result = checkForResultAfterQuantitativeCheck<ValueType>(quantitativeResult.min.initialStatesRange.first, quantitativeResult.max.initialStatesRange.second, comparator);
                    if (result) {
                        printStatistics(*abstractor, game);
                        return result;
                    }

                    // Make sure that all strategies are still valid strategies.
                    STORM_LOG_ASSERT(quantitativeResult.min.player1Strategy.isZero() || quantitativeResult.min.player1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for min is illegal.");
                    STORM_LOG_ASSERT(quantitativeResult.max.player1Strategy.isZero() || quantitativeResult.max.player1Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer1Variables()).getMax() <= 1, "Player 1 strategy for max is illegal.");
                    STORM_LOG_ASSERT(quantitativeResult.min.player2Strategy.isZero() || quantitativeResult.min.player2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for min is illegal.");
                    STORM_LOG_ASSERT(quantitativeResult.max.player2Strategy.isZero() || quantitativeResult.max.player2Strategy.template toAdd<ValueType>().sumAbstract(game.getPlayer2Variables()).getMax() <= 1, "Player 2 strategy for max is illegal.");

                    auto quantitativeRefinementStart = std::chrono::high_resolution_clock::now();
                    // (10) If we arrived at this point, it means that we have all qualitative and quantitative
                    // information about the game, but we could not yet answer the query. In this case, we need to refine.
                    refiner.refine(game, transitionMatrixBdd, quantitativeResult);
                    auto quantitativeRefinementEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_DEBUG("Quantitative refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(quantitativeRefinementEnd - quantitativeRefinementStart).count() << "ms.");

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
            if (preprocessedModel.isJaniModel()) {
                storm::expressions::VariableSetPredicateSplitter splitter(preprocessedModel.asJaniModel().getAllLocationExpressionVariables());

                std::vector<storm::expressions::Expression> splitExpressions = splitter.split(targetStateExpression);
                initialPredicates.insert(initialPredicates.end(), splitExpressions.begin(), splitExpressions.end());

                splitExpressions = splitter.split(constraintExpression);
                initialPredicates.insert(initialPredicates.end(), splitExpressions.begin(), splitExpressions.end());
            } else {
                if (!targetStateExpression.isTrue() && !targetStateExpression.isFalse()) {
                    initialPredicates.push_back(targetStateExpression);
                }
                if (!constraintExpression.isTrue() && !constraintExpression.isFalse()) {
                    initialPredicates.push_back(constraintExpression);
                }
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

        template<storm::dd::DdType Type>
        bool checkQualitativeStrategies(bool prob0, QualitativeResult<Type> const& result, storm::dd::Bdd<Type> const& targetStates) {
            if (prob0) {
                STORM_LOG_ASSERT(result.hasPlayer1Strategy() && (result.getPlayer1States().isZero() || !result.getPlayer1Strategy().isZero()), "Unable to proceed without strategy.");
            } else {
                STORM_LOG_ASSERT(result.hasPlayer1Strategy() && ((result.getPlayer1States() && !targetStates).isZero() || !result.getPlayer1Strategy().isZero()), "Unable to proceed without strategy.");
            }

            STORM_LOG_ASSERT(result.hasPlayer2Strategy() && (result.getPlayer2States().isZero() || !result.getPlayer2Strategy().isZero()), "Unable to proceed without strategy.");
            
            return true;
        }
        
        template<storm::dd::DdType Type>
        bool checkQualitativeStrategies(QualitativeResultMinMax<Type> const& qualitativeResult, storm::dd::Bdd<Type> const& targetStates) {
            bool result = true;
            result &= checkQualitativeStrategies(true, qualitativeResult.prob0Min, targetStates);
            result &= checkQualitativeStrategies(false, qualitativeResult.prob1Min, targetStates);
            result &= checkQualitativeStrategies(true, qualitativeResult.prob0Max, targetStates);
            result &= checkQualitativeStrategies(false, qualitativeResult.prob1Max, targetStates);
            return result;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        QualitativeResultMinMax<Type> GameBasedMdpModelChecker<Type, ModelType>::computeProb01States(boost::optional<QualitativeResultMinMax<Type>> const& previousQualitativeResult, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::OptimizationDirection player1Direction, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates) {
            
            QualitativeResultMinMax<Type> result;
            
            if (reuseQualitativeResults) {
                // Depending on the player 1 direction, we choose a different order of operations.
                if (player1Direction == storm::OptimizationDirection::Minimize) {
                    // (1) min/min: compute prob0 using the game functions
                    result.prob0Min = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true);
                    
                    // (2) min/min: compute prob1 using the MDP functions
                    storm::dd::Bdd<Type> candidates = game.getReachableStates() && !result.prob0Min.player1States;
                    storm::dd::Bdd<Type> prob1MinMinMdp = storm::utility::graph::performProb1A(game, transitionMatrixBdd, previousQualitativeResult ? previousQualitativeResult.get().prob1Min.player1States : targetStates, candidates);

                    // (3) min/min: compute prob1 using the game functions
                    result.prob1Min = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true, boost::make_optional(prob1MinMinMdp));
                    
                    // (4) min/max: compute prob 0 using the game functions
                    result.prob0Max = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true);
                    
                    // (5) min/max: compute prob 1 using the game functions
                    // We know that only previous prob1 states can now be prob 1 states again, because the upper bound
                    // values can only decrease over iterations.
                    boost::optional<storm::dd::Bdd<Type>> prob1Candidates;
                    if (previousQualitativeResult) {
                        prob1Candidates = previousQualitativeResult.get().prob1Max.player1States;
                    }
                    result.prob1Max = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true, prob1Candidates);
                } else {
                    // (1) max/max: compute prob0 using the game functions
                    result.prob0Max = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true);
                    
                    // (2) max/max: compute prob1 using the MDP functions, reuse prob1 states of last iteration to constrain the candidate states.
                    storm::dd::Bdd<Type> candidates = game.getReachableStates() && !result.prob0Max.player1States;
                    if (previousQualitativeResult) {
                        candidates &= previousQualitativeResult.get().prob1Max.player1States;
                    }
                    storm::dd::Bdd<Type> prob1MaxMaxMdp = storm::utility::graph::performProb1E(game, transitionMatrixBdd, constraintStates, targetStates, candidates);
                    
                    // (3) max/max: compute prob1 using the game functions, reuse prob1 states from the MDP precomputation
                    result.prob1Max = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true, boost::make_optional(prob1MaxMaxMdp));
                    
                    // (4) max/min: compute prob0 using the game functions
                    result.prob0Min = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true);
                    
                    // (5) max/min: compute prob1 using the game functions, use prob1 from max/max as the candidate set
                    result.prob1Min = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true, boost::make_optional(prob1MaxMaxMdp));
                }
            } else {
                result.prob0Min = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true);
                result.prob1Min = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Minimize, true, true);
                result.prob0Max = storm::utility::graph::performProb0(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true);
                result.prob1Max = storm::utility::graph::performProb1(game, transitionMatrixBdd, constraintStates, targetStates, player1Direction, storm::OptimizationDirection::Maximize, true, true);
            }
            
            STORM_LOG_TRACE("Qualitative precomputation completed.");
            STORM_LOG_TRACE("[" << player1Direction << ", " << storm::OptimizationDirection::Minimize << "]: " << result.prob0Min.player1States.getNonZeroCount() << " 'no', " << result.prob1Min.player1States.getNonZeroCount() << " 'yes'.");
            STORM_LOG_TRACE("[" << player1Direction << ", " << storm::OptimizationDirection::Maximize << "]: " << result.prob0Max.player1States.getNonZeroCount() << " 'no', " << result.prob1Max.player1States.getNonZeroCount() << " 'yes'.");
            
            STORM_LOG_ASSERT(checkQualitativeStrategies(result, targetStates), "Qualitative strategies appear to be broken.");
            return result;
        }
        
        template<storm::dd::DdType Type, typename ModelType>
        void GameBasedMdpModelChecker<Type, ModelType>::printStatistics(storm::abstraction::MenuGameAbstractor<Type, ValueType> const& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game) const {
            if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                storm::abstraction::AbstractionInformation<Type> const& abstractionInformation = abstractor.getAbstractionInformation();

                std::cout << std::endl;
                std::cout << "Statistics:" << std::endl;
                std::cout << "    * player 1 states (final game): " << game.getReachableStates().getNonZeroCount() << std::endl;
                std::cout << "    * transitions (final game): " << game.getTransitionMatrix().getNonZeroCount() << std::endl;
                std::cout << "    * predicates used in abstraction: " << abstractionInformation.getNumberOfPredicates() << std::endl;
            }
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
