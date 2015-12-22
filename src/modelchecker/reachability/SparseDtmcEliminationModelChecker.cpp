#include "src/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"

#include <algorithm>
#include <random>
#include <chrono>

#include "src/adapters/CarlAdapter.h"

#include "src/settings/modules/SparseDtmcEliminationModelCheckerSettings.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/SettingsManager.h"

#include "src/storage/StronglyConnectedComponentDecomposition.h"

#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/utility/graph.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"

#include "src/exceptions/InvalidPropertyException.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/IllegalArgumentException.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        uint_fast64_t estimateComplexity(ValueType const& value) {
            return 1;
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        uint_fast64_t estimateComplexity(storm::RationalFunction const& value) {
            if (storm::utility::isConstant(value)) {
                return 1;
            }
            if (value.denominator().isConstant()) {
                return value.nominator().complexity();
            } else {
                return value.denominator().complexity() * value.nominator().complexity();
            }
        }
#endif
        
        bool eliminationOrderNeedsDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order) {
            return order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::Forward ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::ForwardReversed ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::Backward ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::BackwardReversed;
        }
        
        bool eliminationOrderNeedsForwardDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order) {
            return order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::Forward ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::ForwardReversed;
        }
        
        bool eliminationOrderNeedsReversedDistances(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order) {
            return order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::ForwardReversed ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::BackwardReversed;
        }
        
        bool eliminationOrderIsPenaltyBased(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order) {
            return order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::StaticPenalty ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::DynamicPenalty ||
            order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::RegularExpression;
        }
        
        bool eliminationOrderIsStatic(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder const& order) {
            return eliminationOrderNeedsDistances(order) || order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::StaticPenalty;
        }
        
        template<typename SparseDtmcModelType>
        SparseDtmcEliminationModelChecker<SparseDtmcModelType>::SparseDtmcEliminationModelChecker(storm::models::sparse::Dtmc<ValueType> const& model, bool computeResultsForInitialStatesOnly) : SparsePropositionalModelChecker<SparseDtmcModelType>(model), computeResultsForInitialStatesOnly(computeResultsForInitialStatesOnly) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::canHandle(storm::logic::Formula const& formula) const {
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                return this->canHandle(probabilityOperatorFormula.getSubformula());
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula const& rewardOperatorFormula = formula.asRewardOperatorFormula();
                return this->canHandle(rewardOperatorFormula.getSubformula());
            } else if (formula.isUntilFormula() || formula.isEventuallyFormula()) {
                if (formula.isUntilFormula()) {
                    storm::logic::UntilFormula const& untilFormula = formula.asUntilFormula();
                    if (untilFormula.getLeftSubformula().isPropositionalFormula() && untilFormula.getRightSubformula().isPropositionalFormula()) {
                        return true;
                    }
                } else if (formula.isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& eventuallyFormula = formula.asEventuallyFormula();
                    if (eventuallyFormula.getSubformula().isPropositionalFormula()) {
                        return true;
                    }
                }
            } else if (formula.isReachabilityRewardFormula()) {
                storm::logic::ReachabilityRewardFormula reachabilityRewardFormula = formula.asReachabilityRewardFormula();
                if (reachabilityRewardFormula.getSubformula().isPropositionalFormula()) {
                    return true;
                }
            } else if (formula.isConditionalPathFormula()) {
                storm::logic::ConditionalPathFormula conditionalPathFormula = formula.asConditionalPathFormula();
                if (conditionalPathFormula.getLeftSubformula().isEventuallyFormula() && conditionalPathFormula.getRightSubformula().isEventuallyFormula()) {
                    return this->canHandle(conditionalPathFormula.getLeftSubformula()) && this->canHandle(conditionalPathFormula.getRightSubformula());
                }
            } else if (formula.isPropositionalFormula()) {
                return true;
            }
            return false;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            storm::storage::BitVector const& phiStates = leftResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector const& psiStates = rightResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            
            // Then, compute the subset of states that has a probability of 0 or 1, respectively.
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(this->getModel(), phiStates, psiStates);
            storm::storage::BitVector statesWithProbability0 = statesWithProbability01.first;
            storm::storage::BitVector statesWithProbability1 = statesWithProbability01.second;
            storm::storage::BitVector maybeStates = ~(statesWithProbability0 | statesWithProbability1);

            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (computeResultsForInitialStatesOnly && this->getModel().getInitialStates().isDisjointFrom(maybeStates)) {
                STORM_LOG_DEBUG("The probability for all initial states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            } else if (maybeStates.empty()) {
                STORM_LOG_DEBUG("The probability for all states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            }
            
            std::vector<ValueType> result(maybeStates.size());
            if (furtherComputationNeeded) {
                // If we compute the results for the initial states only, we can cut off all maybe state that are not
                // reachable from them.
                if (computeResultsForInitialStatesOnly) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), maybeStates, statesWithProbability1);
                
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    maybeStates &= reachableStates;
                }
                
                // Create a vector for the probabilities to go to a state with probability 1 in one step.
                std::vector<ValueType> oneStepProbabilities = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, statesWithProbability1);
                
                // Determine the set of initial states of the sub-model.
                storm::storage::BitVector newInitialStates = this->getModel().getInitialStates() % maybeStates;
                
                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                storm::storage::SparseMatrix<ValueType> submatrixTransposed = submatrix.transpose();
                
                boost::optional<std::vector<ValueType>> missingStateRewards;
                std::vector<ValueType> subresult = computeReachabilityValues(submatrix, oneStepProbabilities, submatrixTransposed, newInitialStates, phiStates, psiStates, missingStateRewards);
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, subresult);
            }

            // Construct full result.
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability0, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability1, storm::utility::one<ValueType>());

            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (computeResultsForInitialStatesOnly) {
                // If we computed the results for the initial (and prob 0 and prob1) states only, we need to filter the
                // result to only communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(~maybeStates | this->getModel().getInitialStates()));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            std::unique_ptr<CheckResult> subResultPointer = this->check(rewardPathFormula.getSubformula());
            storm::storage::BitVector phiStates(this->getModel().getNumberOfStates(), true);
            storm::storage::BitVector const& psiStates = subResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            
            // Do some sanity checks to establish some required properties.
            RewardModelType const& rewardModel = this->getModel().getRewardModel(rewardModelName ? rewardModelName.get() : "");
            STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::IllegalArgumentException, "Input model does not have a reward model.");
            
            // Then, compute the subset of states that has a reachability reward less than infinity.
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(this->getModel().getBackwardTransitions(), trueStates, psiStates);
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~psiStates & ~infinityStates;
            
            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (computeResultsForInitialStatesOnly) {
                if (this->getModel().getInitialStates().isSubsetOf(infinityStates)) {
                    STORM_LOG_DEBUG("The reward of all initial states was found in a preprocessing step.");
                    furtherComputationNeeded = false;
                }
                if (this->getModel().getInitialStates().isSubsetOf(psiStates)) {
                    STORM_LOG_DEBUG("The reward of all initial states was found in a preprocessing step.");
                    furtherComputationNeeded = false;
                }
            }

            std::vector<ValueType> result(maybeStates.size());
            if (furtherComputationNeeded) {
                // If we compute the results for the initial states only, we can cut off all maybe state that are not
                // reachable from them.
                if (computeResultsForInitialStatesOnly) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), maybeStates, psiStates);
                    
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    maybeStates &= reachableStates;
                }
                
                // Create a vector for the probabilities to go to a state with probability 1 in one step.
                std::vector<ValueType> oneStepProbabilities = this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, psiStates);

                // Determine the set of initial states of the sub-model.
                storm::storage::BitVector newInitialStates = this->getModel().getInitialStates() % maybeStates;

                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                storm::storage::SparseMatrix<ValueType> submatrixTransposed = submatrix.transpose();
                
                // Project the state reward vector to all maybe-states.
                boost::optional<std::vector<ValueType>> optionalStateRewards = rewardModel.getTotalRewardVector(maybeStates.getNumberOfSetBits(), this->getModel().getTransitionMatrix(), maybeStates);

                std::vector<ValueType> subresult = computeReachabilityValues(submatrix, oneStepProbabilities, submatrixTransposed, newInitialStates, phiStates, psiStates, optionalStateRewards);
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, subresult);
            }
            
            // Construct full result.
            storm::utility::vector::setVectorValues<ValueType>(result, infinityStates, storm::utility::infinity<ValueType>());
            storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::zero<ValueType>());

            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (computeResultsForInitialStatesOnly) {
                // If we computed the results for the initial (and inf) states only, we need to filter the result to
                // only communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(~maybeStates | this->getModel().getInitialStates()));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            std::chrono::high_resolution_clock::time_point totalTimeStart = std::chrono::high_resolution_clock::now();
            
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            STORM_LOG_THROW(pathFormula.getLeftSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Expected 'eventually' formula.");
            STORM_LOG_THROW(pathFormula.getRightSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Expected 'eventually' formula.");
            
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula().asEventuallyFormula().getSubformula());
            storm::storage::BitVector phiStates = leftResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector psiStates = rightResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            
            // Do some sanity checks to establish some required properties.
            // STORM_LOG_WARN_COND(storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::State, "The chosen elimination method is not available for computing conditional probabilities. Falling back to regular state elimination.");
            STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
            storm::storage::sparse::state_type initialState = *this->getModel().getInitialStates().begin();
            
            storm::storage::SparseMatrix<ValueType> backwardTransitions = this->getModel().getBackwardTransitions();
            
            // Compute the 'true' psi states, i.e. those psi states that can be reached without passing through another psi state first.
            psiStates = storm::utility::graph::getReachableStates(this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), trueStates, psiStates) & psiStates;
            
            std::pair<storm::storage::BitVector, storm::storage::BitVector> statesWithProbability01 = storm::utility::graph::performProb01(backwardTransitions, trueStates, psiStates);
            storm::storage::BitVector statesWithProbabilityGreater0 = ~statesWithProbability01.first;
            storm::storage::BitVector statesWithProbability1 = std::move(statesWithProbability01.second);
            
            STORM_LOG_THROW(this->getModel().getInitialStates().isSubsetOf(statesWithProbabilityGreater0), storm::exceptions::InvalidPropertyException, "The condition of the conditional probability has zero probability.");
            
            // If the initial state is known to have probability 1 of satisfying the condition, we can apply regular model checking.
            if (this->getModel().getInitialStates().isSubsetOf(statesWithProbability1)) {
                STORM_LOG_INFO("The condition holds with probability 1, so the regular reachability probability is computed.");
                std::shared_ptr<storm::logic::BooleanLiteralFormula> trueFormula = std::make_shared<storm::logic::BooleanLiteralFormula>(true);
                std::shared_ptr<storm::logic::UntilFormula> untilFormula = std::make_shared<storm::logic::UntilFormula>(trueFormula, pathFormula.getLeftSubformula().asSharedPointer());
                return this->computeUntilProbabilities(*untilFormula);
            }
            
            // From now on, we know the condition does not have a trivial probability in the initial state.
            
            // Compute the states that can be reached on a path that has a psi state in it.
            storm::storage::BitVector statesWithPsiPredecessor = storm::utility::graph::performProbGreater0(this->getModel().getTransitionMatrix(), trueStates, psiStates);
            storm::storage::BitVector statesReachingPhi = storm::utility::graph::performProbGreater0(backwardTransitions, trueStates, phiStates);
            
            // The set of states we need to consider are those that have a non-zero probability to satisfy the condition or are on some path that has a psi state in it.
            STORM_LOG_TRACE("Initial state: " << this->getModel().getInitialStates());
            STORM_LOG_TRACE("Phi states: " << phiStates);
            STORM_LOG_TRACE("Psi state: " << psiStates);
            STORM_LOG_TRACE("States with probability greater 0 of satisfying the condition: " << statesWithProbabilityGreater0);
            STORM_LOG_TRACE("States with psi predecessor: " << statesWithPsiPredecessor);
            STORM_LOG_TRACE("States reaching phi: " << statesReachingPhi);
            storm::storage::BitVector maybeStates = statesWithProbabilityGreater0 | (statesWithPsiPredecessor & statesReachingPhi);
            STORM_LOG_TRACE("Found " << maybeStates.getNumberOfSetBits() << " relevant states: " << maybeStates);
            
            // Determine the set of initial states of the sub-DTMC.
            storm::storage::BitVector newInitialStates = this->getModel().getInitialStates() % maybeStates;
            STORM_LOG_TRACE("Found new initial states: " << newInitialStates << " (old: " << this->getModel().getInitialStates() << ")");
            
            // Create a dummy vector for the one-step probabilities.
            std::vector<ValueType> oneStepProbabilities(maybeStates.getNumberOfSetBits(), storm::utility::zero<ValueType>());
            
            // We then build the submatrix that only has the transitions of the maybe states.
            storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
            storm::storage::SparseMatrix<ValueType> submatrixTransposed = submatrix.transpose();
            
            // The states we want to eliminate are those that are tagged with "maybe" but are not a phi or psi state.
            phiStates = phiStates % maybeStates;
            
            // If there are no phi states in the reduced model, the conditional probability is trivially zero.
            if (phiStates.empty()) {
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initialState, storm::utility::zero<ValueType>()));
            }
            
            psiStates = psiStates % maybeStates;
            
            // Keep only the states that we do not eliminate in the maybe states.
            maybeStates = phiStates | psiStates;
            
            STORM_LOG_TRACE("Phi states in reduced model " << phiStates);
            STORM_LOG_TRACE("Psi states in reduced model " << psiStates);
            storm::storage::BitVector statesToEliminate = ~maybeStates & ~newInitialStates;
            STORM_LOG_TRACE("Eliminating the states " << statesToEliminate);
            
            // Before starting the model checking process, we assign priorities to states so we can use them to
            // impose ordering constraints later.
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            if (eliminationOrderNeedsDistances(order)) {
                distanceBasedPriorities = getDistanceBasedPriorities(submatrix, submatrixTransposed, newInitialStates, oneStepProbabilities,
                                                             eliminationOrderNeedsForwardDistances(order),
                                                             eliminationOrderNeedsReversedDistances(order));
            }
            
            std::chrono::high_resolution_clock::time_point conversionStart = std::chrono::high_resolution_clock::now();
            FlexibleSparseMatrix flexibleMatrix = getFlexibleSparseMatrix(submatrix);
            FlexibleSparseMatrix flexibleBackwardTransitions = getFlexibleSparseMatrix(submatrixTransposed, true);
            std::chrono::high_resolution_clock::time_point conversionEnd = std::chrono::high_resolution_clock::now();
            
            std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, flexibleMatrix, flexibleBackwardTransitions, oneStepProbabilities, statesToEliminate);

            STORM_LOG_INFO("Computing conditional probilities." << std::endl);
            boost::optional<std::vector<ValueType>> missingStateRewards;
            std::chrono::high_resolution_clock::time_point modelCheckingStart = std::chrono::high_resolution_clock::now();
            uint_fast64_t numberOfStatesToEliminate = statePriorities->size();
            STORM_LOG_INFO("Eliminating " << numberOfStatesToEliminate << " states using the state elimination technique." << std::endl);
            performPrioritizedStateElimination(statePriorities, flexibleMatrix, flexibleBackwardTransitions, oneStepProbabilities, missingStateRewards);
            STORM_LOG_INFO("Eliminated " << numberOfStatesToEliminate << " states." << std::endl);
            
            // Eliminate the transitions going into the initial state (if there are any).
            if (!flexibleBackwardTransitions.getRow(*newInitialStates.begin()).empty()) {
                eliminateState(flexibleMatrix, oneStepProbabilities, *newInitialStates.begin(), flexibleBackwardTransitions, missingStateRewards, statePriorities.get(), false);
            }
            
            // Now we need to basically eliminate all chains of not-psi states after phi states and chains of not-phi
            // states after psi states.
            for (auto const& trans1 : flexibleMatrix.getRow(*newInitialStates.begin())) {
                auto initialStateSuccessor = trans1.getColumn();
                
                STORM_LOG_TRACE("Exploring successor " << initialStateSuccessor << " of the initial state.");
                
                if (phiStates.get(initialStateSuccessor)) {
                    STORM_LOG_TRACE("Is a phi state.");
                    
                    // If the state is both a phi and a psi state, we do not need to eliminate chains.
                    if (psiStates.get(initialStateSuccessor)) {
                        continue;
                    }
                    
                    // At this point, we know that the state satisfies phi and not psi.
                    // This means, we must compute the probability to reach psi states, which in turn means that we need
                    // to eliminate all chains of non-psi states between the current state and psi states.
                    bool hasNonPsiSuccessor = true;
                    while (hasNonPsiSuccessor) {
                        hasNonPsiSuccessor = false;
                        
                        // Only treat the state if it has an outgoing transition other than a self-loop.
                        auto const currentRow = flexibleMatrix.getRow(initialStateSuccessor);
                        if (currentRow.size() > 1 || (!currentRow.empty() && currentRow.front().getColumn() != initialStateSuccessor)) {
                            for (auto const& element : currentRow) {
                                // If any of the successors is a phi state, we eliminate it (wrt. all its phi predecessors).
                                if (!psiStates.get(element.getColumn())) {
                                    typename FlexibleSparseMatrix::row_type const& successorRow = flexibleMatrix.getRow(element.getColumn());
                                    // Eliminate the successor only if there possibly is a psi state reachable through it.
                                    if (successorRow.size() > 1 || (!successorRow.empty() && successorRow.front().getColumn() != element.getColumn())) {
                                        STORM_LOG_TRACE("Found non-psi successor " << element.getColumn() << " that needs to be eliminated.");
                                        eliminateState(flexibleMatrix, oneStepProbabilities, element.getColumn(), flexibleBackwardTransitions, missingStateRewards, nullptr, false, true, phiStates);
                                        hasNonPsiSuccessor = true;
                                    }
                                }
                            }
                            STORM_LOG_ASSERT(!flexibleMatrix.getRow(initialStateSuccessor).empty(), "(1) New transitions expected to be non-empty.");
                        }
                    }
                } else {
                    STORM_LOG_ASSERT(psiStates.get(initialStateSuccessor), "Expected psi state.");
                    STORM_LOG_TRACE("Is a psi state.");
                    
                    // At this point, we know that the state satisfies psi and not phi.
                    // This means, we must compute the probability to reach phi states, which in turn means that we need
                    // to eliminate all chains of non-phi states between the current state and phi states.
                    
                    bool hasNonPhiSuccessor = true;
                    while (hasNonPhiSuccessor) {
                        hasNonPhiSuccessor = false;
                        
                        // Only treat the state if it has an outgoing transition other than a self-loop.
                        auto const currentRow = flexibleMatrix.getRow(initialStateSuccessor);
                        if (currentRow.size() > 1 || (!currentRow.empty() && currentRow.front().getColumn() != initialStateSuccessor)) {
                            for (auto const& element : currentRow) {
                                // If any of the successors is a psi state, we eliminate it (wrt. all its psi predecessors).
                                if (!phiStates.get(element.getColumn())) {
                                    typename FlexibleSparseMatrix::row_type const& successorRow = flexibleMatrix.getRow(element.getColumn());
                                    if (successorRow.size() > 1 || (!successorRow.empty() && successorRow.front().getColumn() != element.getColumn())) {
                                        STORM_LOG_TRACE("Found non-phi successor " << element.getColumn() << " that needs to be eliminated.");
                                        eliminateState(flexibleMatrix, oneStepProbabilities, element.getColumn(), flexibleBackwardTransitions, missingStateRewards, nullptr, false, true, psiStates);
                                        hasNonPhiSuccessor = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            ValueType numerator = storm::utility::zero<ValueType>();
            ValueType denominator = storm::utility::zero<ValueType>();
            
            for (auto const& trans1 : flexibleMatrix.getRow(*newInitialStates.begin())) {
                auto initialStateSuccessor = trans1.getColumn();
                if (phiStates.get(initialStateSuccessor)) {
                    if (psiStates.get(initialStateSuccessor)) {
                        numerator += trans1.getValue();
                        denominator += trans1.getValue();
                    } else {
                        ValueType additiveTerm = storm::utility::zero<ValueType>();
                        for (auto const& trans2 : flexibleMatrix.getRow(initialStateSuccessor)) {
                            if (psiStates.get(trans2.getColumn())) {
                                additiveTerm += trans2.getValue();
                            }
                        }
                        additiveTerm *= trans1.getValue();
                        numerator += additiveTerm;
                        denominator += additiveTerm;
                    }
                } else {
                    STORM_LOG_ASSERT(psiStates.get(initialStateSuccessor), "Expected psi state.");
                    denominator += trans1.getValue();
                    ValueType additiveTerm = storm::utility::zero<ValueType>();
                    for (auto const& trans2 : flexibleMatrix.getRow(initialStateSuccessor)) {
                        if (phiStates.get(trans2.getColumn())) {
                            additiveTerm += trans2.getValue();
                        }
                    }
                    numerator += trans1.getValue() * additiveTerm;
                }
            }
            std::chrono::high_resolution_clock::time_point modelCheckingEnd = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point totalTimeEnd = std::chrono::high_resolution_clock::now();
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::chrono::high_resolution_clock::duration conversionTime = conversionEnd - conversionStart;
                std::chrono::milliseconds conversionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime);
                std::chrono::high_resolution_clock::duration modelCheckingTime = modelCheckingEnd - modelCheckingStart;
                std::chrono::milliseconds modelCheckingTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(modelCheckingTime);
                std::chrono::high_resolution_clock::duration totalTime = totalTimeEnd - totalTimeStart;
                std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                
                STORM_PRINT_AND_LOG(std::endl);
                STORM_PRINT_AND_LOG("Time breakdown:" << std::endl);
                STORM_PRINT_AND_LOG("    * time for conversion: " << conversionTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("    * time for checking: " << modelCheckingTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("------------------------------------------" << std::endl);
                STORM_PRINT_AND_LOG("    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG(std::endl);
            }
            
            return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(initialState, numerator / denominator));
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StatePriorityQueue> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& distanceBasedStatePriorities, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<typename SparseDtmcModelType::ValueType>& oneStepProbabilities, storm::storage::BitVector const& states) {
            
            STORM_LOG_TRACE("Creating state priority queue for states " << states);
            
            // Get the settings to customize the priority queue.
            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            
            std::vector<storm::storage::sparse::state_type> sortedStates(states.begin(), states.end());

            if (order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::Random) {
                std::random_device randomDevice;
                std::mt19937 generator(randomDevice());
                std::shuffle(sortedStates.begin(), sortedStates.end(), generator);
                return std::make_unique<StaticStatePriorityQueue>(sortedStates);
            } else {
                if (eliminationOrderNeedsDistances(order)) {
                    STORM_LOG_THROW(static_cast<bool>(distanceBasedStatePriorities), storm::exceptions::InvalidStateException, "Unable to build state priority queue without distance-based priorities.");
                    std::sort(sortedStates.begin(), sortedStates.end(), [&distanceBasedStatePriorities] (storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) { return distanceBasedStatePriorities.get()[state1] < distanceBasedStatePriorities.get()[state2]; } );
                    return std::make_unique<StaticStatePriorityQueue>(sortedStates);
                } else if (eliminationOrderIsPenaltyBased(order)) {
                    std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> statePenalties(sortedStates.size());
                    PenaltyFunctionType penaltyFunction = order == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::RegularExpression ? computeStatePenaltyRegularExpression : computeStatePenalty;
                    for (uint_fast64_t index = 0; index < sortedStates.size(); ++index) {
                        statePenalties[index] = std::make_pair(sortedStates[index], penaltyFunction(sortedStates[index], transitionMatrix, backwardTransitions, oneStepProbabilities));
                    }
                    
                    std::sort(statePenalties.begin(), statePenalties.end(), [] (std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& statePenalty1, std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& statePenalty2) { return statePenalty1.second < statePenalty2.second; } );
                    
                    if (eliminationOrderIsStatic(order)) {
                        // For the static penalty version, we need to strip the penalties to create the queue.
                        for (uint_fast64_t index = 0; index < sortedStates.size(); ++index) {
                            sortedStates[index] = statePenalties[index].first;
                        }
                        return std::make_unique<StaticStatePriorityQueue>(sortedStates);
                    } else {
                        // For the dynamic penalty version, we need to give the full state-penalty pairs.
                        return std::make_unique<DynamicPenaltyStatePriorityQueue>(statePenalties, penaltyFunction);
                    }
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Illlegal elimination order selected.");
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performPrioritizedStateElimination(std::unique_ptr<StatePriorityQueue>& priorityQueue, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, std::vector<typename SparseDtmcModelType::ValueType>& oneStepProbabilities, boost::optional<std::vector<ValueType>>& stateRewards) {
            while (priorityQueue->hasNextState()) {
                storm::storage::sparse::state_type state = priorityQueue->popNextState();
//                std::cout << "Eliminating state with custom penalty " << computeStatePenalty(state, transitionMatrix, backwardTransitions, oneStepProbabilities) << " and regular expression penalty " << computeStatePenaltyRegularExpression(state, transitionMatrix, backwardTransitions, oneStepProbabilities) << "." << std::endl;
                eliminateState(transitionMatrix, oneStepProbabilities, state, backwardTransitions, stateRewards, priorityQueue.get());
                oneStepProbabilities[state] = storm::utility::zero<ValueType>();
                STORM_LOG_ASSERT(checkConsistent(transitionMatrix, backwardTransitions), "The forward and backward transition matrices became inconsistent.");
            }
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performOrdinaryStateElimination(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, std::vector<typename SparseDtmcModelType::ValueType>& oneStepProbabilities, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
            std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, transitionMatrix, backwardTransitions, oneStepProbabilities, subsystem & ~initialStates);
            
            std::size_t numberOfStatesToEliminate = statePriorities->size();
            STORM_LOG_DEBUG("Eliminating " << numberOfStatesToEliminate << " states using the state elimination technique." << std::endl);
            performPrioritizedStateElimination(statePriorities, transitionMatrix, backwardTransitions, oneStepProbabilities, stateRewards);
            STORM_LOG_DEBUG("Eliminated " << numberOfStatesToEliminate << " states." << std::endl);
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performHybridStateElimination(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, std::vector<typename SparseDtmcModelType::ValueType>& oneStepProbabilities, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
            // When using the hybrid technique, we recursively treat the SCCs up to some size.
            std::vector<storm::storage::sparse::state_type> entryStateQueue;
            STORM_LOG_DEBUG("Eliminating " << subsystem.size() << " states using the hybrid elimination technique." << std::endl);
            uint_fast64_t maximalDepth = treatScc(transitionMatrix, oneStepProbabilities, initialStates, subsystem, forwardTransitions, backwardTransitions, false, 0, storm::settings::sparseDtmcEliminationModelCheckerSettings().getMaximalSccSize(), entryStateQueue, stateRewards, distanceBasedPriorities);
            
            // If the entry states were to be eliminated last, we need to do so now.
            STORM_LOG_DEBUG("Eliminating " << entryStateQueue.size() << " entry states as a last step.");
            if (storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet()) {
                for (auto const& state : entryStateQueue) {
                    eliminateState(transitionMatrix, oneStepProbabilities, state, backwardTransitions, stateRewards);
                }
            }
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() << " states." << std::endl);
            return maximalDepth;
        }
        
        template<typename SparseDtmcModelType>
        std::vector<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeReachabilityValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::optional<std::vector<ValueType>>& stateRewards) {
            std::chrono::high_resolution_clock::time_point totalTimeStart = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point conversionStart = std::chrono::high_resolution_clock::now();
            // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            FlexibleSparseMatrix flexibleMatrix = getFlexibleSparseMatrix(transitionMatrix);
            FlexibleSparseMatrix flexibleBackwardTransitions = getFlexibleSparseMatrix(backwardTransitions);
            auto conversionEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point modelCheckingStart = std::chrono::high_resolution_clock::now();
            
            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            if (eliminationOrderNeedsDistances(order)) {
                distanceBasedPriorities = getDistanceBasedPriorities(transitionMatrix, backwardTransitions, initialStates, oneStepProbabilities,
                                                                     eliminationOrderNeedsForwardDistances(order), eliminationOrderNeedsReversedDistances(order));
            }
            
            // Create a bit vector that represents the subsystem of states we still have to eliminate.
            storm::storage::BitVector subsystem = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
            
            uint_fast64_t maximalDepth = 0;
            if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::State) {
                performOrdinaryStateElimination(flexibleMatrix, flexibleBackwardTransitions, subsystem, initialStates, oneStepProbabilities, stateRewards, distanceBasedPriorities);
            } else if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::Hybrid) {
                maximalDepth = performHybridStateElimination(transitionMatrix, flexibleMatrix, flexibleBackwardTransitions, subsystem, initialStates, oneStepProbabilities, stateRewards, distanceBasedPriorities);
            }
            
            // Make sure that at this point, we have at most one transition and if so, it must be a self-loop. Otherwise,
            // something went wrong.
            if (!flexibleMatrix.getRow(*initialStates.begin()).empty()) {
                STORM_LOG_ASSERT(flexibleMatrix.getRow(*initialStates.begin()).size() == 1, "At most one outgoing transition expected at this point, but found more.");
                STORM_LOG_ASSERT(flexibleMatrix.getRow(*initialStates.begin()).front().getColumn() == *initialStates.begin(), "Remaining entry should be a self-loop, but it is not.");
            }
            
            // Finally eliminate initial state.
            if (!stateRewards) {
                // If we are computing probabilities, then we can simply call the state elimination procedure. It
                // will scale the transition row of the initial state with 1/(1-loopProbability).
                STORM_LOG_INFO("Eliminating initial state " << *initialStates.begin() << "." << std::endl);
                eliminateState(flexibleMatrix, oneStepProbabilities, *initialStates.begin(), flexibleBackwardTransitions, stateRewards);
            } else {
                // If we are computing rewards, we cannot call the state elimination procedure for technical reasons.
                // Instead, we need to get rid of a potential loop in this state explicitly.
                
                // Start by finding the self-loop element. Since it can only be the only remaining outgoing transition
                // of the initial state, this amounts to checking whether the outgoing transitions of the initial
                // state are non-empty.
                if (!flexibleMatrix.getRow(*initialStates.begin()).empty()) {
                    ValueType loopProbability = flexibleMatrix.getRow(*initialStates.begin()).front().getValue();
                    loopProbability = storm::utility::one<ValueType>() / (storm::utility::one<ValueType>() - loopProbability);
                    STORM_LOG_DEBUG("Scaling the reward of the initial state " << stateRewards.get()[(*initialStates.begin())] << " with " << loopProbability);
                    stateRewards.get()[(*initialStates.begin())] *= loopProbability;
                    flexibleMatrix.getRow(*initialStates.begin()).clear();
                }
            }
            
            // Make sure that we have eliminated all transitions from the initial state.
            STORM_LOG_ASSERT(flexibleMatrix.getRow(*initialStates.begin()).empty(), "The transitions of the initial states are non-empty.");
            
            std::chrono::high_resolution_clock::time_point modelCheckingEnd = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point totalTimeEnd = std::chrono::high_resolution_clock::now();
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::chrono::high_resolution_clock::duration conversionTime = conversionEnd - conversionStart;
                std::chrono::milliseconds conversionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime);
                std::chrono::high_resolution_clock::duration modelCheckingTime = modelCheckingEnd - modelCheckingStart;
                std::chrono::milliseconds modelCheckingTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(modelCheckingTime);
                std::chrono::high_resolution_clock::duration totalTime = totalTimeEnd - totalTimeStart;
                std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                
                STORM_PRINT_AND_LOG(std::endl);
                STORM_PRINT_AND_LOG("Time breakdown:" << std::endl);
                STORM_PRINT_AND_LOG("    * time for conversion: " << conversionTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("    * time for checking: " << modelCheckingTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("------------------------------------------" << std::endl);
                STORM_PRINT_AND_LOG("    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG(std::endl);
                STORM_PRINT_AND_LOG("Other:" << std::endl);
                STORM_PRINT_AND_LOG("    * number of states eliminated: " << transitionMatrix.getRowCount() << std::endl);
                if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::Hybrid) {
                    STORM_PRINT_AND_LOG("    * maximal depth of SCC decomposition: " << maximalDepth << std::endl);
                }
            }
            
            // Now, we return the value for the only initial state.
            STORM_LOG_DEBUG("Simplifying and returning result.");
            if (stateRewards) {
                for (auto& reward : stateRewards.get()) {
                    reward = storm::utility::simplify(reward);
                }
                return stateRewards.get();
            } else {
                for (auto& probability : oneStepProbabilities) {
                    probability = storm::utility::simplify(probability);
                }
                return oneStepProbabilities;
            }
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::treatScc(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
            uint_fast64_t maximalDepth = level;
            
            // If the SCCs are large enough, we try to split them further.
            if (scc.getNumberOfSetBits() > maximalSccSize) {
                STORM_LOG_TRACE("SCC is large enough (" << scc.getNumberOfSetBits() << " states) to be decomposed further.");
                
                // Here, we further decompose the SCC into sub-SCCs.
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition(forwardTransitions, scc & ~entryStates, false, false);
                STORM_LOG_TRACE("Decomposed SCC into " << decomposition.size() << " sub-SCCs.");
                
                // Store a bit vector of remaining SCCs so we can be flexible when it comes to the order in which
                // we eliminate the SCCs.
                storm::storage::BitVector remainingSccs(decomposition.size(), true);
                
                // First, get rid of the trivial SCCs.
                storm::storage::BitVector statesInTrivialSccs(matrix.getNumberOfRows());
                for (uint_fast64_t sccIndex = 0; sccIndex < decomposition.size(); ++sccIndex) {
                    storm::storage::StronglyConnectedComponent const& scc = decomposition.getBlock(sccIndex);
                    if (scc.isTrivial()) {
                        // Put the only state of the trivial SCC into the set of states to eliminate.
                        statesInTrivialSccs.set(*scc.begin(), true);
                        remainingSccs.set(sccIndex, false);
                    }
                }
                
                std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, matrix, backwardTransitions, oneStepProbabilities, statesInTrivialSccs);
                STORM_LOG_TRACE("Eliminating " << statePriorities->size() << " trivial SCCs.");
                performPrioritizedStateElimination(statePriorities, matrix, backwardTransitions, oneStepProbabilities, stateRewards);
                STORM_LOG_TRACE("Eliminated all trivial SCCs.");

                // And then recursively treat the remaining sub-SCCs.
                STORM_LOG_TRACE("Eliminating " << remainingSccs.getNumberOfSetBits() << " remaining SCCs on level " << level << ".");
                for (auto sccIndex : remainingSccs) {
                    storm::storage::StronglyConnectedComponent const& newScc = decomposition.getBlock(sccIndex);
                    
                    // Rewrite SCC into bit vector and subtract it from the remaining states.
                    storm::storage::BitVector newSccAsBitVector(forwardTransitions.getRowCount(), newScc.begin(), newScc.end());
                    
                    // Determine the set of entry states of the SCC.
                    storm::storage::BitVector entryStates(forwardTransitions.getRowCount());
                    for (auto const& state : newScc) {
                        for (auto const& predecessor : backwardTransitions.getRow(state)) {
                            if (predecessor.getValue() != storm::utility::zero<ValueType>() && !newSccAsBitVector.get(predecessor.getColumn())) {
                                entryStates.set(state);
                            }
                        }
                    }
                    
                    // Recursively descend in SCC-hierarchy.
                    uint_fast64_t depth = treatScc(matrix, oneStepProbabilities, entryStates, newSccAsBitVector, forwardTransitions, backwardTransitions, eliminateEntryStates || !storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet(), level + 1, maximalSccSize, entryStateQueue, stateRewards, distanceBasedPriorities);
                    maximalDepth = std::max(maximalDepth, depth);
                }
            } else {
                // In this case, we perform simple state elimination in the current SCC.
                STORM_LOG_TRACE("SCC of size " << scc.getNumberOfSetBits() << " is small enough to be eliminated directly.");
                std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, matrix, backwardTransitions, oneStepProbabilities, scc & ~entryStates);
                performPrioritizedStateElimination(statePriorities, matrix, backwardTransitions, oneStepProbabilities, stateRewards);
                STORM_LOG_TRACE("Eliminated all states of SCC.");
            }
            
            // Finally, eliminate the entry states (if we are required to do so).
            if (eliminateEntryStates) {
                STORM_LOG_TRACE("Finally, eliminating/adding entry states.");
                for (auto state : entryStates) {
                    eliminateState(matrix, oneStepProbabilities, state, backwardTransitions, stateRewards);
                }
                STORM_LOG_TRACE("Eliminated/added entry states.");
            } else {
                for (auto state : entryStates) {
                    entryStateQueue.push_back(state);
                }
            }
            
            return maximalDepth;
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::eliminateState(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix& backwardTransitions, boost::optional<std::vector<ValueType>>& stateRewards, StatePriorityQueue* priorityQueue, bool removeForwardTransitions, bool constrained, storm::storage::BitVector const& predecessorConstraint) {
            
            STORM_LOG_TRACE("Eliminating state " << state << ".");
            
            bool hasSelfLoop = false;
            ValueType loopProbability = storm::utility::zero<ValueType>();
            
            // Start by finding loop probability.
            typename FlexibleSparseMatrix::row_type& currentStateSuccessors = matrix.getRow(state);
            for (auto entryIt = currentStateSuccessors.begin(), entryIte = currentStateSuccessors.end(); entryIt != entryIte; ++entryIt) {
                if (entryIt->getColumn() >= state) {
                    if (entryIt->getColumn() == state) {
                        loopProbability = entryIt->getValue();
                        hasSelfLoop = true;
                        
                        // If we do not clear the forward transitions completely, we need to remove the self-loop,
                        // because we scale all the other outgoing transitions with it anyway..
                        if (!removeForwardTransitions) {
                            currentStateSuccessors.erase(entryIt);
                        }
                    }
                    break;
                }
            }
            
            // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
            std::size_t scaledSuccessors = 0;
            if (hasSelfLoop) {
                STORM_LOG_ASSERT(loopProbability != storm::utility::one<ValueType>(), "Must not eliminate state with probability 1 self-loop.");
                loopProbability = storm::utility::one<ValueType>() / (storm::utility::one<ValueType>() - loopProbability);
                storm::utility::simplify(loopProbability);
                for (auto& entry : matrix.getRow(state)) {
                    // Only scale the non-diagonal entries.
                    if (entry.getColumn() != state) {
                        ++scaledSuccessors;
                        entry.setValue(storm::utility::simplify(entry.getValue() * loopProbability));
                    }
                }
                if (!stateRewards) {
                    oneStepProbabilities[state] = storm::utility::simplify(oneStepProbabilities[state] * loopProbability);
                }
            }
            
            STORM_LOG_TRACE((hasSelfLoop ? "State has self-loop." : "State does not have a self-loop."));
            
            // Now connect the predecessors of the state being eliminated with its successors.
            typename FlexibleSparseMatrix::row_type& currentStatePredecessors = backwardTransitions.getRow(state);
            
            // In case we have a constrained elimination, we need to keep track of the new predecessors.
            typename FlexibleSparseMatrix::row_type newCurrentStatePredecessors;

            std::vector<typename FlexibleSparseMatrix::row_type> newBackwardProbabilities(currentStateSuccessors.size());
            for (auto& backwardProbabilities : newBackwardProbabilities) {
                backwardProbabilities.reserve(currentStatePredecessors.size());
            }
            
            // Now go through the predecessors and eliminate the ones (satisfying the constraint if given).
            for (auto const& predecessorEntry : currentStatePredecessors) {
                uint_fast64_t predecessor = predecessorEntry.getColumn();
                STORM_LOG_TRACE("Found predecessor " << predecessor << ".");

                // Skip the state itself as one of its predecessors.
                if (predecessor == state) {
                    assert(hasSelfLoop);
                    continue;
                }
                
                // Skip the state if the elimination is constrained, but the predecessor is not in the constraint.
                if (constrained && !predecessorConstraint.get(predecessor)) {
                    newCurrentStatePredecessors.emplace_back(predecessorEntry);
                    STORM_LOG_TRACE("Not eliminating predecessor " << predecessor << ", because it does not fit the filter.");
                    continue;
                }
                STORM_LOG_TRACE("Eliminating predecessor " << predecessor << ".");
                
                // First, find the probability with which the predecessor can move to the current state, because
                // the forward probabilities of the state to be eliminated need to be scaled with this factor.
                typename FlexibleSparseMatrix::row_type& predecessorForwardTransitions = matrix.getRow(predecessor);
                typename FlexibleSparseMatrix::row_type::iterator multiplyElement = std::find_if(predecessorForwardTransitions.begin(), predecessorForwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() == state; });
                
                // Make sure we have found the probability and set it to zero.
                STORM_LOG_THROW(multiplyElement != predecessorForwardTransitions.end(), storm::exceptions::InvalidStateException, "No probability for successor found.");
                ValueType multiplyFactor = multiplyElement->getValue();
                multiplyElement->setValue(storm::utility::zero<ValueType>());
                
                // At this point, we need to update the (forward) transitions of the predecessor.
                typename FlexibleSparseMatrix::row_type::iterator first1 = predecessorForwardTransitions.begin();
                typename FlexibleSparseMatrix::row_type::iterator last1 = predecessorForwardTransitions.end();
                typename FlexibleSparseMatrix::row_type::iterator first2 = currentStateSuccessors.begin();
                typename FlexibleSparseMatrix::row_type::iterator last2 = currentStateSuccessors.end();
                
                typename FlexibleSparseMatrix::row_type newSuccessors;
                newSuccessors.reserve((last1 - first1) + (last2 - first2));
                std::insert_iterator<typename FlexibleSparseMatrix::row_type> result(newSuccessors, newSuccessors.end());
                
                uint_fast64_t successorOffsetInNewBackwardTransitions = 0;
                // Now we merge the two successor lists. (Code taken from std::set_union and modified to suit our needs).
                for (; first1 != last1; ++result) {
                    // Skip the transitions to the state that is currently being eliminated.
                    if (first1->getColumn() == state || (first2 != last2 && first2->getColumn() == state)) {
                        if (first1->getColumn() == state) {
                            ++first1;
                        }
                        if (first2 != last2 && first2->getColumn() == state) {
                            ++first2;
                        }
                        continue;
                    }
                    
                    if (first2 == last2) {
                        std::copy_if(first1, last1, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() != state; } );
                        break;
                    }
                    if (first2->getColumn() < first1->getColumn()) {
                        auto successorEntry = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                        *result = successorEntry;
                        newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, successorEntry.getValue());
//                        std::cout << "(1) adding " << first2->getColumn() << " -> " << newBackwardProbabilities[successorOffsetInNewBackwardTransitions].back() << "[" << successorOffsetInNewBackwardTransitions << "]" << std::endl;
                        ++first2;
                        ++successorOffsetInNewBackwardTransitions;
                    } else if (first1->getColumn() < first2->getColumn()) {
                        *result = *first1;
                        ++first1;
                    } else {
                        auto probability = storm::utility::simplify(first1->getValue() + storm::utility::simplify(multiplyFactor * first2->getValue()));
                        *result = storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type>(first1->getColumn(), probability);
                        newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, probability);
//                        std::cout << "(2) adding " << first2->getColumn() << " -> " << newBackwardProbabilities[successorOffsetInNewBackwardTransitions].back() << "[" << successorOffsetInNewBackwardTransitions << "]" << std::endl;
                        ++first1;
                        ++first2;
                        ++successorOffsetInNewBackwardTransitions;
                    }
                }
                for (; first2 != last2; ++first2) {
                    if (first2->getColumn() != state) {
                        auto stateProbability = storm::utility::simplify(std::move(*first2 * multiplyFactor));
                        *result = stateProbability;
                        newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, stateProbability.getValue());
//                        std::cout << "(3) adding " << first2->getColumn() << " -> " << newBackwardProbabilities[successorOffsetInNewBackwardTransitions].back() << "[" << successorOffsetInNewBackwardTransitions << "]" << std::endl;
                        ++successorOffsetInNewBackwardTransitions;
                    }
                }
                
                // Now move the new transitions in place.
                predecessorForwardTransitions = std::move(newSuccessors);
                STORM_LOG_TRACE("Fixed new next-state probabilities of predecessor state " << predecessor << ".");
                
                if (!stateRewards) {
                    // Add the probabilities to go to a target state in just one step if we have to compute probabilities.
                    oneStepProbabilities[predecessor] += storm::utility::simplify(multiplyFactor * oneStepProbabilities[state]);
                } else {
                    // If we are computing rewards, we basically scale the state reward of the state to eliminate and
                    // add the result to the state reward of the predecessor.
                    if (hasSelfLoop) {
                        stateRewards.get()[predecessor] += storm::utility::simplify(multiplyFactor * loopProbability * stateRewards.get()[state]);
                    } else {
                        stateRewards.get()[predecessor] += storm::utility::simplify(multiplyFactor * stateRewards.get()[state]);
                    }
                }
                
                if (priorityQueue != nullptr) {
                    STORM_LOG_TRACE("Updating priority of predecessor.");
                    priorityQueue->update(predecessor, matrix, backwardTransitions, oneStepProbabilities);
                }
            }
            
            // Finally, we need to add the predecessor to the set of predecessors of every successor.
            uint_fast64_t successorOffsetInNewBackwardTransitions = 0;
            for (auto const& successorEntry : currentStateSuccessors) {
                if (successorEntry.getColumn() == state) {
                    continue;
                }
                
                typename FlexibleSparseMatrix::row_type& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
//                std::cout << "old backward trans of " << successorEntry.getColumn() << std::endl;
//                for (auto const& trans : successorBackwardTransitions) {
//                    std::cout << trans << std::endl;
//                }
                
                // Delete the current state as a predecessor of the successor state only if we are going to remove the
                // current state's forward transitions.
                if (removeForwardTransitions) {
                    typename FlexibleSparseMatrix::row_type::iterator elimIt = std::find_if(successorBackwardTransitions.begin(), successorBackwardTransitions.end(), [&](storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() == state; });
                    STORM_LOG_ASSERT(elimIt != successorBackwardTransitions.end(), "Expected a proper backward transition from " << successorEntry.getColumn() << " to " << state << ", but found none.");
                    successorBackwardTransitions.erase(elimIt);
                }
                
                typename FlexibleSparseMatrix::row_type::iterator first1 = successorBackwardTransitions.begin();
                typename FlexibleSparseMatrix::row_type::iterator last1 = successorBackwardTransitions.end();
                typename FlexibleSparseMatrix::row_type::iterator first2 = newBackwardProbabilities[successorOffsetInNewBackwardTransitions].begin();
                typename FlexibleSparseMatrix::row_type::iterator last2 = newBackwardProbabilities[successorOffsetInNewBackwardTransitions].end();
                
//                std::cout << "adding backward trans " << successorEntry.getColumn() << "[" << successorOffsetInNewBackwardTransitions << "]" << std::endl;
//                for (auto const& trans : newBackwardProbabilities[successorOffsetInNewBackwardTransitions]) {
//                    std::cout << trans << std::endl;
//                }
                
                typename FlexibleSparseMatrix::row_type newPredecessors;
                newPredecessors.reserve((last1 - first1) + (last2 - first2));
                std::insert_iterator<typename FlexibleSparseMatrix::row_type> result(newPredecessors, newPredecessors.end());
                
                for (; first1 != last1; ++result) {
                    if (first2 == last2) {
                        std::copy(first1, last1, result);
                        break;
                    }
                    if (first2->getColumn() < first1->getColumn()) {
                        if (first2->getColumn() != state) {
                            *result = *first2;
                        }
                        ++first2;
                    } else if (first1->getColumn() == first2->getColumn()) {
                        if (estimateComplexity(first1->getValue()) > estimateComplexity(first2->getValue())) {
                            *result = *first1;
                        } else {
                            *result = *first2;
                        }
                        ++first1;
                        ++first2;
                    } else {
                        *result = *first1;
                        ++first1;
                    }
                }
                if (constrained) {
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() != state && (!constrained || predecessorConstraint.get(a.getColumn())); });
                } else {
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() != state; });
                }
                
                // Now move the new predecessors in place.
                successorBackwardTransitions = std::move(newPredecessors);
//                std::cout << "new backward trans of " << successorEntry.getColumn() << std::endl;
//                for (auto const& trans : successorBackwardTransitions) {
//                    std::cout << trans << std::endl;
//                }
                ++successorOffsetInNewBackwardTransitions;
            }
            STORM_LOG_TRACE("Fixed predecessor lists of successor states.");
            
            if (removeForwardTransitions) {
                // Clear the eliminated row to reduce memory consumption.
                currentStateSuccessors.clear();
                currentStateSuccessors.shrink_to_fit();
            }
            if (!constrained) {
                currentStatePredecessors.clear();
                currentStatePredecessors.shrink_to_fit();
            } else {
                currentStatePredecessors = std::move(newCurrentStatePredecessors);
            }
        }
        
        template<typename SparseDtmcModelType>
        std::vector<uint_fast64_t> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::getDistanceBasedPriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward, bool reverse) {
            std::vector<uint_fast64_t> statePriorities(transitionMatrix.getRowCount());
            std::vector<storm::storage::sparse::state_type> states(transitionMatrix.getRowCount());
            for (std::size_t index = 0; index < states.size(); ++index) {
                states[index] = index;
            }
            
            std::vector<std::size_t> distances = getStateDistances(transitionMatrix, transitionMatrixTransposed, initialStates, oneStepProbabilities,
                                                                   storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::Forward ||
                                                                   storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder::ForwardReversed);
            
            // In case of the forward or backward ordering, we can sort the states according to the distances.
            if (forward ^ reverse) {
                std::sort(states.begin(), states.end(), [&distances] (storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) { return distances[state1] < distances[state2]; } );
            } else {
                // Otherwise, we sort them according to descending distances.
                std::sort(states.begin(), states.end(), [&distances] (storm::storage::sparse::state_type const& state1, storm::storage::sparse::state_type const& state2) { return distances[state1] > distances[state2]; } );
            }
            
            // Now convert the ordering of the states to priorities.
            for (uint_fast64_t index = 0; index < states.size(); ++index) {
                statePriorities[states[index]] = index;
            }
            
            return statePriorities;
        }
        
        template<typename SparseDtmcModelType>
        std::vector<std::size_t> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::getStateDistances(storm::storage::SparseMatrix<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> const& transitionMatrix, storm::storage::SparseMatrix<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> const& oneStepProbabilities, bool forward) {
            if (forward) {
                return storm::utility::graph::getDistances(transitionMatrix, initialStates);
            } else {
                // Since the target states were eliminated from the matrix already, we construct a replacement by
                // treating all states that have some non-zero probability to go to a target state in one step as target
                // states.
                storm::storage::BitVector pseudoTargetStates(transitionMatrix.getRowCount());
                for (std::size_t index = 0; index < oneStepProbabilities.size(); ++index) {
                    if (oneStepProbabilities[index] != storm::utility::zero<ValueType>()) {
                        pseudoTargetStates.set(index);
                    }
                }
                
                return storm::utility::graph::getDistances(transitionMatrixTransposed, pseudoTargetStates);
            }
        }
        
        template<typename SparseDtmcModelType>
        SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::FlexibleSparseMatrix(index_type rows) : data(rows) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::reserveInRow(index_type row, index_type numberOfElements) {
            this->data[row].reserve(numberOfElements);
        }
        
        template<typename SparseDtmcModelType>
        typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::row_type& SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::getRow(index_type index) {
            return this->data[index];
        }
        
        template<typename SparseDtmcModelType>
        typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::row_type const& SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::getRow(index_type index) const {
            return this->data[index];
        }
        
        template<typename SparseDtmcModelType>
        typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::index_type SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::getNumberOfRows() const {
            return this->data.size();
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::hasSelfLoop(storm::storage::sparse::state_type state) {
            for (auto const& entry : this->getRow(state)) {
                if (entry.getColumn() < state) {
                    continue;
                } else if (entry.getColumn() > state) {
                    return false;
                } else if (entry.getColumn() == state) {
                    return true;
                }
            }
            return false;
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::print() const {
            for (uint_fast64_t index = 0; index < this->data.size(); ++index) {
                std::cout << index << " - ";
                for (auto const& element : this->getRow(index)) {
                    std::cout << "(" << element.getColumn() << ", " << element.getValue() << ") ";
                }
                std::cout << std::endl;
            }
        }
        
        template<typename SparseDtmcModelType>
        typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix SparseDtmcEliminationModelChecker<SparseDtmcModelType>::getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne) {
            FlexibleSparseMatrix flexibleMatrix(matrix.getRowCount());
            
            for (typename FlexibleSparseMatrix::index_type rowIndex = 0; rowIndex < matrix.getRowCount(); ++rowIndex) {
                typename storm::storage::SparseMatrix<ValueType>::const_rows row = matrix.getRow(rowIndex);
                flexibleMatrix.reserveInRow(rowIndex, row.getNumberOfEntries());
                
                for (auto const& element : row) {
                    // If the probability is zero, we skip this entry.
                    if (storm::utility::isZero(element.getValue())) {
                        continue;
                    }
                    
                    if (setAllValuesToOne) {
                        flexibleMatrix.getRow(rowIndex).emplace_back(element.getColumn(), storm::utility::one<ValueType>());
                    } else {
                        flexibleMatrix.getRow(rowIndex).emplace_back(element);
                    }
                }
            }
            
            return flexibleMatrix;
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeStatePenalty(storm::storage::sparse::state_type const& state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) {
            uint_fast64_t penalty = 0;
            bool hasParametricSelfLoop = false;
            
            for (auto const& predecessor : backwardTransitions.getRow(state)) {
                for (auto const& successor : transitionMatrix.getRow(state)) {
                    penalty += estimateComplexity(predecessor.getValue()) * estimateComplexity(successor.getValue());
//                    STORM_LOG_TRACE("1) penalty += " << (estimateComplexity(predecessor.getValue()) * estimateComplexity(successor.getValue())) << " because of " << predecessor.getValue() << " and " << successor.getValue() << ".");
                }
                if (predecessor.getColumn() == state) {
                    hasParametricSelfLoop = !storm::utility::isConstant(predecessor.getValue());
                }
                penalty += estimateComplexity(oneStepProbabilities[predecessor.getColumn()]) * estimateComplexity(predecessor.getValue()) * estimateComplexity(oneStepProbabilities[state]);
//                STORM_LOG_TRACE("2) penalty += " << (estimateComplexity(oneStepProbabilities[predecessor.getColumn()]) * estimateComplexity(predecessor.getValue()) * estimateComplexity(oneStepProbabilities[state])) << " because of " << oneStepProbabilities[predecessor.getColumn()] << ", " << predecessor.getValue() << " and " << oneStepProbabilities[state] << ".");
            }
            
            // If it is a self-loop that is parametric, we increase the penalty a lot.
            if (hasParametricSelfLoop) {
                penalty *= 10;
//                STORM_LOG_TRACE("3) penalty *= 100, because of parametric self-loop.");
            }
            
//            STORM_LOG_TRACE("New penalty of state " << state << " is " << penalty << ".");
            return penalty;
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) {
            return backwardTransitions.getRow(state).size() * transitionMatrix.getRow(state).size();
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StatePriorityQueue::update(storm::storage::sparse::state_type, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StaticStatePriorityQueue::StaticStatePriorityQueue(std::vector<storm::storage::sparse::state_type> const& sortedStates) : StatePriorityQueue(), sortedStates(sortedStates), currentPosition(0) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StaticStatePriorityQueue::hasNextState() const {
            return currentPosition < sortedStates.size();
        }
        
        template<typename SparseDtmcModelType>
        storm::storage::sparse::state_type SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StaticStatePriorityQueue::popNextState() {
            ++currentPosition;
            return sortedStates[currentPosition - 1];
        }
        
        template<typename SparseDtmcModelType>
        std::size_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StaticStatePriorityQueue::size() const {
            return sortedStates.size() - currentPosition;
        }
        
        template<typename SparseDtmcModelType>
        SparseDtmcEliminationModelChecker<SparseDtmcModelType>::DynamicPenaltyStatePriorityQueue::DynamicPenaltyStatePriorityQueue(std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> const& sortedStatePenaltyPairs, PenaltyFunctionType const& penaltyFunction) : StatePriorityQueue(), priorityQueue(), stateToPriorityMapping(), penaltyFunction(penaltyFunction) {
            // Insert all state-penalty pairs into our priority queue.
            for (auto const& statePenalty : sortedStatePenaltyPairs) {
                priorityQueue.insert(priorityQueue.end(), statePenalty);
            }
            
            // Insert all state-penalty pairs into auxiliary mapping.
            for (auto const& statePenalty : sortedStatePenaltyPairs) {
                stateToPriorityMapping.emplace(statePenalty);
            }
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::DynamicPenaltyStatePriorityQueue::hasNextState() const {
            return !priorityQueue.empty();
        }
        
        template<typename SparseDtmcModelType>
        storm::storage::sparse::state_type SparseDtmcEliminationModelChecker<SparseDtmcModelType>::DynamicPenaltyStatePriorityQueue::popNextState() {
            auto it = priorityQueue.begin();
            STORM_LOG_TRACE("Popping state " << it->first << " with priority " << it->second << ".");
            storm::storage::sparse::state_type result = it->first;
            priorityQueue.erase(priorityQueue.begin());
            return result;
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::DynamicPenaltyStatePriorityQueue::update(storm::storage::sparse::state_type state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) {
            // First, we need to find the priority until now.
            auto priorityIt = stateToPriorityMapping.find(state);
            
            // If the priority queue does not store the priority of the given state, we must not update it.
            if (priorityIt == stateToPriorityMapping.end()) {
                return;
            }
            uint_fast64_t lastPriority = priorityIt->second;
            
            uint_fast64_t newPriority = penaltyFunction(state, transitionMatrix, backwardTransitions, oneStepProbabilities);
            
            if (lastPriority != newPriority) {
                // Erase and re-insert into the priority queue with the new priority.
                auto queueIt = priorityQueue.find(std::make_pair(state, lastPriority));
                priorityQueue.erase(queueIt);
                priorityQueue.emplace(state, newPriority);
                
                // Finally, update the probability in the mapping.
                priorityIt->second = newPriority;
            }
        }
        
        template<typename SparseDtmcModelType>
        std::size_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::DynamicPenaltyStatePriorityQueue::size() const {
            return priorityQueue.size();
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::checkConsistent(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions) {
            for (uint_fast64_t forwardIndex = 0; forwardIndex < transitionMatrix.getNumberOfRows(); ++forwardIndex) {
                for (auto const& forwardEntry : transitionMatrix.getRow(forwardIndex)) {
                    if (forwardEntry.getColumn() == forwardIndex) {
                        continue;
                    }
                    
                    bool foundCorrespondingElement = false;
                    for (auto const& backwardEntry : backwardTransitions.getRow(forwardEntry.getColumn())) {
                        if (backwardEntry.getColumn() == forwardIndex) {
                            foundCorrespondingElement = true;
                        }
                    }
                    
                    if (!foundCorrespondingElement) {
//                        std::cout << "forward entry: " << forwardIndex << " -> " << forwardEntry << std::endl;
//                        transitionMatrix.print();
//                        backwardTransitions.print();
                        return false;
                    }
                }
            }
            return true;
        }
        
        template class SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<double>>;
        
#ifdef STORM_HAVE_CARL
        template class SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>>;
#endif
    } // namespace modelchecker
} // namespace storm
