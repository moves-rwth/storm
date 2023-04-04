#include "AlphaVectorModelChecker.h"
#include "api/export.h"
#include "storm-pomdp/transformer/MakeStateSetObservationClosed.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "utility/SignalHandler.h"

namespace storm {
namespace pomdp {
namespace modelchecker {
    template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
    AlphaVectorModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::AlphaVectorModelChecker(std::shared_ptr<PomdpModelType> pomdp, storm::pomdp::storage::AlphaVectorPolicy<BeliefValueType> alphaVectorPolicy)
        : inputPomdp(pomdp), inputPolicy(alphaVectorPolicy) {
            STORM_LOG_ASSERT(inputPomdp, "The given POMDP is not initialized.");
            STORM_LOG_ERROR_COND(inputPomdp->isCanonic(), "Input Pomdp is not known to be canonic. This might lead to unexpected verification results.");

            //TODO make parameter
            prec = 1e-12;

            beliefTypeCC =storm::utility::ConstantsComparator<BeliefValueType>(storm::utility::convertNumber<BeliefValueType>(prec), false);
            valueTypeCC = storm::utility::ConstantsComparator<typename PomdpModelType::ValueType>(prec, false);
        }

    template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
    void AlphaVectorModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::check(const storm::logic::Formula &formula){
        STORM_PRINT_AND_LOG("Start checking the MC induced by the alpha vector policy...\n")
        auto formulaInfo = storm::pomdp::analysis::getFormulaInformation(pomdp(), formula);
        std::optional<std::string> rewardModelName;
        std::set<uint32_t> targetObservations;
        if (formulaInfo.isNonNestedReachabilityProbability() || formulaInfo.isNonNestedExpectedRewardFormula()) {
            if (formulaInfo.getTargetStates().observationClosed) {
                targetObservations = formulaInfo.getTargetStates().observations;
            } else {
                storm::transformer::MakeStateSetObservationClosed<PomdpValueType> obsCloser(inputPomdp);
                std::tie(preprocessedPomdp, targetObservations) = obsCloser.transform(formulaInfo.getTargetStates().states);
            }
            if (formulaInfo.isNonNestedReachabilityProbability()) {
                if (!formulaInfo.getSinkStates().empty()) {
                    storm::storage::sparse::ModelComponents<PomdpValueType> components;
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
                    preprocessedPomdp = std::make_shared<PomdpModelType>(std::move(components), true);
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

        // Compute bound using the underlying MDP and a simple policy
        storm::pomdp::modelchecker::PreprocessingPomdpValueBounds<PomdpValueType> bounds;
        auto underlyingMdp = std::make_shared<storm::models::sparse::Mdp<PomdpValueType>>(pomdp().getTransitionMatrix(), pomdp().getStateLabeling(), pomdp().getRewardModels());
        auto resultPtr = storm::api::verifyWithSparseEngine<PomdpValueType>(underlyingMdp, storm::api::createTask<PomdpValueType>(formula.asSharedPointer(), false));
        STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
        STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
        std::vector<PomdpValueType> fullyObservableResult = std::move(resultPtr->template asExplicitQuantitativeCheckResult<PomdpValueType>().getValueVector());

        if(formulaInfo.minimize()){
            bounds.lower.push_back(fullyObservableResult);
        } else {
            bounds.upper.push_back(fullyObservableResult);
        }

        // For cut-offs, we need to provide values. To not skew result too much, we always take the first enabled action
        storm::storage::Scheduler<PomdpValueType> pomdpScheduler(pomdp().getNumberOfStates());
        for(uint64_t state = 0; state < pomdp().getNumberOfStates(); ++state) {
            pomdpScheduler.setChoice(0, state);
        }

        // Model check the DTMC resulting from the policy
        auto scheduledModel = underlyingMdp->applyScheduler(pomdpScheduler, false);
        resultPtr = storm::api::verifyWithSparseEngine<PomdpValueType>(scheduledModel, storm::api::createTask<PomdpValueType>(formula.asSharedPointer(), false));
        STORM_LOG_THROW(resultPtr, storm::exceptions::UnexpectedException, "No check result obtained.");
        STORM_LOG_THROW(resultPtr->isExplicitQuantitativeCheckResult(), storm::exceptions::UnexpectedException, "Unexpected Check result Type");
        auto cutoffVec = resultPtr->template asExplicitQuantitativeCheckResult<PomdpValueType>().getValueVector();
        if(formulaInfo.minimize()){
            bounds.upper.push_back(cutoffVec);
        } else {
            bounds.lower.push_back(cutoffVec);
        }

        auto manager = std::make_shared<BeliefManagerType>(pomdp(), storm::utility::convertNumber<BeliefValueType>(prec), BeliefManagerType::TriangulationMode::Static);
        if (rewardModelName) {
            manager->setRewardModel(rewardModelName);
        }

        auto explorer = std::make_shared<ExplorerType>(manager, bounds);
        buildMarkovChain(targetObservations, formulaInfo.minimize(), rewardModelName.has_value(), manager, explorer, cutoffVec);

        if (explorer->hasComputedValues()) {
            auto printInfo = [&explorer]() {
                std::stringstream str;
                str << "Explored and checked Under-Approximation MDP:\n";
                explorer->getExploredMdp()->printModelInformationToStream(str);
                return str.str();
            };

            std::shared_ptr<storm::models::sparse::Model<BeliefMDPType>> resModel = explorer->getExploredMdp();
            //storm::api::exportSparseModelAsDot(resModel, "/Users/bork/POMDPjl/schedAV.dot");
            STORM_PRINT_AND_LOG("\nResult: " << explorer->getComputedValueAtInitialState() << "\n");
            /*storm::models::sparse::StateLabeling newLabeling(resModel->getStateLabeling());
            auto nrPreprocessingScheds = formulaInfo.minimize() ? explorer->getNrSchedulersForUpperBounds() : explorer->getNrSchedulersForLowerBounds();
            for(uint64_t i = 0; i < nrPreprocessingScheds; ++i){
                newLabeling.addLabel("sched_" + std::to_string(i));
            }
            newLabeling.addLabel("cutoff");
            for(uint64_t i = 0; i < scheduledModel->getNumberOfStates(); ++i){
                if(newLabeling.getStateHasLabel("truncated",i)){
                    newLabeling.addLabelToState("sched_" + std::to_string(explorer->getSchedulerForExploredMdp()->getChoice(i).getDeterministicChoice()),i);
                    newLabeling.addLabelToState("cutoff", i);
                }
            }
            newLabeling.removeLabel("truncated");
            storm::storage::sparse::ModelComponents<BeliefMDPType> modelComponents(scheduledModel->getTransitionMatrix(), newLabeling, scheduledModel->getRewardModels());
            if(scheduledModel->hasChoiceLabeling()){
                modelComponents.choiceLabeling = scheduledModel->getChoiceLabeling();
            }
            storm::models::sparse::Mdp<BeliefMDPType> newMDP(modelComponents);
            auto inducedMC = newMDP.applyScheduler(*(explorer->getSchedulerForExploredMdp()), true);
            scheduledModel = std::static_pointer_cast<storm::models::sparse::Model<BeliefMDPType>>(inducedMC);
            storm::api::exportSparseModelAsDot(scheduledModel, "/Users/bork/POMDPjl/schedAV.dot");
            //BeliefMDPType &resultValue = formulaInfo.minimize() ? result.upperBound : result.lowerBound;
            BeliefMDPType resultValue = explorer->getComputedValueAtInitialState();*/
        }
    }

    template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
    bool AlphaVectorModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::buildMarkovChain(const std::set<uint32_t> &targetObservations, bool min, bool computeRewards, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer, std::vector<typename PomdpModelType::ValueType> const &cutoffVec){
        if (computeRewards) {
            beliefExplorer->startNewExploration(storm::utility::zero<BeliefMDPType>());
        } else {
            beliefExplorer->startNewExploration(storm::utility::one<BeliefMDPType>(), storm::utility::zero<BeliefMDPType>());
        }

        uint64_t sizeThreshold = 100000;
        storm::utility::Stopwatch printUpdateStopwatch;
        printUpdateStopwatch.start();

        //TODO use timelimit
        bool fixPoint = true;
        bool timeLimitExceeded = false;
        bool stopExploration = false;
        //TODO stopping criterion
        while (beliefExplorer->hasUnexploredState()) {
            if (false) {
                STORM_LOG_INFO("Exploration time limit exceeded.");
                timeLimitExceeded = true;
            }

            uint64_t currId = beliefExplorer->exploreNextState();

            if (printUpdateStopwatch.getTimeInSeconds() >= 60) {
                printUpdateStopwatch.restart();
                STORM_PRINT_AND_LOG("### " << beliefExplorer->getCurrentNumberOfMdpStates() << " beliefs in MC" << " ##### " << beliefExplorer->getUnexploredStates().size() << " beliefs queued\n")
            }

            if (timeLimitExceeded) {
                fixPoint = false;
            }
            if (targetObservations.count(beliefManager->getBeliefObservation(currId)) != 0) {
                beliefExplorer->setCurrentStateIsTarget();
                beliefExplorer->addSelfloopTransition();
                beliefExplorer->addChoiceLabelToCurrentState(0, "loop");
            } else {
                if (timeLimitExceeded || beliefExplorer->getCurrentNumberOfMdpStates() >= sizeThreshold /*&& !statistics.beliefMdpDetectedToBeFinite*/) {
                    stopExploration = true;
                    beliefExplorer->setCurrentStateIsTruncated();
                }
                if(!stopExploration) {
                    // determine the best action from the alpha vectors
                    auto chosenActionLabel = getBestActionInBelief(currId, beliefManager,beliefExplorer);
                    uint64_t chosenLocalActionIndex = beliefManager->getBeliefNumberOfChoices(currId) + 1;
                    if(chosenActionLabel == "__unlabeled"){
                        chosenLocalActionIndex = 0;
                    } else {
                        if (pomdp().hasChoiceLabeling()) {
                            auto rowIndex = pomdp().getTransitionMatrix().getRowGroupIndices()[beliefManager->getRepresentativeState(currId)];
                            for (uint64_t action = 0, numActions = beliefManager->getBeliefNumberOfChoices(currId); action < numActions; ++action) {
                                if (pomdp().getChoiceLabeling().getChoiceHasLabel(chosenActionLabel, rowIndex + action)) {
                                    chosenLocalActionIndex = action;
                                    beliefExplorer->addChoiceLabelToCurrentState(0, chosenActionLabel);
                                    break;
                                }
                            }
                        }
                    }

                    // Add successor transitions for the chosen action
                    auto truncationProbability = storm::utility::zero<typename PomdpModelType::ValueType>();
                    auto truncationValueBound = storm::utility::zero<typename PomdpModelType::ValueType>();
                    auto successors = beliefManager->expand(currId, chosenLocalActionIndex);
                    for (auto const &successor : successors) {
                        bool added = beliefExplorer->addTransitionToBelief(0, successor.first, successor.second, stopExploration);
                        if (!added) {
                            STORM_LOG_ASSERT(stopExploration, "Didn't add a transition although exploration shouldn't be stopped.");
                            // We did not explore this successor state. Get a bound on the "missing" value
                            truncationProbability += successor.second;
                            truncationValueBound += successor.second * (min ? beliefExplorer->computeUpperValueBoundAtBelief(successor.first)
                                                                            : beliefExplorer->computeLowerValueBoundAtBelief(successor.first));
                        }
                    }
                    if (computeRewards) {
                        // The truncationValueBound will be added on top of the reward introduced by the current belief state.
                        beliefExplorer->addRewardToCurrentState(0, beliefManager->getBeliefActionReward(currId, chosenLocalActionIndex) + truncationValueBound);
                    }
                } else {
                    //When we stop, we apply simple cut-offs
                    auto cutOffValue = beliefManager->getWeightedSum(currId, cutoffVec);
                    if (computeRewards) {
                        beliefExplorer->addTransitionsToExtraStates(0, storm::utility::one<PomdpValueType>());
                        beliefExplorer->addRewardToCurrentState(0, cutOffValue);
                    } else {
                        beliefExplorer->addTransitionsToExtraStates(0, cutOffValue,storm::utility::one<PomdpValueType>() - cutOffValue);
                    }
                    if(pomdp().hasChoiceLabeling()){
                        beliefExplorer->addChoiceLabelToCurrentState(0, "cutoff");
                    }
                }
            }
            if (storm::utility::resources::isTerminate()) {
                break;
            }
        }

        if (storm::utility::resources::isTerminate()) {
            return false;
        }

        beliefExplorer->finishExploration();
        STORM_PRINT_AND_LOG("Finished exploring Alpha Vector Policy induced Markov chain.\n Start analysis...\n");
        beliefExplorer->computeValuesOfExploredMdp(min ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize);
        // don't overwrite statistics of a previous, successful computation
        return fixPoint;
    }

    template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
    std::string AlphaVectorModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::getBestActionInBelief(uint64_t beliefId, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer) {
        auto multVec = beliefExplorer->computeProductWithSparseMatrix(beliefId, inputPolicy.alphaVectors);
        auto max = multVec.at(0);
        uint64_t maxIndex = 0;
        for (uint64_t i = 1; i < multVec.size(); ++i) {
            if (multVec.at(i) > max) {
                max = multVec.at(i);
                maxIndex = i;
            }
        }
        return inputPolicy.actions[maxIndex];
    }

    template<typename PomdpModelType, typename BeliefValueType, typename BeliefMDPType>
    PomdpModelType const& AlphaVectorModelChecker<PomdpModelType, BeliefValueType, BeliefMDPType>::pomdp() const {
        if (preprocessedPomdp) {
            return *preprocessedPomdp;
        } else {
            return *inputPomdp;
        }
    }

    template
    class AlphaVectorModelChecker<storm::models::sparse::Pomdp<double>>;

    template
    class AlphaVectorModelChecker<storm::models::sparse::Pomdp<double>, storm::RationalNumber>;

    template
    class AlphaVectorModelChecker<storm::models::sparse::Pomdp<storm::RationalNumber>, double>;

    template
    class AlphaVectorModelChecker<storm::models::sparse::Pomdp<storm::RationalNumber>>;
}
}
}
