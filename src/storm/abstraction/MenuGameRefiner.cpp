#include "storm/abstraction/MenuGameRefiner.h"

#include "storm/abstraction/AbstractionInformation.h"
#include "storm/abstraction/MenuGameAbstractor.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/utility/dd.h"
#include "storm/utility/solver.h"

#include "storm/solver/MathsatSmtSolver.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/exceptions/InvalidStateException.h"

#include "storm/settings/SettingsManager.h"

#include "storm-config.h"
#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace abstraction {
        
        using storm::settings::modules::AbstractionSettings;
        
        RefinementPredicates::RefinementPredicates(Source const& source, std::vector<storm::expressions::Expression> const& predicates) : source(source), predicates(predicates) {
            // Intentionally left empty.
        }
        
        RefinementPredicates::Source RefinementPredicates::getSource() const {
            return source;
        }
        
        std::vector<storm::expressions::Expression> const& RefinementPredicates::getPredicates() const {
            return predicates;
        }
        
        void RefinementPredicates::addPredicates(std::vector<storm::expressions::Expression> const& newPredicates) {
            this->predicates.insert(this->predicates.end(), newPredicates.begin(), newPredicates.end());
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        MostProbablePathsResult<Type, ValueType>::MostProbablePathsResult(storm::dd::Add<Type, ValueType> const& maxProbabilities, storm::dd::Bdd<Type> const& spanningTree) : maxProbabilities(maxProbabilities), spanningTree(spanningTree) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type>
        struct PivotStateCandidatesResult {
            storm::dd::Bdd<Type> reachableTransitionsMin;
            storm::dd::Bdd<Type> reachableTransitionsMax;
            storm::dd::Bdd<Type> pivotStates;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        PivotStateResult<Type, ValueType>::PivotStateResult(storm::dd::Bdd<Type> const& pivotState, storm::OptimizationDirection fromDirection, boost::optional<MostProbablePathsResult<Type, ValueType>> const& mostProbablePathsResult) : pivotState(pivotState), fromDirection(fromDirection), mostProbablePathsResult(mostProbablePathsResult) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        MenuGameRefiner<Type, ValueType>::MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver) : abstractor(abstractor), useInterpolation(storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isUseInterpolationSet()), splitAll(false), splitPredicates(false), addedAllGuardsFlag(false), pivotSelectionHeuristic(storm::settings::getModule<storm::settings::modules::AbstractionSettings>().getPivotSelectionHeuristic()), splitter(), equivalenceChecker(std::move(smtSolver)) {
            
            equivalenceChecker.addConstraints(abstractor.getAbstractionInformation().getConstraints());
            
            AbstractionSettings::SplitMode splitMode = storm::settings::getModule<storm::settings::modules::AbstractionSettings>().getSplitMode();
            splitAll = splitMode == AbstractionSettings::SplitMode::All;
            splitPredicates = splitMode == AbstractionSettings::SplitMode::NonGuard;
            
            if (storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isAddAllGuardsSet()) {
                std::vector<storm::expressions::Expression> guards;
                
                std::pair<uint64_t, uint64_t> player1Choices = this->abstractor.get().getPlayer1ChoiceRange();
                for (uint64_t index = player1Choices.first; index < player1Choices.second; ++index) {
                    storm::expressions::Expression guard = this->abstractor.get().getGuard(index);
                    if (!guard.isTrue() && !guard.isFalse()) {
                        guards.push_back(guard);
                    }
                }
                performRefinement(createGlobalRefinement(preprocessPredicates(guards, RefinementPredicates::Source::InitialGuard)));
                
                addedAllGuardsFlag = true;
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        void MenuGameRefiner<Type, ValueType>::refine(std::vector<storm::expressions::Expression> const& predicates) const {
            performRefinement(createGlobalRefinement(preprocessPredicates(predicates, RefinementPredicates::Source::Manual)));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        MostProbablePathsResult<Type, ValueType> getMostProbablePathSpanningTree(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionFilter) {
            storm::dd::Add<Type, ValueType> maxProbabilities = game.getInitialStates().template toAdd<ValueType>();
            
            storm::dd::Bdd<Type> border = game.getInitialStates();
            storm::dd::Bdd<Type> spanningTree = game.getManager().getBddZero();

            storm::dd::Add<Type, ValueType> transitionMatrix = ((transitionFilter && game.getExtendedTransitionMatrix().maxAbstractRepresentative(game.getProbabilisticBranchingVariables())).template toAdd<ValueType>() * game.getExtendedTransitionMatrix()).sumAbstract(game.getPlayer2Variables());
            
            std::set<storm::expressions::Variable> variablesToAbstract(game.getRowVariables());
            variablesToAbstract.insert(game.getPlayer1Variables().begin(), game.getPlayer1Variables().end());
            variablesToAbstract.insert(game.getProbabilisticBranchingVariables().begin(), game.getProbabilisticBranchingVariables().end());
            while (!border.isZero()) {
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
            
            return MostProbablePathsResult<Type, ValueType>(maxProbabilities, spanningTree);
        }
                
        template<storm::dd::DdType Type, typename ValueType>
        PivotStateResult<Type, ValueType> pickPivotState(AbstractionSettings::PivotSelectionHeuristic const& heuristic, storm::abstraction::MenuGame<Type, ValueType> const& game, PivotStateCandidatesResult<Type> const& pivotStateCandidateResult, boost::optional<QualitativeGameResultMinMax<Type>> const& qualitativeResult, boost::optional<QuantitativeGameResultMinMax<Type, ValueType>> const& quantitativeResult) {
            
            // Get easy access to strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy;
            storm::dd::Bdd<Type> minPlayer2Strategy;
            storm::dd::Bdd<Type> maxPlayer1Strategy;
            storm::dd::Bdd<Type> maxPlayer2Strategy;
            if (qualitativeResult) {
                minPlayer1Strategy = qualitativeResult.get().prob0Min.getPlayer1Strategy();
                minPlayer2Strategy = qualitativeResult.get().prob0Min.getPlayer2Strategy();
                maxPlayer1Strategy = qualitativeResult.get().prob1Max.getPlayer1Strategy();
                maxPlayer2Strategy = qualitativeResult.get().prob1Max.getPlayer2Strategy();
            } else if (quantitativeResult) {
                minPlayer1Strategy = quantitativeResult.get().min.getPlayer1Strategy();
                minPlayer2Strategy = quantitativeResult.get().min.getPlayer2Strategy();
                maxPlayer1Strategy = quantitativeResult.get().max.getPlayer1Strategy();
                maxPlayer2Strategy = quantitativeResult.get().max.getPlayer2Strategy();
            } else {
                STORM_LOG_ASSERT(false, "Either qualitative or quantitative result is required.");
            }

            storm::dd::Bdd<Type> pivotStates = pivotStateCandidateResult.pivotStates;
            
            if (heuristic == AbstractionSettings::PivotSelectionHeuristic::NearestMaximalDeviation) {
                // Set up used variables.
                storm::dd::Bdd<Type> initialStates = game.getInitialStates();
                std::set<storm::expressions::Variable> const& rowVariables = game.getRowVariables();
                std::set<storm::expressions::Variable> const& columnVariables = game.getColumnVariables();
                storm::dd::Bdd<Type> transitionsMin = pivotStateCandidateResult.reachableTransitionsMin;
                storm::dd::Bdd<Type> transitionsMax = pivotStateCandidateResult.reachableTransitionsMax;
                storm::dd::Bdd<Type> frontierMin = initialStates;
                storm::dd::Bdd<Type> frontierMax = initialStates;
                storm::dd::Bdd<Type> frontierPivotStates = frontierMin && pivotStates;
                
                // Check whether we have pivot states on the very first level.
                uint64_t level = 0;
                bool foundPivotState = !frontierPivotStates.isZero();
                if (foundPivotState) {
                    STORM_LOG_TRACE("Picked pivot state from " << frontierPivotStates.getNonZeroCount() << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                    return PivotStateResult<Type, ValueType>(frontierPivotStates.existsAbstractRepresentative(rowVariables), storm::OptimizationDirection::Minimize);
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
                                return PivotStateResult<Type, ValueType>(direction == storm::OptimizationDirection::Minimize ? diffMin.maxAbstractRepresentative(rowVariables) : diffMax.maxAbstractRepresentative(rowVariables), direction);
                            } else {
                                STORM_LOG_TRACE("Picked pivot state from " << numberOfPivotStateCandidatesOnLevel << " candidates on level " << level << ", " << pivotStates.getNonZeroCount() << " candidates in total.");
                                
                                storm::OptimizationDirection direction;
                                if (!frontierMinPivotStates.isZero()) {
                                    direction = storm::OptimizationDirection::Minimize;
                                } else {
                                    direction = storm::OptimizationDirection::Maximize;
                                }
                                
                                return PivotStateResult<Type, ValueType>(direction == storm::OptimizationDirection::Minimize ? frontierMinPivotStates.existsAbstractRepresentative(rowVariables) : frontierMaxPivotStates.existsAbstractRepresentative(rowVariables), direction);
                            }
                        }
                    }
                }
            } else {
                // Compute the most probable paths to the reachable states and the corresponding spanning trees.
                MostProbablePathsResult<Type, ValueType> minMostProbablePathsResult = getMostProbablePathSpanningTree(game, minPlayer1Strategy && minPlayer2Strategy);
                MostProbablePathsResult<Type, ValueType> maxMostProbablePathsResult = getMostProbablePathSpanningTree(game, maxPlayer1Strategy && maxPlayer2Strategy);
                storm::dd::Bdd<Type> selectedPivotState;
                
                storm::dd::Add<Type, ValueType> score = pivotStates.template toAdd<ValueType>() * minMostProbablePathsResult.maxProbabilities.maximum(maxMostProbablePathsResult.maxProbabilities);
                
                if (heuristic == AbstractionSettings::PivotSelectionHeuristic::MaxWeightedDeviation && quantitativeResult) {
                    score = score * (quantitativeResult.get().max.values - quantitativeResult.get().min.values);
                }
                selectedPivotState = score.maxAbstractRepresentative(game.getRowVariables());
                STORM_LOG_TRACE("Selected pivot state with score " << score.getMax() << ".");

                storm::OptimizationDirection fromDirection = storm::OptimizationDirection::Minimize;
                storm::dd::Add<Type, ValueType> selectedPivotStateAsAdd = selectedPivotState.template toAdd<ValueType>();
                if ((selectedPivotStateAsAdd * maxMostProbablePathsResult.maxProbabilities).getMax() > (selectedPivotStateAsAdd * minMostProbablePathsResult.maxProbabilities).getMax()) {
                    fromDirection = storm::OptimizationDirection::Maximize;
                }
                
                return PivotStateResult<Type, ValueType>(selectedPivotState, fromDirection, fromDirection == storm::OptimizationDirection::Minimize ? minMostProbablePathsResult : maxMostProbablePathsResult);
            }
            
            STORM_LOG_ASSERT(false, "This point must not be reached, because then no pivot state could be found.");
            return PivotStateResult<Type, ValueType>(storm::dd::Bdd<Type>(), storm::OptimizationDirection::Minimize);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        RefinementPredicates MenuGameRefiner<Type, ValueType>::derivePredicatesFromDifferingChoices(storm::dd::Bdd<Type> const& player1Choice, storm::dd::Bdd<Type> const& lowerChoice, storm::dd::Bdd<Type> const& upperChoice) const {
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
                std::map<uint64_t, std::pair<storm::storage::BitVector, ValueType>> lowerChoiceUpdateToSuccessorMapping = abstractionInformation.template decodeChoiceToUpdateSuccessorMapping<ValueType>(lowerChoice);
                std::map<uint64_t, std::pair<storm::storage::BitVector, ValueType>> upperChoiceUpdateToSuccessorMapping = abstractionInformation.template decodeChoiceToUpdateSuccessorMapping<ValueType>(upperChoice);
                STORM_LOG_ASSERT(lowerChoiceUpdateToSuccessorMapping.size() == upperChoiceUpdateToSuccessorMapping.size(), "Mismatching sizes after decode (" << lowerChoiceUpdateToSuccessorMapping.size() << " vs. " << upperChoiceUpdateToSuccessorMapping.size() << ").");
                
                // First, sort updates according to probability mass.
                std::vector<std::pair<uint64_t, ValueType>> updateIndicesAndMasses;
                for (auto const& entry : lowerChoiceUpdateToSuccessorMapping) {
                    updateIndicesAndMasses.emplace_back(entry.first, entry.second.second);
                }
                std::sort(updateIndicesAndMasses.begin(), updateIndicesAndMasses.end(), [] (std::pair<uint64_t, ValueType> const& a, std::pair<uint64_t, ValueType> const& b) { return a.second > b.second; });
                
                // Now find the update with the highest probability mass among all deviating updates. More specifically,
                // we determine the set of predicate indices for which there is a deviation.
                std::set<uint64_t> deviationPredicates;
                uint64_t orderedUpdateIndex = 0;
                std::vector<storm::expressions::Expression> possibleRefinementPredicates;
                for (; orderedUpdateIndex < updateIndicesAndMasses.size(); ++orderedUpdateIndex) {
                    storm::storage::BitVector const& lower = lowerChoiceUpdateToSuccessorMapping.at(updateIndicesAndMasses[orderedUpdateIndex].first).first;
                    storm::storage::BitVector const& upper = upperChoiceUpdateToSuccessorMapping.at(updateIndicesAndMasses[orderedUpdateIndex].first).first;
                    
                    bool deviates = lower != upper;
                    if (deviates) {
                        std::map<storm::expressions::Variable, storm::expressions::Expression> variableUpdates = abstractor.get().getVariableUpdates(player1Index, updateIndicesAndMasses[orderedUpdateIndex].first);

                        for (uint64_t predicateIndex = 0; predicateIndex < lower.size(); ++predicateIndex) {
                            if (lower[predicateIndex] != upper[predicateIndex]) {
                                possibleRefinementPredicates.push_back(abstractionInformation.getPredicateByIndex(predicateIndex).substitute(variableUpdates).simplify());
                            }
                        }
                        ++orderedUpdateIndex;
                        break;
                    }
                }
                
                STORM_LOG_ASSERT(!possibleRefinementPredicates.empty(), "Expected refinement predicates.");
                
                // Since we can choose any of the deviation predicates to perform the split, we go through the remaining
                // updates and build all deviation predicates. We can then check whether any of the possible refinement
                // predicates also eliminates another deviation.
                std::vector<storm::expressions::Expression> otherRefinementPredicates;
                for (; orderedUpdateIndex < updateIndicesAndMasses.size(); ++orderedUpdateIndex) {
                    storm::storage::BitVector const& lower = lowerChoiceUpdateToSuccessorMapping.at(updateIndicesAndMasses[orderedUpdateIndex].first).first;
                    storm::storage::BitVector const& upper = upperChoiceUpdateToSuccessorMapping.at(updateIndicesAndMasses[orderedUpdateIndex].first).first;
                    
                    bool deviates = lower != upper;
                    if (deviates) {
                        std::map<storm::expressions::Variable, storm::expressions::Expression> newVariableUpdates = abstractor.get().getVariableUpdates(player1Index, updateIndicesAndMasses[orderedUpdateIndex].first);
                        for (uint64_t predicateIndex = 0; predicateIndex < lower.size(); ++predicateIndex) {
                            if (lower[predicateIndex] != upper[predicateIndex]) {
                                otherRefinementPredicates.push_back(abstractionInformation.getPredicateByIndex(predicateIndex).substitute(newVariableUpdates).simplify());
                            }
                        }
                    }
                }
                
                // Finally, go through the refinement predicates and see how many deviations they cover.
                std::vector<uint64_t> refinementPredicateIndexToCount(possibleRefinementPredicates.size(), 0);
                for (uint64_t index = 0; index < possibleRefinementPredicates.size(); ++index) {
                    refinementPredicateIndexToCount[index] = 1;
                }
                for (auto const& otherPredicate : otherRefinementPredicates) {
                    for (uint64_t index = 0; index < possibleRefinementPredicates.size(); ++index) {
                        if (equivalenceChecker.areEquivalentModuloNegation(otherPredicate, possibleRefinementPredicates[index])) {
                            ++refinementPredicateIndexToCount[index];
                        }
                    }
                }

                // Find predicate that covers the most deviations.
                uint64_t chosenPredicateIndex = 0;
                for (uint64_t index = 0; index < possibleRefinementPredicates.size(); ++index) {
                    if (refinementPredicateIndexToCount[index] > refinementPredicateIndexToCount[chosenPredicateIndex]) {
                        chosenPredicateIndex = index;
                    }
                }
                newPredicate = possibleRefinementPredicates[chosenPredicateIndex];

                STORM_LOG_ASSERT(newPredicate.isInitialized(), "Could not derive new predicate as there is no deviation.");
                STORM_LOG_DEBUG("Derived new predicate (based on weakest-precondition): " << newPredicate << ", (equivalent to " << (refinementPredicateIndexToCount[chosenPredicateIndex] - 1) << " other refinement predicates)");
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
            
            // Start with constructing the player 2 states that have a min and a max strategy.
            storm::dd::Bdd<Type> constraint = minPlayer2Strategy.existsAbstract(game.getPlayer2Variables()) && maxPlayer2Strategy.existsAbstract(game.getPlayer2Variables());
            
            // Now construct all player 2 choices that actually exist and differ in the min and max case.
            constraint &= minPlayer2Strategy.exclusiveOr(maxPlayer2Strategy);
            
            // Then restrict the pivot states by requiring existing and different player 2 choices.
            result.pivotStates &= ((minPlayer1Strategy || maxPlayer1Strategy) && constraint).existsAbstract(game.getNondeterminismVariables());
            
            return result;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        RefinementPredicates MenuGameRefiner<Type, ValueType>::derivePredicatesFromPivotState(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& pivotState, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const {
            // Compute the lower and the upper choice for the pivot state.
            std::set<storm::expressions::Variable> variablesToAbstract = game.getNondeterminismVariables();
            variablesToAbstract.insert(game.getRowVariables().begin(), game.getRowVariables().end());
            
            bool player1ChoicesDifferent = !(pivotState && minPlayer1Strategy).exclusiveOr(pivotState && maxPlayer1Strategy).isZero();
            
            boost::optional<RefinementPredicates> predicates;
            
            // Derive predicates from lower choice.
            storm::dd::Bdd<Type> lowerChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && minPlayer1Strategy;
            storm::dd::Bdd<Type> lowerChoice1 = (lowerChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
            storm::dd::Bdd<Type> lowerChoice2 = (lowerChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
            
            bool lowerChoicesDifferent = !lowerChoice1.exclusiveOr(lowerChoice2).isZero() && !lowerChoice1.isZero() && !lowerChoice2.isZero();
            if (lowerChoicesDifferent) {
                STORM_LOG_TRACE("Deriving predicate based on lower choice.");
                predicates = derivePredicatesFromDifferingChoices((pivotState && minPlayer1Strategy).existsAbstract(game.getRowVariables()), lowerChoice1, lowerChoice2);
            }
            
            if (predicates && (!player1ChoicesDifferent || predicates.get().getSource() == RefinementPredicates::Source::Guard)) {
                return predicates.get();
            } else {
                boost::optional<RefinementPredicates> additionalPredicates;
                
                storm::dd::Bdd<Type> upperChoice = pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
                storm::dd::Bdd<Type> upperChoice1 = (upperChoice && minPlayer2Strategy).existsAbstract(variablesToAbstract);
                storm::dd::Bdd<Type> upperChoice2 = (upperChoice && maxPlayer2Strategy).existsAbstract(variablesToAbstract);
                
                bool upperChoicesDifferent = !upperChoice1.exclusiveOr(upperChoice2).isZero() && !upperChoice1.isZero() && !upperChoice2.isZero();
                if (upperChoicesDifferent) {
                    STORM_LOG_TRACE("Deriving predicate based on upper choice.");
                    additionalPredicates = derivePredicatesFromDifferingChoices((pivotState && maxPlayer1Strategy).existsAbstract(game.getRowVariables()), upperChoice1, upperChoice2);
                }
                
                if (additionalPredicates) {
                    if (additionalPredicates.get().getSource() == RefinementPredicates::Source::Guard) {
                        return additionalPredicates.get();
                    } else {
                        predicates.get().addPredicates(additionalPredicates.get().getPredicates());
                    }
                }
            }
            
            STORM_LOG_THROW(static_cast<bool>(predicates), storm::exceptions::InvalidStateException, "Could not derive predicates for refinement.");
            
            return predicates.get();
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::pair<std::vector<std::vector<storm::expressions::Expression>>, std::map<storm::expressions::Variable, storm::expressions::Expression>> MenuGameRefiner<Type, ValueType>::buildTrace(storm::expressions::ExpressionManager& expressionManager, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& spanningTree, storm::dd::Bdd<Type> const& pivotState) const {
            std::vector<std::vector<storm::expressions::Expression>> predicates;
            
            // Prepare some variables.
            AbstractionInformation<Type> const& abstractionInformation = abstractor.get().getAbstractionInformation();
            std::set<storm::expressions::Variable> variablesToAbstract(game.getColumnVariables());
            variablesToAbstract.insert(game.getPlayer1Variables().begin(), game.getPlayer1Variables().end());
            variablesToAbstract.insert(game.getProbabilisticBranchingVariables().begin(), game.getProbabilisticBranchingVariables().end());

            storm::expressions::Expression initialExpression = abstractor.get().getInitialExpression();
            
            std::set<storm::expressions::Variable> oldVariables = initialExpression.getVariables();
            for (auto const& predicate : abstractionInformation.getPredicates()) {
                std::set<storm::expressions::Variable> usedVariables = predicate.getVariables();
                oldVariables.insert(usedVariables.begin(), usedVariables.end());
            }
            
            std::map<storm::expressions::Variable, storm::expressions::Variable> oldToNewVariables;
            for (auto const& variable : oldVariables) {
                oldToNewVariables[variable] = expressionManager.getVariable(variable.getName());
            }
            std::map<storm::expressions::Variable, storm::expressions::Expression> lastSubstitution;
            for (auto const& variable : oldToNewVariables) {
                lastSubstitution[variable.second] = variable.second;
            }
            std::map<storm::expressions::Variable, storm::expressions::Expression> stepVariableToCopiedVariableMap;
            
            // Start with the target state part of the trace.
            storm::storage::BitVector decodedTargetState = abstractionInformation.decodeState(pivotState);
            predicates.emplace_back(abstractionInformation.getPredicates(decodedTargetState));
            for (auto& predicate : predicates.back()) {
                predicate = predicate.changeManager(expressionManager);
            }
            
            // Perform a backward search for an initial state.
            storm::dd::Bdd<Type> currentState = pivotState;
            while ((currentState && game.getInitialStates()).isZero()) {
                storm::dd::Bdd<Type> predecessorTransition = currentState.swapVariables(game.getRowColumnMetaVariablePairs()) && spanningTree;
                std::tuple<storm::storage::BitVector, uint64_t, uint64_t> decodedPredecessor = abstractionInformation.decodeStatePlayer1ChoiceAndUpdate(predecessorTransition);
                
                // Create a new copy of each variable to use for this step.
                std::map<storm::expressions::Variable, storm::expressions::Expression> substitution;
                for (auto const& variablePair : oldToNewVariables) {
                    storm::expressions::Variable variableCopy = expressionManager.declareVariableCopy(variablePair.second);
                    substitution[variablePair.second] = variableCopy;
                    stepVariableToCopiedVariableMap[variableCopy] = variablePair.second;
                }
                
                // Retrieve the variable updates that the predecessor needs to perform to get to the current state.
                auto variableUpdates = abstractor.get().getVariableUpdates(std::get<1>(decodedPredecessor), std::get<2>(decodedPredecessor));
                for (auto const& oldNewVariablePair : oldToNewVariables) {
                    storm::expressions::Variable const& newVariable = oldNewVariablePair.second;

                    // If the variable was set, use its update expression.
                    auto updateIt = variableUpdates.find(oldNewVariablePair.first);
                    if (updateIt != variableUpdates.end()) {
                        auto const& update = *updateIt;

                        if (update.second.hasBooleanType()) {
                            predicates.back().push_back(storm::expressions::iff(lastSubstitution.at(newVariable), update.second.changeManager(expressionManager).substitute(substitution)));
                        } else {
                            predicates.back().push_back(lastSubstitution.at(newVariable) == update.second.changeManager(expressionManager).substitute(substitution));
                        }
                    } else {
                        // Otherwise, make sure that the new variable maintains the old value.
                        if (newVariable.hasBooleanType()) {
                            predicates.back().push_back(storm::expressions::iff(lastSubstitution.at(newVariable), substitution.at(newVariable)));
                        } else {
                            predicates.back().push_back(lastSubstitution.at(newVariable) == substitution.at(newVariable));
                        }
                    }
                }
                
                // Add the guard of the choice.
                predicates.back().push_back(abstractor.get().getGuard(std::get<1>(decodedPredecessor)).changeManager(expressionManager).substitute(substitution));
                
                // Retrieve the predicate valuation in the predecessor.
                predicates.emplace_back(abstractionInformation.getPredicates(std::get<0>(decodedPredecessor)));
                for (auto& predicate : predicates.back()) {
                    predicate = predicate.changeManager(expressionManager).substitute(substitution);
                }
                
                // Move backwards one step.
                lastSubstitution = std::move(substitution);
                currentState = predecessorTransition.existsAbstract(variablesToAbstract);
            }
            
            predicates.back().push_back(initialExpression.changeManager(expressionManager).substitute(lastSubstitution));
            return std::make_pair(predicates, stepVariableToCopiedVariableMap);
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        boost::optional<RefinementPredicates> MenuGameRefiner<Type, ValueType>::derivePredicatesFromInterpolation(storm::abstraction::MenuGame<Type, ValueType> const& game, PivotStateResult<Type, ValueType> const& pivotStateResult, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const {
            
            AbstractionInformation<Type> const& abstractionInformation = abstractor.get().getAbstractionInformation();
            
            // Compute the most probable path from any initial state to the pivot state.
            MostProbablePathsResult<Type, ValueType> mostProbablePathsResult;
            if (!pivotStateResult.mostProbablePathsResult) {
                mostProbablePathsResult = getMostProbablePathSpanningTree(game, pivotStateResult.fromDirection == storm::OptimizationDirection::Minimize ? minPlayer1Strategy && minPlayer2Strategy : maxPlayer1Strategy && maxPlayer2Strategy);
            } else {
                mostProbablePathsResult = pivotStateResult.mostProbablePathsResult.get();
            }
            
            // Create a new expression manager that we can use for the interpolation.
            std::shared_ptr<storm::expressions::ExpressionManager> interpolationManager = abstractionInformation.getExpressionManager().clone();
            
            // Build the trace of the most probable path in terms of which predicates hold in each step.
            std::pair<std::vector<std::vector<storm::expressions::Expression>>, std::map<storm::expressions::Variable, storm::expressions::Expression>> traceAndVariableSubstitution = buildTrace(*interpolationManager, game, mostProbablePathsResult.spanningTree, pivotStateResult.pivotState);
            auto const& trace = traceAndVariableSubstitution.first;
            auto const& variableSubstitution = traceAndVariableSubstitution.second;

            // Create solver and interpolation groups.
            storm::solver::MathsatSmtSolver interpolatingSolver(*interpolationManager, storm::solver::MathsatSmtSolver::Options(true, false, true));
            uint64_t stepCounter = 0;
            auto traceIt = trace.rbegin();
            auto traceIte = trace.rend();
            for (; traceIt != traceIte; ++traceIt) {
                auto const& step = *traceIt;
                interpolatingSolver.push();
                
                interpolatingSolver.setInterpolationGroup(stepCounter);
                for (auto const& predicate : step) {
                    interpolatingSolver.add(predicate);
                }
                ++stepCounter;
            
                storm::solver::SmtSolver::CheckResult result = interpolatingSolver.check();
                // If the result already became unsatisfiable
                if (result == storm::solver::SmtSolver::CheckResult::Unsat) {
                    STORM_LOG_TRACE("Trace formula became unsatisfiable after step " << (stepCounter - 1) << ".");
                    break;
                }
            }
            
            // Now encode the trace as an SMT problem.
            storm::solver::SmtSolver::CheckResult result = interpolatingSolver.check();
            if (result == storm::solver::SmtSolver::CheckResult::Unsat) {
                STORM_LOG_TRACE("Trace formula is unsatisfiable. Starting interpolation.");
                
                std::vector<storm::expressions::Expression> interpolants;
                std::vector<uint64_t> prefix;
                for (uint64_t step = 0; step + 1 < stepCounter; ++step) {
                    prefix.push_back(step);
                    storm::expressions::Expression interpolant = interpolatingSolver.getInterpolant(prefix).substitute(variableSubstitution).changeManager(abstractionInformation.getExpressionManager());
                    if (!interpolant.isTrue() && !interpolant.isFalse()) {
                        STORM_LOG_DEBUG("Derived new predicate (based on interpolation): " << interpolant);
                        interpolants.push_back(interpolant);
                    }
                }
                return boost::make_optional(RefinementPredicates(RefinementPredicates::Source::Interpolation, interpolants));
            } else {
                STORM_LOG_TRACE("Trace formula is satisfiable, not using interpolation.");
            }
            
            return boost::none;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool MenuGameRefiner<Type, ValueType>::refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QualitativeGameResultMinMax<Type> const& qualitativeResult) const {
            STORM_LOG_TRACE("Trying refinement after qualitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = qualitativeResult.prob0Min.getPlayer1Strategy();
            storm::dd::Bdd<Type> minPlayer2Strategy = qualitativeResult.prob0Min.getPlayer2Strategy();
            storm::dd::Bdd<Type> maxPlayer1Strategy = qualitativeResult.prob1Max.getPlayer1Strategy();
            storm::dd::Bdd<Type> maxPlayer2Strategy = qualitativeResult.prob1Max.getPlayer2Strategy();
            
            // Redirect all player 1 choices of the min strategy to that of the max strategy if this leads to a player 2
            // state that is also a prob 0 state.
            auto oldMinPlayer1Strategy = minPlayer1Strategy;
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
            PivotStateResult<Type, ValueType> pivotStateResult = pickPivotState<Type, ValueType>(pivotSelectionHeuristic, game, pivotStateCandidatesResult, qualitativeResult, boost::none);

//            // SANITY CHECK TO MAKE SURE OUR STRATEGIES ARE NOT BROKEN.
//            // FIXME.
//            auto min1ChoiceInPivot = pivotStateResult.pivotState && game.getExtendedTransitionMatrix().toBdd() && minPlayer1Strategy;
//            STORM_LOG_ASSERT(!min1ChoiceInPivot.isZero(), "wth?");
//            bool min1ChoiceInPivotIsProb0Min = !(min1ChoiceInPivot && qualitativeResult.prob0Min.getPlayer2States()).isZero();
//            bool min1ChoiceInPivotIsProb0Max = !(min1ChoiceInPivot && qualitativeResult.prob0Max.getPlayer2States()).isZero();
//            bool min1ChoiceInPivotIsProb1Min = !(min1ChoiceInPivot && qualitativeResult.prob1Min.getPlayer2States()).isZero();
//            bool min1ChoiceInPivotIsProb1Max = !(min1ChoiceInPivot && qualitativeResult.prob1Max.getPlayer2States()).isZero();
//            std::cout << "after redirection (min)" << std::endl;
//            std::cout << "min choice is prob0 in min? " << min1ChoiceInPivotIsProb0Min << ", max? " << min1ChoiceInPivotIsProb0Max << std::endl;
//            std::cout << "min choice is prob1 in min? " << min1ChoiceInPivotIsProb1Min << ", max? " << min1ChoiceInPivotIsProb1Max << std::endl;
//            std::cout << "min" << std::endl;
//            for (auto const& e : (min1ChoiceInPivot && minPlayer2Strategy).template toAdd<ValueType>()) {
//                std::cout << e.first << " -> " << e.second << std::endl;
//            }
//            std::cout << "max" << std::endl;
//            for (auto const& e : (min1ChoiceInPivot && maxPlayer2Strategy).template toAdd<ValueType>()) {
//                std::cout << e.first << " -> " << e.second << std::endl;
//            }
//            bool different = (min1ChoiceInPivot && minPlayer2Strategy) != (min1ChoiceInPivot && maxPlayer2Strategy);
//            std::cout << "min/max choice of player 2 is different? " << different << std::endl;
//            bool min1MinPlayer2Choice = !(min1ChoiceInPivot && minPlayer2Strategy).isZero();
//            bool min1MaxPlayer2Choice = !(min1ChoiceInPivot && maxPlayer2Strategy).isZero();
//            std::cout << "max/min choice there? " << min1MinPlayer2Choice << std::endl;
//            std::cout << "max/max choice there? " << min1MaxPlayer2Choice << std::endl;
//
//            auto max1ChoiceInPivot = pivotStateResult.pivotState && game.getExtendedTransitionMatrix().toBdd() && maxPlayer1Strategy;
//            STORM_LOG_ASSERT(!max1ChoiceInPivot.isZero(), "wth?");
//            bool max1ChoiceInPivotIsProb0Min = !(max1ChoiceInPivot && qualitativeResult.prob0Min.getPlayer2States()).isZero();
//            bool max1ChoiceInPivotIsProb0Max = !(max1ChoiceInPivot && qualitativeResult.prob0Max.getPlayer2States()).isZero();
//            bool max1ChoiceInPivotIsProb1Min = !(max1ChoiceInPivot && qualitativeResult.prob1Min.getPlayer2States()).isZero();
//            bool max1ChoiceInPivotIsProb1Max = !(max1ChoiceInPivot && qualitativeResult.prob1Max.getPlayer2States()).isZero();
//            std::cout << "after redirection (max)" << std::endl;
//            std::cout << "max choice is prob0 in min? " << max1ChoiceInPivotIsProb0Min << ", max? " << max1ChoiceInPivotIsProb0Max << std::endl;
//            std::cout << "max choice is prob1 in min? " << max1ChoiceInPivotIsProb1Min << ", max? " << max1ChoiceInPivotIsProb1Max << std::endl;
//            different = (max1ChoiceInPivot && minPlayer2Strategy) != (max1ChoiceInPivot && maxPlayer2Strategy);
//            std::cout << "min/max choice of player 2 is different? " << different << std::endl;
//            bool max1MinPlayer2Choice = !(max1ChoiceInPivot && minPlayer2Strategy).isZero();
//            bool max1MaxPlayer2Choice = !(max1ChoiceInPivot && maxPlayer2Strategy).isZero();
//            std::cout << "max/min choice there? " << max1MinPlayer2Choice << std::endl;
//            std::cout << "max/max choice there? " << max1MaxPlayer2Choice << std::endl;

            boost::optional<RefinementPredicates> predicates;
            if (useInterpolation) {
                predicates = derivePredicatesFromInterpolation(game, pivotStateResult, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            }
            if (predicates) {
                STORM_LOG_TRACE("Obtained predicates by interpolation.");
            } else {
                predicates = derivePredicatesFromPivotState(game, pivotStateResult.pivotState, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            }
            STORM_LOG_THROW(static_cast<bool>(predicates), storm::exceptions::InvalidStateException, "Predicates needed to continue.");
            
            // Derive predicate based on the selected pivot state.
            std::vector<storm::expressions::Expression> preparedPredicates = preprocessPredicates(predicates.get().getPredicates(), predicates.get().getSource());
            performRefinement(createGlobalRefinement(preparedPredicates));
            return true;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool MenuGameRefiner<Type, ValueType>::refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QuantitativeGameResultMinMax<Type, ValueType> const& quantitativeResult) const {
            STORM_LOG_TRACE("Refining after quantitative check.");
            // Get all relevant strategies.
            storm::dd::Bdd<Type> minPlayer1Strategy = quantitativeResult.min.getPlayer1Strategy();
            storm::dd::Bdd<Type> minPlayer2Strategy = quantitativeResult.min.getPlayer2Strategy();
            storm::dd::Bdd<Type> maxPlayer1Strategy = quantitativeResult.max.getPlayer1Strategy();
            storm::dd::Bdd<Type> maxPlayer2Strategy = quantitativeResult.max.getPlayer2Strategy();
            
            // Compute all reached pivot states.
            PivotStateCandidatesResult<Type> pivotStateCandidatesResult = computePivotStates(game, transitionMatrixBdd, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            
            STORM_LOG_ASSERT(!pivotStateCandidatesResult.pivotStates.isZero(), "Unable to refine without pivot state candidates.");
            
            // Now that we have the pivot state candidates, we need to pick one.
            PivotStateResult<Type, ValueType> pivotStateResult = pickPivotState<Type, ValueType>(pivotSelectionHeuristic, game, pivotStateCandidatesResult, boost::none, quantitativeResult);

            boost::optional<RefinementPredicates> predicates;
            if (useInterpolation) {
                predicates = derivePredicatesFromInterpolation(game, pivotStateResult, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            }
            if (predicates) {
                STORM_LOG_TRACE("Obtained predicates by interpolation.");
            } else {
                predicates = derivePredicatesFromPivotState(game, pivotStateResult.pivotState, minPlayer1Strategy, minPlayer2Strategy, maxPlayer1Strategy, maxPlayer2Strategy);
            }
            STORM_LOG_THROW(static_cast<bool>(predicates), storm::exceptions::InvalidStateException, "Predicates needed to continue.");
            
            std::vector<storm::expressions::Expression> preparedPredicates = preprocessPredicates(predicates.get().getPredicates(), predicates.get().getSource());
            performRefinement(createGlobalRefinement(preparedPredicates));
            return true;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::vector<storm::expressions::Expression> MenuGameRefiner<Type, ValueType>::preprocessPredicates(std::vector<storm::expressions::Expression> const& predicates, RefinementPredicates::Source const& source) const {
            bool split = source == RefinementPredicates::Source::WeakestPrecondition && splitPredicates;
            split |= source == RefinementPredicates::Source::Interpolation && splitPredicates;
            split |= splitAll;
            
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
                            if (equivalenceChecker.areEquivalent(atom, oldPredicate) || equivalenceChecker.areEquivalent(atom, !oldPredicate)) {
                                addAtom = false;
                                break;
                            }
                        }
                        for (auto const& addedAtom : cleanedAtoms) {
                            if (equivalenceChecker.areEquivalent(addedAtom, atom) || equivalenceChecker.areEquivalent(addedAtom, !atom)) {
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
                STORM_LOG_TRACE("Refining with " << command.getPredicates().size() << " predicates.");
                for (auto const& predicate : command.getPredicates()) {
                    STORM_LOG_TRACE(predicate);
                }
                abstractor.get().refine(command);
            }
            
            STORM_LOG_TRACE("Current set of predicates:");
            for (auto const& predicate : abstractor.get().getAbstractionInformation().getPredicates()) {
                STORM_LOG_TRACE(predicate);
            }
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        bool MenuGameRefiner<Type, ValueType>::addedAllGuards() const {
            return addedAllGuardsFlag;
        }
        
        template class MenuGameRefiner<storm::dd::DdType::CUDD, double>;
        template class MenuGameRefiner<storm::dd::DdType::Sylvan, double>;

#ifdef STORM_HAVE_CARL
        // Currently, this instantiation does not work.
        // template class MenuGameRefiner<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        
    }
}
