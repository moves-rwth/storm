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

#include "src/logic/FragmentSpecification.h"

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
        SparseDtmcEliminationModelChecker<SparseDtmcModelType>::SparseDtmcEliminationModelChecker(storm::models::sparse::Dtmc<ValueType> const& model) : SparsePropositionalModelChecker<SparseDtmcModelType>(model) {
            // Intentionally left empty.
        }
        
        template<typename SparseDtmcModelType>
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::prctl().setCumulativeRewardFormulasAllowed(false).setInstantaneousFormulasAllowed(false);
            fragment.setNestedOperatorsAllowed(false).setLongRunAverageProbabilitiesAllowed(true).setConditionalProbabilityFormulasAllowed(true).setOnlyEventuallyFormuluasInConditionalFormulasAllowed(true);
            return formula.isInFragment(fragment);
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) {
            storm::logic::StateFormula const& stateFormula = checkTask.getFormula();
            std::unique_ptr<CheckResult> subResultPointer = this->check(stateFormula);
            storm::storage::BitVector const& psiStates = subResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();

            storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
            uint_fast64_t numberOfStates = transitionMatrix.getRowCount();
            if (psiStates.empty()) {
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::vector<ValueType>(numberOfStates, storm::utility::zero<ValueType>())));
            }
            if (psiStates.full()) {
                return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<ValueType>(std::vector<ValueType>(numberOfStates, storm::utility::one<ValueType>())));
            }
            
            storm::storage::BitVector const& initialStates = this->getModel().getInitialStates();
            STORM_LOG_THROW(initialStates.getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::IllegalArgumentException, "Cannot compute long-run probabilities for all states.");
            
            storm::storage::SparseMatrix<ValueType> backwardTransitions = this->getModel().getBackwardTransitions();
            storm::storage::BitVector maybeStates = storm::utility::graph::performProbGreater0(backwardTransitions, storm::storage::BitVector(transitionMatrix.getRowCount(), true), psiStates);
            
            std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());
            
            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (checkTask.isOnlyInitialStatesRelevantSet() && initialStates.isDisjointFrom(maybeStates)) {
                STORM_LOG_DEBUG("The long-run probability for all initial states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            }
            if (maybeStates.empty()) {
                STORM_LOG_DEBUG("The long-run probability for all states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            }
            
            if (furtherComputationNeeded) {
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, storm::storage::BitVector(numberOfStates, true), storm::storage::BitVector(numberOfStates, false));
                    
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    maybeStates &= reachableStates;
                }
                
                std::vector<ValueType> stateValues(maybeStates.size(), storm::utility::zero<ValueType>());
                storm::utility::vector::setVectorValues(stateValues, psiStates, storm::utility::one<ValueType>());
                result = computeLongRunValues(transitionMatrix, backwardTransitions, initialStates, maybeStates, checkTask.isOnlyInitialStatesRelevantSet(), stateValues);
            }
            
            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
                // If we computed the results for the initial states only, we need to filter the result to only
                // communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(initialStates));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula> const& checkTask) {
            // Do some sanity checks to establish some required properties.
            RewardModelType const& rewardModel = this->getModel().getRewardModel(checkTask.isRewardModelSet() ? checkTask.getRewardModel() : "");
            STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::IllegalArgumentException, "Input model does not have a reward model.");

            storm::storage::BitVector const& initialStates = this->getModel().getInitialStates();
            STORM_LOG_THROW(initialStates.getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::IllegalArgumentException, "Cannot compute long-run probabilities for all states.");
            
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
            uint_fast64_t numberOfStates = transitionMatrix.getRowCount();
            
            // Get the state-reward values from the reward model.
            std::vector<ValueType> stateRewardValues = rewardModel.getTotalRewardVector(this->getModel().getTransitionMatrix());
        
            storm::storage::BitVector maybeStates(stateRewardValues.size());
            uint_fast64_t index = 0;
            for (auto const& value : stateRewardValues) {
                if (value != storm::utility::zero<ValueType>()) {
                    maybeStates.set(index, true);
                }
                ++index;
            }
            
            storm::storage::SparseMatrix<ValueType> backwardTransitions = this->getModel().getBackwardTransitions();
            
            storm::storage::BitVector allStates(numberOfStates, true);
            maybeStates = storm::utility::graph::performProbGreater0(backwardTransitions, allStates, maybeStates);
            
            std::vector<ValueType> result(numberOfStates, storm::utility::zero<ValueType>());

            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (checkTask.isOnlyInitialStatesRelevantSet() && initialStates.isDisjointFrom(maybeStates)) {
                furtherComputationNeeded = false;
            }
            
            if (furtherComputationNeeded) {
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, storm::storage::BitVector(numberOfStates, true), storm::storage::BitVector(numberOfStates, false));
                    
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    maybeStates &= reachableStates;
                }
                
                result = computeLongRunValues(transitionMatrix, backwardTransitions, initialStates, maybeStates, checkTask.isOnlyInitialStatesRelevantSet(), stateRewardValues);
            }
            
            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
                // If we computed the results for the initial states only, we need to filter the result to only
                // communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(initialStates));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::vector<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeLongRunValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& maybeStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& stateValues) {
            
            std::chrono::high_resolution_clock::time_point totalTimeStart = std::chrono::high_resolution_clock::now();
            
            // Start by decomposing the DTMC into its BSCCs.
            std::chrono::high_resolution_clock::time_point sccDecompositionStart = std::chrono::high_resolution_clock::now();
            storm::storage::StronglyConnectedComponentDecomposition<ValueType> bsccDecomposition(transitionMatrix, storm::storage::BitVector(transitionMatrix.getRowCount(), true), false, true);
            auto sccDecompositionEnd = std::chrono::high_resolution_clock::now();

            std::chrono::high_resolution_clock::time_point conversionStart = std::chrono::high_resolution_clock::now();
            
            // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            FlexibleSparseMatrix flexibleMatrix = getFlexibleSparseMatrix(transitionMatrix);
            flexibleMatrix.filter(maybeStates, maybeStates);
            FlexibleSparseMatrix flexibleBackwardTransitions = getFlexibleSparseMatrix(backwardTransitions);
            flexibleBackwardTransitions.filter(maybeStates, maybeStates);
            auto conversionEnd = std::chrono::high_resolution_clock::now();
            
            std::chrono::high_resolution_clock::time_point modelCheckingStart = std::chrono::high_resolution_clock::now();

            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            if (eliminationOrderNeedsDistances(order)) {
                distanceBasedPriorities = getDistanceBasedPriorities(transitionMatrix, backwardTransitions, initialStates, stateValues,
                                                                     eliminationOrderNeedsForwardDistances(order), eliminationOrderNeedsReversedDistances(order));
            }
            
            uint_fast64_t numberOfStates = transitionMatrix.getRowCount();
            storm::storage::BitVector regularStatesInBsccs(numberOfStates);
            storm::storage::BitVector relevantBsccs(bsccDecomposition.size());
            storm::storage::BitVector bsccRepresentativesAsBitVector(numberOfStates);
            std::vector<storm::storage::sparse::state_type> bsccRepresentatives;
            uint_fast64_t currentIndex = 0;
            for (auto const& bscc : bsccDecomposition) {
                // Since all states in an SCC can reach all other states, we only need to check whether an arbitrary
                // state is a maybe state.
                if (maybeStates.get(*bscc.cbegin())) {
                    relevantBsccs.set(currentIndex);
                    bsccRepresentatives.push_back(*bscc.cbegin());
                    bsccRepresentativesAsBitVector.set(*bscc.cbegin(), true);
                    for (auto const& state : bscc) {
                        regularStatesInBsccs.set(state, true);
                    }
                }
                ++currentIndex;
            }
            regularStatesInBsccs &= ~bsccRepresentativesAsBitVector;
            
            // Compute the average time to stay in each state for all states in BSCCs.
            std::vector<ValueType> averageTimeInStates(stateValues.size(), storm::utility::one<ValueType>());
            
            // First, we eliminate all states in BSCCs (except for the representative states).
            {
                std::unique_ptr<StatePriorityQueue> priorityQueue = createStatePriorityQueue(distanceBasedPriorities, flexibleMatrix, flexibleBackwardTransitions, stateValues, regularStatesInBsccs);
                
                ValueUpdateCallback valueUpdateCallback = [&stateValues,&averageTimeInStates] (storm::storage::sparse::state_type const& state, ValueType const& loopProbability) {
                    stateValues[state] = storm::utility::simplify(loopProbability * stateValues[state]);
                    averageTimeInStates[state] = storm::utility::simplify(loopProbability * averageTimeInStates[state]);
                };
                
                PredecessorUpdateCallback predecessorCallback = [&stateValues,&averageTimeInStates] (storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) {
                    stateValues[predecessor] = storm::utility::simplify(stateValues[predecessor] + storm::utility::simplify(probability * stateValues[state]));
                    averageTimeInStates[predecessor] = storm::utility::simplify(averageTimeInStates[predecessor] + storm::utility::simplify(probability * averageTimeInStates[state]));
                };
                
                boost::optional<PriorityUpdateCallback> priorityUpdateCallback = PriorityUpdateCallback([&flexibleMatrix,&flexibleBackwardTransitions,&stateValues,&priorityQueue] (storm::storage::sparse::state_type const& state) {
                    priorityQueue->update(state, flexibleMatrix, flexibleBackwardTransitions, stateValues);
                });
                
                boost::optional<PredecessorFilterCallback> predecessorFilterCallback = boost::none;
                
                while (priorityQueue->hasNextState()) {
                    storm::storage::sparse::state_type state = priorityQueue->popNextState();
                    eliminateState(state, flexibleMatrix, flexibleBackwardTransitions, valueUpdateCallback, predecessorCallback, priorityUpdateCallback, predecessorFilterCallback, true);
                    STORM_LOG_ASSERT(checkConsistent(flexibleMatrix, flexibleBackwardTransitions), "The forward and backward transition matrices became inconsistent.");
                }
            }
            
            // Now, we set the values of all states in BSCCs to that of the representative value (and clear the
            // transitions of the representative states while doing so).
            auto representativeIt = bsccRepresentatives.begin();
            for (auto sccIndex : relevantBsccs) {
                // We only need to set the values for all states of the BSCC if we are not computing the values for the
                // initial states only.
                ValueType bsccValue = stateValues[*representativeIt] / averageTimeInStates[*representativeIt];
                auto const& bscc = bsccDecomposition[sccIndex];
                if (!computeResultsForInitialStatesOnly) {
                    for (auto const& state : bscc) {
                        stateValues[state] = bsccValue;
                    }
                } else {
                    for (auto const& state : bscc) {
                        stateValues[state] = storm::utility::zero<ValueType>();
                    }
                    stateValues[*representativeIt] = bsccValue;
                }

                typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::row_type& representativeForwardRow = flexibleMatrix.getRow(*representativeIt);
                representativeForwardRow.clear();
                representativeForwardRow.shrink_to_fit();
                
                typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::row_type& representativeBackwardRow = flexibleBackwardTransitions.getRow(*representativeIt);
                auto it = representativeBackwardRow.begin(), ite = representativeBackwardRow.end();
                for (; it != ite; ++it) {
                    if (it->getColumn() == *representativeIt) {
                        break;
                    }
                }
                representativeBackwardRow.erase(it);

                ++representativeIt;
            }

            // If there are states remaining that are not in BSCCs, we need to eliminate them now.
            storm::storage::BitVector remainingStates = maybeStates & ~regularStatesInBsccs;
            
            // Set the value initial value of all states not in a BSCC to zero, because a) any previous value would
            // incorrectly influence the result and b) the value have been erroneously changed for the predecessors of
            // BSCCs by the previous state elimination.
            for (auto state : remainingStates) {
                if (!bsccRepresentativesAsBitVector.get(state)) {
                    stateValues[state] = storm::utility::zero<ValueType>();
                }
            }
            
            // We only need to eliminate the remaining states if there was some BSCC that has a non-zero value, i.e.
            // that consists of maybe states.
            if (!relevantBsccs.empty()) {
                performOrdinaryStateElimination(flexibleMatrix, flexibleBackwardTransitions, remainingStates, initialStates, computeResultsForInitialStatesOnly, stateValues, distanceBasedPriorities);
            }
            
            std::chrono::high_resolution_clock::time_point modelCheckingEnd = std::chrono::high_resolution_clock::now();
            std::chrono::high_resolution_clock::time_point totalTimeEnd = std::chrono::high_resolution_clock::now();
            
            if (storm::settings::generalSettings().isShowStatisticsSet()) {
                std::chrono::high_resolution_clock::duration sccDecompositionTime = sccDecompositionEnd - sccDecompositionStart;
                std::chrono::milliseconds sccDecompositionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(sccDecompositionTime);
                std::chrono::high_resolution_clock::duration conversionTime = conversionEnd - conversionStart;
                std::chrono::milliseconds conversionTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(conversionTime);
                std::chrono::high_resolution_clock::duration modelCheckingTime = modelCheckingEnd - modelCheckingStart;
                std::chrono::milliseconds modelCheckingTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(modelCheckingTime);
                std::chrono::high_resolution_clock::duration totalTime = totalTimeEnd - totalTimeStart;
                std::chrono::milliseconds totalTimeInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(totalTime);
                
                STORM_PRINT_AND_LOG(std::endl);
                STORM_PRINT_AND_LOG("Time breakdown:" << std::endl);
                STORM_PRINT_AND_LOG("    * time for SCC decomposition: " << sccDecompositionTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("    * time for conversion: " << conversionTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("    * time for checking: " << modelCheckingTimeInMilliseconds.count() << "ms" << std::endl);
                STORM_PRINT_AND_LOG("------------------------------------------" << std::endl);
                STORM_PRINT_AND_LOG("    * total time: " << totalTimeInMilliseconds.count() << "ms" << std::endl);
            }
            
            // Now, we return the value for the only initial state.
            STORM_LOG_DEBUG("Simplifying and returning result.");
            for (auto& value : stateValues) {
                value = storm::utility::simplify(value);
            }
            return stateValues;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) {
            storm::logic::BoundedUntilFormula const& pathFormula = checkTask.getFormula();
            
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            std::unique_ptr<CheckResult> leftResultPointer = this->check(pathFormula.getLeftSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(pathFormula.getRightSubformula());
            storm::storage::BitVector const& phiStates = leftResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector const& psiStates = rightResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            
            // Start by determining the states that have a non-zero probability of reaching the target states within the
            // time bound.
            storm::storage::BitVector statesWithProbabilityGreater0 = storm::utility::graph::performProbGreater0(this->getModel().getBackwardTransitions(), phiStates, psiStates, true, pathFormula.getDiscreteTimeBound());
            statesWithProbabilityGreater0 &= ~psiStates;
            
            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (checkTask.isOnlyInitialStatesRelevantSet() && this->getModel().getInitialStates().isDisjointFrom(statesWithProbabilityGreater0)) {
                STORM_LOG_DEBUG("The probability for all initial states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            } else if (statesWithProbabilityGreater0.empty()) {
                STORM_LOG_DEBUG("The probability for all states was found in a preprocessing step.");
                furtherComputationNeeded = false;
            }
            
            storm::storage::SparseMatrix<ValueType> const& transitionMatrix = this->getModel().getTransitionMatrix();
            storm::storage::BitVector const& initialStates = this->getModel().getInitialStates();
            
            std::vector<ValueType> result(transitionMatrix.getRowCount(), storm::utility::zero<ValueType>());

            if (furtherComputationNeeded) {
                uint_fast64_t timeBound = pathFormula.getDiscreteTimeBound();
                
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(transitionMatrix, initialStates, phiStates, psiStates, true, timeBound);
                    
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    statesWithProbabilityGreater0 &= reachableStates;
                }

                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = transitionMatrix.getSubmatrix(true, statesWithProbabilityGreater0, statesWithProbabilityGreater0, true);
                
                std::vector<std::size_t> distancesFromInitialStates;
                storm::storage::BitVector relevantStates;
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
                    // Determine the set of initial states of the sub-model.
                    storm::storage::BitVector subInitialStates = this->getModel().getInitialStates() % statesWithProbabilityGreater0;

                    // Precompute the distances of the relevant states to the initial states.
                    distancesFromInitialStates = storm::utility::graph::getDistances(submatrix, subInitialStates, statesWithProbabilityGreater0);
                    
                    // Set all states to be relevant for later use.
                    relevantStates = storm::storage::BitVector(statesWithProbabilityGreater0.getNumberOfSetBits(), true);
                }
                
                // Create the vector of one-step probabilities to go to target states.
                std::vector<ValueType> b = transitionMatrix.getConstrainedRowSumVector(statesWithProbabilityGreater0, psiStates);
                
                // Create the vector with which to multiply.
                std::vector<ValueType> subresult(b);
                std::vector<ValueType> tmp(subresult.size());
                
                // Subtract one from the time bound because initializing the sub-result to b already accounts for one step.
                --timeBound;
                
                // Perform matrix-vector multiplications until the time-bound is met.
                for (uint_fast64_t timeStep = 0; timeStep < timeBound; ++timeStep) {
                    submatrix.multiplyWithVector(subresult, tmp);
                    storm::utility::vector::addVectors(tmp, b, subresult);
                    
                    // If we are computing the results for the initial states only, we can use the minimal distance from
                    // each state to the initial states to determine whether we still need to consider the values for
                    // these states. If not, we can null-out all their probabilities.
                    if (checkTask.isOnlyInitialStatesRelevantSet()) {
                        for (auto state : relevantStates) {
                            if (distancesFromInitialStates[state] > (timeBound - timeStep)) {
                                for (auto& element : submatrix.getRow(state)) {
                                    element.setValue(storm::utility::zero<ValueType>());
                                }
                                b[state] = storm::utility::zero<ValueType>();
                                relevantStates.set(state, false);
                            }
                        }
                    }
                }
                
                // Set the values of the resulting vector accordingly.
                storm::utility::vector::setVectorValues(result, statesWithProbabilityGreater0, subresult);
            }
            storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::one<ValueType>());
            
            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
                // If we computed the results for the initial (and prob 0 and prob1) states only, we need to filter the
                // result to only communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(this->getModel().getInitialStates() | psiStates));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            
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
            if (checkTask.isOnlyInitialStatesRelevantSet() && this->getModel().getInitialStates().isDisjointFrom(maybeStates)) {
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
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
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
                
                std::vector<ValueType> subresult = computeReachabilityValues(submatrix, oneStepProbabilities, submatrixTransposed, newInitialStates, checkTask.isOnlyInitialStatesRelevantSet(), phiStates, psiStates, oneStepProbabilities);
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, subresult);
            }

            // Construct full result.
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability0, storm::utility::zero<ValueType>());
            storm::utility::vector::setVectorValues<ValueType>(result, statesWithProbability1, storm::utility::one<ValueType>());

            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
                // If we computed the results for the initial (and prob 0 and prob1) states only, we need to filter the
                // result to only communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(~maybeStates | this->getModel().getInitialStates()));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            std::unique_ptr<CheckResult> subResultPointer = this->check(eventuallyFormula.getSubformula());
            storm::storage::BitVector phiStates(this->getModel().getNumberOfStates(), true);
            storm::storage::BitVector const& psiStates = subResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            
            // Do some sanity checks to establish some required properties.
            RewardModelType const& rewardModel = this->getModel().getRewardModel(checkTask.isRewardModelSet() ? checkTask.getRewardModel() : "");
            STORM_LOG_THROW(!rewardModel.empty(), storm::exceptions::IllegalArgumentException, "Input model does not have a reward model.");
            
            // Then, compute the subset of states that has a reachability reward less than infinity.
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            storm::storage::BitVector infinityStates = storm::utility::graph::performProb1(this->getModel().getBackwardTransitions(), trueStates, psiStates);
            infinityStates.complement();
            storm::storage::BitVector maybeStates = ~psiStates & ~infinityStates;
            
            // Determine whether we need to perform some further computation.
            bool furtherComputationNeeded = true;
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
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
                if (checkTask.isOnlyInitialStatesRelevantSet()) {
                    // Determine the set of states that is reachable from the initial state without jumping over a target state.
                    storm::storage::BitVector reachableStates = storm::utility::graph::getReachableStates(this->getModel().getTransitionMatrix(), this->getModel().getInitialStates(), maybeStates, psiStates);
                    
                    // Subtract from the maybe states the set of states that is not reachable (on a path from the initial to a target state).
                    maybeStates &= reachableStates;
                }
                
                // Determine the set of initial states of the sub-model.
                storm::storage::BitVector newInitialStates = this->getModel().getInitialStates() % maybeStates;

                // We then build the submatrix that only has the transitions of the maybe states.
                storm::storage::SparseMatrix<ValueType> submatrix = this->getModel().getTransitionMatrix().getSubmatrix(false, maybeStates, maybeStates);
                storm::storage::SparseMatrix<ValueType> submatrixTransposed = submatrix.transpose();
                
                // Project the state reward vector to all maybe-states.
                std::vector<ValueType> stateRewardValues = rewardModel.getTotalRewardVector(maybeStates.getNumberOfSetBits(), this->getModel().getTransitionMatrix(), maybeStates);

                std::vector<ValueType> subresult = computeReachabilityValues(submatrix, stateRewardValues, submatrixTransposed, newInitialStates, checkTask.isOnlyInitialStatesRelevantSet(), phiStates, psiStates, this->getModel().getTransitionMatrix().getConstrainedRowSumVector(maybeStates, psiStates));
                storm::utility::vector::setVectorValues<ValueType>(result, maybeStates, subresult);
            }
            
            // Construct full result.
            storm::utility::vector::setVectorValues<ValueType>(result, infinityStates, storm::utility::infinity<ValueType>());
            storm::utility::vector::setVectorValues<ValueType>(result, psiStates, storm::utility::zero<ValueType>());

            // Construct check result based on whether we have computed values for all states or just the initial states.
            std::unique_ptr<CheckResult> checkResult(new ExplicitQuantitativeCheckResult<ValueType>(result));
            if (checkTask.isOnlyInitialStatesRelevantSet()) {
                // If we computed the results for the initial (and inf) states only, we need to filter the result to
                // only communicate these results.
                checkResult->filter(ExplicitQualitativeCheckResult(~maybeStates | this->getModel().getInitialStates()));
            }
            return checkResult;
        }
        
        template<typename SparseDtmcModelType>
        std::unique_ptr<CheckResult> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula> const& checkTask) {
            storm::logic::ConditionalFormula const& conditionalFormula = checkTask.getFormula();
            
            // Retrieve the appropriate bitvectors by model checking the subformulas.
            STORM_LOG_THROW(conditionalFormula.getSubformula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Expected 'eventually' formula.");
            STORM_LOG_THROW(conditionalFormula.getConditionFormula().isEventuallyFormula(), storm::exceptions::InvalidPropertyException, "Expected 'eventually' formula.");
            
            std::unique_ptr<CheckResult> leftResultPointer = this->check(conditionalFormula.getSubformula().asEventuallyFormula().getSubformula());
            std::unique_ptr<CheckResult> rightResultPointer = this->check(conditionalFormula.getConditionFormula().asEventuallyFormula().getSubformula());
            storm::storage::BitVector phiStates = leftResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector psiStates = rightResultPointer->asExplicitQualitativeCheckResult().getTruthValuesVector();
            storm::storage::BitVector trueStates(this->getModel().getNumberOfStates(), true);
            
            // Do some sanity checks to establish some required properties.
            // STORM_LOG_WARN_COND(storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::State, "The chosen elimination method is not available for computing conditional probabilities. Falling back to regular state elimination.");
            STORM_LOG_THROW(this->getModel().getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::IllegalArgumentException, "Input model is required to have exactly one initial state.");
            STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::IllegalArgumentException, "Cannot compute conditional probabilities for all states.");
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
                std::shared_ptr<storm::logic::UntilFormula> untilFormula = std::make_shared<storm::logic::UntilFormula>(trueFormula, conditionalFormula.getSubformula().asSharedPointer());
                return this->computeUntilProbabilities(*untilFormula);
            }
            
            // From now on, we know the condition does not have a trivial probability in the initial state.
            
            // Compute the states that can be reached on a path that has a psi state in it.
            storm::storage::BitVector statesWithPsiPredecessor = storm::utility::graph::performProbGreater0(this->getModel().getTransitionMatrix(), trueStates, psiStates);
            storm::storage::BitVector statesReachingPhi = storm::utility::graph::performProbGreater0(backwardTransitions, trueStates, phiStates);
            
            // The set of states we need to consider are those that have a non-zero probability to satisfy the condition or are on some path that has a psi state in it.
            storm::storage::BitVector maybeStates = statesWithProbabilityGreater0 | (statesWithPsiPredecessor & statesReachingPhi);
            
            // Determine the set of initial states of the sub-DTMC.
            storm::storage::BitVector newInitialStates = this->getModel().getInitialStates() % maybeStates;
            
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
            
            storm::storage::BitVector statesToEliminate = ~maybeStates & ~newInitialStates;
            
            // Before starting the model checking process, we assign priorities to states so we can use them to
            // impose ordering constraints later.
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            if (eliminationOrderNeedsDistances(order)) {
                distanceBasedPriorities = getDistanceBasedPriorities(submatrix, submatrixTransposed, newInitialStates, oneStepProbabilities,
                                                             eliminationOrderNeedsForwardDistances(order),
                                                             eliminationOrderNeedsReversedDistances(order));
            }
            
            FlexibleSparseMatrix flexibleMatrix = getFlexibleSparseMatrix(submatrix);
            FlexibleSparseMatrix flexibleBackwardTransitions = getFlexibleSparseMatrix(submatrixTransposed, true);
            
            std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, flexibleMatrix, flexibleBackwardTransitions, oneStepProbabilities, statesToEliminate);
            performPrioritizedStateElimination(statePriorities, flexibleMatrix, flexibleBackwardTransitions, oneStepProbabilities, this->getModel().getInitialStates(), true);
            
            // Prepare some callbacks for the elimination procedure.
            ValueUpdateCallback valueUpdateCallback = [&oneStepProbabilities] (storm::storage::sparse::state_type const& state, ValueType const& loopProbability) { oneStepProbabilities[state] = storm::utility::simplify(loopProbability * oneStepProbabilities[state]); };
            PredecessorUpdateCallback predecessorUpdateCallback = [&oneStepProbabilities] (storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) { oneStepProbabilities[predecessor] = storm::utility::simplify(oneStepProbabilities[predecessor] * storm::utility::simplify(probability * oneStepProbabilities[state])); };
            boost::optional<PredecessorFilterCallback> phiFilterCallback = PredecessorFilterCallback([&phiStates] (storm::storage::sparse::state_type const& state) { return phiStates.get(state); });
            boost::optional<PredecessorFilterCallback> psiFilterCallback = PredecessorFilterCallback([&psiStates] (storm::storage::sparse::state_type const& state) { return psiStates.get(state); });
            
            // Eliminate the transitions going into the initial state (if there are any).
            if (!flexibleBackwardTransitions.getRow(*newInitialStates.begin()).empty()) {
                eliminateState(*newInitialStates.begin(), flexibleMatrix, flexibleBackwardTransitions, valueUpdateCallback, predecessorUpdateCallback, boost::none, boost::none, false);
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
                                        eliminateState(element.getColumn(), flexibleMatrix, flexibleBackwardTransitions, valueUpdateCallback, predecessorUpdateCallback, boost::none, phiFilterCallback, false);
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
                                        eliminateState(element.getColumn(), flexibleMatrix, flexibleBackwardTransitions, valueUpdateCallback, predecessorUpdateCallback, boost::none, psiFilterCallback, false);
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
        std::unique_ptr<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::StatePriorityQueue> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::createNaivePriorityQueue(storm::storage::BitVector const& states) {
            std::vector<storm::storage::sparse::state_type> sortedStates(states.begin(), states.end());
            return std::unique_ptr<StatePriorityQueue>(new StaticStatePriorityQueue(sortedStates));
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performPrioritizedStateElimination(std::unique_ptr<StatePriorityQueue>& priorityQueue, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, std::vector<ValueType>& values, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly) {
            
            ValueUpdateCallback valueUpdateCallback = [&values] (storm::storage::sparse::state_type const& state, ValueType const& loopProbability) { values[state] = storm::utility::simplify(loopProbability * values[state]); };
            PredecessorUpdateCallback predecessorCallback = [&values] (storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) { values[predecessor] = storm::utility::simplify(values[predecessor] + storm::utility::simplify(probability * values[state])); };
            boost::optional<PriorityUpdateCallback> priorityUpdateCallback = PriorityUpdateCallback([&transitionMatrix,&backwardTransitions,&values,&priorityQueue] (storm::storage::sparse::state_type const& state) { priorityQueue->update(state, transitionMatrix, backwardTransitions, values); });
            boost::optional<PredecessorFilterCallback> predecessorFilterCallback = boost::none;
            
            while (priorityQueue->hasNextState()) {
                storm::storage::sparse::state_type state = priorityQueue->popNextState();
                bool removeForwardTransitions = computeResultsForInitialStatesOnly && !initialStates.get(state);
                eliminateState(state, transitionMatrix, backwardTransitions, valueUpdateCallback, predecessorCallback, priorityUpdateCallback, predecessorFilterCallback, removeForwardTransitions);
                if (removeForwardTransitions) {
                    values[state] = storm::utility::zero<ValueType>();
                }
                STORM_LOG_ASSERT(checkConsistent(transitionMatrix, backwardTransitions), "The forward and backward transition matrices became inconsistent.");
            }
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performOrdinaryStateElimination(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
            std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, transitionMatrix, backwardTransitions, values, subsystem);
            
            std::size_t numberOfStatesToEliminate = statePriorities->size();
            STORM_LOG_DEBUG("Eliminating " << numberOfStatesToEliminate << " states using the state elimination technique." << std::endl);
            performPrioritizedStateElimination(statePriorities, transitionMatrix, backwardTransitions, values, initialStates, computeResultsForInitialStatesOnly);
            STORM_LOG_DEBUG("Eliminated " << numberOfStatesToEliminate << " states." << std::endl);
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::performHybridStateElimination(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
            // When using the hybrid technique, we recursively treat the SCCs up to some size.
            std::vector<storm::storage::sparse::state_type> entryStateQueue;
            STORM_LOG_DEBUG("Eliminating " << subsystem.size() << " states using the hybrid elimination technique." << std::endl);
            uint_fast64_t maximalDepth = treatScc(transitionMatrix, values, initialStates, subsystem, initialStates, forwardTransitions, backwardTransitions, false, 0, storm::settings::sparseDtmcEliminationModelCheckerSettings().getMaximalSccSize(), entryStateQueue, computeResultsForInitialStatesOnly, distanceBasedPriorities);
            
            // If the entry states were to be eliminated last, we need to do so now.
            if (storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet()) {
                STORM_LOG_DEBUG("Eliminating " << entryStateQueue.size() << " entry states as a last step.");
                std::vector<storm::storage::sparse::state_type> sortedStates(entryStateQueue.begin(), entryStateQueue.end());
                std::unique_ptr<StatePriorityQueue> queuePriorities = std::unique_ptr<StatePriorityQueue>(new StaticStatePriorityQueue(sortedStates));
                performPrioritizedStateElimination(queuePriorities, transitionMatrix, backwardTransitions, values, initialStates, computeResultsForInitialStatesOnly);
            }
            STORM_LOG_DEBUG("Eliminated " << subsystem.size() << " states." << std::endl);
            return maximalDepth;
        }
        
        template<typename SparseDtmcModelType>
        std::vector<typename SparseDtmcEliminationModelChecker<SparseDtmcModelType>::ValueType> SparseDtmcEliminationModelChecker<SparseDtmcModelType>::computeReachabilityValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& values, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates,  bool computeResultsForInitialStatesOnly, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& oneStepProbabilitiesToTarget) {
            // Then, we convert the reduced matrix to a more flexible format to be able to perform state elimination more easily.
            FlexibleSparseMatrix flexibleMatrix = getFlexibleSparseMatrix(transitionMatrix);
            FlexibleSparseMatrix flexibleBackwardTransitions = getFlexibleSparseMatrix(backwardTransitions);
            
            storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationOrder order = storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationOrder();
            boost::optional<std::vector<uint_fast64_t>> distanceBasedPriorities;
            if (eliminationOrderNeedsDistances(order)) {
                distanceBasedPriorities = getDistanceBasedPriorities(transitionMatrix, backwardTransitions, initialStates, oneStepProbabilitiesToTarget,
                                                                     eliminationOrderNeedsForwardDistances(order), eliminationOrderNeedsReversedDistances(order));
            }
            
            // Create a bit vector that represents the subsystem of states we still have to eliminate.
            storm::storage::BitVector subsystem = storm::storage::BitVector(transitionMatrix.getRowCount(), true);
            
            uint_fast64_t maximalDepth = 0;
            if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::State) {
                performOrdinaryStateElimination(flexibleMatrix, flexibleBackwardTransitions, subsystem, initialStates, computeResultsForInitialStatesOnly, values, distanceBasedPriorities);
            } else if (storm::settings::sparseDtmcEliminationModelCheckerSettings().getEliminationMethod() == storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::EliminationMethod::Hybrid) {
                maximalDepth = performHybridStateElimination(transitionMatrix, flexibleMatrix, flexibleBackwardTransitions, subsystem, initialStates, computeResultsForInitialStatesOnly, values, distanceBasedPriorities);
            }
        
            STORM_LOG_ASSERT(flexibleMatrix.empty(), "Not all transitions were eliminated.");
            STORM_LOG_ASSERT(flexibleBackwardTransitions.empty(), "Not all transitions were eliminated.");
            
            // Now, we return the value for the only initial state.
            STORM_LOG_DEBUG("Simplifying and returning result.");
            for (auto& value : values) {
                value = storm::utility::simplify(value);
            }
            return values;
        }
        
        template<typename SparseDtmcModelType>
        uint_fast64_t SparseDtmcEliminationModelChecker<SparseDtmcModelType>::treatScc(FlexibleSparseMatrix& matrix, std::vector<ValueType>& values, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::BitVector const& initialStates, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, bool computeResultsForInitialStatesOnly, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities) {
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
                
                std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, matrix, backwardTransitions, values, statesInTrivialSccs);
                STORM_LOG_TRACE("Eliminating " << statePriorities->size() << " trivial SCCs.");
                performPrioritizedStateElimination(statePriorities, matrix, backwardTransitions, values, initialStates, computeResultsForInitialStatesOnly);
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
                    uint_fast64_t depth = treatScc(matrix, values, entryStates, newSccAsBitVector, initialStates, forwardTransitions, backwardTransitions, eliminateEntryStates || !storm::settings::sparseDtmcEliminationModelCheckerSettings().isEliminateEntryStatesLastSet(), level + 1, maximalSccSize, entryStateQueue, computeResultsForInitialStatesOnly, distanceBasedPriorities);
                    maximalDepth = std::max(maximalDepth, depth);
                }
            } else {
                // In this case, we perform simple state elimination in the current SCC.
                STORM_LOG_TRACE("SCC of size " << scc.getNumberOfSetBits() << " is small enough to be eliminated directly.");
                std::unique_ptr<StatePriorityQueue> statePriorities = createStatePriorityQueue(distanceBasedPriorities, matrix, backwardTransitions, values, scc & ~entryStates);
                performPrioritizedStateElimination(statePriorities, matrix, backwardTransitions, values, initialStates, computeResultsForInitialStatesOnly);
                STORM_LOG_TRACE("Eliminated all states of SCC.");
            }
            
            // Finally, eliminate the entry states (if we are required to do so).
            if (eliminateEntryStates) {
                STORM_LOG_TRACE("Finally, eliminating entry states.");
                std::unique_ptr<StatePriorityQueue> naivePriorities = createNaivePriorityQueue(entryStates);
                performPrioritizedStateElimination(naivePriorities, matrix, backwardTransitions, values, initialStates, computeResultsForInitialStatesOnly);
                STORM_LOG_TRACE("Eliminated/added entry states.");
            } else {
                STORM_LOG_TRACE("Finally, adding entry states to queue.");
                for (auto state : entryStates) {
                    entryStateQueue.push_back(state);
                }
            }
            
            return maximalDepth;
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::eliminateState(storm::storage::sparse::state_type state, FlexibleSparseMatrix& matrix, FlexibleSparseMatrix& backwardTransitions,
                                                                                    ValueUpdateCallback const& callback, PredecessorUpdateCallback const& predecessorCallback,
                                                                                    boost::optional<PriorityUpdateCallback> const& priorityUpdateCallback,
                                                                                    boost::optional<PredecessorFilterCallback> const& predecessorFilterCallback, bool removeForwardTransitions) {
            
            STORM_LOG_TRACE("Eliminating state " << state << ".");
            
            // Start by finding loop probability.
            bool hasSelfLoop = false;
            ValueType loopProbability = storm::utility::zero<ValueType>();
            typename FlexibleSparseMatrix::row_type& currentStateSuccessors = matrix.getRow(state);
            for (auto entryIt = currentStateSuccessors.begin(), entryIte = currentStateSuccessors.end(); entryIt != entryIte; ++entryIt) {
                if (entryIt->getColumn() >= state) {
                    if (entryIt->getColumn() == state) {
                        loopProbability = entryIt->getValue();
                        hasSelfLoop = true;
                        
                        // If we do not clear the forward transitions completely, we need to remove the self-loop,
                        // because we scale all the other outgoing transitions with it anyway.
                        if (!removeForwardTransitions) {
                            currentStateSuccessors.erase(entryIt);
                        }
                    }
                    break;
                }
            }
            
            // Scale all entries in this row with (1 / (1 - loopProbability)) only in case there was a self-loop.
            STORM_LOG_TRACE((hasSelfLoop ? "State has self-loop." : "State does not have a self-loop."));
            if (hasSelfLoop) {
                STORM_LOG_ASSERT(loopProbability != storm::utility::one<ValueType>(), "Must not eliminate state with probability 1 self-loop.");
                loopProbability = storm::utility::simplify(storm::utility::one<ValueType>() / (storm::utility::one<ValueType>() - loopProbability));
                for (auto& entry : matrix.getRow(state)) {
                    // Only scale the non-diagonal entries.
                    if (entry.getColumn() != state) {
                        entry.setValue(storm::utility::simplify(entry.getValue() * loopProbability));
                    }
                }
                callback(state, loopProbability);
            }
            
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
                if (predecessorFilterCallback && !predecessorFilterCallback.get()(predecessor)) {
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
                        ++first2;
                        ++successorOffsetInNewBackwardTransitions;
                    } else if (first1->getColumn() < first2->getColumn()) {
                        *result = *first1;
                        ++first1;
                    } else {
                        auto probability = storm::utility::simplify(first1->getValue() + storm::utility::simplify(multiplyFactor * first2->getValue()));
                        *result = storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type>(first1->getColumn(), probability);
                        newBackwardProbabilities[successorOffsetInNewBackwardTransitions].emplace_back(predecessor, probability);
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
                        ++successorOffsetInNewBackwardTransitions;
                    }
                }
                
                // Now move the new transitions in place.
                predecessorForwardTransitions = std::move(newSuccessors);
                STORM_LOG_TRACE("Fixed new next-state probabilities of predecessor state " << predecessor << ".");

                predecessorCallback(predecessor, multiplyFactor, state);

                if (priorityUpdateCallback) {
                    STORM_LOG_TRACE("Updating priority of predecessor.");
                    priorityUpdateCallback.get()(predecessor);
                }
            }
            
            // Finally, we need to add the predecessor to the set of predecessors of every successor.
            uint_fast64_t successorOffsetInNewBackwardTransitions = 0;
            for (auto const& successorEntry : currentStateSuccessors) {
                if (successorEntry.getColumn() == state) {
                    continue;
                }
                
                typename FlexibleSparseMatrix::row_type& successorBackwardTransitions = backwardTransitions.getRow(successorEntry.getColumn());
                
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
                if (predecessorFilterCallback) {
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() != state && predecessorFilterCallback.get()(a.getColumn()); });
                } else {
                    std::copy_if(first2, last2, result, [&] (storm::storage::MatrixEntry<typename FlexibleSparseMatrix::index_type, typename FlexibleSparseMatrix::value_type> const& a) { return a.getColumn() != state; });
                }
                
                // Now move the new predecessors in place.
                successorBackwardTransitions = std::move(newPredecessors);
                ++successorOffsetInNewBackwardTransitions;
            }
            STORM_LOG_TRACE("Fixed predecessor lists of successor states.");
            
            if (removeForwardTransitions) {
                // Clear the eliminated row to reduce memory consumption.
                currentStateSuccessors.clear();
                currentStateSuccessors.shrink_to_fit();
            }
            if (predecessorFilterCallback) {
                currentStatePredecessors = std::move(newCurrentStatePredecessors);
            } else {
                currentStatePredecessors.clear();
                currentStatePredecessors.shrink_to_fit();
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
        bool SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::empty() const {
            for (auto const& row : this->data) {
                if (!row.empty()) {
                    return false;
                }
            }
            return true;
        }
        
        template<typename SparseDtmcModelType>
        void SparseDtmcEliminationModelChecker<SparseDtmcModelType>::FlexibleSparseMatrix::filter(storm::storage::BitVector const& rowFilter, storm::storage::BitVector const& columnFilter) {
            for (uint_fast64_t rowIndex = 0; rowIndex < this->data.size(); ++rowIndex) {
                auto& row = this->data[rowIndex];
                if (!rowFilter.get(rowIndex)) {
                    row.clear();
                    row.shrink_to_fit();
                    continue;
                }
                row_type newRow;
                for (auto const& element : row) {
                    if (columnFilter.get(element.getColumn())) {
                        newRow.push_back(element);
                    }
                }
                row = std::move(newRow);
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
