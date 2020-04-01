#include "ApproximatePOMDPModelchecker.h"

#include <tuple>

#include <boost/algorithm/string.hpp>

#include "storm-pomdp/analysis/FormulaInformation.h"

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/graph.h"
#include "storm/logic/Formulas.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/utility/vector.h"
#include "storm/api/properties.h"
#include "storm/api/export.h"
#include "storm-pomdp/builder/BeliefMdpExplorer.h"
#include "storm-pomdp/modelchecker/TrivialPomdpValueBoundsModelChecker.h"

#include "storm/utility/macros.h"
#include "storm/utility/SignalHandler.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template<typename PomdpModelType, typename BeliefValueType>
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::Options::Options() {
                initialGridResolution = 10;
                explorationThreshold = storm::utility::zero<ValueType>();
                doRefinement = true;
                refinementPrecision = storm::utility::convertNumber<ValueType>(1e-4);
                numericPrecision = storm::NumberTraits<ValueType>::IsExact ? storm::utility::zero<ValueType>() : storm::utility::convertNumber<ValueType>(1e-9);
                cacheSubsimplices = false;
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::Result::Result(ValueType lower, ValueType upper) : lowerBound(lower), upperBound(upper) {
                // Intentionally left empty
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ValueType
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::Result::diff(bool relative) const {
                ValueType diff = upperBound - lowerBound;
                if (diff < storm::utility::zero<ValueType>()) {
                    STORM_LOG_WARN_COND(diff >= 1e-6, "Upper bound '" << upperBound << "' is smaller than lower bound '" << lowerBound << "': Difference is " << diff << ".");
                    diff = storm::utility::zero<ValueType >();
                }
                if (relative && !storm::utility::isZero(upperBound)) {
                    diff /= upperBound;
                }
                return diff;
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::Statistics::Statistics() :  overApproximationBuildAborted(false), underApproximationBuildAborted(false), aborted(false) {
                // intentionally left empty;
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ApproximatePOMDPModelchecker(PomdpModelType const& pomdp, Options options) : pomdp(pomdp), options(options) {
                cc = storm::utility::ConstantsComparator<ValueType>(storm::utility::convertNumber<ValueType>(this->options.numericPrecision), false);
            }

            template<typename PomdpModelType, typename BeliefValueType>
            typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::Result ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::check(storm::logic::Formula const& formula) {
                // Reset all collected statistics
                statistics = Statistics();
                // Extract the relevant information from the formula
                auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(pomdp, formula);
                
                // Compute some initial bounds on the values for each state of the pomdp
                auto initialPomdpValueBounds = TrivialPomdpValueBoundsModelChecker<storm::models::sparse::Pomdp<ValueType>>(pomdp).getValueBounds(formula, formulaInfo);
                Result result(initialPomdpValueBounds.lower[pomdp.getInitialStates().getNextSetIndex(0)], initialPomdpValueBounds.upper[pomdp.getInitialStates().getNextSetIndex(0)]);
                
                boost::optional<std::string> rewardModelName;
                if (formulaInfo.isNonNestedReachabilityProbability() || formulaInfo.isNonNestedExpectedRewardFormula()) {
                    // FIXME: Instead of giving up, introduce a new observation for target states and make sink states absorbing.
                    STORM_LOG_THROW(formulaInfo.getTargetStates().observationClosed, storm::exceptions::NotSupportedException, "There are non-target states with the same observation as a target state. This is currently not supported");
                    if (formulaInfo.isNonNestedReachabilityProbability()) {
                        if (!formulaInfo.getSinkStates().empty()) {
                            auto reachableFromSinkStates = storm::utility::graph::getReachableStates(pomdp.getTransitionMatrix(), formulaInfo.getSinkStates().states, formulaInfo.getSinkStates().states, ~formulaInfo.getSinkStates().states);
                            reachableFromSinkStates &= ~formulaInfo.getSinkStates().states;
                            STORM_LOG_THROW(reachableFromSinkStates.empty(), storm::exceptions::NotSupportedException, "There are sink states that can reach non-sink states. This is currently not supported");
                        }
                    } else {
                        // Expected reward formula!
                        rewardModelName = formulaInfo.getRewardModelName();
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unsupported formula '" << formula << "'.");
                }
                
                if (options.doRefinement) {
                    refineReachability(formulaInfo.getTargetStates().observations, formulaInfo.minimize(), rewardModelName, initialPomdpValueBounds.lower, initialPomdpValueBounds.upper, result);
                } else {
                    computeReachabilityOTF(formulaInfo.getTargetStates().observations, formulaInfo.minimize(), rewardModelName, initialPomdpValueBounds.lower, initialPomdpValueBounds.upper, result);
                }
                if (storm::utility::resources::isTerminate()) {
                    statistics.aborted = true;
                }
                return result;
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::printStatisticsToStream(std::ostream& stream) const {
                stream << "##### Grid Approximation Statistics ######" << std::endl;
                stream << "# Input model: " << std::endl;
                pomdp.printModelInformationToStream(stream);
                stream << "# Max. Number of states with same observation: " << pomdp.getMaxNrStatesWithSameObservation() << std::endl;
                
                if (statistics.aborted) {
                    stream << "# Computation aborted early" << std::endl;
                }
                
                // Refinement information:
                if (statistics.refinementSteps) {
                    stream << "# Number of refinement steps: " << statistics.refinementSteps.get() << std::endl;
                }
                
                // The overapproximation MDP:
                if (statistics.overApproximationStates) {
                    stream << "# Number of states in the ";
                    if (options.doRefinement) {
                        stream << "final ";
                    }
                    stream << "grid MDP for the over-approximation: ";
                    if (statistics.overApproximationBuildAborted) {
                        stream << ">=";
                    }
                    stream << statistics.overApproximationStates.get() << std::endl;
                    stream << "# Time spend for building the over-approx grid MDP(s): " << statistics.overApproximationBuildTime << std::endl;
                    stream << "# Time spend for checking the over-approx grid MDP(s): " << statistics.overApproximationCheckTime << std::endl;
                }
                
                // The underapproximation MDP:
                if (statistics.underApproximationStates) {
                    stream << "# Number of states in the ";
                    if (options.doRefinement) {
                        stream << "final ";
                    }
                    stream << "grid MDP for the under-approximation: ";
                    if (statistics.underApproximationBuildAborted) {
                        stream << ">=";
                    }
                    stream << statistics.underApproximationStates.get() << std::endl;
                    stream << "# Time spend for building the under-approx grid MDP(s): " << statistics.underApproximationBuildTime << std::endl;
                    stream << "# Time spend for checking the under-approx grid MDP(s): " << statistics.underApproximationCheckTime << std::endl;
                }

                stream << "##########################################" << std::endl;
            }
            

            
            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::computeReachabilityOTF(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, Result& result) {
                
                if (options.explorationThreshold > storm::utility::zero<ValueType>()) {
                    STORM_PRINT("Exploration threshold: " << options.explorationThreshold << std::endl)
                }
                
                uint64_t underApproxSizeThreshold = 0;
                { // Overapproximation
                    std::vector<uint64_t> observationResolutionVector(pomdp.getNrObservations(), options.initialGridResolution);
                    auto manager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                    if (rewardModelName) {
                        manager->setRewardModel(rewardModelName);
                    }
                    auto approx = computeOverApproximation(targetObservations, min, rewardModelName.is_initialized(), lowerPomdpValueBounds, upperPomdpValueBounds, observationResolutionVector, manager);
                    if (approx) {
                        STORM_PRINT_AND_LOG("Explored and checked Over-Approximation MDP:\n");
                        approx->getExploredMdp()->printModelInformationToStream(std::cout);
                        ValueType& resultValue = min ? result.lowerBound : result.upperBound;
                        resultValue = approx->getComputedValueAtInitialState();
                        underApproxSizeThreshold = approx->getExploredMdp()->getNumberOfStates();
                    }
                }
                { // Underapproximation (Uses a fresh Belief manager)
                    auto manager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                    if (rewardModelName) {
                        manager->setRewardModel(rewardModelName);
                    }
                    auto approx = computeUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), lowerPomdpValueBounds, upperPomdpValueBounds, underApproxSizeThreshold, manager);
                    if (approx) {
                        STORM_PRINT_AND_LOG("Explored and checked Under-Approximation MDP:\n");
                        approx->getExploredMdp()->printModelInformationToStream(std::cout);
                        ValueType& resultValue = min ? result.upperBound : result.lowerBound;
                        resultValue = approx->getComputedValueAtInitialState();
                    }
                }
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::refineReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, Result& result) {
                
                // Set up exploration data
                std::vector<uint64_t> observationResolutionVector(pomdp.getNrObservations(), options.initialGridResolution);
                auto beliefManager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                if (rewardModelName) {
                    beliefManager->setRewardModel(rewardModelName);
                }
                
                // OverApproximaion
                auto overApproximation = computeOverApproximation(targetObservations, min, rewardModelName.is_initialized(), lowerPomdpValueBounds, upperPomdpValueBounds, observationResolutionVector, beliefManager);
                if (!overApproximation) {
                    return;
                }
                ValueType& overApproxValue = min ? result.lowerBound : result.upperBound;
                overApproxValue = overApproximation->getComputedValueAtInitialState();
                
                // UnderApproximation TODO: use same belief manager?)
                uint64_t underApproxSizeThreshold = overApproximation->getExploredMdp()->getNumberOfStates();
                auto underApproximation = computeUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), lowerPomdpValueBounds, upperPomdpValueBounds, underApproxSizeThreshold, beliefManager);
                if (!underApproximation) {
                    return;
                }
                ValueType& underApproxValue = min ? result.upperBound : result.lowerBound;
                underApproxValue = underApproximation->getComputedValueAtInitialState();
                
                // ValueType lastMinScore = storm::utility::infinity<ValueType>();
                // Start refinement
                statistics.refinementSteps = 0;
                while (result.diff() > options.refinementPrecision) {
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                    // TODO the actual refinement
                    /*
                    // choose which observation(s) to refine
                    std::vector<ValueType> obsAccumulator(pomdp.getNrObservations(), storm::utility::zero<ValueType>());
                    std::vector<uint64_t> beliefCount(pomdp.getNrObservations(), 0);
                    bsmap_type::right_map::const_iterator underApproxStateBeliefIter = res->underApproxBeliefStateMap.right.begin();
                    while (underApproxStateBeliefIter != res->underApproxBeliefStateMap.right.end()) {
                        auto currentBelief = res->beliefList[underApproxStateBeliefIter->second];
                        beliefCount[currentBelief.observation] += 1;
                        bsmap_type::left_const_iterator overApproxBeliefStateIter = res->overApproxBeliefStateMap.left.find(underApproxStateBeliefIter->second);
                        if (overApproxBeliefStateIter != res->overApproxBeliefStateMap.left.end()) {
                            // If there is an over-approximate value for the belief, use it
                            auto diff = res->overApproxMap[overApproxBeliefStateIter->second] - res->underApproxMap[underApproxStateBeliefIter->first];
                            obsAccumulator[currentBelief.observation] += diff;
                        } else {
                            //otherwise, we approximate a value TODO this is critical, we have to think about it
                            auto overApproxValue = storm::utility::zero<ValueType>();
                            auto temp = computeSubSimplexAndLambdas(currentBelief.probabilities, observationResolutionVector[currentBelief.observation], pomdp.getNumberOfStates());
                            auto subSimplex = temp.first;
                            auto lambdas = temp.second;
                            for (size_t j = 0; j < lambdas.size(); ++j) {
                                if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                    uint64_t approxId = getBeliefIdInVector(res->beliefList, currentBelief.observation, subSimplex[j]);
                                    bsmap_type::left_const_iterator approxIter = res->overApproxBeliefStateMap.left.find(approxId);
                                    if (approxIter != res->overApproxBeliefStateMap.left.end()) {
                                        overApproxValue += lambdas[j] * res->overApproxMap[approxIter->second];
                                    } else {
                                        overApproxValue += lambdas[j];
                                    }
                                }
                            }
                            obsAccumulator[currentBelief.observation] += overApproxValue - res->underApproxMap[underApproxStateBeliefIter->first];
                        }
                        ++underApproxStateBeliefIter;
                    }


                    //for (uint64_t i = 0; i < obsAccumulator.size(); ++i) {
                       // obsAccumulator[i] /= storm::utility::convertNumber<ValueType>(beliefCount[i]);
                    //}
                    changedObservations.clear();

                    //TODO think about some other scoring methods
                    auto maxAvgDifference = *std::max_element(obsAccumulator.begin(), obsAccumulator.end());
                    //if (cc.isEqual(maxAvgDifference, lastMinScore) || cc.isLess(lastMinScore, maxAvgDifference)) {
                    lastMinScore = maxAvgDifference;
                    auto maxRes = *std::max_element(observationResolutionVector.begin(), observationResolutionVector.end());
                    STORM_PRINT("Set all to " << maxRes + 1 << std::endl)
                    for (uint64_t i = 0; i < pomdp.getNrObservations(); ++i) {
                        observationResolutionVector[i] = maxRes + 1;
                        changedObservations.insert(i);
                    }
                    //} else {
                   //     lastMinScore = std::min(maxAvgDifference, lastMinScore);
                   //     STORM_PRINT("Max Score: " << maxAvgDifference << std::endl)
                   //     STORM_PRINT("Last Min Score: " << lastMinScore << std::endl)
                   //     //STORM_PRINT("Obs(beliefCount): Score " << std::endl << "-------------------------------------" << std::endl)
                    //    for (uint64_t i = 0; i < pomdp.getNrObservations(); ++i) {
                            //STORM_PRINT(i << "(" << beliefCount[i] << "): " << obsAccumulator[i])
                   //         if (cc.isEqual(obsAccumulator[i], maxAvgDifference)) {
                                //STORM_PRINT(" *** ")
                  //              observationResolutionVector[i] += 1;
                   //             changedObservations.insert(i);
                    //        }
                            //STORM_PRINT(std::endl)
                    //    }
                    //}
                    if (underApproxModelSize < std::numeric_limits<uint64_t>::max() - 101) {
                        underApproxModelSize += 100;
                    }
                    STORM_PRINT(
                            "==============================" << std::endl << "Refinement Step " << refinementCounter << std::endl << "------------------------------" << std::endl)
                    res = computeRefinementStep(targetObservations, min, observationResolutionVector, computeRewards,
                                                res, changedObservations, initialOverApproxMap, initialUnderApproxMap, underApproxModelSize);
                    //storm::api::exportSparseModelAsDot(res->overApproxModelPtr, "oa_model_" + std::to_string(refinementCounter +1) + ".dot");
                    STORM_LOG_ERROR_COND((!min && cc.isLess(res->underApproxValue, res->overApproxValue)) || (min && cc.isLess(res->overApproxValue, res->underApproxValue)) ||
                                         cc.isEqual(res->underApproxValue, res->overApproxValue),
                                         "The value for the under-approximation is larger than the value for the over-approximation.");
                    */
                    ++statistics.refinementSteps.get();
                }
            }

            template<typename PomdpModelType, typename BeliefValueType>
            std::shared_ptr<typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ExplorerType> ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::computeOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, std::vector<uint64_t>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager) {
                statistics.overApproximationBuildTime.start();
                storm::builder::BeliefMdpExplorer<storm::models::sparse::Pomdp<ValueType>> explorer(beliefManager, lowerPomdpValueBounds, upperPomdpValueBounds);
                if (computeRewards) {
                    explorer.startNewExploration(storm::utility::zero<ValueType>());
                } else {
                    explorer.startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                }
                
                // Expand the beliefs to generate the grid on-the-fly
                while (explorer.hasUnexploredState()) {
                    uint64_t currId = explorer.exploreNextState();
                    
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    if (targetObservations.count(currObservation) != 0) {
                        explorer.setCurrentStateIsTarget();
                        explorer.addSelfloopTransition();
                    } else {
                        bool stopExploration = false;
                        if (storm::utility::abs<ValueType>(explorer.getUpperValueBoundAtCurrentState() - explorer.getLowerValueBoundAtCurrentState()) < options.explorationThreshold) {
                            stopExploration = true;
                            explorer.setCurrentStateIsTruncated();
                        }
                        for (uint64 action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                            ValueType truncationProbability = storm::utility::zero<ValueType>();
                            ValueType truncationValueBound = storm::utility::zero<ValueType>();
                            auto successorGridPoints = beliefManager->expandAndTriangulate(currId, action, observationResolutionVector);
                            for (auto const& successor : successorGridPoints) {
                                bool added = explorer.addTransitionToBelief(action, successor.first, successor.second, stopExploration);
                                if (!added) {
                                    STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                                    // We did not explore this successor state. Get a bound on the "missing" value
                                    truncationProbability += successor.second;
                                    truncationValueBound += successor.second * (min ? explorer.computeLowerValueBoundAtBelief(successor.first) : explorer.computeUpperValueBoundAtBelief(successor.first));
                                }
                            }
                            if (stopExploration) {
                                if (computeRewards) {
                                    explorer.addTransitionsToExtraStates(action, truncationProbability);
                                } else {
                                    explorer.addTransitionsToExtraStates(action, truncationValueBound, truncationProbability - truncationValueBound);
                                }
                            }
                            if (computeRewards) {
                                // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                explorer.computeRewardAtCurrentState(action, truncationValueBound);
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        statistics.overApproximationBuildAborted = true;
                        break;
                    }
                }
                statistics.overApproximationStates = explorer.getCurrentNumberOfMdpStates();
                if (storm::utility::resources::isTerminate()) {
                    statistics.overApproximationBuildTime.stop();
                    return nullptr;
                }
                
                explorer.finishExploration();
                statistics.overApproximationBuildTime.stop();

                statistics.overApproximationCheckTime.start();
                explorer.computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.overApproximationCheckTime.stop();
                
                return std::make_shared<ExplorerType>(std::move(explorer));
            }

            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::refineOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, std::vector<uint64_t>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& overApproximation) {
            /*TODO:
                       template<typename PomdpModelType, typename BeliefValueType>
            std::shared_ptr<RefinementComponents<ValueType>>
            ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::computeRefinementStep(std::set<uint32_t> const &targetObservations, bool min,
                                                                                            std::vector<uint64_t> &observationResolutionVector,
                                                                                            bool computeRewards,
                                                                                            std::shared_ptr<RefinementComponents<ValueType>> refinementComponents,
                                                                                            std::set<uint32_t> changedObservations,
                                                                                            boost::optional<std::map<uint64_t, ValueType>> overApproximationMap,
                                                                                            boost::optional<std::map<uint64_t, ValueType>> underApproximationMap,
                                                                                            uint64_t maxUaModelSize) {
                bool initialBoundMapsSet = overApproximationMap && underApproximationMap;
                std::map<uint64_t, ValueType> initialOverMap;
                std::map<uint64_t, ValueType> initialUnderMap;
                if (initialBoundMapsSet) {
                    initialOverMap = overApproximationMap.value();
                    initialUnderMap = underApproximationMap.value();
                }
                // Note that a persistent cache is not support by the current data structure. The resolution for the given belief also has to be stored somewhere to cache effectively
                std::map<uint64_t, std::vector<std::map<uint64_t, ValueType>>> subSimplexCache;
                std::map<uint64_t, std::vector<ValueType>> lambdaCache;

                // Map to save the weighted values resulting from the initial preprocessing for newly added beliefs / indices in beliefSpace
                std::map<uint64_t, ValueType> weightedSumOverMap;
                std::map<uint64_t, ValueType> weightedSumUnderMap;

                statistics.overApproximationBuildTime.start();

                uint64_t nextBeliefId = refinementComponents->beliefList.size();
                uint64_t nextStateId = refinementComponents->overApproxModelPtr->getNumberOfStates();
                std::set<uint64_t> relevantStates; // The MDP states where the observation has changed
                for (auto const &iter : refinementComponents->overApproxBeliefStateMap.left) {
                    auto currentBelief = refinementComponents->beliefList[iter.first];
                    if (changedObservations.find(currentBelief.observation) != changedObservations.end()) {
                        relevantStates.insert(iter.second);
                    }
                }

                std::set<std::pair<uint64_t, uint64_t>> statesAndActionsToCheck; // The predecessors of states where the observation has changed
                for (uint64_t state = 0; state < refinementComponents->overApproxModelPtr->getNumberOfStates(); ++state) {
                    for (uint_fast64_t row = 0; row < refinementComponents->overApproxModelPtr->getTransitionMatrix().getRowGroupSize(state); ++row) {
                        for (typename storm::storage::SparseMatrix<ValueType>::const_iterator itEntry = refinementComponents->overApproxModelPtr->getTransitionMatrix().getRow(
                                state, row).begin();
                             itEntry != refinementComponents->overApproxModelPtr->getTransitionMatrix().getRow(state, row).end(); ++itEntry) {
                            if (relevantStates.find(itEntry->getColumn()) != relevantStates.end()) {
                                statesAndActionsToCheck.insert(std::make_pair(state, row));
                                break;
                            }
                        }
                    }
                }

                std::deque<uint64_t> beliefsToBeExpanded;

                std::map<std::pair<uint64_t, uint64_t>, std::map<uint64_t, ValueType>> transitionsStateActionPair;
                for (auto const &stateActionPair : statesAndActionsToCheck) {
                    auto currId = refinementComponents->overApproxBeliefStateMap.right.at(stateActionPair.first);
                    auto action = stateActionPair.second;
                    std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(refinementComponents->beliefList[currId],
                                                                                                                              action);
                    std::map<uint64_t, ValueType> transitionInActionBelief;
                    for (auto iter = actionObservationProbabilities.begin(); iter != actionObservationProbabilities.end(); ++iter) {
                        // Expand and triangulate the successor
                        uint32_t observation = iter->first;
                        uint64_t idNextBelief = getBeliefAfterActionAndObservation(refinementComponents->beliefList, refinementComponents->beliefIsTarget,
                                                                                   targetObservations, refinementComponents->beliefList[currId], action, observation, nextBeliefId);
                        nextBeliefId = refinementComponents->beliefList.size();
                        //Triangulate here and put the possibly resulting belief in the grid
                        std::vector<std::map<uint64_t, ValueType>> subSimplex;
                        std::vector<ValueType> lambdas;
                        //TODO add caching
                        if (options.cacheSubsimplices && subSimplexCache.count(idNextBelief) > 0) {
                            subSimplex = subSimplexCache[idNextBelief];
                            lambdas = lambdaCache[idNextBelief];
                        } else {
                            auto temp = computeSubSimplexAndLambdas(refinementComponents->beliefList[idNextBelief].probabilities,
                                                                    observationResolutionVector[refinementComponents->beliefList[idNextBelief].observation],
                                                                    pomdp.getNumberOfStates());
                            subSimplex = temp.first;
                            lambdas = temp.second;
                            if (options.cacheSubsimplices) {
                                subSimplexCache[idNextBelief] = subSimplex;
                                lambdaCache[idNextBelief] = lambdas;
                            }
                        }
                        for (size_t j = 0; j < lambdas.size(); ++j) {
                            if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                auto approxId = getBeliefIdInVector(refinementComponents->beliefGrid, observation, subSimplex[j]);
                                if (approxId == uint64_t(-1)) {
                                    // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                    storm::pomdp::Belief<ValueType> gridBelief = {nextBeliefId, observation, subSimplex[j]};
                                    refinementComponents->beliefList.push_back(gridBelief);
                                    refinementComponents->beliefGrid.push_back(gridBelief);
                                    refinementComponents->beliefIsTarget.push_back(targetObservations.find(observation) != targetObservations.end());
                                    // compute overapproximate value using MDP result map
                                    if (initialBoundMapsSet) {
                                        auto tempWeightedSumOver = storm::utility::zero<ValueType>();
                                        auto tempWeightedSumUnder = storm::utility::zero<ValueType>();
                                        for (uint64_t i = 0; i < subSimplex[j].size(); ++i) {
                                            tempWeightedSumOver += subSimplex[j][i] * storm::utility::convertNumber<ValueType>(initialOverMap[i]);
                                            tempWeightedSumUnder += subSimplex[j][i] * storm::utility::convertNumber<ValueType>(initialUnderMap[i]);
                                        }
                                        weightedSumOverMap[nextBeliefId] = tempWeightedSumOver;
                                        weightedSumUnderMap[nextBeliefId] = tempWeightedSumUnder;
                                    }
                                    beliefsToBeExpanded.push_back(nextBeliefId);
                                    refinementComponents->overApproxBeliefStateMap.insert(bsmap_type::value_type(nextBeliefId, nextStateId));
                                    transitionInActionBelief[nextStateId] = iter->second * lambdas[j];
                                    ++nextBeliefId;
                                    ++nextStateId;
                                } else {
                                    transitionInActionBelief[refinementComponents->overApproxBeliefStateMap.left.at(approxId)] = iter->second * lambdas[j];
                                }
                            }
                        }
                    }
                    if (!transitionInActionBelief.empty()) {
                        transitionsStateActionPair[stateActionPair] = transitionInActionBelief;
                    }
                }

                std::set<uint64_t> stoppedExplorationStateSet;

                // Expand newly added beliefs
                while (!beliefsToBeExpanded.empty()) {
                    uint64_t currId = beliefsToBeExpanded.front();
                    beliefsToBeExpanded.pop_front();
                    bool isTarget = refinementComponents->beliefIsTarget[currId];

                    if (initialBoundMapsSet &&
                        cc.isLess(weightedSumOverMap[currId] - weightedSumUnderMap[currId], storm::utility::convertNumber<ValueType>(options.explorationThreshold))) {
                        STORM_PRINT("Stop Exploration in State " << refinementComponents->overApproxBeliefStateMap.left.at(currId) << " with Value " << weightedSumOverMap[currId]
                                                                 << std::endl)
                        transitionsStateActionPair[std::make_pair(refinementComponents->overApproxBeliefStateMap.left.at(currId), 0)] = {{1, weightedSumOverMap[currId]},
                                                                                                                                         {0, storm::utility::one<ValueType>() -
                                                                                                                                             weightedSumOverMap[currId]}};
                        stoppedExplorationStateSet.insert(refinementComponents->overApproxBeliefStateMap.left.at(currId));
                        continue;
                    }

                    if (isTarget) {
                        // Depending on whether we compute rewards, we select the right initial result
                        // MDP stuff
                        transitionsStateActionPair[std::make_pair(refinementComponents->overApproxBeliefStateMap.left.at(currId), 0)] =
                                {{refinementComponents->overApproxBeliefStateMap.left.at(currId), storm::utility::one<ValueType>()}};
                    } else {
                        uint64_t representativeState = pomdp.getStatesWithObservation(refinementComponents->beliefList[currId].observation).front();
                        uint64_t numChoices = pomdp.getNumberOfChoices(representativeState);
                        std::vector<ValueType> actionRewardsInState(numChoices);

                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(refinementComponents->beliefList[currId], action);
                            std::map<uint64_t, ValueType> transitionInActionBelief;
                            for (auto iter = actionObservationProbabilities.begin(); iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                // THIS CALL IS SLOW
                                // TODO speed this up
                                uint64_t idNextBelief = getBeliefAfterActionAndObservation(refinementComponents->beliefList, refinementComponents->beliefIsTarget,
                                                                                           targetObservations, refinementComponents->beliefList[currId], action, observation,
                                                                                           nextBeliefId);
                                nextBeliefId = refinementComponents->beliefList.size();
                                //Triangulate here and put the possibly resulting belief in the grid
                                std::vector<std::map<uint64_t, ValueType>> subSimplex;
                                std::vector<ValueType> lambdas;

                                if (options.cacheSubsimplices && subSimplexCache.count(idNextBelief) > 0) {
                                    subSimplex = subSimplexCache[idNextBelief];
                                    lambdas = lambdaCache[idNextBelief];
                                } else {
                                    auto temp = computeSubSimplexAndLambdas(refinementComponents->beliefList[idNextBelief].probabilities,
                                                                            observationResolutionVector[refinementComponents->beliefList[idNextBelief].observation],
                                                                            pomdp.getNumberOfStates());
                                    subSimplex = temp.first;
                                    lambdas = temp.second;
                                    if (options.cacheSubsimplices) {
                                        subSimplexCache[idNextBelief] = subSimplex;
                                        lambdaCache[idNextBelief] = lambdas;
                                    }
                                }

                                for (size_t j = 0; j < lambdas.size(); ++j) {
                                    if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                        auto approxId = getBeliefIdInVector(refinementComponents->beliefGrid, observation, subSimplex[j]);
                                        if (approxId == uint64_t(-1)) {
                                            // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                            storm::pomdp::Belief<ValueType> gridBelief = {nextBeliefId, observation, subSimplex[j]};
                                            refinementComponents->beliefList.push_back(gridBelief);
                                            refinementComponents->beliefGrid.push_back(gridBelief);
                                            refinementComponents->beliefIsTarget.push_back(targetObservations.find(observation) != targetObservations.end());
                                            // compute overapproximate value using MDP result map
                                            if (initialBoundMapsSet) {
                                                auto tempWeightedSumOver = storm::utility::zero<ValueType>();
                                                auto tempWeightedSumUnder = storm::utility::zero<ValueType>();
                                                for (uint64_t i = 0; i < subSimplex[j].size(); ++i) {
                                                    tempWeightedSumOver += subSimplex[j][i] * storm::utility::convertNumber<ValueType>(initialOverMap[i]);
                                                    tempWeightedSumUnder += subSimplex[j][i] * storm::utility::convertNumber<ValueType>(initialUnderMap[i]);
                                                }
                                                weightedSumOverMap[nextBeliefId] = tempWeightedSumOver;
                                                weightedSumUnderMap[nextBeliefId] = tempWeightedSumUnder;
                                            }
                                            beliefsToBeExpanded.push_back(nextBeliefId);
                                            refinementComponents->overApproxBeliefStateMap.insert(bsmap_type::value_type(nextBeliefId, nextStateId));
                                            transitionInActionBelief[nextStateId] = iter->second * lambdas[j];
                                            ++nextBeliefId;
                                            ++nextStateId;
                                        } else {
                                            transitionInActionBelief[refinementComponents->overApproxBeliefStateMap.left.at(approxId)] = iter->second * lambdas[j];
                                        }
                                    }
                                }
                            }
                            if (!transitionInActionBelief.empty()) {
                                transitionsStateActionPair[std::make_pair(refinementComponents->overApproxBeliefStateMap.left.at(currId), action)] = transitionInActionBelief;
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        statistics.overApproximationBuildAborted = true;
                        break;
                    }
                }

                statistics.overApproximationStates = nextStateId;
                if (storm::utility::resources::isTerminate()) {
                    statistics.overApproximationBuildTime.stop();
                    // Return the result from the old refinement step
                    return refinementComponents;
                }
                storm::models::sparse::StateLabeling mdpLabeling(nextStateId);
                mdpLabeling.addLabel("init");
                mdpLabeling.addLabel("target");
                mdpLabeling.addLabelToState("init", refinementComponents->overApproxBeliefStateMap.left.at(refinementComponents->initialBeliefId));
                mdpLabeling.addLabelToState("target", 1);
                uint_fast64_t currentRow = 0;
                uint_fast64_t currentRowGroup = 0;
                storm::storage::SparseMatrixBuilder<ValueType> smb(0, nextStateId, 0, false, true);
                auto oldTransitionMatrix = refinementComponents->overApproxModelPtr->getTransitionMatrix();
                smb.newRowGroup(currentRow);
                smb.addNextValue(currentRow, 0, storm::utility::one<ValueType>());
                ++currentRow;
                ++currentRowGroup;
                smb.newRowGroup(currentRow);
                smb.addNextValue(currentRow, 1, storm::utility::one<ValueType>());
                ++currentRow;
                ++currentRowGroup;
                for (uint64_t state = 2; state < nextStateId; ++state) {
                    smb.newRowGroup(currentRow);
                    //STORM_PRINT("Loop State: " << state << std::endl)
                    uint64_t numChoices = pomdp.getNumberOfChoices(
                            pomdp.getStatesWithObservation(refinementComponents->beliefList[refinementComponents->overApproxBeliefStateMap.right.at(state)].observation).front());
                    bool isTarget = refinementComponents->beliefIsTarget[refinementComponents->overApproxBeliefStateMap.right.at(state)];
                    for (uint64_t action = 0; action < numChoices; ++action) {
                        if (transitionsStateActionPair.find(std::make_pair(state, action)) == transitionsStateActionPair.end()) {
                            for (auto const &entry : oldTransitionMatrix.getRow(state, action)) {
                                smb.addNextValue(currentRow, entry.getColumn(), entry.getValue());
                            }
                        } else {
                            for (auto const &iter : transitionsStateActionPair[std::make_pair(state, action)]) {
                                smb.addNextValue(currentRow, iter.first, iter.second);
                            }
                        }
                        ++currentRow;
                        if (isTarget) {
                            // If the state is a target, we only have one action, thus we add the target label and stop the iteration
                            mdpLabeling.addLabelToState("target", state);
                            break;
                        }
                        if (stoppedExplorationStateSet.find(state) != stoppedExplorationStateSet.end()) {
                            break;
                        }
                    }
                    ++currentRowGroup;
                }
                storm::storage::sparse::ModelComponents<ValueType, RewardModelType> modelComponents(smb.build(), mdpLabeling);
                storm::models::sparse::Mdp<ValueType, RewardModelType> overApproxMdp(modelComponents);
                if (computeRewards) {
                    storm::models::sparse::StandardRewardModel<ValueType> mdpRewardModel(boost::none, std::vector<ValueType>(modelComponents.transitionMatrix.getRowCount()));
                    for (auto const &iter : refinementComponents->overApproxBeliefStateMap.left) {
                        auto currentBelief = refinementComponents->beliefList[iter.first];
                        auto representativeState = pomdp.getStatesWithObservation(currentBelief.observation).front();
                        for (uint64_t action = 0; action < overApproxMdp.getNumberOfChoices(iter.second); ++action) {
                            // Add the reward
                            mdpRewardModel.setStateActionReward(overApproxMdp.getChoiceIndex(storm::storage::StateActionPair(iter.second, action)),
                                                                getRewardAfterAction(pomdp.getChoiceIndex(storm::storage::StateActionPair(representativeState, action)),
                                                                                     currentBelief));
                        }
                    }
                    overApproxMdp.addRewardModel("std", mdpRewardModel);
                    overApproxMdp.restrictRewardModels(std::set<std::string>({"std"}));
                }
                overApproxMdp.printModelInformationToStream(std::cout);
                statistics.overApproximationBuildTime.stop();
                STORM_PRINT("Over Approximation MDP build took " << statistics.overApproximationBuildTime << " seconds." << std::endl);
                
                auto model = std::make_shared<storm::models::sparse::Mdp<ValueType, RewardModelType>>(overApproxMdp);
                auto modelPtr = std::static_pointer_cast<storm::models::sparse::Model<ValueType, RewardModelType>>(model);
                std::string propertyString = computeRewards ? "R" : "P";
                propertyString += min ? "min" : "max";
                propertyString += "=? [F \"target\"]";
                std::vector<storm::jani::Property> propertyVector = storm::api::parseProperties(propertyString);
                std::shared_ptr<storm::logic::Formula const> property = storm::api::extractFormulasFromProperties(propertyVector).front();
                auto task = storm::api::createTask<ValueType>(property, false);
                statistics.overApproximationCheckTime.start();
                std::unique_ptr<storm::modelchecker::CheckResult> res(storm::api::verifyWithSparseEngine<ValueType>(model, task));
                statistics.overApproximationCheckTime.stop();
                if (storm::utility::resources::isTerminate() && !res) {
                    return refinementComponents; // Return the result from the previous iteration
                }
                STORM_PRINT("Time Overapproximation: " << statistics.overApproximationCheckTime << std::endl)
                STORM_LOG_ASSERT(res, "Result not exist.");
                res->filter(storm::modelchecker::ExplicitQualitativeCheckResult(storm::storage::BitVector(overApproxMdp.getNumberOfStates(), true)));
                auto overApproxResultMap = res->asExplicitQuantitativeCheckResult<ValueType>().getValueMap();
                auto overApprox = overApproxResultMap[refinementComponents->overApproxBeliefStateMap.left.at(refinementComponents->initialBeliefId)];

                //auto underApprox = weightedSumUnderMap[initialBelief.id];
                auto underApproxComponents = computeUnderapproximation(refinementComponents->beliefList, refinementComponents->beliefIsTarget, targetObservations,
                                                                       refinementComponents->initialBeliefId, min, computeRewards, maxUaModelSize);
                STORM_PRINT("Over-Approximation Result: " << overApprox << std::endl);
                if (storm::utility::resources::isTerminate() && !underApproxComponents) {
                    return std::make_unique<RefinementComponents<ValueType>>(
                        RefinementComponents<ValueType>{modelPtr, overApprox, refinementComponents->underApproxValue, overApproxResultMap, {}, refinementComponents->beliefList, refinementComponents->beliefGrid, refinementComponents->beliefIsTarget, refinementComponents->overApproxBeliefStateMap, {}, refinementComponents->initialBeliefId});
                }
                STORM_PRINT("Under-Approximation Result: " << underApproxComponents->underApproxValue << std::endl);

                return std::make_shared<RefinementComponents<ValueType>>(
                        RefinementComponents<ValueType>{modelPtr, overApprox, underApproxComponents->underApproxValue, overApproxResultMap,
                                                        underApproxComponents->underApproxMap, refinementComponents->beliefList, refinementComponents->beliefGrid,
                                                        refinementComponents->beliefIsTarget, refinementComponents->overApproxBeliefStateMap,
                                                        underApproxComponents->underApproxBeliefStateMap, refinementComponents->initialBeliefId});
            }
            */
            }

            template<typename PomdpModelType, typename BeliefValueType>
            std::shared_ptr<typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ExplorerType> ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::computeUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, uint64_t maxStateCount, std::shared_ptr<BeliefManagerType>& beliefManager) {
                
                statistics.underApproximationBuildTime.start();
                storm::builder::BeliefMdpExplorer<storm::models::sparse::Pomdp<ValueType>> explorer(beliefManager, lowerPomdpValueBounds, upperPomdpValueBounds);
                if (computeRewards) {
                    explorer.startNewExploration(storm::utility::zero<ValueType>());
                } else {
                    explorer.startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                }
                
                // Expand the beliefs to generate the grid on-the-fly
                if (options.explorationThreshold > storm::utility::zero<ValueType>()) {
                    STORM_PRINT("Exploration threshold: " << options.explorationThreshold << std::endl)
                }
                while (explorer.hasUnexploredState()) {
                    uint64_t currId = explorer.exploreNextState();
                    
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    if (targetObservations.count(currObservation) != 0) {
                        explorer.setCurrentStateIsTarget();
                        explorer.addSelfloopTransition();
                    } else {
                        bool stopExploration = false;
                        if (storm::utility::abs<ValueType>(explorer.getUpperValueBoundAtCurrentState() - explorer.getLowerValueBoundAtCurrentState()) < options.explorationThreshold) {
                            stopExploration = true;
                            explorer.setCurrentStateIsTruncated();
                        } else if (explorer.getCurrentNumberOfMdpStates() >= maxStateCount) {
                            stopExploration = true;
                            explorer.setCurrentStateIsTruncated();
                        }
                        for (uint64 action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                            ValueType truncationProbability = storm::utility::zero<ValueType>();
                            ValueType truncationValueBound = storm::utility::zero<ValueType>();
                            auto successors = beliefManager->expand(currId, action);
                            for (auto const& successor : successors) {
                                bool added = explorer.addTransitionToBelief(action, successor.first, successor.second, stopExploration);
                                if (!added) {
                                    STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                                    // We did not explore this successor state. Get a bound on the "missing" value
                                    truncationProbability += successor.second;
                                    truncationValueBound += successor.second * (min ? explorer.computeUpperValueBoundAtBelief(successor.first) : explorer.computeLowerValueBoundAtBelief(successor.first));
                                }
                            }
                            if (stopExploration) {
                                if (computeRewards) {
                                    explorer.addTransitionsToExtraStates(action, truncationProbability);
                                } else {
                                    explorer.addTransitionsToExtraStates(action, truncationValueBound, truncationProbability - truncationValueBound);
                                }
                            }
                            if (computeRewards) {
                                // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                explorer.computeRewardAtCurrentState(action, truncationValueBound);
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        statistics.underApproximationBuildAborted = true;
                        break;
                    }
                }
                statistics.underApproximationStates = explorer.getCurrentNumberOfMdpStates();
                if (storm::utility::resources::isTerminate()) {
                    statistics.underApproximationBuildTime.stop();
                    return nullptr;
                }
                
                explorer.finishExploration();
                statistics.underApproximationBuildTime.stop();

                statistics.underApproximationCheckTime.start();
                explorer.computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.underApproximationCheckTime.stop();

                return std::make_shared<ExplorerType>(std::move(explorer));
            }

            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::refineUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, uint64_t maxStateCount, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& underApproximation) {
                // TODO
            }

            template class ApproximatePOMDPModelchecker<storm::models::sparse::Pomdp<double>>;
            template class ApproximatePOMDPModelchecker<storm::models::sparse::Pomdp<storm::RationalNumber>>;

        }
    }
}
