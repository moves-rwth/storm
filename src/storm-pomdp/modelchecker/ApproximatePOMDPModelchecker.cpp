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
                beliefMdpSizeThreshold = boost::none;
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
                    stream << "# Maximal resolution for over-approximation: " << statistics.overApproximationMaxResolution.get() << std::endl;
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
                    stream << "# Exploration state limit for under-approximation: " << statistics.underApproximationStateLimit.get() << std::endl;
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
                    auto approx = std::make_shared<ExplorerType>(manager, lowerPomdpValueBounds, upperPomdpValueBounds);
                    buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), false, nullptr, observationResolutionVector, manager, approx);
                    if (approx->hasComputedValues()) {
                        STORM_PRINT_AND_LOG("Explored and checked Over-Approximation MDP:\n");
                        approx->getExploredMdp()->printModelInformationToStream(std::cout);
                        ValueType& resultValue = min ? result.lowerBound : result.upperBound;
                        resultValue = approx->getComputedValueAtInitialState();
                        underApproxSizeThreshold = std::max(approx->getExploredMdp()->getNumberOfStates(), underApproxSizeThreshold);
                    }
                }
                { // Underapproximation (Uses a fresh Belief manager)
                    auto manager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                    if (rewardModelName) {
                        manager->setRewardModel(rewardModelName);
                    }
                    auto approx = std::make_shared<ExplorerType>(manager, lowerPomdpValueBounds, upperPomdpValueBounds);
                    if (options.beliefMdpSizeThreshold && options.beliefMdpSizeThreshold.get() > 0) {
                        underApproxSizeThreshold = options.beliefMdpSizeThreshold.get();
                    }
                    if (underApproxSizeThreshold == 0) {
                        underApproxSizeThreshold = pomdp.getNumberOfStates() * pomdp.getMaxNrStatesWithSameObservation(); // Heuristically select this (only relevant if the over-approx could not be build)
                    }
                    buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), underApproxSizeThreshold, manager, approx);
                    if (approx->hasComputedValues()) {
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
                auto overApproxBeliefManager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                auto underApproxBeliefManager = std::make_shared<BeliefManagerType>(pomdp, options.numericPrecision);
                if (rewardModelName) {
                    overApproxBeliefManager->setRewardModel(rewardModelName);
                    underApproxBeliefManager->setRewardModel(rewardModelName);
                }
                
                // OverApproximaion
                auto overApproximation = std::make_shared<ExplorerType>(overApproxBeliefManager, lowerPomdpValueBounds, upperPomdpValueBounds);
                buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), false, nullptr, observationResolutionVector, overApproxBeliefManager, overApproximation);
                if (!overApproximation->hasComputedValues()) {
                    return;
                }
                ValueType& overApproxValue = min ? result.lowerBound : result.upperBound;
                overApproxValue = overApproximation->getComputedValueAtInitialState();
                
                // UnderApproximation
                uint64_t underApproxSizeThreshold;
                if (options.beliefMdpSizeThreshold && options.beliefMdpSizeThreshold.get() > 0ull) {
                    underApproxSizeThreshold = options.beliefMdpSizeThreshold.get();
                } else {
                    underApproxSizeThreshold = overApproximation->getExploredMdp()->getNumberOfStates();
                }
                auto underApproximation = std::make_shared<ExplorerType>(underApproxBeliefManager, lowerPomdpValueBounds, upperPomdpValueBounds);
                buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), underApproxSizeThreshold, underApproxBeliefManager, underApproximation);
                if (!underApproximation->hasComputedValues()) {
                    return;
                }
                ValueType& underApproxValue = min ? result.upperBound : result.lowerBound;
                underApproxValue = underApproximation->getComputedValueAtInitialState();
                
                // ValueType lastMinScore = storm::utility::infinity<ValueType>();
                // Start refinement
                statistics.refinementSteps = 0;
                ValueType refinementAggressiveness = storm::utility::convertNumber<ValueType>(0.0);
                while (result.diff() > options.refinementPrecision) {
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                    ++statistics.refinementSteps.get();
                    STORM_LOG_INFO("Starting refinement step " << statistics.refinementSteps.get() << ". Current difference between lower and upper bound is " << result.diff() << ".");
                    
                    // Refine over-approximation
                    STORM_LOG_DEBUG("Refining over-approximation with aggressiveness " << refinementAggressiveness << ".");
                    if (min) {
                        overApproximation->takeCurrentValuesAsLowerBounds();
                    } else {
                        overApproximation->takeCurrentValuesAsUpperBounds();
                    }
                    buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), true, &refinementAggressiveness, observationResolutionVector, overApproxBeliefManager, overApproximation);
                    if (overApproximation->hasComputedValues()) {
                        overApproxValue = overApproximation->getComputedValueAtInitialState();
                    } else {
                        break;
                    }
                    
                    if (result.diff() > options.refinementPrecision) {
                        // Refine under-approximation
                        underApproxSizeThreshold = storm::utility::convertNumber<uint64_t, ValueType>(storm::utility::convertNumber<ValueType>(underApproxSizeThreshold) * (storm::utility::one<ValueType>() + refinementAggressiveness));
                        underApproxSizeThreshold = std::max<uint64_t>(underApproxSizeThreshold, overApproximation->getExploredMdp()->getNumberOfStates());
                        STORM_LOG_DEBUG("Refining under-approximation with size threshold " << underApproxSizeThreshold << ".");
                        buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), underApproxSizeThreshold, underApproxBeliefManager, underApproximation);
                        if (underApproximation->hasComputedValues()) {
                            underApproxValue = underApproximation->getComputedValueAtInitialState();
                        } else {
                            break;
                        }
                    }
                }
            }

            /*!
             * Heuristically rates the quality of the approximation described by the given successor observation info.
             * Here, 0 means a bad approximation and 1 means a good approximation.
             */
            template<typename PomdpModelType, typename BeliefValueType>
            typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ValueType ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::rateObservation(typename ExplorerType::SuccessorObservationInformation const& info, uint64_t const& observationResolution, uint64_t const& maxResolution) {
                auto n = storm::utility::convertNumber<ValueType, uint64_t>(info.support.size());
                auto one = storm::utility::one<ValueType>();
                if (storm::utility::isOne(n)) {
                    // If the belief is Dirac, it has to be approximated precisely.
                    // In this case, we return the best possible rating
                    return one;
                } else {
                    // Create the rating for this observation at this choice from the given info
                    ValueType obsChoiceRating = info.maxProbabilityToSuccessorWithObs / info.observationProbability;
                    // At this point, obsRating is the largest triangulation weight (which ranges from 1/n to 1
                    // Normalize the rating so that it ranges from 0 to 1, where
                    // 0 means that the actual belief lies in the middle of the triangulating simplex (i.e. a "bad" approximation) and 1 means that the belief is precisely approximated.
                    obsChoiceRating = (obsChoiceRating * n - one) / (n - one);
                    // Scale the ratings with the resolutions, so that low resolutions get a lower rating (and are thus more likely to be refined)
                    obsChoiceRating *= storm::utility::convertNumber<ValueType>(observationResolution) / storm::utility::convertNumber<ValueType>(maxResolution);
                    return obsChoiceRating;
                }
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            std::vector<typename ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::ValueType> ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::getObservationRatings(std::shared_ptr<ExplorerType> const& overApproximation, std::vector<uint64_t> const& observationResolutionVector, uint64_t const& maxResolution) {
                uint64_t numMdpChoices = overApproximation->getExploredMdp()->getNumberOfChoices();

                std::vector<ValueType> resultingRatings(pomdp.getNrObservations(), storm::utility::one<ValueType>());
                
                std::map<uint32_t, typename ExplorerType::SuccessorObservationInformation> gatheredSuccessorObservations; // Declare here to avoid reallocations
                for (uint64_t mdpChoice = 0; mdpChoice < numMdpChoices; ++mdpChoice) {
                    gatheredSuccessorObservations.clear();
                    overApproximation->gatherSuccessorObservationInformationAtMdpChoice(mdpChoice, gatheredSuccessorObservations);
                    for (auto const& obsInfo : gatheredSuccessorObservations) {
                        auto const& obs = obsInfo.first;
                        ValueType obsChoiceRating = rateObservation(obsInfo.second, observationResolutionVector[obs], maxResolution);
 
                        // The rating of the observation will be the minimum over all choice-based observation ratings
                        resultingRatings[obs] = std::min(resultingRatings[obs], obsChoiceRating);
                    }
                }
                return resultingRatings;
            }
            
            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::buildOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, ValueType* refinementAggressiveness, std::vector<uint64_t>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& overApproximation) {
                STORM_LOG_ASSERT(!refine || refinementAggressiveness != nullptr, "Refinement enabled but no aggressiveness given");
                STORM_LOG_ASSERT(!refine || *refinementAggressiveness >= storm::utility::zero<ValueType>(), "Can not refine with negative  aggressiveness.");
                STORM_LOG_ASSERT(!refine || *refinementAggressiveness <= storm::utility::one<ValueType>(), "Refinement with aggressiveness > 1 is invalid.");
                
                // current maximal resolution (needed for refinement heuristic)
                uint64_t oldMaxResolution = *std::max_element(observationResolutionVector.begin(), observationResolutionVector.end());

                statistics.overApproximationBuildTime.start();
                storm::storage::BitVector refinedObservations;
                if (!refine) {
                    // If we build the model from scratch, we first have to setup the explorer for the overApproximation.
                    if (computeRewards) {
                        overApproximation->startNewExploration(storm::utility::zero<ValueType>());
                    } else {
                        overApproximation->startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                    }
                } else {
                    // If we refine the existing overApproximation, we need to find out which observation resolutions need refinement.
                    auto obsRatings = getObservationRatings(overApproximation, observationResolutionVector, oldMaxResolution);
                    ValueType minRating = *std::min_element(obsRatings.begin(), obsRatings.end());
                    // Potentially increase the aggressiveness so that at least one observation actually gets refinement.
                    *refinementAggressiveness = std::max(minRating, *refinementAggressiveness);
                    refinedObservations = storm::utility::vector::filter<ValueType>(obsRatings, [&refinementAggressiveness](ValueType const& r) { return r <= *refinementAggressiveness;});
                    STORM_LOG_DEBUG("Refining the resolution of " << refinedObservations.getNumberOfSetBits() << "/" << refinedObservations.size() << " observations.");
                    for (auto const& obs : refinedObservations) {
                        // Heuristically increment the resolution at the refined observations (also based on the refinementAggressiveness)
                        ValueType incrementValue = storm::utility::one<ValueType>() + (*refinementAggressiveness) * storm::utility::convertNumber<ValueType>(observationResolutionVector[obs]);
                        observationResolutionVector[obs] += storm::utility::convertNumber<uint64_t>(storm::utility::ceil(incrementValue));
                    }
                    overApproximation->restartExploration();
                }
                statistics.overApproximationMaxResolution = *std::max_element(observationResolutionVector.begin(), observationResolutionVector.end());
                
                // Start exploration
                std::map<uint32_t, typename ExplorerType::SuccessorObservationInformation> gatheredSuccessorObservations; // Declare here to avoid reallocations
                while (overApproximation->hasUnexploredState()) {
                    uint64_t currId = overApproximation->exploreNextState();
                    
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    if (targetObservations.count(currObservation) != 0) {
                        overApproximation->setCurrentStateIsTarget();
                        overApproximation->addSelfloopTransition();
                    } else {
                        bool stopExploration = false;
                        if (storm::utility::abs<ValueType>(overApproximation->getUpperValueBoundAtCurrentState() - overApproximation->getLowerValueBoundAtCurrentState()) < options.explorationThreshold) {
                            stopExploration = true;
                            overApproximation->setCurrentStateIsTruncated();
                        }
                        for (uint64 action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                            // Check whether we expand this state/action pair
                            // We always expand if we are not doing refinement of if the state was not available in the "old" MDP.
                            // Otherwise, a heuristic decides.
                            bool expandStateAction = true;
                            if (refine && overApproximation->currentStateHasOldBehavior()) {
                                // Compute a rating of the current state/action pair
                                ValueType stateActionRating = storm::utility::one<ValueType>();
                                gatheredSuccessorObservations.clear();
                                overApproximation->gatherSuccessorObservationInformationAtCurrentState(action, gatheredSuccessorObservations);
                                for (auto const& obsInfo : gatheredSuccessorObservations) {
                                    if (refinedObservations.get(obsInfo.first)) {
                                        ValueType obsRating = rateObservation(obsInfo.second, observationResolutionVector[obsInfo.first], oldMaxResolution);
                                        stateActionRating = std::min(stateActionRating, obsRating);
                                    }
                                }
                                // Only refine if this rating is below the doubled refinementAggressiveness
                                expandStateAction = stateActionRating < storm::utility::convertNumber<ValueType>(2.0) * (*refinementAggressiveness);
                            }
                            if (expandStateAction) {
                                ValueType truncationProbability = storm::utility::zero<ValueType>();
                                ValueType truncationValueBound = storm::utility::zero<ValueType>();
                                auto successorGridPoints = beliefManager->expandAndTriangulate(currId, action, observationResolutionVector);
                                for (auto const& successor : successorGridPoints) {
                                    bool added = overApproximation->addTransitionToBelief(action, successor.first, successor.second, stopExploration);
                                    if (!added) {
                                        STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                                        // We did not explore this successor state. Get a bound on the "missing" value
                                        truncationProbability += successor.second;
                                        truncationValueBound += successor.second * (min ? overApproximation->computeLowerValueBoundAtBelief(successor.first) : overApproximation->computeUpperValueBoundAtBelief(successor.first));
                                    }
                                }
                                if (stopExploration) {
                                    if (computeRewards) {
                                        overApproximation->addTransitionsToExtraStates(action, truncationProbability);
                                    } else {
                                        overApproximation->addTransitionsToExtraStates(action, truncationValueBound, truncationProbability - truncationValueBound);
                                    }
                                }
                                if (computeRewards) {
                                    // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                    overApproximation->computeRewardAtCurrentState(action, truncationValueBound);
                                }
                            } else {
                                // Do not refine here
                                overApproximation->restoreOldBehaviorAtCurrentState(action);
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        statistics.overApproximationBuildAborted = true;
                        break;
                    }
                }
                // TODO: Drop unreachable states (sometimes?)
                statistics.overApproximationStates = overApproximation->getCurrentNumberOfMdpStates();
                if (storm::utility::resources::isTerminate()) {
                    statistics.overApproximationBuildTime.stop();
                    return;
                }
                
                overApproximation->finishExploration();
                statistics.overApproximationBuildTime.stop();
                
                statistics.overApproximationCheckTime.start();
                overApproximation->computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.overApproximationCheckTime.stop();
            }

            template<typename PomdpModelType, typename BeliefValueType>
            void ApproximatePOMDPModelchecker<PomdpModelType, BeliefValueType>::buildUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, uint64_t maxStateCount, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& underApproximation) {
                
                statistics.underApproximationBuildTime.start();
                statistics.underApproximationStateLimit = maxStateCount;
                if (!underApproximation->hasComputedValues()) {
                    // Build a new under approximation
                    if (computeRewards) {
                        underApproximation->startNewExploration(storm::utility::zero<ValueType>());
                    } else {
                        underApproximation->startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                    }
                } else {
                    // Restart the building process
                    underApproximation->restartExploration();
                }
                
                // Expand the beliefs
                while (underApproximation->hasUnexploredState()) {
                    uint64_t currId = underApproximation->exploreNextState();
                    
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    if (targetObservations.count(currObservation) != 0) {
                        underApproximation->setCurrentStateIsTarget();
                        underApproximation->addSelfloopTransition();
                    } else {
                        bool stopExploration = false;
                        if (!underApproximation->currentStateHasOldBehavior()) {
                            if (storm::utility::abs<ValueType>(underApproximation->getUpperValueBoundAtCurrentState() - underApproximation->getLowerValueBoundAtCurrentState()) < options.explorationThreshold) {
                                stopExploration = true;
                                underApproximation->setCurrentStateIsTruncated();
                            } else if (underApproximation->getCurrentNumberOfMdpStates() >= maxStateCount) {
                                stopExploration = true;
                                underApproximation->setCurrentStateIsTruncated();
                            }
                        }
                        for (uint64 action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                            // Always restore old behavior if available
                            if (underApproximation->currentStateHasOldBehavior()) {
                                underApproximation->restoreOldBehaviorAtCurrentState(action);
                            } else {
                                ValueType truncationProbability = storm::utility::zero<ValueType>();
                                ValueType truncationValueBound = storm::utility::zero<ValueType>();
                                auto successors = beliefManager->expand(currId, action);
                                for (auto const& successor : successors) {
                                    bool added = underApproximation->addTransitionToBelief(action, successor.first, successor.second, stopExploration);
                                    if (!added) {
                                        STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                                        // We did not explore this successor state. Get a bound on the "missing" value
                                        truncationProbability += successor.second;
                                        // Some care has to be taken here: Essentially, we are triangulating a value for the under-approximation out of other
                                        // under-approximation values. In general, this does not yield a sound underapproximation anymore.
                                        // However, in our case this is still the case as the under-approximation values are based on a memoryless scheduler.
                                        truncationValueBound += successor.second * (min ? underApproximation->computeUpperValueBoundAtBelief(successor.first) : underApproximation->computeLowerValueBoundAtBelief(successor.first));
                                    }
                                }
                                if (stopExploration) {
                                    if (computeRewards) {
                                        underApproximation->addTransitionsToExtraStates(action, truncationProbability);
                                    } else {
                                        underApproximation->addTransitionsToExtraStates(action, truncationValueBound, truncationProbability - truncationValueBound);
                                    }
                                }
                                if (computeRewards) {
                                    // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                    underApproximation->computeRewardAtCurrentState(action, truncationValueBound);
                                }
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        statistics.underApproximationBuildAborted = true;
                        break;
                    }
                }
                statistics.underApproximationStates = underApproximation->getCurrentNumberOfMdpStates();
                if (storm::utility::resources::isTerminate()) {
                    statistics.underApproximationBuildTime.stop();
                    return;
                }
                
                underApproximation->finishExploration();
                statistics.underApproximationBuildTime.stop();

                statistics.underApproximationCheckTime.start();
                underApproximation->computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.underApproximationCheckTime.stop();

            }

            template class ApproximatePOMDPModelchecker<storm::models::sparse::Pomdp<double>>;
            template class ApproximatePOMDPModelchecker<storm::models::sparse::Pomdp<storm::RationalNumber>>;

        }
    }
}
