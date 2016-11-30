#include "storm/abstraction/MenuGameRefiner.h"

#include "storm/abstraction/AbstractionInformation.h"
#include "storm/abstraction/MenuGameAbstractor.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/utility/dd.h"
#include "storm/utility/solver.h"

#include "storm/solver/MathsatSmtSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/AbstractionSettings.h"

namespace storm {
    namespace abstraction {
        
        RefinementPredicates::RefinementPredicates(Source const& source, std::vector<storm::expressions::Expression> const& predicates) : source(source), predicates(predicates) {
            // Intentionally left empty.
        }
        
        RefinementPredicates::Source RefinementPredicates::getSource() const {
            return source;
        }
        
        std::vector<storm::expressions::Expression> const& RefinementPredicates::getPredicates() const {
            return predicates;
        }
        
        template<storm::dd::DdType Type>
        struct PivotStateCandidatesResult {
            storm::dd::Bdd<Type> reachableTransitionsMin;
            storm::dd::Bdd<Type> reachableTransitionsMax;
            storm::dd::Bdd<Type> pivotStates;
        };
        
        template<storm::dd::DdType Type>
        PivotStateResult<Type>::PivotStateResult(storm::dd::Bdd<Type> const& pivotState, storm::OptimizationDirection fromDirection) : pivotState(pivotState), fromDirection(fromDirection) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        MenuGameRefiner<Type, ValueType>::MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver) : abstractor(abstractor), splitPredicates(storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isSplitPredicatesSet()), splitGuards(storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isSplitGuardsSet()), splitter(), equivalenceChecker(std::move(smtSolver)) {
            
            if (storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isAddAllGuardsSet()) {
                std::vector<storm::expressions::Expression> guards;
                
                std::pair<uint64_t, uint64_t> player1Choices = this->abstractor.get().getPlayer1ChoiceRange();
                for (uint64_t index = player1Choices.first; index < player1Choices.second; ++index) {
                    guards.push_back(this->abstractor.get().getGuard(index));
                }
                performRefinement(createGlobalRefinement(preprocessPredicates(guards, storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isSplitInitialGuardsSet())));
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void MenuGameRefiner<Type, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) const {
            performRefinement(createGlobalRefinement(predicates));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::dd::Bdd<Type> getMostProbablePathSpanningTree(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& targetState, storm::dd::Bdd<Type> const& transitionFilter) {
            storm::dd::Add<Type, ValueType> maxProbabilities = game.getInitialStates().template toAdd<ValueType>();
            
            storm::dd::Bdd<Type> border = game.getInitialStates();
            storm::dd::Bdd<Type> spanningTree = game.getManager().getBddZero();

            storm::dd::Add<Type, ValueType> transitionMatrix = ((transitionFilter && game.getExtendedTransitionMatrix().maxAbstractRepresentative(game.getProbabilisticBranchingVariables())).template toAdd<ValueType>() * game.getExtendedTransitionMatrix()).sumAbstract(game.getPlayer2Variables());
            
            std::set<storm::expressions::Variable> variablesToAbstract(game.getRowVariables());
            variablesToAbstract.insert(game.getPlayer1Variables().begin(), game.getPlayer1Variables().end());
            variablesToAbstract.insert(game.getProbabilisticBranchingVariables().begin(), game.getProbabilisticBranchingVariables().end());
            while (!border.isZero() && (border && targetState).isZero()) {
                // Determine the new maximal probabilities to all states.
                storm::dd::Add<Type, ValueType> tmp = border.template toAdd<ValueType>() * transitionMatrix * maxProbabilities;
                storm::dd::Bdd<Type> newMaxProbabilityChoices = tmp.maxAbstractRepresentative(variablesToAbstract);
                storm::dd::Add<Type, ValueType> newMaxProbabilities = tmp.maxAbstract(variablesToAbstract).swapVariables(game.getRowColumnMetaVariablePairs());

                // Determine the probability values for which states strictly increased.
                storm::dd::Bdd<Type> updateStates = newMaxProbabilities.greater(maxProbabilities);
                maxProbabilities = updateStates.ite(newMaxProbabilities, maxProbabilities);
                
                // Delete all edges in the spanning tree that lead to states that need to be updated.
                spanningTree &= ((!updateStates).swapVariables(game.getRowColumnMetaVariablePairs()));
                
                // Add all edges that achieve the new maximal value to the spanning tree.
                spanningTree |= updateStates.swapVariables(game.getRowColumnMetaVariablePairs()) && newMaxProbabilityChoices;
                
                // Continue exploration from states that have been updated.
                border = updateStates;
            }
            
            return spanningTree;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        PivotStateResult<Type> pickPivotState(storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& transitionsMin, storm::dd::Bdd<Type> const& transitionsMax, std::set<storm::expressions::Variable> const& rowVariables, std::set<storm::expressions::Variable> const& columnVariables, storm::dd::Bdd<Type> const& pivotStates, boost::optional<QuantitativeResultMinMax<Type, ValueType>> const& quantitativeResult = boost::none) {
            
            // Set up used variables.
            storm::dd::Bdd<Type> frontierMin = initialStates;
            storm::dd::Bdd<Type> frontierMax = initialStates;
            storm::dd::Bdd<Type> frontierPivotStates = frontierMin && pivotStates;
            
            // Check whether we have pivot states on the very first level.
            uint64_t level = 0;
            bool foundPivotState = !frontierPivotStates.isZero();
            if (foundPivotState) {
                STORM_LOG_TRACE("Picked pivot state from " << frontierPivotStates.getNonZeroCount() << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                return PivotStateResult<Type>(frontierPivotStates.existsAbstractRepresentative(rowVariables), storm::OptimizationDirection::Minimize);
            } else {
                // Otherwise, we perform a simulatenous BFS in the sense that we make one step in both the min and max
                // transitions and check for pivot states we encounter.
                while (!foundPivotState) {
                    frontierMin = frontierMin.relationalProduct(transitionsMin, rowVariables, columnVariables);
                    frontierMax = frontierMax.relationalProduct(transitionsMax, rowVariables, columnVariables);
                    ++level;
                    
                    storm::dd::Bdd<Type> frontierMinPivotStates = frontierMin && pivotStates;
                    storm::dd::Bdd<Type> frontierMaxPivotStates = frontierMax && pivotStates;
                    uint64_t numberOfPivotStateCandidatesOnLevel = frontierMinPivotStates.getNonZeroCount() + frontierMaxPivotStates.getNonZeroCount();
                    
                    if (!frontierMinPivotStates.isZero() || !frontierMaxPivotStates.isZero()) {
                        if (quantitativeResult) {
                            storm::dd::Add<Type, ValueType> frontierMinPivotStatesAdd = frontierMinPivotStates.template toAdd<ValueType>();
                            storm::dd::Add<Type, ValueType> frontierMaxPivotStatesAdd = frontierMaxPivotStates.template toAdd<ValueType>();
                            storm::dd::Add<Type, ValueType> diffMin = frontierMinPivotStatesAdd * quantitativeResult.get().max.values - frontierMinPivotStatesAdd * quantitativeResult.get().min.values;
                            storm::dd::Add<Type, ValueType> diffMax = frontierMaxPivotStatesAdd * quantitativeResult.get().max.values - frontierMaxPivotStatesAdd * quantitativeResult.get().min.values;
                            
                            ValueType diffValue;
                            storm::OptimizationDirection direction;
                            if (diffMin.getMax() >= diffMax.getMax()) {
                                direction = storm::OptimizationDirection::Minimize;
                                diffValue = diffMin.getMax();
                            } else {
                                direction = storm::OptimizationDirection::Maximize;
                                diffValue = diffMax.getMax();
                            }
                            
                            STORM_LOG_TRACE("Picked pivot state with difference " << diffValue << " from " << numberOfPivotStateCandidatesOnLevel << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                            return PivotStateResult<Type>(direction == storm::OptimizationDirection::Minimize ? diffMin.maxAbstractRepresentative(rowVariables) : diffMax.maxAbstractRepresentative(rowVariables), direction);
                        } else {
                            STORM_LOG_TRACE("Picked pivot state from " << numberOfPivotStateCandidatesOnLevel << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                            
                            storm::OptimizationDirection direction;
                            if (!frontierMinPivotStates.isZero()) {
                                direction = storm::OptimizationDirection::Minimize;
                            } else {
                                direction = storm::OptimizationDirection::Maximize;
                            }
                            
                            return PivotStateResult<Type>(direction == storm::OptimizationDirection::Minimize ? frontierMinPivotStates.existsAbstractRepresentative(rowVariables) : frontierMaxPivotStates.existsAbstractRepresentative(rowVariables), direction);
                        }
                    }
                }
            }
            
            STORM_LOG_ASSERT(false, "This point must not be reached, because then no pivot state could be found.");
            return PivotStateResult<Type>(storm::dd::Bdd<Type>(), storm::OptimizationDirection::Minimize);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        RefinementPredicates MenuGameRefiner<Type, ValueType>::derivePredicatesFromDifferingChoices(storm::dd::Bdd<Type> const& pivotState, storm::dd::Bdd<Type> const& player1Choice, storm::dd::Bdd<Type> const& lowerChoice, storm::dd::Bdd<Type> const& upperChoice) const {
            // Prepare result.
            storm::expressions::Expression newPredicate;
            bool fromGuard = false;
            
            // Get abstraction informatin for easier access.
            AbstractionInformation<Type> const& abstractionInformation = abstractor.get().getAbstractionInformation();
            
            // Decode the index of the command chosen by player 1.
            storm::dd::Add<Type, ValueType> player1ChoiceAsAdd = player1Choice.template toAdd<ValueType>();
            auto pl1It = player1ChoiceAsAdd.begin();
            uint_fast64_t player1Index = abstractionInformation.decodePlayer1Choice((*pl1It).first, abstractionInformation.getPlayer1VariableCount());
            
            // Check whether there are bottom states in the game and whether one of the choices actually picks the
            // bottom state as the successor.
            bool buttomStateSuccessor = !((abstractionInformation.getBottomStateBdd(false, false) && lowerChoice) || (abstractionInformation.getBottomStateBdd(false, false) && upperChoice)).isZero();
            
            // If one of the choices picks the bottom state, the new predicate is based on the guard of the appropriate
            // command (that is the player 1 choice).
            if (buttomStateSuccessor) {
                STORM_LOG_TRACE("One of the successors is a bottom state, taking a guard as a new predicate.");
                newPredicate = abstractor.get().getGuard(player1Index);
                fromGuard = true;
                STORM_LOG_DEBUG("Derived new predicate (based on guard): " << newPredicate);
            } else {
                STORM_LOG_TRACE("No bottom state successor. Deriving a new predicate using weakest precondition.");
                
                // Decode both choices to explicit mappings.
                std::map<uint_fast64_t, storm::storage::BitVector> lowerChoiceUpdateToSuccessorMapping = abstractionInformation.decodeChoiceToUpdateSuccessorMapping(lowerChoice);
                std::map<uint_fast64_t, storm::storage::BitVector> upperChoiceUpdateToSuccessorMapping = abstractionInformation.decodeChoiceToUpdateSuccessorMapping(upperChoice);
                STORM_LOG_ASSERT(lowerChoiceUpdateToSuccessorMapping.size() == upperChoiceUpdateToSuccessorMapping.size(), "Mismatching sizes after decode (" << lowerChoiceUpdateToSuccessorMapping.size() << " vs. " << upperChoiceUpdateToSuccessorMapping.size() << ").");
                
                // Now go through the mappings and find points of deviation. Currently, we take the first deviation.
                auto lowerIt = lowerChoiceUpdateToSuccessorMapping.begin();
                auto lowerIte = lowerChoiceUpdateToSuccessorMapping.end();
                auto upperIt = upperChoiceUpdateToSuccessorMapping.begin();
                for (; lowerIt != lowerIte; ++lowerIt, ++upperIt) {
                    STORM_LOG_ASSERT(lowerIt->first == upperIt->first, "Update indices mismatch.");
                    uint_fast64_t updateIndex = lowerIt->first;
                    bool deviates = lowerIt->second != upperIt->second;
                    if (deviates) {
                        for (uint_fast64_t predicateIndex = 0; predicateIndex < lowerIt->second.size(); ++predicateIndex) {
                            if (lowerIt->second.get(predicateIndex) != upperIt->second.get(predicateIndex)) {
                                // Now we know the point of the deviation (command, update, predicate).
                                newPredicate = abstractionInformation.getPredicateByIndex(predicateIndex).substitute(abstractor.get().getVariableUpdates(player1Index, updateIndex)).simplify();
                                break;
                            }
                        }
                    }
                }
                STORM_LOG_ASSERT(newPredicate.isInitialized(), "Could not derive new predicate as there is no deviation.");
                STORM_LOG_DEBUG("Derived new predicate (based on weakest-precondition): " << newPredicate);
            }
            
            STORM_LOG_TRACE("Current set of predicates:");
            for (auto const& predicate : abstractionInformation.getPredicates()) {
                STORM_LOG_TRACE(predicate);
            }
            return RefinementPredicates(fromGuard ? RefinementPredicates::Source::Guard : RefinementPredicates::Source::WeakestPrecondition, {newPredicate});
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        PivotStateCandidatesResult<Type> computePivotStates(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) {
            
            PivotStateCandidatesResult<Type> result;
            
            // Build the fragment of transitions that is reachable by either the min or the max strategies.
            result.reachableTransitionsMin = (transitionMatrixBdd && minPlayer1Strategy && minPlayer2Strategy).existsAbstract(game.getNondeterminismVariables());
            result.reachableTransitionsMax = (transitionMatrixBdd && maxPlayer1Strategy && maxPlayer2Strategy).existsAbstract(game.getNondeterminismVariables());
            
            // Start with all reachable states as potential pivot states.
            result.pivotStates = storm::utility::dd::computeReachableStates(game.getInitialStates(), result.reachableTransitionsMin, game.getRowVariables(), game.getColumnVariables()) ||
            storm::utility::dd::computeReachableStates(game.getInitialStates(), result.reachableTransitionsMax, game.getRowVariables(), game.getColumnVariables());
            
            // Then constrain these states by the requirement that for either the lower or upper player 1 choice the player 2 choices must be different and
            // that the difference is not because of a missing strategy in either case.
            
            // Start with constructing the player 2 states that have a prob 0 (min) and prob 1 (max) strategy.
            storm::dd::Bdd<Type> constraint = minPlayer2Strategy.existsAbstract(game.getPlayer2Variables()) && maxPlayer2Strategy.existsAbstract(game.getPlayer2Variables());
            
            // Now construct all player 2 choices that actually exist and differ in the min and max case.
            constraint &= minPlayer2Strategy.exclusiveOr(maxPlayer2Strategy);
            
            // Then restrict the pivot states by requiring existing and different player 2 choices.
            result.pivotStates &= ((minPlayer1Strategy && maxPlayer1Strategy) && constraint).existsAbstract(game.getNondeterminismVariables());
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        RefinementPredicates MenuGameRefiner<Type, ValueType>::derivePredicatesFromPivotState(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& pivotState, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const {
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
                
                RefinementPredicates predicates = derivePredicatesFromDifferingChoices(pivotState, (pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
                auto refinementEnd = std::chrono::high_resolution_clock::now();
                STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
                return predicates;
            } else {
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
                
                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Refining based on upper choice.");
                    auto refinementStart = std::chrono::high_resolution_clock::now();
                    RefinementPredicates predicates = derivePredicatesFromDifferingChoices(pivotState, (pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                    auto refinementEnd = std::chrono::high_resolution_clock::now();
                    STORM_LOG_TRACE("Refinement completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(refinementEnd - refinementStart).count() << "ms.");
                    return predicates;
                } else {
                    STORM_LOG_ASSERT(false, "Did not find choices from which to derive predicates.");
                }
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::vector<std::vector<storm::expressions::Expression>> MenuGameRefiner<Type, ValueType>::buildTrace(storm::expressions::ExpressionManager& expressionManager, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& spanningTree, storm::dd::Bdd<Type> const& pivotState) const {
            std::vector<std::vector<storm::expressions::Expression>> result;
            
            // Prepare some variables.
            AbstractionInformation<Type> const& abstractionInformation = abstractor.get().getAbstractionInformation();
            std::set<storm::expressions::Variable> variablesToAbstract(game.getColumnVariables());
            variablesToAbstract.insert(game.getPlayer1Variables().begin(), game.getPlayer1Variables().end());
            variablesToAbstract.insert(game.getProbabilisticBranchingVariables().begin(), game.getProbabilisticBranchingVariables().end());

            std::map<storm::expressions::Variable, storm::expressions::Variable> oldToNewVariables;
            for (auto const& variable : abstractionInformation.getExpressionManager().getVariables()) {
                oldToNewVariables[variable] = expressionManager.getVariable(variable.getName());
            }
            std::map<storm::expressions::Variable, storm::expressions::Expression> lastSubstitution;
            for (auto const& variable : oldToNewVariables) {
                lastSubstitution[variable.second] = variable.second;
            }
            std::map<storm::expressions::Variable, storm::expressions::Variable> stepVariableToCopiedVariableMap;
            
            // Start with the target state part of the trace.
            storm::storage::BitVector decodedTargetState = abstractionInformation.decodeState(pivotState);
            result.emplace_back(abstractionInformation.getPredicates(decodedTargetState));
            for (auto& predicate : result.back()) {
                predicate = predicate.changeManager(expressionManager);
            }
            
            pivotState.template toAdd<ValueType>().exportToDot("pivot.dot");
            
            // Perform a backward search for an initial state.
            storm::dd::Bdd<Type> currentState = pivotState;
            uint64_t cnt = 0;
            while ((currentState && game.getInitialStates()).isZero()) {
                storm::dd::Bdd<Type> predecessorTransition = currentState.swapVariables(game.getRowColumnMetaVariablePairs()) && spanningTree;
                std::tuple<storm::storage::BitVector, uint64_t, uint64_t> decodedPredecessor = abstractionInformation.decodeStatePlayer1ChoiceAndUpdate(predecessorTransition);
                std::cout << "got predecessor " << std::get<0>(decodedPredecessor) << ", choice " << std::get<1>(decodedPredecessor) << " and update " << std::get<2>(decodedPredecessor) << std::endl;
                
                // predecessorTransition.template toAdd<ValueType>().exportToDot("pred_" + std::to_string(cnt) + ".dot");
                
                // Create a new copy of each variable to use for this step.
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
                for (auto const& variablePair : oldToNewVariables) {
                    storm::expressions::Variable variableCopy = expressionManager.declareVariableCopy(variablePair.second);
                    substitution[variablePair.second] = variableCopy;
                    stepVariableToCopiedVariableMap[variableCopy] = variablePair.second;
                }
                
                // Retrieve the variable updates that the predecessor needs to perform to get to the current state.
                auto variableUpdates = abstractor.get().getVariableUpdates(std::get<1>(decodedPredecessor), std::get<2>(decodedPredecessor));
                for (auto const& update : variableUpdates) {
                    storm::expressions::Variable newVariable = oldToNewVariables.at(update.first);
                    if (update.second.hasBooleanType()) {
                        result.back().push_back(storm::expressions::iff(lastSubstitution.at(oldToNewVariables.at(update.first)), update.second.changeManager(expressionManager).substitute(substitution)));
                    } else {
                        result.back().push_back(lastSubstitution.at(oldToNewVariables.at(update.first)) == update.second.changeManager(expressionManager).substitute(substitution));
                    }
                }
                
                // Add the guard of the choice.
                result.back().push_back(abstractor.get().getGuard(std::get<1>(decodedPredecessor)).changeManager(expressionManager).substitute(substitution));
                
                // Retrieve the predicate valuation in the predecessor.
                result.emplace_back(abstractionInformation.getPredicates(std::get<0>(decodedPredecessor)));
                for (auto& predicate : result.back()) {
                    predicate = predicate.changeManager(expressionManager).substitute(substitution);
                }
                
                // Move backwards one step.
                lastSubstitution = std::move(substitution);
                currentState = predecessorTransition.existsAbstract(variablesToAbstract);
                ++cnt;
            }
            
            result.back().push_back(abstractor.get().getInitialExpression().changeManager(expressionManager).substitute(lastSubstitution));
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::optional<std::vector<storm::expressions::Expression>> MenuGameRefiner<Type, ValueType>::derivePredicatesFromInterpolation(storm::abstraction::MenuGame<Type, ValueType> const& game, PivotStateResult<Type> const& pivotStateResult, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const {
            
            // Compute the most probable path from any initial state to the pivot state.
            storm::dd::Bdd<Type> spanningTree = getMostProbablePathSpanningTree(game, pivotStateResult.pivotState, pivotStateResult.fromDirection == storm::OptimizationDirection::Minimize ? minPlayer1Strategy && minPlayer2Strategy : maxPlayer1Strategy && maxPlayer2Strategy);
            
            // Create a new expression manager that we can use for the interpolation.
            std::shared_ptr<storm::expressions::ExpressionManager> interpolationManager = abstractor.get().getAbstractionInformation().getExpressionManager().clone();
            
            // Build the trace of the most probable path in terms of which predicates hold in each step.
            std::vector<std::vector<storm::expressions::Expression>> trace = buildTrace(*interpolationManager, game, spanningTree, pivotStateResult.pivotState);

            // Now encode the trace as an SMT problem.
            storm::solver::MathsatSmtSolver interpolatingSolver(*interpolationManager, storm::solver::MathsatSmtSolver::Options(true, false, true));
            
            uint64_t stepCounter = 0;
            for (auto const& step : trace) {
                std::cout << "group " << stepCounter << std::endl;
                interpolatingSolver.setInterpolationGroup(stepCounter);
                for (auto const& predicate : step) {
                    std::cout << predicate << std::endl;
                    interpolatingSolver.add(predicate);
                }
                ++stepCounter;
            }
            
            storm::solver::SmtSolver::CheckResult result = interpolatingSolver.check();
            if (result == storm::solver::SmtSolver::CheckResult::Unsat) {
                STORM_LOG_TRACE("Trace formula is unsatisfiable. Starting interpolation.");
                
                std::vector<storm::expressions::Expression> interpolants;
                std::vector<uint64_t> prefix;
                for (uint64_t step = stepCounter; step > 1; --step) {
                    prefix.push_back(step - 1);
                    storm::expressions::Expression interpolant = interpolatingSolver.getInterpolant(prefix);
                    STORM_LOG_ASSERT(!interpolant.isTrue() && !interpolant.isFalse(), "Expected other interpolant.");
                    interpolants.push_back(interpolant);
                }
                return boost::make_optional(interpolants);
            } else {
                STORM_LOG_TRACE("Trace formula is satisfiable.");
                std::cout << interpolatingSolver.getModelAsValuation().toString(true) << std::endl;
            }
            
            return boost::none;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool MenuGameRefiner<Type, ValueType>::refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QualitativeResultMinMax<Type> const& qualitativeResult) const {
            STORM_LOG_TRACE("Trying refinement after qualitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = qualitativeResult.prob0Min.getPlayer1Strategy();
            storm::dd::Bdd<Type> minPlayer2Strategy = qualitativeResult.prob0Min.getPlayer2Strategy();
            storm::dd::Bdd<Type> maxPlayer1Strategy = qualitativeResult.prob1Max.getPlayer1Strategy();
            storm::dd::Bdd<Type> maxPlayer2Strategy = qualitativeResult.prob1Max.getPlayer2Strategy();
            
            // Redirect all player 1 choices of the min strategy to that of the max strategy if this leads to a player 2
            // state that is also a prob 0 state.
            minPlayer1Strategy = (maxPlayer1Strategy && qualitativeResult.prob0Min.getPlayer2States()).existsAbstract(game.getPlayer1Variables()).ite(maxPlayer1Strategy, minPlayer1Strategy);
            
            // Compute all reached pivot states.
            PivotStateCandidatesResult<Type> pivotStateCandidatesResult = computePivotStates(game, transitionMatrixBdd, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            
            // We can only refine in case we have a reachable player 1 state with a player 2 successor (under either
            // player 1's min or max strategy) such that from this player 2 state, both prob0 min and prob1 max define
            // strategies and they differ. Hence, it is possible that we arrive at a point where no suitable pivot state
            // is found. In this case, we abort the qualitative refinement here.
            if (pivotStateCandidatesResult.pivotStates.isZero()) {
                return false;
            }
            STORM_LOG_ASSERT(!pivotStateCandidatesResult.pivotStates.isZero(), "Unable to proceed without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            PivotStateResult<Type> pivotStateResult = pickPivotState<Type, ValueType>(game.getInitialStates(), pivotStateCandidatesResult.reachableTransitionsMin, pivotStateCandidatesResult.reachableTransitionsMax, game.getRowVariables(), game.getColumnVariables(), pivotStateCandidatesResult.pivotStates);
            
            boost::optional<std::vector<storm::expressions::Expression>> interpolationPredicates = derivePredicatesFromInterpolation(game, pivotStateResult, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            if (interpolationPredicates) {
                std::cout << "Got interpolation predicates!" << std::endl;
                for (auto const& pred : interpolationPredicates.get()) {
                    std::cout << "pred: " << pred << std::endl;
                }
            }
            
            // Derive predicate based on the selected pivot state.
            RefinementPredicates predicates = derivePredicatesFromPivotState(game, pivotStateResult.pivotState, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            std::vector<storm::expressions::Expression> preparedPredicates = preprocessPredicates(predicates.getPredicates(), (predicates.getSource() == RefinementPredicates::Source::Guard && splitGuards) || (predicates.getSource() == RefinementPredicates::Source::WeakestPrecondition && splitPredicates));
            performRefinement(createGlobalRefinement(preparedPredicates));
            return true;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool MenuGameRefiner<Type, ValueType>::refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QuantitativeResultMinMax<Type, ValueType> const& quantitativeResult) const {
            STORM_LOG_TRACE("Refining after quantitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = quantitativeResult.min.player1Strategy;
            storm::dd::Bdd<Type> minPlayer2Strategy = quantitativeResult.min.player2Strategy;
            storm::dd::Bdd<Type> maxPlayer1Strategy = quantitativeResult.max.player1Strategy;
            storm::dd::Bdd<Type> maxPlayer2Strategy = quantitativeResult.max.player2Strategy;
            
            // Compute all reached pivot states.
            PivotStateCandidatesResult<Type> pivotStateCandidatesResult = computePivotStates(game, transitionMatrixBdd, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            
            STORM_LOG_ASSERT(!pivotStateCandidatesResult.pivotStates.isZero(), "Unable to refine without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            PivotStateResult<Type> pivotStateResult = pickPivotState<Type, ValueType>(game.getInitialStates(), pivotStateCandidatesResult.reachableTransitionsMin, pivotStateCandidatesResult.reachableTransitionsMax, game.getRowVariables(), game.getColumnVariables(), pivotStateCandidatesResult.pivotStates);
            
            boost::optional<std::vector<storm::expressions::Expression>> interpolationPredicates = derivePredicatesFromInterpolation(game, pivotStateResult, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            if (interpolationPredicates) {
                std::cout << "Got interpolation predicates!" << std::endl;
                for (auto const& pred : interpolationPredicates.get()) {
                    std::cout << "pred: " << pred << std::endl;
                }
            }
            
            // Derive predicate based on the selected pivot state.
            RefinementPredicates predicates = derivePredicatesFromPivotState(game, pivotStateResult.pivotState, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            std::vector<storm::expressions::Expression> preparedPredicates = preprocessPredicates(predicates.getPredicates(), (predicates.getSource() == RefinementPredicates::Source::Guard && splitGuards) || (predicates.getSource() == RefinementPredicates::Source::WeakestPrecondition && splitPredicates));
            performRefinement(createGlobalRefinement(preparedPredicates));
            return true;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::vector<storm::expressions::Expression> MenuGameRefiner<Type, ValueType>::preprocessPredicates(std::vector<storm::expressions::Expression> const& predicates, bool split) const {
            if (split) {
                AbstractionInformation<Type> const& abstractionInformation = abstractor.get().getAbstractionInformation();
                std::vector<storm::expressions::Expression> cleanedAtoms;
                
                for (auto const& predicate : predicates) {
                    
                    // Split the predicates.
                    std::vector<storm::expressions::Expression> atoms = splitter.split(predicate);
                    
                    // Check which of the atoms are redundant in the sense that they are equivalent to a predicate we already have.
                    for (auto const& atom : atoms) {
                        // Check whether the newly found atom is equivalent to an atom we already have in the predicate
                        // set or in the set that is to be added.
                        bool addAtom = true;
                        for (auto const& oldPredicate : abstractionInformation.getPredicates()) {
                            if (equivalenceChecker.areEquivalent(atom, oldPredicate)) {
                                addAtom = false;
                                break;
                            }
                        }
                        for (auto const& addedAtom : cleanedAtoms) {
                            if (equivalenceChecker.areEquivalent(addedAtom, atom)) {
                                addAtom = false;
                                break;
                            }
                        }
                        
                        if (addAtom) {
                            cleanedAtoms.push_back(atom);
                        }
                    }
                }
                
                return cleanedAtoms;
            } else {
                // If no splitting of the predicates is required, just forward the refinement request to the abstractor.
            }
            
            return predicates;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::vector<RefinementCommand> MenuGameRefiner<Type, ValueType>::createGlobalRefinement(std::vector<storm::expressions::Expression> const& predicates) const {
            std::vector<RefinementCommand> commands;
            commands.emplace_back(predicates);
            return commands;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void MenuGameRefiner<Type, ValueType>::performRefinement(std::vector<RefinementCommand> const& refinementCommands) const {
            for (auto const& command : refinementCommands) {
                abstractor.get().refine(command);
            }
        }
        
        template class MenuGameRefiner<storm::dd::DdType::CUDD, double>;
        template class MenuGameRefiner<storm::dd::DdType::Sylvan, double>;
        
    }
}
