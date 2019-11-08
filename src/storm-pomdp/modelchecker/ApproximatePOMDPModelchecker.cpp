#include "ApproximatePOMDPModelchecker.h"

#include <boost/algorithm/string.hpp>


#include "storm/utility/ConstantsComparator.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/utility/vector.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/api/properties.h"
#include "storm-parsers/api/storm-parsers.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template<typename ValueType, typename RewardModelType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::ApproximatePOMDPModelchecker() {
                cc = storm::utility::ConstantsComparator<ValueType>(storm::utility::convertNumber<ValueType>(0.00000000001), false);
            }

            template<typename ValueType, typename RewardModelType>
            std::unique_ptr<POMDPCheckResult<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::refineReachabilityProbability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                    std::set<uint32_t> const &targetObservations, bool min,
                                                                                                    uint64_t startingResolution, uint64_t stepSize, uint64_t maxNrOfRefinements) {
                uint64_t currentResolution = startingResolution;
                uint64_t currentRefinement = 0;
                std::unique_ptr<POMDPCheckResult<ValueType>> res = std::make_unique<POMDPCheckResult<ValueType>>(
                        POMDPCheckResult<ValueType>{storm::utility::one<ValueType>(), storm::utility::zero<ValueType>()});
                while (currentRefinement < maxNrOfRefinements && !cc.isEqual(storm::utility::zero<ValueType>(), res->OverapproximationValue - res->UnderapproximationValue)) {
                    STORM_PRINT("--------------------------------------------------------------" << std::endl)
                    STORM_PRINT("Refinement Step " << currentRefinement + 1 << " - Resolution " << currentResolution << std::endl)
                    STORM_PRINT("--------------------------------------------------------------" << std::endl)
                    res = computeReachabilityProbability(pomdp, targetObservations, min, currentResolution);
                    currentResolution += stepSize;
                    ++currentRefinement;
                }
                STORM_PRINT("Procedure took " << currentRefinement << " refinement steps" << std::endl)
                return res;
            }

            template<typename ValueType, typename RewardModelType>
            std::unique_ptr<POMDPCheckResult<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeReachabilityProbabilityOTF(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                        std::set<uint32_t> targetObservations, bool min, uint64_t gridResolution) {
                STORM_PRINT("Use On-The-Fly Grid Generation" << std::endl)

                uint64_t maxIterations = 100;
                bool finished = false;
                uint64_t iteration = 0;
                std::vector<storm::pomdp::Belief<ValueType>> beliefList;
                std::vector<bool> beliefIsTarget;
                std::vector<storm::pomdp::Belief<ValueType>> beliefGrid;
                std::map<uint64_t, ValueType> result;
                std::map<uint64_t, ValueType> result_backup;
                //Use caching to avoid multiple computation of the subsimplices and lambdas
                std::map<uint64_t, std::vector<std::vector<ValueType>>> subSimplexCache;
                std::map<uint64_t, std::vector<ValueType>> lambdaCache;
                std::map<uint64_t, uint64_t> chosenActions;

                std::deque<uint64_t> beliefsToBeExpanded;

                // Belief ID -> Observation -> Probability
                std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> observationProbabilities;
                // current ID -> action -> next ID
                std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> nextBelieves;

                uint64_t nextId = 0;
                storm::utility::Stopwatch expansionTimer(true);
                // Initial belief always has ID 0
                storm::pomdp::Belief<ValueType> initialBelief = getInitialBelief(pomdp, nextId);
                ++nextId;
                beliefList.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());

                // for the initial belief, add the triangulated initial states
                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> initTemp = computeSubSimplexAndLambdas(
                        initialBelief.probabilities, gridResolution);
                std::vector<std::vector<ValueType>> initSubSimplex = initTemp.first;
                std::vector<ValueType> initLambdas = initTemp.second;
                subSimplexCache[0] = initSubSimplex;
                lambdaCache[0] = initLambdas;
                bool initInserted = false;


                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (!cc.isEqual(initLambdas[j], storm::utility::zero<ValueType>())) {
                        uint64_t searchResult = getBeliefIdInVector(beliefList, initialBelief.observation, initSubSimplex[j]);
                        if (searchResult == uint64_t(-1) || (searchResult == 0 && !initInserted)) {
                            if (searchResult == 0) {
                                // the initial belief is on the grid itself
                                initInserted = true;
                                beliefGrid.push_back(initialBelief);
                                beliefsToBeExpanded.push_back(0);
                            } else {
                                // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                storm::pomdp::Belief<ValueType> gridBelief = {nextId, initialBelief.observation, initSubSimplex[j]};
                                beliefList.push_back(gridBelief);
                                beliefGrid.push_back(gridBelief);
                                beliefIsTarget.push_back(
                                        targetObservations.find(initialBelief.observation) != targetObservations.end());
                                beliefsToBeExpanded.push_back(nextId);
                                ++nextId;
                            }
                        }
                    }
                }


                //beliefsToBeExpanded.push_back(initialBelief.id); TODO I'm curious what happens if we do this instead of first triangulating. Should do nothing special if belief is on grid, otherwise it gets interesting

                // Expand the beliefs to generate the grid on-the-fly to avoid unreachable grid points
                while (!beliefsToBeExpanded.empty()) {
                    uint64_t currId = beliefsToBeExpanded.front();
                    beliefsToBeExpanded.pop_front();
                    bool isTarget = beliefIsTarget[currId];
                    if (isTarget) {
                        result.emplace(std::make_pair(currId, storm::utility::one<ValueType>()));
                        result_backup.emplace(std::make_pair(currId, storm::utility::one<ValueType>()));
                    } else {
                        result.emplace(std::make_pair(currId, storm::utility::zero<ValueType>()));
                        result_backup.emplace(std::make_pair(currId, storm::utility::zero<ValueType>()));

                        uint64_t numChoices = pomdp.getNumberOfChoices(
                                pomdp.getStatesWithObservation(beliefList[currId].observation).front());
                        std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction(numChoices);
                        std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction(numChoices);

                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                                    pomdp, beliefList[currId], action);
                            std::map<uint32_t, uint64_t> actionObservationBelieves;
                            for (auto iter = actionObservationProbabilities.begin();
                                 iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                // THIS CALL IS SLOW
                                // TODO speed this up
                                uint64_t idNextBelief = getBeliefAfterActionAndObservation(pomdp, beliefList, beliefIsTarget, targetObservations, beliefList[currId], action,
                                                                                           observation, nextId);
                                nextId = beliefList.size();
                                actionObservationBelieves[observation] = idNextBelief;
                                //Triangulate here and put the possibly resulting belief in the grid
                                std::vector<std::vector<ValueType>> subSimplex;
                                std::vector<ValueType> lambdas;
                                if (subSimplexCache.count(idNextBelief) > 0) {
                                    // TODO is this necessary here? Think later
                                    subSimplex = subSimplexCache[idNextBelief];
                                    lambdas = lambdaCache[idNextBelief];
                                } else {
                                    std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                            beliefList[idNextBelief].probabilities, gridResolution);
                                    subSimplex = temp.first;
                                    lambdas = temp.second;
                                    subSimplexCache[idNextBelief] = subSimplex;
                                    lambdaCache[idNextBelief] = lambdas;
                                }

                                for (size_t j = 0; j < lambdas.size(); ++j) {
                                    if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                        if (getBeliefIdInVector(beliefGrid, observation, subSimplex[j]) == uint64_t(-1)) {
                                            // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                            storm::pomdp::Belief<ValueType> gridBelief = {nextId, observation, subSimplex[j]};
                                            beliefList.push_back(gridBelief);
                                            beliefGrid.push_back(gridBelief);
                                            beliefIsTarget.push_back(
                                                    targetObservations.find(observation) != targetObservations.end());
                                            beliefsToBeExpanded.push_back(nextId);
                                            ++nextId;
                                        }
                                    }
                                }
                            }
                            observationProbabilitiesInAction[action] = actionObservationProbabilities;
                            nextBelievesInAction[action] = actionObservationBelieves;
                        }
                        observationProbabilities.emplace(
                                std::make_pair(currId, observationProbabilitiesInAction));
                        nextBelieves.emplace(std::make_pair(currId, nextBelievesInAction));
                    }
                }
                expansionTimer.stop();
                STORM_PRINT("Grid size: " << beliefGrid.size() << std::endl)
                STORM_PRINT("#Believes in List: " << beliefList.size() << std::endl)
                STORM_PRINT("Belief space expansion took " << expansionTimer << std::endl)

                storm::utility::Stopwatch overApproxTimer(true);
                // Value Iteration
                while (!finished && iteration < maxIterations) {
                    storm::utility::Stopwatch iterationTimer(true);
                    STORM_LOG_DEBUG("Iteration " << iteration + 1);
                    bool improvement = false;
                    for (size_t i = 0; i < beliefGrid.size(); ++i) {
                        storm::pomdp::Belief<ValueType> currentBelief = beliefGrid[i];
                        bool isTarget = beliefIsTarget[currentBelief.id];
                        if (!isTarget) {
                            // we can take any state with the observation as they have the same number of choices
                            uint64_t numChoices = pomdp.getNumberOfChoices(
                                    pomdp.getStatesWithObservation(currentBelief.observation).front());
                            // Initialize the values for the value iteration
                            ValueType chosenValue = min ? storm::utility::infinity<ValueType>()
                                                        : -storm::utility::infinity<ValueType>();
                            uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                            ValueType currentValue;

                            for (uint64_t action = 0; action < numChoices; ++action) {
                                currentValue = storm::utility::zero<ValueType>();
                                for (auto iter = observationProbabilities[currentBelief.id][action].begin();
                                     iter != observationProbabilities[currentBelief.id][action].end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    storm::pomdp::Belief<ValueType> nextBelief = beliefList[nextBelieves[currentBelief.id][action][observation]];
                                    // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                                    // cache the values  to not always re-calculate
                                    std::vector<std::vector<ValueType>> subSimplex;
                                    std::vector<ValueType> lambdas;
                                    if (subSimplexCache.count(nextBelief.id) > 0) {
                                        subSimplex = subSimplexCache[nextBelief.id];
                                        lambdas = lambdaCache[nextBelief.id];
                                    } else {
                                        // TODO is this necessary here? Everything should have already been computed
                                        std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                                nextBelief.probabilities, gridResolution);
                                        subSimplex = temp.first;
                                        lambdas = temp.second;
                                        subSimplexCache[nextBelief.id] = subSimplex;
                                        lambdaCache[nextBelief.id] = lambdas;
                                    }
                                    auto sum = storm::utility::zero<ValueType>();
                                    for (size_t j = 0; j < lambdas.size(); ++j) {
                                        if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                            sum += lambdas[j] * result_backup.at(
                                                    getBeliefIdInVector(beliefGrid, observation, subSimplex[j]));
                                        }
                                    }
                                    currentValue += iter->second * sum;
                                }
                                // Update the selected actions
                                if ((min && cc.isLess(storm::utility::zero<ValueType>(), chosenValue - currentValue)) ||
                                    (!min &&
                                     cc.isLess(storm::utility::zero<ValueType>(), currentValue - chosenValue)) ||
                                    cc.isEqual(storm::utility::zero<ValueType>(), chosenValue - currentValue)) {
                                    chosenValue = currentValue;
                                    chosenActionIndex = action;
                                }
                            }
                            result[currentBelief.id] = chosenValue;
                            chosenActions[currentBelief.id] = chosenActionIndex;
                            // Check if the iteration brought an improvement
                            if (cc.isLess(storm::utility::zero<ValueType>(), result[currentBelief.id] - result_backup[currentBelief.id])) {
                                improvement = true;
                            }
                        }
                    }
                    finished = !improvement;
                    // back up
                    result_backup = result;

                    ++iteration;
                    iterationTimer.stop();
                    STORM_PRINT("Iteration " << iteration << ": " << iterationTimer << std::endl);
                }

                STORM_PRINT("Overapproximation took " << iteration << " iterations" << std::endl);

                auto overApprox = storm::utility::zero<ValueType>();
                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (initLambdas[j] != storm::utility::zero<ValueType>()) {
                        overApprox += initLambdas[j] *
                                      result_backup[getBeliefIdInVector(beliefGrid, initialBelief.observation,
                                                                        initSubSimplex[j])];
                    }
                }
                overApproxTimer.stop();

                storm::utility::Stopwatch underApproxTimer(true);
                ValueType underApprox = computeUnderapproximationWithMDP(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves,
                                                                         result, chosenActions, gridResolution, initialBelief.id, min, false);
                underApproxTimer.stop();

                STORM_PRINT("Time Overapproximation: " << overApproxTimer
                                                       << std::endl
                                                       << "Time Underapproximation: " << underApproxTimer
                                                       << std::endl);

                STORM_PRINT("Over-Approximation Result: " << overApprox << std::endl);
                STORM_PRINT("Under-Approximation Result: " << underApprox << std::endl);

                return std::make_unique<POMDPCheckResult<ValueType>>(
                        POMDPCheckResult<ValueType>{overApprox, underApprox});

            }

            template<typename ValueType, typename RewardModelType>
            std::unique_ptr<POMDPCheckResult<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeReachabilityRewardOTF(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                   std::set<uint32_t> targetObservations, bool min, uint64_t gridResolution) {
                STORM_PRINT("Use On-The-Fly Grid Generation" << std::endl)

                RewardModelType pomdpRewardModel = pomdp.getUniqueRewardModel();

                uint64_t maxIterations = 100;
                bool finished = false;
                uint64_t iteration = 0;
                std::vector<storm::pomdp::Belief<ValueType>> beliefList;
                std::vector<bool> beliefIsTarget;
                std::vector<storm::pomdp::Belief<ValueType>> beliefGrid;
                std::map<uint64_t, ValueType> result;
                std::map<uint64_t, ValueType> result_backup;
                //Use caching to avoid multiple computation of the subsimplices and lambdas
                std::map<uint64_t, std::vector<std::vector<ValueType>>> subSimplexCache;
                std::map<uint64_t, std::vector<ValueType>> lambdaCache;
                std::map<uint64_t, uint64_t> chosenActions;

                std::deque<uint64_t> beliefsToBeExpanded;

                // Belief ID -> Observation -> Probability
                std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> observationProbabilities;
                // current ID -> action -> next ID
                std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> nextBelieves;
                // current ID -> action -> reward
                std::map<uint64_t, std::vector<ValueType>> beliefActionRewards;

                uint64_t nextId = 0;
                storm::utility::Stopwatch expansionTimer(true);
                // Initial belief always has ID 0
                storm::pomdp::Belief<ValueType> initialBelief = getInitialBelief(pomdp, nextId);
                ++nextId;
                beliefList.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());

                // for the initial belief, add the triangulated initial states
                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> initTemp = computeSubSimplexAndLambdas(
                        initialBelief.probabilities, gridResolution);
                std::vector<std::vector<ValueType>> initSubSimplex = initTemp.first;
                std::vector<ValueType> initLambdas = initTemp.second;
                subSimplexCache[0] = initSubSimplex;
                lambdaCache[0] = initLambdas;
                bool initInserted = false;


                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (!cc.isEqual(initLambdas[j], storm::utility::zero<ValueType>())) {
                        uint64_t searchResult = getBeliefIdInVector(beliefList, initialBelief.observation, initSubSimplex[j]);
                        if (searchResult == uint64_t(-1) || (searchResult == 0 && !initInserted)) {
                            if (searchResult == 0) {
                                // the initial belief is on the grid itself
                                initInserted = true;
                                beliefGrid.push_back(initialBelief);
                                beliefsToBeExpanded.push_back(0);
                            } else {
                                // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                storm::pomdp::Belief<ValueType> gridBelief = {nextId, initialBelief.observation, initSubSimplex[j]};
                                beliefList.push_back(gridBelief);
                                beliefGrid.push_back(gridBelief);
                                beliefIsTarget.push_back(
                                        targetObservations.find(initialBelief.observation) != targetObservations.end());
                                beliefsToBeExpanded.push_back(nextId);
                                ++nextId;
                            }
                        }
                    }
                }


                //beliefsToBeExpanded.push_back(initialBelief.id); TODO I'm curious what happens if we do this instead of first triangulating. Should do nothing special if belief is on grid, otherwise it gets interesting

                // Expand the beliefs to generate the grid on-the-fly to avoid unreachable grid points
                while (!beliefsToBeExpanded.empty()) {
                    uint64_t currId = beliefsToBeExpanded.front();
                    beliefsToBeExpanded.pop_front();
                    bool isTarget = beliefIsTarget[currId];

                    result.emplace(std::make_pair(currId, storm::utility::zero<ValueType>()));
                    result_backup.emplace(std::make_pair(currId, storm::utility::zero<ValueType>()));
                    if (!isTarget) {

                        uint64_t representativeState = pomdp.getStatesWithObservation(beliefList[currId].observation).front();
                        uint64_t numChoices = pomdp.getNumberOfChoices(representativeState);
                        std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction(numChoices);
                        std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction(numChoices);
                        std::vector<ValueType> actionRewardsInState(numChoices);

                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(pomdp, beliefList[currId], action);
                            std::map<uint32_t, uint64_t> actionObservationBelieves;
                            for (auto iter = actionObservationProbabilities.begin(); iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                // THIS CALL IS SLOW
                                // TODO speed this up
                                uint64_t idNextBelief = getBeliefAfterActionAndObservation(pomdp, beliefList, beliefIsTarget, targetObservations, beliefList[currId], action,
                                                                                           observation, nextId);
                                nextId = beliefList.size();
                                actionObservationBelieves[observation] = idNextBelief;
                                //Triangulate here and put the possibly resulting belief in the grid
                                std::vector<std::vector<ValueType>> subSimplex;
                                std::vector<ValueType> lambdas;
                                if (subSimplexCache.count(idNextBelief) > 0) {
                                    // TODO is this necessary here? Think later
                                    subSimplex = subSimplexCache[idNextBelief];
                                    lambdas = lambdaCache[idNextBelief];
                                } else {
                                    std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                            beliefList[idNextBelief].probabilities, gridResolution);
                                    subSimplex = temp.first;
                                    lambdas = temp.second;
                                    subSimplexCache[idNextBelief] = subSimplex;
                                    lambdaCache[idNextBelief] = lambdas;
                                }

                                for (size_t j = 0; j < lambdas.size(); ++j) {
                                    if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                        if (getBeliefIdInVector(beliefGrid, observation, subSimplex[j]) == uint64_t(-1)) {
                                            // if the triangulated belief was not found in the list, we place it in the grid and add it to the work list
                                            storm::pomdp::Belief<ValueType> gridBelief = {nextId, observation, subSimplex[j]};
                                            beliefList.push_back(gridBelief);
                                            beliefGrid.push_back(gridBelief);
                                            beliefIsTarget.push_back(
                                                    targetObservations.find(observation) != targetObservations.end());
                                            beliefsToBeExpanded.push_back(nextId);
                                            ++nextId;
                                        }
                                    }
                                }
                            }
                            observationProbabilitiesInAction[action] = actionObservationProbabilities;
                            nextBelievesInAction[action] = actionObservationBelieves;
                            actionRewardsInState[action] = getRewardAfterAction(pomdp, pomdp.getChoiceIndex(storm::storage::StateActionPair(representativeState, action)),
                                                                                beliefList[currId]);
                        }
                        observationProbabilities.emplace(
                                std::make_pair(currId, observationProbabilitiesInAction));
                        nextBelieves.emplace(std::make_pair(currId, nextBelievesInAction));
                        beliefActionRewards.emplace(std::make_pair(currId, actionRewardsInState));

                    }
                }
                expansionTimer.stop();
                STORM_PRINT("Grid size: " << beliefGrid.size() << std::endl)
                STORM_PRINT("#Believes in List: " << beliefList.size() << std::endl)
                STORM_PRINT("Belief space expansion took " << expansionTimer << std::endl)

                storm::utility::Stopwatch overApproxTimer(true);
                // Value Iteration
                while (!finished && iteration < maxIterations) {
                    storm::utility::Stopwatch iterationTimer(true);
                    STORM_LOG_DEBUG("Iteration " << iteration + 1);
                    bool improvement = false;
                    for (size_t i = 0; i < beliefGrid.size(); ++i) {
                        storm::pomdp::Belief<ValueType> currentBelief = beliefGrid[i];
                        bool isTarget = beliefIsTarget[currentBelief.id];
                        if (!isTarget) {
                            // we can take any state with the observation as they have the same number of choices
                            uint64_t numChoices = pomdp.getNumberOfChoices(
                                    pomdp.getStatesWithObservation(currentBelief.observation).front());
                            // Initialize the values for the value iteration
                            ValueType chosenValue = min ? storm::utility::infinity<ValueType>()
                                                        : -storm::utility::infinity<ValueType>();
                            uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                            ValueType currentValue;

                            for (uint64_t action = 0; action < numChoices; ++action) {
                                currentValue = beliefActionRewards[currentBelief.id][action];
                                for (auto iter = observationProbabilities[currentBelief.id][action].begin();
                                     iter != observationProbabilities[currentBelief.id][action].end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    storm::pomdp::Belief<ValueType> nextBelief = beliefList[nextBelieves[currentBelief.id][action][observation]];
                                    // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                                    // cache the values  to not always re-calculate
                                    std::vector<std::vector<ValueType>> subSimplex;
                                    std::vector<ValueType> lambdas;
                                    if (subSimplexCache.count(nextBelief.id) > 0) {
                                        subSimplex = subSimplexCache[nextBelief.id];
                                        lambdas = lambdaCache[nextBelief.id];
                                    } else {
                                        //TODO This should not ne reachable
                                        std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                                nextBelief.probabilities, gridResolution);
                                        subSimplex = temp.first;
                                        lambdas = temp.second;
                                        subSimplexCache[nextBelief.id] = subSimplex;
                                        lambdaCache[nextBelief.id] = lambdas;
                                    }
                                    auto sum = storm::utility::zero<ValueType>();
                                    for (size_t j = 0; j < lambdas.size(); ++j) {
                                        if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                            sum += lambdas[j] * result_backup.at(
                                                    getBeliefIdInVector(beliefGrid, observation, subSimplex[j]));
                                        }
                                    }

                                    currentValue += iter->second * sum;
                                }
                                // Update the selected actions
                                if ((min && cc.isLess(storm::utility::zero<ValueType>(), chosenValue - currentValue)) ||
                                    (!min &&
                                     cc.isLess(storm::utility::zero<ValueType>(), currentValue - chosenValue)) ||
                                    cc.isEqual(storm::utility::zero<ValueType>(), chosenValue - currentValue)) {

                                    chosenValue = currentValue;
                                    chosenActionIndex = action;
                                }
                            }

                            result[currentBelief.id] = chosenValue;

                            chosenActions[currentBelief.id] = chosenActionIndex;
                            // Check if the iteration brought an improvement
                            if (cc.isLess(storm::utility::zero<ValueType>(), result_backup[currentBelief.id] - result[currentBelief.id]) ||
                                cc.isLess(storm::utility::zero<ValueType>(), result[currentBelief.id] - result_backup[currentBelief.id])) {
                                improvement = true;
                            }
                        }
                    }
                    finished = !improvement;
                    // back up
                    result_backup = result;

                    ++iteration;
                    iterationTimer.stop();
                    STORM_PRINT("Iteration " << iteration << ": " << iterationTimer << std::endl);
                }

                STORM_PRINT("Overapproximation took " << iteration << " iterations" << std::endl);

                auto overApprox = storm::utility::zero<ValueType>();
                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (initLambdas[j] != storm::utility::zero<ValueType>()) {
                        overApprox += initLambdas[j] *
                                      result_backup[getBeliefIdInVector(beliefGrid, initialBelief.observation,
                                                                        initSubSimplex[j])];
                    }
                }
                overApproxTimer.stop();

                storm::utility::Stopwatch underApproxTimer(true);
                ValueType underApprox = computeUnderapproximationWithMDP(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves,
                                                                         result, chosenActions, gridResolution, initialBelief.id, min, true);
                underApproxTimer.stop();

                STORM_PRINT("Time Overapproximation: " << overApproxTimer
                                                       << std::endl
                                                       << "Time Underapproximation: " << underApproxTimer
                                                       << std::endl);

                STORM_PRINT("Over-Approximation Result: " << overApprox << std::endl);
                STORM_PRINT("Under-Approximation Result: " << underApprox << std::endl);

                return std::make_unique<POMDPCheckResult<ValueType>>(
                        POMDPCheckResult<ValueType>{overApprox, underApprox});

            }


            template<typename ValueType, typename RewardModelType>
            std::unique_ptr<POMDPCheckResult<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeReachabilityProbability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                     std::set<uint32_t> targetObservations, bool min, uint64_t gridResolution) {
                storm::utility::Stopwatch beliefGridTimer(true);
                uint64_t maxIterations = 100;
                bool finished = false;
                uint64_t iteration = 0;

                std::vector<storm::pomdp::Belief<ValueType>> beliefList;
                std::vector<bool> beliefIsTarget;
                uint64_t nextId = 0;
                // Initial belief always has ID 0
                storm::pomdp::Belief<ValueType> initialBelief = getInitialBelief(pomdp, nextId);
                ++nextId;
                beliefList.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());


                std::vector<storm::pomdp::Belief<ValueType>> beliefGrid;
                constructBeliefGrid(pomdp, targetObservations, gridResolution, beliefList, beliefGrid, beliefIsTarget,
                                    nextId);
                nextId = beliefList.size();
                beliefGridTimer.stop();

                storm::utility::Stopwatch overApproxTimer(true);
                // Belief ID -> Value
                std::map<uint64_t, ValueType> result;
                std::map<uint64_t, ValueType> result_backup;
                // Belief ID -> ActionIndex
                std::map<uint64_t, uint64_t> chosenActions;

                // Belief ID -> Observation -> Probability
                std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> observationProbabilities;
                // current ID -> action -> next ID
                std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> nextBelieves;

                storm::utility::Stopwatch nextBeliefGeneration(true);
                for (size_t i = 0; i < beliefGrid.size(); ++i) {
                    auto currentBelief = beliefGrid[i];
                    bool isTarget = beliefIsTarget[currentBelief.id];
                    if (isTarget) {
                        result.emplace(std::make_pair(currentBelief.id, storm::utility::one<ValueType>()));
                        result_backup.emplace(std::make_pair(currentBelief.id, storm::utility::one<ValueType>()));
                    } else {
                        result.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));
                        result_backup.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));

                        //TODO put this in extra function
                        uint64_t numChoices = pomdp.getNumberOfChoices(
                                pomdp.getStatesWithObservation(currentBelief.observation).front());
                        std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction(numChoices);
                        std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction(numChoices);

                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                                    pomdp, currentBelief, action);
                            std::map<uint32_t, uint64_t> actionObservationBelieves;
                            for (auto iter = actionObservationProbabilities.begin();
                                 iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                // THIS CALL IS SLOW
                                // TODO speed this up
                                actionObservationBelieves[observation] = getBeliefAfterActionAndObservation(pomdp,
                                                                                                            beliefList,
                                                                                                            beliefIsTarget,
                                                                                                            targetObservations,
                                                                                                            currentBelief,
                                                                                                            action,
                                                                                                            observation,
                                                                                                            nextId);
                                nextId = beliefList.size();
                            }
                            observationProbabilitiesInAction[action] = actionObservationProbabilities;
                            nextBelievesInAction[action] = actionObservationBelieves;
                        }
                        observationProbabilities.emplace(
                                std::make_pair(currentBelief.id, observationProbabilitiesInAction));
                        nextBelieves.emplace(std::make_pair(currentBelief.id, nextBelievesInAction));
                    }
                }
                nextBeliefGeneration.stop();

                //Use chaching to avoid multiple computation of the subsimplices and lambdas
                std::map<uint64_t, std::vector<std::vector<ValueType>>> subSimplexCache;
                std::map<uint64_t, std::vector<ValueType>> lambdaCache;

                STORM_PRINT("Time generation of next believes: " << nextBeliefGeneration << std::endl)
                // Value Iteration
                while (!finished && iteration < maxIterations) {
                    storm::utility::Stopwatch iterationTimer(true);
                    STORM_LOG_DEBUG("Iteration " << iteration + 1);
                    bool improvement = false;
                    for (size_t i = 0; i < beliefGrid.size(); ++i) {
                        storm::pomdp::Belief<ValueType> currentBelief = beliefGrid[i];
                        bool isTarget = beliefIsTarget[currentBelief.id];
                        if (!isTarget) {
                            // we can take any state with the observation as they have the same number of choices
                            uint64_t numChoices = pomdp.getNumberOfChoices(
                                    pomdp.getStatesWithObservation(currentBelief.observation).front());
                            // Initialize the values for the value iteration
                            ValueType chosenValue = min ? storm::utility::infinity<ValueType>()
                                                        : -storm::utility::infinity<ValueType>();
                            uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                            ValueType currentValue;

                            for (uint64_t action = 0; action < numChoices; ++action) {
                                currentValue = storm::utility::zero<ValueType>();
                                for (auto iter = observationProbabilities[currentBelief.id][action].begin();
                                     iter != observationProbabilities[currentBelief.id][action].end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    storm::pomdp::Belief<ValueType> nextBelief = beliefList[nextBelieves[currentBelief.id][action][observation]];
                                    // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                                    // cache the values  to not always re-calculate
                                    std::vector<std::vector<ValueType>> subSimplex;
                                    std::vector<ValueType> lambdas;
                                    if (subSimplexCache.count(nextBelief.id) > 0) {
                                        subSimplex = subSimplexCache[nextBelief.id];
                                        lambdas = lambdaCache[nextBelief.id];
                                    } else {
                                        std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                                nextBelief.probabilities, gridResolution);
                                        subSimplex = temp.first;
                                        lambdas = temp.second;
                                        subSimplexCache[nextBelief.id] = subSimplex;
                                        lambdaCache[nextBelief.id] = lambdas;
                                    }
                                    auto sum = storm::utility::zero<ValueType>();
                                    for (size_t j = 0; j < lambdas.size(); ++j) {
                                        if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                            sum += lambdas[j] * result_backup.at(
                                                    getBeliefIdInVector(beliefGrid, observation, subSimplex[j]));
                                        }
                                    }
                                    currentValue += iter->second * sum;
                                }
                                // Update the selected actions
                                if ((min && cc.isLess(storm::utility::zero<ValueType>(), chosenValue - currentValue)) ||
                                    (!min &&
                                     cc.isLess(storm::utility::zero<ValueType>(), currentValue - chosenValue)) ||
                                    cc.isEqual(storm::utility::zero<ValueType>(), chosenValue - currentValue)) {
                                    chosenValue = currentValue;
                                    chosenActionIndex = action;
                                }
                            }
                            result[currentBelief.id] = chosenValue;
                            chosenActions[currentBelief.id] = chosenActionIndex;
                            // Check if the iteration brought an improvement
                            if (cc.isLess(storm::utility::zero<ValueType>(), result[currentBelief.id] - result_backup[currentBelief.id])) {
                                improvement = true;
                            }
                        }
                    }
                    finished = !improvement;
                    // back up
                    result_backup = result;

                    ++iteration;
                    iterationTimer.stop();
                    STORM_PRINT("Iteration " << iteration << ": " << iterationTimer << std::endl);
                }

                STORM_PRINT("Overapproximation took " << iteration << " iterations" << std::endl);

                beliefGrid.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());

                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                        initialBelief.probabilities, gridResolution);
                std::vector<std::vector<ValueType>> initSubSimplex = temp.first;
                std::vector<ValueType> initLambdas = temp.second;

                auto overApprox = storm::utility::zero<ValueType>();
                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (initLambdas[j] != storm::utility::zero<ValueType>()) {
                        overApprox += initLambdas[j] *
                                      result_backup[getBeliefIdInVector(beliefGrid, initialBelief.observation,
                                                                        initSubSimplex[j])];
                    }
                }
                overApproxTimer.stop();

                // Now onto the under-approximation
                bool useMdp = true;
                storm::utility::Stopwatch underApproxTimer(true);
                ValueType underApprox = useMdp ? computeUnderapproximationWithMDP(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves,
                                                                                  result, chosenActions, gridResolution, initialBelief.id, min, false) :
                                        computeUnderapproximationWithDTMC(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves, result,
                                                                          chosenActions, gridResolution, initialBelief.id, min, false);
                underApproxTimer.stop();

                STORM_PRINT("Time Belief Grid Generation: " << beliefGridTimer << std::endl
                                                            << "Time Overapproximation: " << overApproxTimer
                                                            << std::endl
                                                            << "Time Underapproximation: " << underApproxTimer
                                                            << std::endl);

                STORM_PRINT("Over-Approximation Result: " << overApprox << std::endl);
                STORM_PRINT("Under-Approximation Result: " << underApprox << std::endl);

                return std::make_unique<POMDPCheckResult<ValueType>>(
                        POMDPCheckResult<ValueType>{overApprox, underApprox});
            }

            //TODO This function reuses a lot of code from the probability computation, refactor to minimize code duplication!
            template<typename ValueType, typename RewardModelType>
            std::unique_ptr<POMDPCheckResult<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeReachabilityReward(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                std::set<uint32_t> targetObservations, bool min, uint64_t gridResolution) {
                storm::utility::Stopwatch beliefGridTimer(true);
                uint64_t maxIterations = 100;
                bool finished = false;
                uint64_t iteration = 0;

                RewardModelType pomdpRewardModel = pomdp.getUniqueRewardModel();

                std::vector<storm::pomdp::Belief<ValueType>> beliefList;
                std::vector<bool> beliefIsTarget;
                uint64_t nextId = 0;
                // Initial belief always has ID 0
                storm::pomdp::Belief<ValueType> initialBelief = getInitialBelief(pomdp, nextId);
                ++nextId;
                beliefList.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());


                std::vector<storm::pomdp::Belief<ValueType>> beliefGrid;
                constructBeliefGrid(pomdp, targetObservations, gridResolution, beliefList, beliefGrid, beliefIsTarget,
                                    nextId);
                nextId = beliefList.size();
                beliefGridTimer.stop();

                storm::utility::Stopwatch overApproxTimer(true);
                // Belief ID -> Value
                std::map<uint64_t, ValueType> result;
                std::map<uint64_t, ValueType> result_backup;
                // Belief ID -> ActionIndex
                std::map<uint64_t, uint64_t> chosenActions;

                // Belief ID -> Observation -> Probability
                std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> observationProbabilities;
                // current ID -> action -> next ID
                std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> nextBelieves;
                // current ID -> action -> reward
                std::map<uint64_t, std::vector<ValueType>> beliefActionRewards;

                storm::utility::Stopwatch nextBeliefGeneration(true);
                for (size_t i = 0; i < beliefGrid.size(); ++i) {
                    auto currentBelief = beliefGrid[i];
                    bool isTarget = beliefIsTarget[currentBelief.id];
                    result.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));
                    result_backup.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));
                    if (!isTarget) {
                        //TODO put this in extra function
                        // As we need to grab some parameters which are the same for all states with the same observation, we simply select some state as the representative
                        uint64_t representativeState = pomdp.getStatesWithObservation(currentBelief.observation).front();
                        uint64_t numChoices = pomdp.getNumberOfChoices(representativeState);
                        std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction(numChoices);
                        std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction(numChoices);

                        std::vector<ValueType> actionRewardsInState(numChoices);

                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                                    pomdp, currentBelief, action);
                            std::map<uint32_t, uint64_t> actionObservationBelieves;
                            for (auto iter = actionObservationProbabilities.begin();
                                 iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                // THIS CALL IS SLOW
                                // TODO speed this up
                                actionObservationBelieves[observation] = getBeliefAfterActionAndObservation(pomdp,
                                                                                                            beliefList,
                                                                                                            beliefIsTarget,
                                                                                                            targetObservations,
                                                                                                            currentBelief,
                                                                                                            action,
                                                                                                            observation,
                                                                                                            nextId);
                                nextId = beliefList.size();
                            }
                            observationProbabilitiesInAction[action] = actionObservationProbabilities;
                            nextBelievesInAction[action] = actionObservationBelieves;

                            actionRewardsInState[action] = getRewardAfterAction(pomdp, pomdp.getChoiceIndex(storm::storage::StateActionPair(representativeState, action)),
                                                                                currentBelief);
                        }
                        observationProbabilities.emplace(
                                std::make_pair(currentBelief.id, observationProbabilitiesInAction));
                        nextBelieves.emplace(std::make_pair(currentBelief.id, nextBelievesInAction));
                        beliefActionRewards.emplace(std::make_pair(currentBelief.id, actionRewardsInState));
                    }

                }
                nextBeliefGeneration.stop();

                //Use chaching to avoid multiple computation of the subsimplices and lambdas
                std::map<uint64_t, std::vector<std::vector<ValueType>>> subSimplexCache;
                std::map<uint64_t, std::vector<ValueType>> lambdaCache;

                STORM_PRINT("Time generation of next believes: " << nextBeliefGeneration << std::endl)
                // Value Iteration
                while (!finished && iteration < maxIterations) {
                    storm::utility::Stopwatch iterationTimer(true);
                    STORM_LOG_DEBUG("Iteration " << iteration + 1);
                    bool improvement = false;
                    for (size_t i = 0; i < beliefGrid.size(); ++i) {
                        storm::pomdp::Belief<ValueType> currentBelief = beliefGrid[i];
                        bool isTarget = beliefIsTarget[currentBelief.id];
                        if (!isTarget) {
                            // we can take any state with the observation as they have the same number of choices
                            uint64_t numChoices = pomdp.getNumberOfChoices(
                                    pomdp.getStatesWithObservation(currentBelief.observation).front());
                            // Initialize the values for the value iteration
                            ValueType chosenValue = min ? storm::utility::infinity<ValueType>()
                                                        : -storm::utility::infinity<ValueType>();
                            uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                            ValueType currentValue;

                            for (uint64_t action = 0; action < numChoices; ++action) {
                                currentValue = beliefActionRewards[currentBelief.id][action];
                                for (auto iter = observationProbabilities[currentBelief.id][action].begin();
                                     iter != observationProbabilities[currentBelief.id][action].end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    storm::pomdp::Belief<ValueType> nextBelief = beliefList[nextBelieves[currentBelief.id][action][observation]];
                                    // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                                    // cache the values  to not always re-calculate
                                    std::vector<std::vector<ValueType>> subSimplex;
                                    std::vector<ValueType> lambdas;
                                    if (subSimplexCache.count(nextBelief.id) > 0) {
                                        subSimplex = subSimplexCache[nextBelief.id];
                                        lambdas = lambdaCache[nextBelief.id];
                                    } else {
                                        std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                                nextBelief.probabilities, gridResolution);
                                        subSimplex = temp.first;
                                        lambdas = temp.second;
                                        subSimplexCache[nextBelief.id] = subSimplex;
                                        lambdaCache[nextBelief.id] = lambdas;
                                    }
                                    auto sum = storm::utility::zero<ValueType>();
                                    for (size_t j = 0; j < lambdas.size(); ++j) {
                                        if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                            sum += lambdas[j] * result_backup.at(
                                                    getBeliefIdInVector(beliefGrid, observation, subSimplex[j]));
                                        }
                                    }

                                    currentValue += iter->second * sum;
                                }
                                // Update the selected actions
                                if ((min && cc.isLess(storm::utility::zero<ValueType>(), chosenValue - currentValue)) ||
                                    (!min &&
                                     cc.isLess(storm::utility::zero<ValueType>(), currentValue - chosenValue)) ||
                                    cc.isEqual(storm::utility::zero<ValueType>(), chosenValue - currentValue)) {

                                    chosenValue = currentValue;
                                    chosenActionIndex = action;
                                }
                            }

                            result[currentBelief.id] = chosenValue;

                            chosenActions[currentBelief.id] = chosenActionIndex;
                            // Check if the iteration brought an improvement
                            if (cc.isLess(storm::utility::zero<ValueType>(), result_backup[currentBelief.id] - result[currentBelief.id]) ||
                                cc.isLess(storm::utility::zero<ValueType>(), result[currentBelief.id] - result_backup[currentBelief.id])) {
                                improvement = true;
                            }
                        }
                    }
                    finished = !improvement;
                    // back up
                    result_backup = result;

                    ++iteration;
                    iterationTimer.stop();
                    STORM_PRINT("Iteration " << iteration << ": " << iterationTimer << std::endl);
                }

                STORM_PRINT("Overapproximation took " << iteration << " iterations" << std::endl);

                beliefGrid.push_back(initialBelief);
                beliefIsTarget.push_back(
                        targetObservations.find(initialBelief.observation) != targetObservations.end());

                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                        initialBelief.probabilities, gridResolution);
                std::vector<std::vector<ValueType>> initSubSimplex = temp.first;
                std::vector<ValueType> initLambdas = temp.second;

                auto overApprox = storm::utility::zero<ValueType>();
                for (size_t j = 0; j < initLambdas.size(); ++j) {
                    if (initLambdas[j] != storm::utility::zero<ValueType>()) {
                        overApprox += initLambdas[j] *
                                      result_backup[getBeliefIdInVector(beliefGrid, initialBelief.observation,
                                                                        initSubSimplex[j])];
                    }
                }
                overApproxTimer.stop();

                // Now onto the under-approximation
                bool useMdp = true;
                storm::utility::Stopwatch underApproxTimer(true);
                ValueType underApprox = useMdp ? computeUnderapproximationWithMDP(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves,
                                                                                  result, chosenActions, gridResolution, initialBelief.id, min, true) :
                                        computeUnderapproximationWithDTMC(pomdp, beliefList, beliefIsTarget, targetObservations, observationProbabilities, nextBelieves,
                                                                          result, chosenActions, gridResolution, initialBelief.id, min, true);
                underApproxTimer.stop();

                STORM_PRINT("Time Belief Grid Generation: " << beliefGridTimer << std::endl
                                                            << "Time Overapproximation: " << overApproxTimer
                                                            << std::endl
                                                            << "Time Underapproximation: " << underApproxTimer
                                                            << std::endl);
                STORM_PRINT("Over-Approximation Result: " << overApprox << std::endl);
                STORM_PRINT("Under-Approximation Result: " << underApprox << std::endl);

                return std::make_unique<POMDPCheckResult<ValueType>>(
                        POMDPCheckResult<ValueType>{overApprox, underApprox});
            }

            template<typename ValueType, typename RewardModelType>
            ValueType
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeUnderapproximationWithDTMC(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                        std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                                                                        std::vector<bool> &beliefIsTarget,
                                                                                                        std::set<uint32_t> &targetObservations,
                                                                                                        std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                                                                                        std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                                                                                        std::map<uint64_t, ValueType> &result,
                                                                                                        std::map<uint64_t, uint64_t> chosenActions,
                                                                                                        uint64_t gridResolution, uint64_t initialBeliefId, bool min,
                                                                                                        bool computeReward) {
                std::set<uint64_t> visitedBelieves;
                std::deque<uint64_t> believesToBeExpanded;
                std::map<uint64_t, uint64_t> beliefStateMap;
                std::vector<std::map<uint64_t, ValueType>> transitions;
                std::vector<uint64_t> targetStates;

                uint64_t stateId = 0;
                beliefStateMap[initialBeliefId] = stateId;
                ++stateId;

                // Expand the believes
                visitedBelieves.insert(initialBeliefId);
                believesToBeExpanded.push_back(initialBeliefId);
                while (!believesToBeExpanded.empty()) {
                    auto currentBeliefId = believesToBeExpanded.front();
                    std::map<uint64_t, ValueType> transitionsInState;
                    STORM_LOG_DEBUG("Exploring Belief " << beliefList[currentBeliefId].observation << "||"
                                                        << beliefList[currentBeliefId].probabilities);
                    if (beliefIsTarget[currentBeliefId]) {
                        // add a self-loop to target states and save them
                        transitionsInState[beliefStateMap[currentBeliefId]] = storm::utility::one<ValueType>();
                        targetStates.push_back(beliefStateMap[currentBeliefId]);
                    } else {
                        if (chosenActions.find(currentBeliefId) == chosenActions.end()) {
                            // If the current Belief is not part of the grid, we have not computed the action to choose yet
                            chosenActions[currentBeliefId] = extractBestAction(pomdp, beliefList, beliefIsTarget,
                                                                               targetObservations,
                                                                               observationProbabilities,
                                                                               nextBelieves, result, gridResolution,
                                                                               currentBeliefId, beliefList.size(), min);
                        }
                        for (auto iter = observationProbabilities[currentBeliefId][chosenActions[currentBeliefId]].begin();
                             iter !=
                             observationProbabilities[currentBeliefId][chosenActions[currentBeliefId]].end(); ++iter) {
                            uint32_t observation = iter->first;
                            uint64_t nextBeliefId = nextBelieves[currentBeliefId][chosenActions[currentBeliefId]][observation];
                            if (visitedBelieves.insert(nextBeliefId).second) {
                                beliefStateMap[nextBeliefId] = stateId;
                                ++stateId;
                                believesToBeExpanded.push_back(nextBeliefId);
                            }
                            transitionsInState[beliefStateMap[nextBeliefId]] = iter->second;
                        }
                    }
                    transitions.push_back(transitionsInState);
                    believesToBeExpanded.pop_front();
                }

                storm::models::sparse::StateLabeling labeling(transitions.size());
                labeling.addLabel("init");
                labeling.addLabel("target");
                labeling.addLabelToState("init", 0);
                for (auto targetState : targetStates) {
                    labeling.addLabelToState("target", targetState);
                }

                storm::models::sparse::StandardRewardModel<ValueType> rewardModel(std::vector<ValueType>(beliefStateMap.size()));
                for (auto const &iter : beliefStateMap) {
                    auto currentBelief = beliefList[iter.first];
                    // Add the reward collected by taking the chosen Action in the belief
                    rewardModel.setStateReward(iter.second, getRewardAfterAction(pomdp, pomdp.getChoiceIndex(
                            storm::storage::StateActionPair(pomdp.getStatesWithObservation(currentBelief.observation).front(), chosenActions[iter.first])),
                                                                                 currentBelief));
                }

                std::unordered_map<std::string, RewardModelType> rewardModels = {{"std", rewardModel}};

                storm::storage::sparse::ModelComponents<ValueType, RewardModelType> modelComponents(buildTransitionMatrix(transitions), labeling, rewardModels);

                storm::models::sparse::Dtmc<ValueType, RewardModelType> underApproxDtmc(modelComponents);
                auto model = std::make_shared<storm::models::sparse::Dtmc<ValueType, RewardModelType>>(underApproxDtmc);
                model->printModelInformationToStream(std::cout);

                std::string propertyString;
                if (computeReward) {
                    propertyString = min ? "Rmin=? [F \"target\"]" : "Rmax=? [F \"target\"]";
                } else {
                    propertyString = min ? "Pmin=? [F \"target\"]" : "Pmax=? [F \"target\"]";
                }
                std::vector<storm::jani::Property> propertyVector = storm::api::parseProperties(propertyString);
                std::shared_ptr<storm::logic::Formula const> property = storm::api::extractFormulasFromProperties(
                        propertyVector).front();

                std::unique_ptr<storm::modelchecker::CheckResult> res(
                        storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(property,
                                                                                                               true)));
                STORM_LOG_ASSERT(res, "Result does not exist.");
                res->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                return res->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
            }

            template<typename ValueType, typename RewardModelType>
            ValueType
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeUnderapproximationWithMDP(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                       std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                                                                       std::vector<bool> &beliefIsTarget,
                                                                                                       std::set<uint32_t> &targetObservations,
                                                                                                       std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                                                                                       std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                                                                                       std::map<uint64_t, ValueType> &result,
                                                                                                       std::map<uint64_t, uint64_t> chosenActions,
                                                                                                       uint64_t gridResolution, uint64_t initialBeliefId, bool min,
                                                                                                       bool computeRewards) {
                std::set<uint64_t> visitedBelieves;
                std::deque<uint64_t> believesToBeExpanded;
                std::map<uint64_t, uint64_t> beliefStateMap;
                std::vector<std::vector<std::map<uint64_t, ValueType>>> transitions;
                std::vector<uint64_t> targetStates;

                uint64_t stateId = 0;
                beliefStateMap[initialBeliefId] = stateId;
                ++stateId;

                // Expand the believes
                visitedBelieves.insert(initialBeliefId);
                believesToBeExpanded.push_back(initialBeliefId);
                while (!believesToBeExpanded.empty()) {
                    auto currentBeliefId = believesToBeExpanded.front();
                    std::vector<std::map<uint64_t, ValueType>> actionTransitionStorage;
                    // for targets, we only consider one action with one transition
                    if (beliefIsTarget[currentBeliefId]) {
                        // add a self-loop to target states and save them
                        std::map<uint64_t, ValueType> transitionsInStateWithAction;
                        transitionsInStateWithAction[beliefStateMap[currentBeliefId]] = storm::utility::one<ValueType>();
                        targetStates.push_back(beliefStateMap[currentBeliefId]);
                        actionTransitionStorage.push_back(transitionsInStateWithAction);
                    } else {
                        uint64_t numChoices = pomdp.getNumberOfChoices(
                                pomdp.getStatesWithObservation(beliefList[currentBeliefId].observation).front());
                        if (chosenActions.find(currentBeliefId) == chosenActions.end()) {
                            // If the current Belief is not part of the grid, the next states have not been computed yet.
                            std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction;
                            std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction;
                            for (uint64_t action = 0; action < numChoices; ++action) {
                                std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                                        pomdp, beliefList[currentBeliefId], action);
                                std::map<uint32_t, uint64_t> actionObservationBelieves;
                                for (auto iter = actionObservationProbabilities.begin();
                                     iter != actionObservationProbabilities.end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    actionObservationBelieves[observation] = getBeliefAfterActionAndObservation(pomdp,
                                                                                                                beliefList,
                                                                                                                beliefIsTarget,
                                                                                                                targetObservations,
                                                                                                                beliefList[currentBeliefId],
                                                                                                                action,
                                                                                                                observation,
                                                                                                                beliefList.size());
                                }
                                observationProbabilitiesInAction.push_back(actionObservationProbabilities);
                                nextBelievesInAction.push_back(actionObservationBelieves);
                            }
                            observationProbabilities.emplace(std::make_pair(currentBeliefId, observationProbabilitiesInAction));
                            nextBelieves.emplace(std::make_pair(currentBeliefId, nextBelievesInAction));
                        }
                        // Iterate over all actions and add the corresponding transitions
                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint64_t, ValueType> transitionsInStateWithAction;

                            for (auto iter = observationProbabilities[currentBeliefId][action].begin();
                                 iter !=
                                 observationProbabilities[currentBeliefId][action].end(); ++iter) {
                                uint32_t observation = iter->first;
                                uint64_t nextBeliefId = nextBelieves[currentBeliefId][action][observation];
                                if (visitedBelieves.insert(nextBeliefId).second) {
                                    beliefStateMap[nextBeliefId] = stateId;
                                    ++stateId;
                                    believesToBeExpanded.push_back(nextBeliefId);
                                }
                                transitionsInStateWithAction[beliefStateMap[nextBeliefId]] = iter->second;
                            }
                            actionTransitionStorage.push_back(transitionsInStateWithAction);
                        }
                    }
                    transitions.push_back(actionTransitionStorage);
                    believesToBeExpanded.pop_front();
                }

                storm::models::sparse::StateLabeling labeling(transitions.size());
                labeling.addLabel("init");
                labeling.addLabel("target");
                labeling.addLabelToState("init", 0);
                for (auto targetState : targetStates) {
                    labeling.addLabelToState("target", targetState);
                }

                storm::storage::sparse::ModelComponents<ValueType, RewardModelType> modelComponents(
                        buildTransitionMatrix(transitions), labeling);

                storm::models::sparse::Mdp<ValueType, RewardModelType> underApproxMdp(modelComponents);

                if (computeRewards) {
                    storm::models::sparse::StandardRewardModel<ValueType> rewardModel(boost::none, std::vector<ValueType>(modelComponents.transitionMatrix.getRowCount()));
                    for (auto const &iter : beliefStateMap) {
                        auto currentBelief = beliefList[iter.first];
                        auto representativeState = pomdp.getStatesWithObservation(currentBelief.observation).front();
                        for (uint64_t action = 0; action < underApproxMdp.getNumberOfChoices(iter.second); ++action) {
                            // Add the reward
                            rewardModel.setStateActionReward(underApproxMdp.getChoiceIndex(storm::storage::StateActionPair(iter.second, action)),
                                                             getRewardAfterAction(pomdp, pomdp.getChoiceIndex(storm::storage::StateActionPair(representativeState, action)),
                                                                                  currentBelief));
                        }
                    }
                    underApproxMdp.addRewardModel("std", rewardModel);
                    underApproxMdp.restrictRewardModels(std::set<std::string>({"std"}));
                }

                auto model = std::make_shared<storm::models::sparse::Mdp<ValueType, RewardModelType>>(underApproxMdp);
                model->printModelInformationToStream(std::cout);

                std::string propertyString;
                if (computeRewards) {
                    propertyString = min ? "Rmin=? [F \"target\"]" : "Rmax=? [F \"target\"]";
                } else {
                    propertyString = min ? "Pmin=? [F \"target\"]" : "Pmax=? [F \"target\"]";
                }
                std::vector<storm::jani::Property> propertyVector = storm::api::parseProperties(propertyString);
                std::shared_ptr<storm::logic::Formula const> property = storm::api::extractFormulasFromProperties(
                        propertyVector).front();

                std::unique_ptr<storm::modelchecker::CheckResult> res(
                        storm::api::verifyWithSparseEngine<ValueType>(model, storm::api::createTask<ValueType>(property,
                                                                                                               true)));
                STORM_LOG_ASSERT(res, "Result does not exist.");
                res->filter(storm::modelchecker::ExplicitQualitativeCheckResult(model->getInitialStates()));
                return res->asExplicitQuantitativeCheckResult<ValueType>().getValueMap().begin()->second;
            }


            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::buildTransitionMatrix(
                    std::vector<std::vector<std::map<uint64_t, ValueType>>> transitions) {
                uint_fast64_t currentRow = 0;
                uint_fast64_t currentRowGroup = 0;
                uint64_t nrColumns = transitions.size();
                uint64_t nrRows = 0;
                uint64_t nrEntries = 0;
                for (auto const &actionTransitions : transitions) {
                    for (auto const &map : actionTransitions) {
                        nrEntries += map.size();
                        ++nrRows;
                    }
                }
                storm::storage::SparseMatrixBuilder<ValueType> smb(nrRows, nrColumns, nrEntries, true, true);
                for (auto const &actionTransitions : transitions) {
                    smb.newRowGroup(currentRow);
                    for (auto const &map : actionTransitions) {
                        for (auto const &transition : map) {
                            smb.addNextValue(currentRow, transition.first, transition.second);
                        }
                        ++currentRow;
                    }
                    ++currentRowGroup;
                }
                return smb.build();
            }

            template<typename ValueType, typename RewardModelType>
            storm::storage::SparseMatrix<ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::buildTransitionMatrix(
                    std::vector<std::map<uint64_t, ValueType>> transitions) {
                uint_fast64_t currentRow = 0;
                uint64_t nrEntries = 0;
                for (auto const &map : transitions) {
                    nrEntries += map.size();
                }
                storm::storage::SparseMatrixBuilder<ValueType> smb(transitions.size(), transitions.size(), nrEntries);
                for (auto const &map : transitions) {
                    for (auto const &transition : map) {
                        smb.addNextValue(currentRow, transition.first, transition.second);
                    }
                    ++currentRow;
                }
                return smb.build();
            }

            template<typename ValueType, typename RewardModelType>
            uint64_t ApproximatePOMDPModelchecker<ValueType, RewardModelType>::extractBestAction(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                    std::vector<bool> &beliefIsTarget,
                    std::set<uint32_t> &targetObservations,
                    std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                    std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                    std::map<uint64_t, ValueType> &result,
                    uint64_t gridResolution, uint64_t currentBeliefId, uint64_t nextId, bool min) {
                storm::pomdp::Belief<ValueType> currentBelief = beliefList[currentBeliefId];

                //TODO put this in extra function
                std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction;
                std::vector<std::map<uint32_t, uint64_t>> nextBelievesInAction;
                uint64_t numChoices = pomdp.getNumberOfChoices(
                        pomdp.getStatesWithObservation(currentBelief.observation).front());
                for (uint64_t action = 0; action < numChoices; ++action) {
                    std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                            pomdp, currentBelief, action);
                    std::map<uint32_t, uint64_t> actionObservationBelieves;
                    for (auto iter = actionObservationProbabilities.begin();
                         iter != actionObservationProbabilities.end(); ++iter) {
                        uint32_t observation = iter->first;
                        actionObservationBelieves[observation] = getBeliefAfterActionAndObservation(pomdp,
                                                                                                    beliefList,
                                                                                                    beliefIsTarget,
                                                                                                    targetObservations,
                                                                                                    currentBelief,
                                                                                                    action,
                                                                                                    observation,
                                                                                                    nextId);
                        nextId = beliefList.size();
                    }
                    observationProbabilitiesInAction.push_back(actionObservationProbabilities);
                    nextBelievesInAction.push_back(actionObservationBelieves);
                }
                observationProbabilities.emplace(std::make_pair(currentBeliefId, observationProbabilitiesInAction));
                nextBelieves.emplace(std::make_pair(currentBeliefId, nextBelievesInAction));

                // choose the action which results in the value computed by the over-approximation
                ValueType chosenValue = min ? storm::utility::infinity<ValueType>()
                                            : -storm::utility::infinity<ValueType>();
                uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                ValueType currentValue;

                for (uint64_t action = 0; action < numChoices; ++action) {
                    currentValue = storm::utility::zero<ValueType>(); // simply change this for rewards?
                    for (auto iter = observationProbabilities[currentBelief.id][action].begin();
                         iter != observationProbabilities[currentBelief.id][action].end(); ++iter) {
                        uint32_t observation = iter->first;
                        storm::pomdp::Belief<ValueType> nextBelief = beliefList[nextBelieves[currentBelief.id][action][observation]];

                        // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                        auto temp = computeSubSimplexAndLambdas(nextBelief.probabilities, gridResolution);
                        std::vector<std::vector<ValueType>> subSimplex = temp.first;
                        std::vector<ValueType> lambdas = temp.second;

                        auto sum = storm::utility::zero<ValueType>();
                        for (size_t j = 0; j < lambdas.size(); ++j) {
                            if (!cc.isEqual(lambdas[j], storm::utility::zero<ValueType>())) {
                                sum += lambdas[j] * result.at(
                                        getBeliefIdInVector(beliefList, observation, subSimplex[j]));
                            }
                        }
                        currentValue += iter->second * sum;
                    }

                    // Update the selected actions
                    if ((min && cc.isLess(storm::utility::zero<ValueType>(), chosenValue - currentValue)) ||
                        (!min &&
                         cc.isLess(storm::utility::zero<ValueType>(), currentValue - chosenValue)) ||
                        cc.isEqual(storm::utility::zero<ValueType>(), chosenValue - currentValue)) {
                        chosenValue = currentValue;
                        chosenActionIndex = action;
                    }
                }
                return chosenActionIndex;
            }


            template<typename ValueType, typename RewardModelType>
            uint64_t ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefIdInVector(
                    std::vector<storm::pomdp::Belief<ValueType>> const &grid, uint32_t observation,
                    std::vector<ValueType> probabilities) {
                // TODO This one is quite slow
                for (auto const &belief : grid) {
                    if (belief.observation == observation) {
                        bool same = true;
                        for (size_t i = 0; i < belief.probabilities.size(); ++i) {
                            if (!cc.isEqual(belief.probabilities[i], probabilities[i])) {
                                same = false;
                                break;
                            }
                        }
                        if (same) {
                            return belief.id;
                        }
                    }
                }

                return -1;
            }

            template<typename ValueType, typename RewardModelType>
            storm::pomdp::Belief<ValueType> ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getInitialBelief(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, uint64_t id) {
                STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() < 2,
                                 "POMDP contains more than one initial state");
                STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() == 1,
                                 "POMDP does not contain an initial state");
                std::vector<ValueType> distribution(pomdp.getNumberOfStates(), storm::utility::zero<ValueType>());
                uint32_t observation = 0;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (pomdp.getInitialStates()[state] == 1) {
                        distribution[state] = storm::utility::one<ValueType>();
                        observation = pomdp.getObservation(state);
                    }
                }
                return storm::pomdp::Belief<ValueType>{id, observation, distribution};
            }

            template<typename ValueType, typename RewardModelType>
            void ApproximatePOMDPModelchecker<ValueType, RewardModelType>::constructBeliefGrid(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    std::set<uint32_t> target_observations, uint64_t gridResolution,
                    std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                    std::vector<storm::pomdp::Belief<ValueType>> &grid, std::vector<bool> &beliefIsKnown,
                    uint64_t nextId) {
                bool isTarget;
                uint64_t newId = nextId;

                for (uint32_t observation = 0; observation < pomdp.getNrObservations(); ++observation) {
                    std::vector<uint64_t> statesWithObservation = pomdp.getStatesWithObservation(observation);
                    isTarget = target_observations.find(observation) != target_observations.end();

                    // TODO this can probably be condensed
                    if (statesWithObservation.size() == 1) {
                        // If there is only one state with the observation, we can directly add the corresponding belief
                        std::vector<ValueType> distribution(pomdp.getNumberOfStates(),
                                                            storm::utility::zero<ValueType>());
                        distribution[statesWithObservation.front()] = storm::utility::one<ValueType>();
                        storm::pomdp::Belief<ValueType> belief = {newId, observation, distribution};
                        STORM_LOG_TRACE(
                                "Add Belief " << std::to_string(newId) << " [(" << std::to_string(observation) << "),"
                                              << distribution << "]");
                        beliefList.push_back(belief);
                        grid.push_back(belief);
                        beliefIsKnown.push_back(isTarget);
                        ++newId;
                    } else {
                        // Otherwise we have to enumerate all possible distributions with regards to the grid
                        // helper is used to derive the distribution of the belief
                        std::vector<ValueType> helper(statesWithObservation.size(), ValueType(0));
                        helper[0] = storm::utility::convertNumber<ValueType>(gridResolution);
                        bool done = false;
                        uint64_t index = 0;

                        while (!done) {
                            std::vector<ValueType> distribution(pomdp.getNumberOfStates(),
                                                                storm::utility::zero<ValueType>());
                            for (size_t i = 0; i < statesWithObservation.size() - 1; ++i) {
                                distribution[statesWithObservation[i]] = (helper[i] - helper[i + 1]) /
                                                                         storm::utility::convertNumber<ValueType>(
                                                                                 gridResolution);
                            }
                            distribution[statesWithObservation.back()] =
                                    helper[statesWithObservation.size() - 1] /
                                    storm::utility::convertNumber<ValueType>(gridResolution);

                            storm::pomdp::Belief<ValueType> belief = {newId, observation, distribution};
                            STORM_LOG_TRACE(
                                    "Add Belief " << std::to_string(newId) << " [(" << std::to_string(observation)
                                                  << ")," << distribution << "]");
                            beliefList.push_back(belief);
                            grid.push_back(belief);
                            beliefIsKnown.push_back(isTarget);
                            if (helper[statesWithObservation.size() - 1] ==
                                storm::utility::convertNumber<ValueType>(gridResolution)) {
                                // If the last entry of helper is the gridResolution, we have enumerated all necessary distributions
                                done = true;
                            } else {
                                // Update helper by finding the index to increment
                                index = statesWithObservation.size() - 1;
                                while (helper[index] == helper[index - 1]) {
                                    --index;
                                }
                                STORM_LOG_ASSERT(index > 0, "Error in BeliefGrid generation - index wrong");
                                // Increment the value at the index
                                ++helper[index];
                                // Reset all indices greater than the changed one to 0
                                ++index;
                                while (index < statesWithObservation.size()) {
                                    helper[index] = 0;
                                    ++index;
                                }
                            }
                            ++newId;
                        }
                    }
                }
            }

            template<typename ValueType, typename RewardModelType>
            std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeSubSimplexAndLambdas(
                    std::vector<ValueType> probabilities, uint64_t resolution) {
                // This is the Freudenthal Triangulation as described in Lovejoy (a whole lotta math)
                // Variable names are based on the paper
                uint64_t probSize = probabilities.size();
                std::vector<ValueType> x(probSize);
                std::vector<ValueType> v(probSize);
                std::vector<ValueType> d(probSize);
                auto convResolution = storm::utility::convertNumber<ValueType>(resolution);

                for (size_t i = 0; i < probSize; ++i) {
                    for (size_t j = i; j < probSize; ++j) {
                        x[i] += convResolution * probabilities[j];
                    }
                    v[i] = storm::utility::floor(x[i]);
                    d[i] = x[i] - v[i];
                }

                auto p = storm::utility::vector::getSortedIndices(d);

                std::vector<std::vector<ValueType>> qs(probSize, std::vector<ValueType>(probSize));
                for (size_t i = 0; i < probSize; ++i) {
                    if (i == 0) {
                        for (size_t j = 0; j < probSize; ++j) {
                            qs[i][j] = v[j];
                        }
                    } else {
                        for (size_t j = 0; j < probSize; ++j) {
                            if (j == p[i - 1]) {
                                qs[i][j] = qs[i - 1][j] + storm::utility::one<ValueType>();
                            } else {
                                qs[i][j] = qs[i - 1][j];
                            }
                        }
                    }
                }
                std::vector<std::vector<ValueType>> subSimplex(probSize, std::vector<ValueType>(probSize));
                for (size_t j = 0; j < probSize; ++j) {
                    for (size_t i = 0; i < probSize - 1; ++i) {
                        subSimplex[j][i] = (qs[j][i] - qs[j][i + 1]) / convResolution;
                    }

                    subSimplex[j][probSize - 1] = qs[j][probSize - 1] / convResolution;
                }

                std::vector<ValueType> lambdas(probSize, storm::utility::zero<ValueType>());
                auto sum = storm::utility::zero<ValueType>();
                for (size_t i = 1; i < probSize; ++i) {
                    lambdas[i] = d[p[i - 1]] - d[p[i]];
                    sum += d[p[i - 1]] - d[p[i]];
                }
                lambdas[0] = storm::utility::one<ValueType>() - sum;

                return std::make_pair(subSimplex, lambdas);
            }


            template<typename ValueType, typename RewardModelType>
            std::map<uint32_t, ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeObservationProbabilitiesAfterAction(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    storm::pomdp::Belief<ValueType> belief,
                    uint64_t actionIndex) {
                std::map<uint32_t, ValueType> res;
                // the id is not important here as we immediately discard the belief (very hacky, I don't like it either)
                std::vector<ValueType> postProbabilities = getBeliefAfterAction(pomdp, belief, actionIndex, 0).probabilities;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    uint32_t observation = pomdp.getObservation(state);
                    if (postProbabilities[state] != storm::utility::zero<ValueType>()) {
                        if (res.count(observation) == 0) {
                            res[observation] = postProbabilities[state];
                        } else {
                            res[observation] += postProbabilities[state];
                        }
                    }
                }
                return res;
            }

            template<typename ValueType, typename RewardModelType>
            storm::pomdp::Belief<ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefAfterAction(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    storm::pomdp::Belief<ValueType> belief,
                    uint64_t actionIndex, uint64_t id) {
                std::vector<ValueType> distributionAfter(pomdp.getNumberOfStates(), storm::utility::zero<ValueType>());
                uint32_t observation = 0;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (belief.probabilities[state] != storm::utility::zero<ValueType>()) {
                        auto row = pomdp.getTransitionMatrix().getRow(pomdp.getChoiceIndex(storm::storage::StateActionPair(state, actionIndex)));
                        for (auto const &entry : row) {
                            observation = pomdp.getObservation(entry.getColumn());
                            distributionAfter[entry.getColumn()] += belief.probabilities[state] * entry.getValue();
                        }
                    }
                }
                return storm::pomdp::Belief<ValueType>{id, observation, distributionAfter};
            }

            template<typename ValueType, typename RewardModelType>
            uint64_t ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefAfterActionAndObservation(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                    std::vector<bool> &beliefIsTarget, std::set<uint32_t> &targetObservations, storm::pomdp::Belief<ValueType> belief, uint64_t actionIndex, uint32_t observation,
                    uint64_t id) {
                storm::utility::Stopwatch distrWatch(true);
                std::vector<ValueType> distributionAfter(pomdp.getNumberOfStates()); //, storm::utility::zero<ValueType>());
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (belief.probabilities[state] != storm::utility::zero<ValueType>()) {
                        auto row = pomdp.getTransitionMatrix().getRow(pomdp.getChoiceIndex(storm::storage::StateActionPair(state, actionIndex)));
                        for (auto const &entry : row) {
                            if (pomdp.getObservation(entry.getColumn()) == observation) {
                                distributionAfter[entry.getColumn()] += belief.probabilities[state] * entry.getValue();
                            }
                        }
                    }
                }
                distrWatch.stop();
                // We have to normalize the distribution
                storm::utility::Stopwatch normalizationWatch(true);
                auto sum = storm::utility::zero<ValueType>();
                for (ValueType const &entry : distributionAfter) {
                    sum += entry;
                }

                for (size_t i = 0; i < pomdp.getNumberOfStates(); ++i) {
                    distributionAfter[i] /= sum;
                }
                normalizationWatch.stop();
                if (getBeliefIdInVector(beliefList, observation, distributionAfter) != uint64_t(-1)) {
                    storm::utility::Stopwatch getWatch(true);
                    auto res = getBeliefIdInVector(beliefList, observation, distributionAfter);
                    getWatch.stop();
                    //STORM_PRINT("Distribution: "<< distrWatch.getTimeInNanoseconds() << " / Normalization: " << normalizationWatch.getTimeInNanoseconds() << " / getId: " << getWatch.getTimeInNanoseconds() << std::endl)
                    return res;
                } else {
                    storm::utility::Stopwatch pushWatch(true);
                    beliefList.push_back(storm::pomdp::Belief<ValueType>{id, observation, distributionAfter});
                    beliefIsTarget.push_back(targetObservations.find(observation) != targetObservations.end());
                    pushWatch.stop();
                    //STORM_PRINT("Distribution: "<< distrWatch.getTimeInNanoseconds() << " / Normalization: " << normalizationWatch.getTimeInNanoseconds() << " / generateBelief: " << pushWatch.getTimeInNanoseconds() << std::endl)
                    return id;
                }
            }

            template<typename ValueType, typename RewardModelType>
            ValueType ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getRewardAfterAction(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                                                                     uint64_t action, storm::pomdp::Belief<ValueType> belief) {
                auto result = storm::utility::zero<ValueType>();
                for (size_t i = 0; i < belief.probabilities.size(); ++i) {
                    result += belief.probabilities[i] * pomdp.getUniqueRewardModel().getTotalStateActionReward(i, action, pomdp.getTransitionMatrix());
                }
                return result;
            }


            template
            class ApproximatePOMDPModelchecker<double>;

#ifdef STORM_HAVE_CARL

            //template class ApproximatePOMDPModelchecker<storm::RationalFunction>;
            template
            class ApproximatePOMDPModelchecker<storm::RationalNumber>;

#endif
        }
    }
}