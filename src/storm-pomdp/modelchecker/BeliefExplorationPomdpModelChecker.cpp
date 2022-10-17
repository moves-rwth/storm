#include "BeliefExplorationPomdpModelChecker.h"

#include <tuple>

#include <boost/algorithm/string.hpp>

#include "storm-pomdp/analysis/FormulaInformation.h"
#include "storm-pomdp/analysis/FiniteBeliefMdpDetection.h"
#include "storm-pomdp/transformer/MakeStateSetObservationClosed.h"

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/NumberTraits.h"
#include "storm/utility/graph.h"
#include "storm/logic/Formulas.h"

#include "storm-pomdp/builder/BeliefMdpExplorer.h"
#include "storm-pomdp/modelchecker/PomdpParametricTransformationModelChecker.h"
#include "storm-pomdp/modelchecker/PreprocessingPomdpValueBoundsModelChecker.h"
#include "storm/api/export.h"
#include "storm/api/properties.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/vector.h"

#include "storm/utility/macros.h"
#include "storm/utility/SignalHandler.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Result::Result(ValueType lower, ValueType upper) : lowerBound(lower), upperBound(upper) {
                // Intentionally left empty
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            typename BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::ValueType
            BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Result::diff(bool relative) const {
                ValueType diff = upperBound - lowerBound;
                if (diff < storm::utility::zero<ValueType>()) {
                    STORM_LOG_WARN_COND(diff >= storm::utility::convertNumber<ValueType>(1e-6), "Upper bound '" << upperBound << "' is smaller than lower bound '" << lowerBound << "': Difference is " << diff << ".");
                    diff = storm::utility::zero<ValueType>();
                }
                if (relative && !storm::utility::isZero(upperBound)) {
                    diff /= upperBound;
                }
                return diff;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            bool BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Result::updateLowerBound(ValueType const& value) {
                if (value > lowerBound) {
                    lowerBound = value;
                    return true;
                }
                return false;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            bool BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Result::updateUpperBound(ValueType const& value) {
                if (value < upperBound) {
                    upperBound = value;
                    return true;
                }
                return false;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Statistics::Statistics() :  beliefMdpDetectedToBeFinite(false), refinementFixpointDetected(false), overApproximationBuildAborted(false), underApproximationBuildAborted(false), aborted(false) {
                // intentionally left empty;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::BeliefExplorationPomdpModelChecker(std::shared_ptr<PomdpModelType> pomdp, Options options) : inputPomdp(pomdp), options(options) {
                STORM_LOG_ASSERT(inputPomdp, "The given POMDP is not initialized.");
                STORM_LOG_ERROR_COND(inputPomdp->isCanonic(), "Input Pomdp is not known to be canonic. This might lead to unexpected verification results.");

                beliefTypeCC = storm::utility::ConstantsComparator<BeliefValueType>(storm::utility::convertNumber<BeliefValueType>(this->options.numericPrecision), false);
                valueTypeCC = storm::utility::ConstantsComparator<ValueType>(this->options.numericPrecision, false);
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::precomputeValueBounds(storm::logic::Formula const& formula, storm::solver::MinMaxMethod minMaxMethod) {
                auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(pomdp(), formula);

                // Compute some initial bounds on the values for each state of the pomdp
                // We work with the Belief MDP value type, so if the POMDP is exact, but the belief MDP is not, we need to convert
                auto initialPomdpValueBounds = PreprocessingPomdpValueBoundsModelChecker<ValueType>(pomdp(), minMaxMethod).getValueBounds(formula, formulaInfo);

                std::vector<ValueType> pMCValueBound;
                if(options.useParametricPreprocessing && options.unfold){
                    STORM_LOG_WARN("Using the transformation to a pMC is currently not supported. The preprocessing step is skipped.");
                    //initialPomdpValueBounds.parametric = PomdpParametricTransformationModelChecker<ValueType>(pomdp()).computeValuesForFMPolicy(formula, formulaInfo, options.paramMemBound, storm::storage::PomdpMemoryPattern::Full, options.paramGDEps, options.paramGDMaxInstantiations);
                }
                pomdpValueBounds.trivialPomdpValueBounds = initialPomdpValueBounds;

                // If we clip and compute rewards, compute the values necessary for the correction terms
                if((options.clippingThresholdInit > 0 || options.useGridClipping) && formula.isRewardOperatorFormula()){
                    pomdpValueBounds.extremePomdpValueBound =
                        PreprocessingPomdpValueBoundsModelChecker<ValueType>(pomdp(), minMaxMethod).getExtremeValueBound(formula, formulaInfo);
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            typename BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::Result BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::check(storm::logic::Formula const& formula, std::vector<std::vector<ValueType>> additionalUnderApproximationBounds) {
                STORM_LOG_ASSERT(options.unfold || options.discretize, "Invoked belief exploration but no task (unfold or discretize) given.");
                
                // Potentially reset preprocessed model from previous call
                preprocessedPomdp.reset();
                
                // Reset all collected statistics
                statistics = Statistics();
                statistics.totalTime.start();
                // Extract the relevant information from the formula
                auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(pomdp(), formula);
                
                precomputeValueBounds(formula, options.preProcMinMaxMethod);
                if(!additionalUnderApproximationBounds.empty()){
                    if(formulaInfo.minimize()){
                        pomdpValueBounds.trivialPomdpValueBounds.upper.insert(pomdpValueBounds.trivialPomdpValueBounds.upper.end(), std::make_move_iterator(additionalUnderApproximationBounds.begin()), std::make_move_iterator(additionalUnderApproximationBounds.end()));
                    } else {
                        pomdpValueBounds.trivialPomdpValueBounds.lower.insert(pomdpValueBounds.trivialPomdpValueBounds.lower.end(), std::make_move_iterator(additionalUnderApproximationBounds.begin()), std::make_move_iterator(additionalUnderApproximationBounds.end()));
                    }
                }
                uint64_t initialPomdpState = pomdp().getInitialStates().getNextSetIndex(0);
                Result result(pomdpValueBounds.trivialPomdpValueBounds.getHighestLowerBound(initialPomdpState), pomdpValueBounds.trivialPomdpValueBounds.getSmallestUpperBound(initialPomdpState));
                STORM_LOG_INFO("Initial value bounds are [" << result.lowerBound << ", " << result.upperBound << "]");

                boost::optional<std::string> rewardModelName;
                std::set<uint32_t> targetObservations;
                if (formulaInfo.isNonNestedReachabilityProbability() || formulaInfo.isNonNestedExpectedRewardFormula()) {
                    if (formulaInfo.getTargetStates().observationClosed) {
                        targetObservations = formulaInfo.getTargetStates().observations;
                    } else {
                        storm::transformer::MakeStateSetObservationClosed<ValueType> obsCloser(inputPomdp);
                        std::tie(preprocessedPomdp, targetObservations) = obsCloser.transform(formulaInfo.getTargetStates().states);
                    }
                    if (formulaInfo.isNonNestedReachabilityProbability()) {
                        if (!formulaInfo.getSinkStates().empty()) {
                            storm::storage::sparse::ModelComponents<ValueType> components;
                            components.stateLabeling = pomdp().getStateLabeling();
                            components.rewardModels = pomdp().getRewardModels();
                            auto matrix = pomdp().getTransitionMatrix();
                            matrix.makeRowGroupsAbsorbing(formulaInfo.getSinkStates().states);
                            components.transitionMatrix = matrix;
                            components.observabilityClasses = pomdp().getObservations();
                            if(pomdp().hasChoiceLabeling()){
                                components.choiceLabeling = pomdp().getChoiceLabeling();
                            }
                            if(pomdp().hasObservationValuations()){
                                components.observationValuations = pomdp().getObservationValuations();
                            }
                            preprocessedPomdp = std::make_shared<storm::models::sparse::Pomdp<ValueType>>(std::move(components), true);
                            auto reachableFromSinkStates = storm::utility::graph::getReachableStates(pomdp().getTransitionMatrix(), formulaInfo.getSinkStates().states, formulaInfo.getSinkStates().states, ~formulaInfo.getSinkStates().states);
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
                if (storm::pomdp::detectFiniteBeliefMdp(pomdp(), formulaInfo.getTargetStates().states)) {
                    STORM_LOG_INFO("Detected that the belief MDP is finite.");
                    statistics.beliefMdpDetectedToBeFinite = true;
                }

                if (options.refine) {
                    refineReachability(targetObservations, formulaInfo.minimize(), rewardModelName, pomdpValueBounds, result);
                } else {
                    computeReachability(targetObservations, formulaInfo.minimize(), rewardModelName, pomdpValueBounds, result);
                }
                // "clear" results in case they were actually not requested (this will make the output a bit more clear)
                if ((formulaInfo.minimize() && !options.discretize) || (formulaInfo.maximize() && !options.unfold)) {
                    result.lowerBound = -storm::utility::infinity<ValueType>();
                }
                if ((formulaInfo.maximize() && !options.discretize) || (formulaInfo.minimize() && !options.unfold)) {
                    result.upperBound = storm::utility::infinity<ValueType>();
                }

                if (storm::utility::resources::isTerminate()) {
                    statistics.aborted = true;
                }
                statistics.totalTime.stop();
                return result;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::printStatisticsToStream(std::ostream& stream) const {
                stream << "##### Grid Approximation Statistics ######\n";
                stream << "# Input model: \n";
                pomdp().printModelInformationToStream(stream);
                stream << "# Max. Number of states with same observation: " << pomdp().getMaxNrStatesWithSameObservation() << '\n';
                if (statistics.beliefMdpDetectedToBeFinite) {
                    stream << "# Pre-computations detected that the belief MDP is finite.\n";
                }
                if (statistics.aborted) {
                    stream << "# Computation aborted early\n";
                }

                stream << "# Total check time: " << statistics.totalTime << '\n';
                // Refinement information:
                if (statistics.refinementSteps) {
                    stream << "# Number of refinement steps: " << statistics.refinementSteps.get() << '\n';
                }
                if (statistics.refinementFixpointDetected) {
                    stream << "# Detected a refinement fixpoint.\n";
                }

                // The overapproximation MDP:
                if (statistics.overApproximationStates) {
                    stream << "# Number of states in the ";
                    if (options.refine) {
                        stream << "final ";
                    }
                    stream << "grid MDP for the over-approximation: ";
                    if (statistics.overApproximationBuildAborted) {
                        stream << ">=";
                    }
                    stream << statistics.overApproximationStates.get() << '\n';
                    stream << "# Maximal resolution for over-approximation: " << statistics.overApproximationMaxResolution.get() << '\n';
                    stream << "# Time spend for building the over-approx grid MDP(s): " << statistics.overApproximationBuildTime << '\n';
                    stream << "# Time spend for checking the over-approx grid MDP(s): " << statistics.overApproximationCheckTime << '\n';
                }

                // The underapproximation MDP:
                if (statistics.underApproximationStates) {
                    stream << "# Number of states in the ";
                    if (options.refine) {
                        stream << "final ";
                    }
                    stream << "belief MDP for the under-approximation: ";
                    if (statistics.underApproximationBuildAborted) {
                        stream << ">=";
                    }
                    stream << statistics.underApproximationStates.get() << '\n';
                    if(statistics.nrClippingAttempts) {
                        stream << "# Clipping attempts (clipped states) for the under-approximation: ";
                        if (statistics.underApproximationBuildAborted) {
                            stream << ">=";
                        }
                        stream << statistics.nrClippingAttempts.get() << " (" << statistics.nrClippedStates.get() << ")\n";
                        stream << "# Total clipping preprocessing time: " << statistics.clippingPreTime << "\n";
                        stream << "# Total clipping time: " << statistics.clipWatch << "\n";
                    } else if (statistics.nrTruncatedStates){
                        stream << "# Truncated states for the under-approximation: ";
                        if (statistics.underApproximationBuildAborted) {
                            stream << ">=";
                        }
                        stream << statistics.nrTruncatedStates.get() << "\n";
                    }
                    if (statistics.underApproximationStateLimit) {
                        stream << "# Exploration state limit for under-approximation: " << statistics.underApproximationStateLimit.get() << '\n';
                    }
                    stream << "# Time spend for building the under-approx grid MDP(s): " << statistics.underApproximationBuildTime << '\n';
                    stream << "# Time spend for checking the under-approx grid MDP(s): " << statistics.underApproximationCheckTime << '\n';
                }

                stream << "##########################################\n";
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::computeReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::POMDPValueBounds<ValueType> const& valueBounds, Result &result) {
                auto trivialPOMDPBounds = valueBounds.trivialPomdpValueBounds;
                if (options.discretize) {
                    std::vector<BeliefValueType> observationResolutionVector(pomdp().getNrObservations(),
                                                                             storm::utility::convertNumber<BeliefValueType>(options.resolutionInit));
                    auto manager = std::make_shared<BeliefManagerType>(pomdp(), storm::utility::convertNumber<BeliefValueType>(options.numericPrecision),
                                                                       options.dynamicTriangulation ? BeliefManagerType::TriangulationMode::Dynamic
                                                                                                    : BeliefManagerType::TriangulationMode::Static);
                    if (rewardModelName) {
                        manager->setRewardModel(rewardModelName);
                    }
                    auto approx = std::make_shared<ExplorerType>(manager, trivialPOMDPBounds);
                    HeuristicParameters heuristicParameters = {
                            .gapThreshold = options.gapThresholdInit,
                            .observationThreshold = options.obsThresholdInit, // Actually not relevant without refinement
                            .sizeThreshold = options.sizeThresholdInit == 0 ? std::numeric_limits<uint64_t>::max() : options.sizeThresholdInit,
                            .clippingThreshold = options.clippingThresholdInit,
                            .optimalChoiceValueEpsilon = options.optimalChoiceValueThresholdInit,};

                    buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), false, heuristicParameters, observationResolutionVector, manager, approx);
                    if (approx->hasComputedValues()) {
                        auto printInfo = [&approx]() {
                            std::stringstream str;
                            str << "Explored and checked Over-Approximation MDP:\n";
                            approx->getExploredMdp()->printModelInformationToStream(str);
                            return str.str();
                        };
                        STORM_LOG_INFO(printInfo());
                        ValueType &resultValue = min ? result.lowerBound : result.upperBound;
                        resultValue = approx->getComputedValueAtInitialState();
                    }
                }
                if (options.unfold) { // Underapproximation (uses a fresh Belief manager)
                    auto manager = std::make_shared<BeliefManagerType>(pomdp(), storm::utility::convertNumber<BeliefValueType>(options.numericPrecision),
                                                                       options.dynamicTriangulation ? BeliefManagerType::TriangulationMode::Dynamic
                                                                                                    : BeliefManagerType::TriangulationMode::Static);
                    if (rewardModelName) {
                        manager->setRewardModel(rewardModelName);
                    }

                    auto approx = std::make_shared<ExplorerType>(manager, trivialPOMDPBounds, options.explorationHeuristic);
                    HeuristicParameters heuristicParameters = {
                            .gapThreshold = options.gapThresholdInit,
                            .observationThreshold = options.obsThresholdInit, // Actually not relevant without refinement
                            .sizeThreshold = options.sizeThresholdInit,
                            .clippingThreshold = options.clippingThresholdInit,
                            .optimalChoiceValueEpsilon = options.optimalChoiceValueThresholdInit,};
                    if (heuristicParameters.sizeThreshold == 0) {
                        if (options.explorationTimeLimit) {
                            heuristicParameters.sizeThreshold = std::numeric_limits<uint64_t>::max();
                        } else {
                            heuristicParameters.sizeThreshold = pomdp().getNumberOfStates() * pomdp().getMaxNrStatesWithSameObservation();
                            STORM_PRINT_AND_LOG("Heuristically selected an under-approximation mdp size threshold of " << heuristicParameters.sizeThreshold << ".\n")
                        }
                    }
                    // If we clip and compute rewards
                    if((options.clippingThresholdInit > 0 || options.useGridClipping) && rewardModelName.is_initialized()) {
                        approx->setExtremeValueBound(valueBounds.extremePomdpValueBound);
                    }
                    buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), false, heuristicParameters, manager, approx);
                    if (approx->hasComputedValues()) {
                        auto printInfo = [&approx]() {
                            std::stringstream str;
                            str << "Explored and checked Under-Approximation MDP:\n";
                            approx->getExploredMdp()->printModelInformationToStream(str);
                            return str.str();
                        };

                        std::shared_ptr<storm::models::sparse::Model<ValueType>> scheduledModel = approx->getExploredMdp();
                        storm::models::sparse::StateLabeling newLabeling(scheduledModel->getStateLabeling());
                        auto nrPreprocessingScheds = min ? approx->getNrSchedulersForUpperBounds() : approx->getNrSchedulersForLowerBounds();
                        for(uint64_t i = 0; i < nrPreprocessingScheds; ++i){
                            newLabeling.addLabel("sched_" + std::to_string(i));
                        }
                        newLabeling.addLabel("cutoff");
                        for(uint64_t i = 0; i < scheduledModel->getNumberOfStates(); ++i){
                            if(newLabeling.getStateHasLabel("truncated",i)){
                                newLabeling.addLabelToState("sched_" + std::to_string(approx->getSchedulerForExploredMdp()->getChoice(i).getDeterministicChoice()),i);
                                newLabeling.addLabelToState("cutoff", i);
                            }
                        }
                        newLabeling.removeLabel("truncated");
                        storm::storage::sparse::ModelComponents<ValueType> modelComponents(scheduledModel->getTransitionMatrix(), newLabeling, scheduledModel->getRewardModels());
                        if(scheduledModel->hasChoiceLabeling()){
                            modelComponents.choiceLabeling = scheduledModel->getChoiceLabeling();
                        }
                        storm::models::sparse::Mdp<ValueType> newMDP(modelComponents);
                        auto inducedMC = newMDP.applyScheduler(*(approx->getSchedulerForExploredMdp()), true);
                        scheduledModel = std::static_pointer_cast<storm::models::sparse::Model<ValueType>>(inducedMC);
                        result.schedulerAsMarkovChain = scheduledModel;
                        if(min){
                            result.cutoffSchedulers = approx->getUpperValueBoundSchedulers();
                        } else {
                            result.cutoffSchedulers = approx->getLowerValueBoundSchedulers();
                        }
                        ValueType &resultValue = min ? result.upperBound : result.lowerBound;
                        resultValue = approx->getComputedValueAtInitialState();
                    }
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::refineReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::POMDPValueBounds<ValueType> const & valueBounds, Result &result) {
                statistics.refinementSteps = 0;
                auto trivialPOMDPBounds = valueBounds.trivialPomdpValueBounds;
                // Set up exploration data
                std::vector<BeliefValueType> observationResolutionVector;
                std::shared_ptr<BeliefManagerType> overApproxBeliefManager;
                std::shared_ptr<ExplorerType> overApproximation;
                HeuristicParameters overApproxHeuristicPar;
                if (options.discretize) { // Setup and build first OverApproximation
                    observationResolutionVector = std::vector<BeliefValueType>(pomdp().getNrObservations(), storm::utility::convertNumber<BeliefValueType>(options.resolutionInit));
                    overApproxBeliefManager = std::make_shared<BeliefManagerType>(pomdp(), storm::utility::convertNumber<BeliefValueType>(options.numericPrecision), options.dynamicTriangulation ? BeliefManagerType::TriangulationMode::Dynamic : BeliefManagerType::TriangulationMode::Static);
                    if (rewardModelName) {
                        overApproxBeliefManager->setRewardModel(rewardModelName);
                    }
                    overApproximation = std::make_shared<ExplorerType>(overApproxBeliefManager, trivialPOMDPBounds, storm::builder::ExplorationHeuristic::BreadthFirst);
                    overApproxHeuristicPar.gapThreshold = options.gapThresholdInit;
                    overApproxHeuristicPar.observationThreshold = options.obsThresholdInit;
                    overApproxHeuristicPar.sizeThreshold = options.sizeThresholdInit == 0 ? std::numeric_limits<uint64_t>::max() : options.sizeThresholdInit;
                    overApproxHeuristicPar.optimalChoiceValueEpsilon = options.optimalChoiceValueThresholdInit;
                    overApproxHeuristicPar.clippingThreshold = options.clippingThresholdInit;
                    buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), false, overApproxHeuristicPar, observationResolutionVector, overApproxBeliefManager, overApproximation);
                    if (!overApproximation->hasComputedValues() || storm::utility::resources::isTerminate()) {
                        return;
                    }
                    ValueType const& newValue = overApproximation->getComputedValueAtInitialState();
                    bool betterBound = min ? result.updateLowerBound(newValue) : result.updateUpperBound(newValue);
                    if (betterBound) {
                        STORM_LOG_INFO("Over-approx result for refinement improved after " << statistics.totalTime << " seconds in refinement step #" << statistics.refinementSteps.get() << ". New value is '" << newValue << "'.\n");
                    }
                }

                std::shared_ptr<BeliefManagerType> underApproxBeliefManager;
                std::shared_ptr<ExplorerType> underApproximation;
                HeuristicParameters underApproxHeuristicPar;
                if (options.unfold) { // Setup and build first UnderApproximation
                    underApproxBeliefManager = std::make_shared<BeliefManagerType>(pomdp(), storm::utility::convertNumber<BeliefValueType>(options.numericPrecision), options.dynamicTriangulation ? BeliefManagerType::TriangulationMode::Dynamic : BeliefManagerType::TriangulationMode::Static);
                    if (rewardModelName) {
                        underApproxBeliefManager->setRewardModel(rewardModelName);
                    }
                    //TODO different exploration heuristics in over-approximation?
                    underApproximation = std::make_shared<ExplorerType>(underApproxBeliefManager, trivialPOMDPBounds, options.explorationHeuristic);
                    underApproxHeuristicPar.gapThreshold = options.gapThresholdInit;
                    underApproxHeuristicPar.optimalChoiceValueEpsilon = options.optimalChoiceValueThresholdInit;
                    underApproxHeuristicPar.sizeThreshold = options.sizeThresholdInit;
                    underApproxHeuristicPar.clippingThreshold = options.clippingThresholdInit;
                    if (underApproxHeuristicPar.sizeThreshold == 0) {
                        // Select a decent value automatically
                        underApproxHeuristicPar.sizeThreshold = pomdp().getNumberOfStates() * pomdp().getMaxNrStatesWithSameObservation();
                    }
                    if((options.clippingThresholdInit > 0 || options.useGridClipping) && rewardModelName.is_initialized()) {
                        underApproximation->setExtremeValueBound(valueBounds.extremePomdpValueBound);
                    }
                    buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), false, underApproxHeuristicPar, underApproxBeliefManager, underApproximation);
                    if (!underApproximation->hasComputedValues() || storm::utility::resources::isTerminate()) {
                        return;
                    }
                    ValueType const& newValue = underApproximation->getComputedValueAtInitialState();
                    bool betterBound = min ? result.updateUpperBound(newValue) : result.updateLowerBound(newValue);
                    if (betterBound) {
                        STORM_LOG_INFO("Under-approx result for refinement improved after " << statistics.totalTime << " seconds in refinement step #" << statistics.refinementSteps.get() << ". New value is '" << newValue << "'.\n");
                    }
                }
                
                // Do some output
                STORM_LOG_INFO("Completed iteration #" << statistics.refinementSteps.get() << ". Current checktime is " << statistics.totalTime << ".");
                bool computingLowerBound = false;
                bool computingUpperBound = false;
                if (options.discretize) {
                    STORM_LOG_INFO("\tOver-approx MDP has size " << overApproximation->getExploredMdp()->getNumberOfStates() << ".");
                    (min ? computingLowerBound : computingUpperBound) = true;
                }
                if (options.unfold) {
                    STORM_LOG_INFO("\tUnder-approx MDP has size " << underApproximation->getExploredMdp()->getNumberOfStates() << ".");
                    (min ? computingUpperBound : computingLowerBound) = true;
                }
                if (computingLowerBound && computingUpperBound) {
                    STORM_LOG_INFO("\tCurrent result is [" << result.lowerBound << ", " << result.upperBound << "].");
                } else if (computingLowerBound) {
                    STORM_LOG_INFO("\tCurrent result is ≥" << result.lowerBound << ".");
                } else if (computingUpperBound) {
                    STORM_LOG_INFO("\tCurrent result is ≤" << result.upperBound << ".");
                }
                
                // Start refinement
                STORM_LOG_WARN_COND(options.refineStepLimit.is_initialized() || !storm::utility::isZero(options.refinePrecision), "No termination criterion for refinement given. Consider to specify a steplimit, a non-zero precisionlimit, or a timeout");
                STORM_LOG_WARN_COND(storm::utility::isZero(options.refinePrecision) || (options.unfold && options.discretize), "Refinement goal precision is given, but only one bound is going to be refined.");
                while ((!options.refineStepLimit.is_initialized() || statistics.refinementSteps.get() < options.refineStepLimit.get()) && result.diff() > options.refinePrecision) {
                    bool overApproxFixPoint = true;
                    bool underApproxFixPoint = true;
                    if (options.discretize) {
                        // Refine over-approximation
                        if (min) {
                            overApproximation->takeCurrentValuesAsLowerBounds();
                        } else {
                            overApproximation->takeCurrentValuesAsUpperBounds();
                        }
                        overApproxHeuristicPar.gapThreshold *= options.gapThresholdFactor;
                        overApproxHeuristicPar.sizeThreshold = storm::utility::convertNumber<uint64_t, ValueType>(storm::utility::convertNumber<ValueType, uint64_t>(overApproximation->getExploredMdp()->getNumberOfStates()) * options.sizeThresholdFactor);
                        overApproxHeuristicPar.observationThreshold += options.obsThresholdIncrementFactor * (storm::utility::one<ValueType>() - overApproxHeuristicPar.observationThreshold);
                        overApproxHeuristicPar.optimalChoiceValueEpsilon *= options.optimalChoiceValueThresholdFactor;
                        overApproxFixPoint = buildOverApproximation(targetObservations, min, rewardModelName.is_initialized(), true, overApproxHeuristicPar, observationResolutionVector, overApproxBeliefManager, overApproximation);
                        if (overApproximation->hasComputedValues() && !storm::utility::resources::isTerminate()) {
                            ValueType const& newValue = overApproximation->getComputedValueAtInitialState();
                            bool betterBound = min ? result.updateLowerBound(newValue) : result.updateUpperBound(newValue);
                            if (betterBound) {
                                STORM_LOG_INFO("Over-approx result for refinement improved after " << statistics.totalTime << " in refinement step #" << (statistics.refinementSteps.get() + 1) << ". New value is '" << newValue << "'.");
                            }
                        } else {
                            break;
                        }
                    }

                    if (options.unfold && result.diff() > options.refinePrecision) {
                        // Refine under-approximation
                        underApproxHeuristicPar.gapThreshold *= options.gapThresholdFactor;
                        underApproxHeuristicPar.sizeThreshold = storm::utility::convertNumber<uint64_t, ValueType>(storm::utility::convertNumber<ValueType, uint64_t>(underApproximation->getExploredMdp()->getNumberOfStates()) * options.sizeThresholdFactor);
                        underApproxHeuristicPar.optimalChoiceValueEpsilon *= options.optimalChoiceValueThresholdFactor;
                        underApproxFixPoint = buildUnderApproximation(targetObservations, min, rewardModelName.is_initialized(), true, underApproxHeuristicPar, underApproxBeliefManager, underApproximation);
                        if (underApproximation->hasComputedValues() && !storm::utility::resources::isTerminate()) {
                            ValueType const& newValue = underApproximation->getComputedValueAtInitialState();
                            bool betterBound = min ? result.updateUpperBound(newValue) : result.updateLowerBound(newValue);
                            if (betterBound) {
                                STORM_LOG_INFO("Under-approx result for refinement improved after " << statistics.totalTime << " in refinement step #" << (statistics.refinementSteps.get() + 1) << ". New value is '" << newValue << "'.");
                            }
                        } else {
                            break;
                        }
                    }
                    
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    } else {
                        ++statistics.refinementSteps.get();
                        // Don't make too many outputs (to avoid logfile clutter)
                        if (statistics.refinementSteps.get() <= 1000) {
                            STORM_LOG_INFO("Completed iteration #" << statistics.refinementSteps.get() << ". Current checktime is " << statistics.totalTime << ".");
                            bool computingLowerBound = false;
                            bool computingUpperBound = false;
                            if (options.discretize) {
                                STORM_LOG_INFO("\tOver-approx MDP has size " << overApproximation->getExploredMdp()->getNumberOfStates() << ".");
                                (min ? computingLowerBound : computingUpperBound) = true;
                            }
                            if (options.unfold) {
                                STORM_LOG_INFO("\tUnder-approx MDP has size " << underApproximation->getExploredMdp()->getNumberOfStates() << ".");
                                (min ? computingUpperBound : computingLowerBound) = true;
                            }
                            if (computingLowerBound && computingUpperBound) {
                                STORM_LOG_INFO("\tCurrent result is [" << result.lowerBound << ", " << result.upperBound << "].");
                            } else if (computingLowerBound) {
                                STORM_LOG_INFO("\tCurrent result is ≥" << result.lowerBound << ".");
                            } else if (computingUpperBound) {
                                STORM_LOG_INFO("\tCurrent result is ≤" << result.upperBound << ".");
                            }
                            STORM_LOG_WARN_COND(statistics.refinementSteps.get() < 1000, "Refinement requires  more than 1000 iterations.");
                        }
                    }
                    if (overApproxFixPoint && underApproxFixPoint) {
                        STORM_LOG_INFO("Refinement fixpoint reached after " << statistics.refinementSteps.get() << " iterations.\n");
                        statistics.refinementFixpointDetected = true;
                        break;
                    }
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            BeliefValueType BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::rateObservation(typename ExplorerType::SuccessorObservationInformation const& info, BeliefValueType const& observationResolution, BeliefValueType const& maxResolution) {
                auto n = storm::utility::convertNumber<BeliefValueType, uint64_t>(info.support.size());
                auto one = storm::utility::one<BeliefValueType>();
                if (storm::utility::isOne(n)) {
                    // If the belief is Dirac, it has to be approximated precisely.
                    // In this case, we return the best possible rating
                    return one;
                } else {
                    // Create the rating for this observation at this choice from the given info
                    BeliefValueType obsChoiceRating = storm::utility::convertNumber<BeliefValueType, ValueType>(info.maxProbabilityToSuccessorWithObs / info.observationProbability);
                    // At this point, obsRating is the largest triangulation weight (which ranges from 1/n to 1
                    // Normalize the rating so that it ranges from 0 to 1, where
                    // 0 means that the actual belief lies in the middle of the triangulating simplex (i.e. a "bad" approximation) and 1 means that the belief is precisely approximated.
                    obsChoiceRating = (obsChoiceRating * n - one) / (n - one);
                    // Scale the ratings with the resolutions, so that low resolutions get a lower rating (and are thus more likely to be refined)
                    obsChoiceRating *= observationResolution / maxResolution;
                    return obsChoiceRating;
                }
            }
            
            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            std::vector<BeliefValueType> BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::getObservationRatings(std::shared_ptr<ExplorerType> const& overApproximation, std::vector<BeliefValueType> const& observationResolutionVector) {
                uint64_t numMdpStates = overApproximation->getExploredMdp()->getNumberOfStates();
                auto const& choiceIndices = overApproximation->getExploredMdp()->getNondeterministicChoiceIndices();
                BeliefValueType maxResolution = *std::max_element(observationResolutionVector.begin(), observationResolutionVector.end());

                std::vector<BeliefValueType> resultingRatings(pomdp().getNrObservations(), storm::utility::one<BeliefValueType>());
                
                std::map<uint32_t, typename ExplorerType::SuccessorObservationInformation> gatheredSuccessorObservations; // Declare here to avoid reallocations
                for (uint64_t mdpState = 0; mdpState < numMdpStates; ++mdpState) {
                    // Check whether this state is reached under an optimal scheduler.
                    // The heuristic assumes that the remaining states are not relevant for the observation score.
                    if (overApproximation->stateIsOptimalSchedulerReachable(mdpState)) {
                        for (uint64_t mdpChoice = choiceIndices[mdpState]; mdpChoice < choiceIndices[mdpState + 1]; ++mdpChoice) {
                            // Similarly, only optimal actions are relevant
                            if (overApproximation->actionIsOptimal(mdpChoice)) {
                                // score the observations for this choice
                                gatheredSuccessorObservations.clear();
                                overApproximation->gatherSuccessorObservationInformationAtMdpChoice(mdpChoice, gatheredSuccessorObservations);
                                for (auto const& obsInfo : gatheredSuccessorObservations) {
                                    auto const& obs = obsInfo.first;
                                    BeliefValueType obsChoiceRating = rateObservation(obsInfo.second, observationResolutionVector[obs], maxResolution);

                                    // The rating of the observation will be the minimum over all choice-based observation ratings
                                    resultingRatings[obs] = std::min(resultingRatings[obs], obsChoiceRating);
                                }
                            }
                        }
                    }
                }
                return resultingRatings;
            }
            
            template<typename ValueType>
            ValueType getGap(ValueType const& l, ValueType const& u) {
                STORM_LOG_ASSERT(l >= storm::utility::zero<ValueType>() && u >= storm::utility::zero<ValueType>(), "Gap computation currently does not handle negative values.");
                if (storm::utility::isInfinity(u)) {
                    if (storm::utility::isInfinity(l)) {
                        return storm::utility::zero<ValueType>();
                    } else {
                        return u;
                    }
                } else if (storm::utility::isZero(u)) {
                    STORM_LOG_ASSERT(storm::utility::isZero(l), "Upper bound is zero but lower bound is " << l << ".");
                    return u;
                } else {
                    STORM_LOG_ASSERT(!storm::utility::isInfinity(l), "Lower bound is infinity, but upper bound is " << u << ".");
                    // get the relative gap
                    return storm::utility::abs<ValueType>(u-l) * storm::utility::convertNumber<ValueType, uint64_t>(2) / (l+u);
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            bool BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::buildOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, HeuristicParameters const& heuristicParameters, std::vector<BeliefValueType>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& overApproximation) {

                // Detect whether the refinement reached a fixpoint.
                bool fixPoint = true;

                statistics.overApproximationBuildTime.start();
                storm::storage::BitVector refinedObservations;
                if (!refine) {
                    // If we build the model from scratch, we first have to set up the explorer for the overApproximation.
                    if (computeRewards) {
                        overApproximation->startNewExploration(storm::utility::zero<ValueType>());
                    } else {
                        overApproximation->startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                    }
                } else {
                    // If we refine the existing overApproximation, our heuristic also wants to know which states are reachable under an optimal policy
                    overApproximation->computeOptimalChoicesAndReachableMdpStates(heuristicParameters.optimalChoiceValueEpsilon, true);
                    // We also need to find out which observation resolutions needs refinement.
                    // current maximal resolution (needed for refinement heuristic)
                    auto obsRatings = getObservationRatings(overApproximation, observationResolutionVector);
                    // If there is a score < 1, we have not reached a fixpoint, yet
                    auto numericPrecision = storm::utility::convertNumber<BeliefValueType>(options.numericPrecision);
                    if (std::any_of(obsRatings.begin(), obsRatings.end(), [&numericPrecision](BeliefValueType const& value){return value + numericPrecision < storm::utility::one<BeliefValueType>();})) {
                        STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because there are still observations to refine.");
                        fixPoint = false;
                    }
                    refinedObservations = storm::utility::vector::filter<BeliefValueType>(obsRatings, [&heuristicParameters](BeliefValueType const& r) { return r <= storm::utility::convertNumber<BeliefValueType>(heuristicParameters.observationThreshold);});
                    STORM_LOG_DEBUG("Refining the resolution of " << refinedObservations.getNumberOfSetBits() << "/" << refinedObservations.size() << " observations.");
                    for (auto const& obs : refinedObservations) {
                        // Increment the resolution at the refined observations.
                        // Use storm's rational number to detect overflows properly.
                        storm::RationalNumber newObsResolutionAsRational = storm::utility::convertNumber<storm::RationalNumber>(observationResolutionVector[obs]) * storm::utility::convertNumber<storm::RationalNumber>(options.resolutionFactor);
                        static_assert(storm::NumberTraits<BeliefValueType>::IsExact || std::is_same<BeliefValueType, double>::value, "Unhandled belief value type");
                        if (!storm::NumberTraits<BeliefValueType>::IsExact && newObsResolutionAsRational > storm::utility::convertNumber<storm::RationalNumber>(std::numeric_limits<double>::max())) {
                            observationResolutionVector[obs] = storm::utility::convertNumber<BeliefValueType>(std::numeric_limits<double>::max());
                        } else {
                            observationResolutionVector[obs] = storm::utility::convertNumber<BeliefValueType>(newObsResolutionAsRational);
                        }
                    }
                    overApproximation->restartExploration();
                }
                statistics.overApproximationMaxResolution = storm::utility::ceil(*std::max_element(observationResolutionVector.begin(), observationResolutionVector.end()));

                // Start exploration
                storm::utility::Stopwatch explorationTime;
                if (options.explorationTimeLimit) {
                    explorationTime.start();
                }
                bool timeLimitExceeded = false;
                std::map<uint32_t, typename ExplorerType::SuccessorObservationInformation> gatheredSuccessorObservations; // Declare here to avoid reallocations
                uint64_t numRewiredOrExploredStates = 0;
                while (overApproximation->hasUnexploredState()) {
                    if (!timeLimitExceeded && options.explorationTimeLimit && static_cast<uint64_t>(explorationTime.getTimeInSeconds()) > options.explorationTimeLimit.get()) {
                        STORM_LOG_INFO("Exploration time limit exceeded.");
                        timeLimitExceeded = true;
                        STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because the exploration time limit is exceeded.");
                        fixPoint = false;
                    }

                    uint64_t currId = overApproximation->exploreNextState();
                    bool hasOldBehavior = refine && overApproximation->currentStateHasOldBehavior();
                    if (!hasOldBehavior) {
                        STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because a new state is explored");
                        fixPoint = false; // Exploring a new state!
                    }
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    if (targetObservations.count(currObservation) != 0) {
                        overApproximation->setCurrentStateIsTarget();
                        overApproximation->addSelfloopTransition();
                    } else {
                        // We need to decide how to treat this state (and each individual enabled action). There are the following cases:
                        // 1 The state has no old behavior and
                        //   1.1 we explore all actions or
                        //   1.2 we truncate all actions
                        // 2 The state has old behavior and was truncated in the last iteration and
                        //   2.1 we explore all actions or
                        //   2.2 we truncate all actions (essentially restoring old behavior, but we do the truncation step again to benefit from updated bounds)
                        // 3 The state has old behavior and was not truncated in the last iteration and the current action
                        //   3.1 should be rewired or
                        //   3.2 should get the old behavior but either
                        //       3.2.1 none of the successor observation has been refined since the last rewiring or exploration of this action
                        //       3.2.2 rewiring is only delayed as it could still have an effect in a later refinement step

                        // Find out in which case we are
                        bool exploreAllActions = false;
                        bool truncateAllActions = false;
                        bool restoreAllActions = false;
                        bool checkRewireForAllActions = false;
                        // Get the relative gap
                        ValueType gap = getGap(overApproximation->getLowerValueBoundAtCurrentState(), overApproximation->getUpperValueBoundAtCurrentState());
                        if (!hasOldBehavior) {
                            // Case 1
                            // If we explore this state and if it has no old behavior, it is clear that an "old" optimal scheduler can be extended to a scheduler that reaches this state
                            if (!timeLimitExceeded && gap > heuristicParameters.gapThreshold && numRewiredOrExploredStates < heuristicParameters.sizeThreshold) {
                                exploreAllActions = true; // Case 1.1
                            } else {
                                truncateAllActions = true; // Case 1.2
                                overApproximation->setCurrentStateIsTruncated();
                            }
                        } else {
                            if (overApproximation->getCurrentStateWasTruncated()) {
                                // Case 2
                                if (!timeLimitExceeded && overApproximation->currentStateIsOptimalSchedulerReachable() && gap > heuristicParameters.gapThreshold && numRewiredOrExploredStates < heuristicParameters.sizeThreshold) {
                                    exploreAllActions = true; // Case 2.1
                                    STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because a previously truncated state is now explored.");
                                    fixPoint = false;
                                } else {
                                    truncateAllActions = true; // Case 2.2
                                    overApproximation->setCurrentStateIsTruncated();
                                    if (fixPoint) {
                                        // Properly check whether this can still be a fixpoint
                                        if (overApproximation->currentStateIsOptimalSchedulerReachable() && !storm::utility::isZero(gap)) {
                                            STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because we truncate a state with non-zero gap " << gap << " that is reachable via an optimal sched.");
                                            fixPoint = false;
                                        }
                                        //} else {
                                        // In this case we truncated a state that is not reachable under optimal schedulers.
                                        // If no other state is explored (i.e. fixPoint remains true), these states should still not be reachable in subsequent iterations
                                    }
                                }
                            } else {
                                // Case 3
                                // The decision for rewiring also depends on the corresponding action, but we have some criteria that lead to case 3.2 (independent of the action)
                                if (!timeLimitExceeded && overApproximation->currentStateIsOptimalSchedulerReachable() && gap > heuristicParameters.gapThreshold && numRewiredOrExploredStates < heuristicParameters.sizeThreshold) {
                                    checkRewireForAllActions = true; // Case 3.1 or Case 3.2
                                } else {
                                    restoreAllActions = true; // Definitely Case 3.2
                                    // We still need to check for each action whether rewiring makes sense later
                                    checkRewireForAllActions = true;
                                }
                            }
                        }
                        bool expandedAtLeastOneAction = false;
                        for (uint64_t action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                            bool expandCurrentAction = exploreAllActions || truncateAllActions;
                            if (checkRewireForAllActions) {
                                assert(refine);
                                // In this case, we still need to check whether this action needs to be expanded
                                assert(!expandCurrentAction);
                                // Check the action dependent conditions for rewiring
                                // First, check whether this action has been rewired since the last refinement of one of the successor observations (i.e. whether rewiring would actually change the successor states)
                                assert(overApproximation->currentStateHasOldBehavior());
                                if (overApproximation->getCurrentStateActionExplorationWasDelayed(action) || overApproximation->currentStateHasSuccessorObservationInObservationSet(action, refinedObservations)) {
                                    // Then, check whether the other criteria for rewiring are satisfied
                                    if (!restoreAllActions && overApproximation->actionAtCurrentStateWasOptimal(action)) {
                                        // Do the rewiring now! (Case 3.1)
                                        expandCurrentAction = true;
                                        STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because we rewire a state.");
                                        fixPoint = false;
                                    } else {
                                        // Delay the rewiring (Case 3.2.2)
                                        overApproximation->setCurrentChoiceIsDelayed(action);
                                        if (fixPoint) {
                                            // Check whether this delay means that a fixpoint has not been reached
                                            if (!overApproximation->getCurrentStateActionExplorationWasDelayed(action) || (overApproximation->currentStateIsOptimalSchedulerReachable() && overApproximation->actionAtCurrentStateWasOptimal(action) && !storm::utility::isZero(gap))) {
                                                STORM_LOG_INFO_COND(!fixPoint, "Not reaching a refinement fixpoint because we delay a rewiring of a state with non-zero gap " << gap << " that is reachable via an optimal scheduler.");
                                                fixPoint = false;
                                            }
                                        }
                                    }
                                } // else { Case 3.2.1 }
                            }

                            if (expandCurrentAction) {
                                expandedAtLeastOneAction = true;
                                if (!truncateAllActions) {
                                    // Cases 1.1, 2.1, or 3.1
                                    auto successorGridPoints = beliefManager->expandAndTriangulate(currId, action, observationResolutionVector);
                                    for (auto const& successor : successorGridPoints) {
                                        overApproximation->addTransitionToBelief(action, successor.first, successor.second, false);
                                    }
                                    if (computeRewards) {
                                        overApproximation->computeRewardAtCurrentState(action);
                                    }
                                } else {
                                    // Cases 1.2 or 2.2
                                    ValueType truncationProbability = storm::utility::zero<ValueType>();
                                    ValueType truncationValueBound = storm::utility::zero<ValueType>();
                                    auto successorGridPoints = beliefManager->expandAndTriangulate(currId, action, observationResolutionVector);
                                    for (auto const& successor : successorGridPoints) {
                                        bool added = overApproximation->addTransitionToBelief(action, successor.first, successor.second, true);
                                        if (!added) {
                                            // We did not explore this successor state. Get a bound on the "missing" value
                                            truncationProbability += successor.second;
                                            truncationValueBound += successor.second * (min ? overApproximation->computeLowerValueBoundAtBelief(successor.first) : overApproximation->computeUpperValueBoundAtBelief(successor.first));
                                        }
                                    }
                                    if (computeRewards) {
                                        // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                        overApproximation->addTransitionsToExtraStates(action, truncationProbability);
                                        overApproximation->computeRewardAtCurrentState(action, truncationValueBound);
                                    } else {
                                        overApproximation->addTransitionsToExtraStates(action, truncationValueBound, truncationProbability - truncationValueBound);
                                    }
                                }
                            } else {
                                // Case 3.2
                                overApproximation->restoreOldBehaviorAtCurrentState(action);
                            }
                        }
                        if (expandedAtLeastOneAction) {
                            ++numRewiredOrExploredStates;
                        }
                    }

                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                }

                if (storm::utility::resources::isTerminate()) {
                    // don't overwrite statistics of a previous, successful computation
                    if (!statistics.overApproximationStates) {
                        statistics.overApproximationBuildAborted = true;
                        statistics.overApproximationStates = overApproximation->getCurrentNumberOfMdpStates();
                    }
                    statistics.overApproximationBuildTime.stop();
                    return false;
                }

                overApproximation->finishExploration();
                statistics.overApproximationBuildTime.stop();

                statistics.overApproximationCheckTime.start();
                overApproximation->computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.overApproximationCheckTime.stop();

                // don't overwrite statistics of a previous, successful computation
                if (!storm::utility::resources::isTerminate() || !statistics.overApproximationStates) {
                    statistics.overApproximationStates = overApproximation->getExploredMdp()->getNumberOfStates();
                }
                return fixPoint;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            bool BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::buildUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, HeuristicParameters const& heuristicParameters, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& underApproximation) {
                bool useBeliefClipping = heuristicParameters.clippingThreshold > storm::utility::zero<ValueType>() || options.useGridClipping;
                statistics.underApproximationBuildTime.start();
                if(useBeliefClipping){
                    if(options.useGridClipping){
                        STORM_PRINT_AND_LOG("Use Belief Clipping with grid beliefs \n")
                    } else {
                        STORM_PRINT_AND_LOG("Use Belief Clipping with threshold "
                                                    << storm::utility::to_string(heuristicParameters.clippingThreshold)
                                                    << "\n");
                    }
                    statistics.nrClippingAttempts = 0;
                    statistics.nrClippedStates = 0;
                    if(options.disableClippingReduction){
                        STORM_PRINT_AND_LOG("Disable clipping candidate set reduction \n");
                    }
                }

                uint64_t nrCutoffStrategies = min ? underApproximation->getNrSchedulersForUpperBounds() : underApproximation->getNrSchedulersForLowerBounds();

                bool fixPoint = true;
                if (heuristicParameters.sizeThreshold != std::numeric_limits<uint64_t>::max()) {
                    statistics.underApproximationStateLimit = heuristicParameters.sizeThreshold;
                }
                if (!refine) {
                    // Build a new under approximation
                    if (computeRewards) {
                        if(useBeliefClipping){
                            // If we clip, use the sink state for infinite correction values
                            underApproximation->startNewExploration(storm::utility::zero<ValueType>(), storm::utility::infinity<ValueType>());
                        } else {
                            underApproximation->startNewExploration(storm::utility::zero<ValueType>());
                        }
                    } else {
                        underApproximation->startNewExploration(storm::utility::one<ValueType>(), storm::utility::zero<ValueType>());
                    }
                } else {
                    // Restart the building process
                    underApproximation->restartExploration();
                }

                // Expand the beliefs
                storm::utility::Stopwatch explorationTime;
                storm::utility::Stopwatch printUpdateStopwatch;
                printUpdateStopwatch.start();
                if (options.explorationTimeLimit) {
                    explorationTime.start();
                }
                bool timeLimitExceeded = false;
                while (underApproximation->hasUnexploredState()) {
                    if (!timeLimitExceeded && options.explorationTimeLimit && static_cast<uint64_t>(explorationTime.getTimeInSeconds()) > options.explorationTimeLimit.get()) {
                        STORM_LOG_INFO("Exploration time limit exceeded.");
                        timeLimitExceeded = true;
                    }
                    if (printUpdateStopwatch.getTimeInSeconds() >= 60) {
                        printUpdateStopwatch.restart();
                        STORM_PRINT_AND_LOG("### " << underApproximation->getCurrentNumberOfMdpStates() << " beliefs in underapproximation MDP" << " ##### " << underApproximation->getUnexploredStates().size() << " beliefs queued\n")
                        if(underApproximation->getCurrentNumberOfMdpStates() > heuristicParameters.sizeThreshold && useBeliefClipping){
                            STORM_PRINT_AND_LOG("##### Clipping Attempts: " << statistics.nrClippingAttempts.get() << " ##### " << "Clipped States: " << statistics.nrClippedStates.get() << "\n");
                        }
                    }

                    uint64_t currId = underApproximation->exploreNextState();
                    uint32_t currObservation = beliefManager->getBeliefObservation(currId);
                    uint64_t addedActions = 0;
                    bool stateAlreadyExplored = refine && underApproximation->currentStateHasOldBehavior() && !underApproximation->getCurrentStateWasTruncated();
                    if (!stateAlreadyExplored || timeLimitExceeded) {
                        fixPoint = false;
                    }
                    if (targetObservations.count(beliefManager->getBeliefObservation(currId)) != 0) {
                        underApproximation->setCurrentStateIsTarget();
                        underApproximation->addSelfloopTransition();
                        underApproximation->addChoiceLabelToCurrentState(0, "loop");
                    } else {
                        bool stopExploration = false;
                        bool clipBelief = false;
                        if (timeLimitExceeded) {
                            clipBelief = useBeliefClipping;
                            stopExploration = true;
                            underApproximation->setCurrentStateIsTruncated();
                        } else if (!stateAlreadyExplored) {
                            // Check whether we want to explore the state now!
                            ValueType gap =
                                getGap(underApproximation->getLowerValueBoundAtCurrentState(), underApproximation->getUpperValueBoundAtCurrentState());
                            if ((gap < heuristicParameters.gapThreshold) || (gap == 0 && options.cutZeroGap)) {
                                stopExploration = true;
                                underApproximation->setCurrentStateIsTruncated();
                            } else if (underApproximation->getCurrentNumberOfMdpStates() >=
                                       heuristicParameters.sizeThreshold /*&& !statistics.beliefMdpDetectedToBeFinite*/) {
                                clipBelief = useBeliefClipping;
                                stopExploration = true;
                                underApproximation->setCurrentStateIsTruncated();
                            }
                        }

                        if (clipBelief) {
                            if (options.useGridClipping) {
                                // Use a belief grid as clipping candidates
                                clipToGrid(currId, computeRewards, min, beliefManager, underApproximation);
                                addedActions += beliefManager->getBeliefNumberOfChoices(currId);
                            } else {
                                // Use clipping with explored beliefs as candidates ("classic" clipping)
                                if (clipToExploredBeliefs(currId, storm::utility::convertNumber<BeliefValueType>(heuristicParameters.clippingThreshold),
                                                          computeRewards, 10, beliefManager, underApproximation)) {
                                    ++addedActions;
                                }
                            }
                        }  // end Clipping Procedure
                        if(!options.useExplicitCutoff || !stopExploration){
                            // Add successor transitions or cut-off transitions when exploration is stopped
                            for (uint64_t action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                                // Always restore old behavior if available
                                if(pomdp().hasChoiceLabeling()){
                                    if(pomdp().getChoiceLabeling().getLabelsOfChoice(beliefManager->getRepresentativeState(currId)+action).size() > 0) {
                                        auto rowIndex = pomdp().getTransitionMatrix().getRowGroupIndices()[beliefManager->getRepresentativeState(currId)];
                                        underApproximation->addChoiceLabelToCurrentState(
                                            addedActions + action,*(pomdp().getChoiceLabeling().getLabelsOfChoice(rowIndex+action).begin()));
                                    }
                                }
                                if (stateAlreadyExplored) {
                                    underApproximation->restoreOldBehaviorAtCurrentState(action);
                                } else {
                                    auto truncationProbability = storm::utility::zero<ValueType>();
                                    auto truncationValueBound = storm::utility::zero<ValueType>();
                                    auto successors = beliefManager->expand(currId, action);
                                    for (auto const& successor : successors) {
                                        bool added = underApproximation->addTransitionToBelief(addedActions + action, successor.first, successor.second,
                                                                                               stopExploration);
                                        if (!added) {
                                            STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                                            // We did not explore this successor state. Get a bound on the "missing" value
                                            truncationProbability += successor.second;
                                            // Some care has to be taken here: Essentially, we are triangulating a value for the under-approximation out of other under-approximation values. In general, this does not yield a sound underapproximation anymore. However, in our case this is still the case as the under-approximation values are based on a memoryless scheduler.
                                            truncationValueBound +=
                                                successor.second * (min ? underApproximation->computeUpperValueBoundAtBelief(successor.first)
                                                                        : underApproximation->computeLowerValueBoundAtBelief(successor.first));
                                        }
                                    }
                                    if (stopExploration) {
                                        if (computeRewards) {
                                            underApproximation->addTransitionsToExtraStates(addedActions + action, truncationProbability);
                                        } else {
                                            underApproximation->addTransitionsToExtraStates(addedActions + action, truncationValueBound,
                                                                                            truncationProbability - truncationValueBound);
                                        }
                                    }
                                    if (computeRewards) {
                                        // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                                        if (!clipBelief) {
                                            underApproximation->computeRewardAtCurrentState(action, truncationValueBound);
                                        } else {
                                            underApproximation->addRewardToCurrentState(
                                                addedActions + action, beliefManager->getBeliefActionReward(currId, action) + truncationValueBound);
                                        }
                                    }
                                }
                            }
                        } else {
                            for (uint64_t i = 0; i < nrCutoffStrategies; ++i) {
                                auto cutOffValue = min ? underApproximation->computeUpperValueBoundForScheduler(currId, i) : underApproximation->computeLowerValueBoundForScheduler(currId, i) ;
                                if (computeRewards) {
                                    underApproximation->addTransitionsToExtraStates(i, storm::utility::one<ValueType>());
                                    underApproximation->addRewardToCurrentState(i, cutOffValue);
                                } else {
                                    underApproximation->addTransitionsToExtraStates(i, cutOffValue,storm::utility::one<ValueType>() - cutOffValue);
                                }
                                if(pomdp().hasChoiceLabeling()){
                                    underApproximation->addChoiceLabelToCurrentState(i, "sched_" + std::to_string(i));
                                }
                            }
                        }
                    }
                    if (storm::utility::resources::isTerminate()) {
                        break;
                    }
                }

                if (storm::utility::resources::isTerminate()) {
                    // don't overwrite statistics of a previous, successful computation
                    if (!statistics.underApproximationStates) {
                        statistics.underApproximationBuildAborted = true;
                        statistics.underApproximationStates = underApproximation->getCurrentNumberOfMdpStates();
                    }
                    statistics.underApproximationBuildTime.stop();
                    return false;
                }

                underApproximation->finishExploration();
                statistics.underApproximationBuildTime.stop();
                printUpdateStopwatch.stop();
                STORM_PRINT_AND_LOG("Finished exploring Underapproximation MDP.\n Start analysis...\n");
                statistics.underApproximationCheckTime.start();
                underApproximation->computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
                statistics.underApproximationCheckTime.stop();
                if (underApproximation->getExploredMdp()->getStateLabeling().getStates("truncated").getNumberOfSetBits() > 0) {
                    statistics.nrTruncatedStates = underApproximation->getExploredMdp()->getStateLabeling().getStates("truncated").getNumberOfSetBits();
                }
                // don't overwrite statistics of a previous, successful computation
                if (!storm::utility::resources::isTerminate() || !statistics.underApproximationStates) {
                    statistics.underApproximationStates = underApproximation->getExploredMdp()->getNumberOfStates();
                }
                return fixPoint;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            bool
            BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::clipToExploredBeliefs(uint64_t clippingStateId, BeliefValueType threshold, bool computeRewards, uint64_t reducedCandidateSetSize, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer) {// Preprocess suitable candidate beliefs
                std::priority_queue<std::pair<BeliefValueType, uint64_t>, std::vector<std::pair<BeliefValueType, uint64_t>>, std::less<>> restrictedCandidates;
                std::vector<uint64_t> candidates(reducedCandidateSetSize);
                statistics.clippingPreTime.start();
                // If the reduction is not disabled, we only pick the nrCandidate candidates with the smallest 1-norm of the difference between the candidate and the clipping belief
                if(!options.disableClippingReduction) {
                    for (auto const &candidateBelief : beliefExplorer->getBeliefsWithObservationInMdp(beliefManager->getBeliefObservation(clippingStateId))) {
                        if (!beliefManager->isEqual(candidateBelief, clippingStateId)) {
                            if (restrictedCandidates.size() < reducedCandidateSetSize) {
                                restrictedCandidates.push(std::make_pair(beliefManager->computeDifference1norm(candidateBelief, clippingStateId), candidateBelief));
                            } else {
                                auto currentWorst = restrictedCandidates.top().first;
                                if (currentWorst > beliefManager->computeDifference1norm(candidateBelief, clippingStateId)) {
                                    restrictedCandidates.pop();
                                    restrictedCandidates.push(std::make_pair(beliefManager->computeDifference1norm(candidateBelief, clippingStateId), candidateBelief));
                                }
                            }
                        }
                    }

                    while (!restrictedCandidates.empty()) {
                        candidates.push_back(restrictedCandidates.top().second);
                        restrictedCandidates.pop();
                    }
                }
                statistics.clippingPreTime.stop();
                // Belief is to be clipped, find the best candidate from the restricted list
                statistics.nrClippingAttempts = statistics.nrClippingAttempts.get() + 1;
                statistics.clipWatch.start();
                auto clippingResult = beliefManager->clipBelief(clippingStateId, threshold, options.disableClippingReduction ? beliefExplorer->getBeliefsInMdp() : candidates);
                statistics.clipWatch.stop();
                if (clippingResult.isClippable) {
                    // An adequate clipping candidate has been found, add the transitions
                    beliefExplorer->setCurrentStateIsClipped();
                    statistics.nrClippedStates = statistics.nrClippedStates.get() + 1;
                    BeliefValueType transitionProb = utility::one<BeliefValueType>() - clippingResult.delta;
                    bool addedSucc = beliefExplorer->addTransitionToBelief(0, clippingResult.targetBelief, utility::convertNumber<BeliefMDPType>(transitionProb) , true);
                    if (computeRewards) {
                        //Determine a sound reward bound for the transition to the target state
                        //Compute an over-/underapproximation for the original belief
                        auto rewardBound = utility::zero<BeliefValueType>();
                        for (auto const &deltaValue : clippingResult.deltaValues) {
                            if(valueTypeCC.isEqual(beliefExplorer->getExtremeValueBoundAtPOMDPState(deltaValue.first), utility::infinity<BeliefMDPType>())){
                                rewardBound = utility::infinity<BeliefValueType>();
                                break;
                            } else {
                                rewardBound += deltaValue.second * utility::convertNumber<BeliefValueType>(beliefExplorer->getExtremeValueBoundAtPOMDPState(deltaValue.first));
                            }
                        }
                        if(beliefTypeCC.isEqual(rewardBound, utility::infinity<BeliefValueType>())){
                            beliefExplorer->addTransitionsToExtraStates(0, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(clippingResult.delta));
                        } else {
                            rewardBound /= clippingResult.delta;

                            beliefExplorer->addTransitionsToExtraStates(0, utility::convertNumber<BeliefMDPType>(clippingResult.delta));
                            beliefExplorer->addClippingRewardToCurrentState(0, utility::convertNumber<BeliefMDPType>(rewardBound));
                        }
                        beliefExplorer->addRewardToCurrentState(0, utility::zero<BeliefMDPType>());
                    } else {
                        beliefExplorer->addTransitionsToExtraStates(0, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(clippingResult.delta));
                    }
                } // end isClippable
                return clippingResult.isClippable;
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::clipToGrid(uint64_t clippingStateId, bool computeRewards, bool min, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer) {
                // Add all transitions to states which are already in the MDP, clip all others to a grid
                // To make the resulting MDP smaller, we eliminate intermediate successor states when clipping is applied
                for (uint64_t action = 0, numActions = beliefManager->getBeliefNumberOfChoices(clippingStateId); action < numActions; ++action) {
                    auto rewardBound = utility::zero<BeliefValueType>();
                    auto successors = beliefManager->expand(clippingStateId, action);
                    auto absDelta = utility::zero<BeliefValueType>();
                    for (auto const &successor : successors) {
                        // Add transition if successor is in explored space.
                        // We can directly add the transitions as there is at most one successor for each observation
                        // Therefore no belief can be clipped to an already added successor
                        bool added = beliefExplorer->addTransitionToBelief(action, successor.first, successor.second, true);
                        if(!added){
                            // The successor is not in the explored space. Clip it
                            statistics.nrClippingAttempts = statistics.nrClippingAttempts.get() + 1;
                            auto clipping = beliefManager->clipBeliefToGrid(successor.first, options.clippingGridRes,
                                                                            beliefExplorer->getStateExtremeBoundIsInfinite());
                            if (clipping.isClippable) {
                                // The belief is not on the grid and there is a candidate with finite reward
                                statistics.nrClippedStates = statistics.nrClippedStates.get() + 1;
                                // Transition probability to candidate is (probability to successor) * (clipping transition probability)
                                BeliefValueType transitionProb = (utility::one<BeliefValueType>() - clipping.delta) * utility::convertNumber<BeliefValueType>(successor.second);
                                beliefExplorer->addTransitionToBelief(action, clipping.targetBelief, utility::convertNumber<BeliefMDPType>(transitionProb), false);
                                // Collect weighted clipping values
                                absDelta += clipping.delta * utility::convertNumber<BeliefValueType>(successor.second);
                                if (computeRewards) {
                                    // collect cumulative reward bounds
                                    auto localRew = utility::zero<BeliefValueType>();
                                    for (auto const &deltaValue : clipping.deltaValues) {
                                        localRew += deltaValue.second * utility::convertNumber<BeliefValueType>((beliefExplorer->getExtremeValueBoundAtPOMDPState(deltaValue.first)));
                                    }
                                    if(localRew == utility::infinity<ValueType>()){
                                        STORM_LOG_WARN("Infinite reward in clipping!");
                                    }
                                    rewardBound += localRew * utility::convertNumber<BeliefValueType>(successor.second);
                                }
                            } else if(clipping.onGrid){
                                // If the belief is not clippable, but on the grid, it may need to be explored, too
                                bool inserted = beliefExplorer->addTransitionToBelief(action, successor.first, successor.second, false);
                            } else {
                                // Otherwise, the reward for all candidates is infinite, clipping does not make sense. Cut it off instead
                                absDelta += utility::convertNumber<BeliefValueType>(successor.second);
                                rewardBound += utility::convertNumber<BeliefValueType>(successor.second) * utility::convertNumber<BeliefValueType>(min ? beliefExplorer->computeUpperValueBoundAtBelief(successor.first)
                                                                                                                   : beliefExplorer->computeLowerValueBoundAtBelief(successor.first));
                            }
                        }
                    }
                    // Add the collected clipping transition if necessary
                    if (absDelta != utility::zero<BeliefValueType>()) {
                        if (computeRewards) {
                                if(rewardBound == utility::infinity<BeliefValueType>()){
                                    // If the reward is infinite, add a transition to the sink state to collect infinite reward
                                    beliefExplorer->addTransitionsToExtraStates(action, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(absDelta));
                                } else {
                                    beliefExplorer->addTransitionsToExtraStates(action, utility::convertNumber<BeliefMDPType>(absDelta));
                                    BeliefValueType totalRewardVal = rewardBound / absDelta;
                                    beliefExplorer->addClippingRewardToCurrentState(action, utility::convertNumber<BeliefMDPType>(totalRewardVal));
                                }
                        } else {
                            beliefExplorer->addTransitionsToExtraStates(action, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(absDelta));
                        }
                    }
                    if(computeRewards){
                        beliefExplorer->computeRewardAtCurrentState(action);
                    }
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            void BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::clipToGridExplicitly(uint64_t clippingStateId, bool computeRewards, bool min, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer, uint64_t localActionIndex) {
                statistics.nrClippingAttempts = statistics.nrClippingAttempts.get() + 1;
                auto clipping = beliefManager->clipBeliefToGrid(clippingStateId, options.clippingGridRes,
                                                                beliefExplorer->getStateExtremeBoundIsInfinite());
                if (clipping.isClippable) {
                    // The belief is not on the grid and there is a candidate with finite reward
                    statistics.nrClippedStates = statistics.nrClippedStates.get() + 1;
                    // Transition probability to candidate is clipping value
                    BeliefValueType transitionProb = (utility::one<BeliefValueType>() - clipping.delta);
                    bool addedCandidate = beliefExplorer->addTransitionToBelief(localActionIndex, clipping.targetBelief, utility::convertNumber<BeliefMDPType>(transitionProb), false);
                    beliefExplorer->markAsGridBelief(clipping.targetBelief);
                    if (computeRewards) {
                        // collect cumulative reward bounds
                        auto reward = utility::zero<BeliefValueType>();
                        for (auto const &deltaValue : clipping.deltaValues) {
                            reward += deltaValue.second * utility::convertNumber<BeliefValueType>((beliefExplorer->getExtremeValueBoundAtPOMDPState(deltaValue.first)));
                        }
                        if(reward == utility::infinity<ValueType>()){
                            STORM_LOG_WARN("Infinite reward in clipping!");
                            // If the reward is infinite, add a transition to the sink state to collect infinite reward in our semantics
                            beliefExplorer->addTransitionsToExtraStates(localActionIndex, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(clipping.delta));
                        } else {
                            beliefExplorer->addTransitionsToExtraStates(localActionIndex, utility::convertNumber<BeliefMDPType>(clipping.delta));
                            BeliefValueType totalRewardVal = reward / clipping.delta;
                            beliefExplorer->addClippingRewardToCurrentState(localActionIndex, utility::convertNumber<BeliefMDPType>(totalRewardVal));
                        }
                    } else {
                        beliefExplorer->addTransitionsToExtraStates(localActionIndex, utility::zero<BeliefMDPType>(), utility::convertNumber<BeliefMDPType>(clipping.delta));
                    }
                    beliefExplorer->addChoiceLabelToCurrentState(localActionIndex, "clip");
                } else {
                    if(clipping.onGrid){
                        // If the belief is not clippable, but on the grid, it may need to be explored, too
                        beliefExplorer->markAsGridBelief(clippingStateId);
                    }
                }
            }

            template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
            PomdpModelType const& BeliefExplorationPomdpModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::pomdp() const {
                if (preprocessedPomdp) {
                    return *preprocessedPomdp;
                } else {
                    return *inputPomdp;
                }
            }

            template
            class BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<double>>;

            template
            class BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<double>, storm::RationalNumber>;

            template
            class BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<storm::RationalNumber>, double>;

            template
            class BeliefExplorationPomdpModelChecker<storm::models::sparse::Pomdp<storm::RationalNumber>>;

        }
    }
}
