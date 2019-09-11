#include <storm/utility/ConstantsComparator.h>
#include "ApproximatePOMDPModelchecker.h"
#include "storm/utility/vector.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template<typename ValueType, typename RewardModelType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::ApproximatePOMDPModelchecker() {
                //Intentionally left empty
            }

            template<typename ValueType, typename RewardModelType>
            /*std::unique_ptr<POMDPCheckResult>*/ void
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeReachabilityProbability(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    std::set<uint32_t> target_observations, bool min, uint64_t gridResolution) {
                //TODO add timing
                uint64_t maxIterations = 100;
                bool finished = false;
                uint64_t iteration = 0;


                std::vector<Belief<ValueType>> beliefGrid;
                std::vector<bool> beliefIsKnown;
                constructBeliefGrid(pomdp, target_observations, gridResolution, beliefGrid, beliefIsKnown);
                std::map<uint64_t, ValueType> result;
                std::map<uint64_t, ValueType> result_backup;

                std::vector<std::vector<std::map<uint32_t, ValueType>>> observationProbabilities;
                std::vector<std::vector<std::map<uint32_t, Belief<ValueType>>>> nextBelieves;

                uint64_t nextId = beliefGrid.size();
                for (size_t i = 0; i < beliefGrid.size(); ++i) {
                    auto currentBelief = beliefGrid[i];
                    bool isTarget = beliefIsKnown[i];
                    if (isTarget) {
                        result.emplace(std::make_pair(currentBelief.id, storm::utility::one<ValueType>()));
                        result_backup.emplace(std::make_pair(currentBelief.id, storm::utility::one<ValueType>()));
                    } else {
                        result.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));
                        result_backup.emplace(std::make_pair(currentBelief.id, storm::utility::zero<ValueType>()));

                        std::vector<std::map<uint32_t, ValueType>> observationProbabilitiesInAction;
                        std::vector<std::map<uint32_t, Belief<ValueType>>> nextBelievesInAction;

                        uint64_t numChoices = pomdp.getNumberOfChoices(
                                pomdp.getStatesWithObservation(currentBelief.observation).front());
                        for (uint64_t action = 0; action < numChoices; ++action) {
                            std::map<uint32_t, ValueType> actionObservationProbabilities = computeObservationProbabilitiesAfterAction(
                                    pomdp, currentBelief, action);
                            std::map<uint32_t, Belief<ValueType>> actionObservationBelieves;
                            for (auto iter = actionObservationProbabilities.begin();
                                 iter != actionObservationProbabilities.end(); ++iter) {
                                uint32_t observation = iter->first;
                                actionObservationBelieves[observation] = getBeliefAfterActionAndObservation(pomdp,
                                                                                                            currentBelief,
                                                                                                            action,
                                                                                                            observation,
                                                                                                            nextId);
                                ++nextId;
                            }
                            observationProbabilitiesInAction.push_back(actionObservationProbabilities);
                            nextBelievesInAction.push_back(actionObservationBelieves);
                        }
                        observationProbabilities.push_back(observationProbabilitiesInAction);
                        nextBelieves.push_back(nextBelievesInAction);
                    }
                }

                // Value Iteration
                while (!finished && iteration < maxIterations) {
                    STORM_LOG_DEBUG("Iteration " << std::to_string(iteration));
                    bool improvement = false;
                    for (size_t i = 0; i < beliefGrid.size(); ++i) {
                        bool isTarget = beliefIsKnown[i];
                        if (!isTarget) {
                            Belief<ValueType> currentBelief = beliefGrid[i];
                            // we can take any state with the observation as they have the same number of choices
                            uint64_t numChoices = pomdp.getNumberOfChoices(
                                    pomdp.getStatesWithObservation(currentBelief.observation).front());

                            // Initialize the values for the value iteration
                            ValueType bestValue = min ? storm::utility::infinity<ValueType>()
                                                      : -storm::utility::infinity<ValueType>();
                            uint64_t chosenActionIndex = std::numeric_limits<uint64_t>::infinity();
                            ValueType currentValue;

                            for (uint64_t action = 0; action < numChoices; ++action) {
                                currentValue = storm::utility::zero<ValueType>(); // simply change this for rewards?

                                for (auto iter = observationProbabilities[i][action].begin();
                                     iter != observationProbabilities[i][action].end(); ++iter) {
                                    uint32_t observation = iter->first;
                                    Belief<ValueType> nextBelief = nextBelieves[i][action][observation];

                                    // compute subsimplex and lambdas according to the Lovejoy paper to approximate the next belief
                                    std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                                            currentBelief.probabilities, gridResolution);
                                    std::vector<std::vector<ValueType>> subSimplex = temp.first;
                                    std::vector<ValueType> lambdas = temp.second;

                                    ValueType sum = storm::utility::zero<ValueType>();
                                    for (size_t j = 0; j < lambdas.size(); ++j) {
                                        if (lambdas[j] != storm::utility::zero<ValueType>()) {
                                            sum += lambdas[j] * result_backup.at(
                                                    getBeliefIdInGrid(beliefGrid, observation, subSimplex[j]));
                                        }
                                    }

                                    currentValue += iter->second * sum;
                                }

                                // Update the selected actions
                                auto cc = storm::utility::ConstantsComparator<ValueType>();
                                if ((min && cc.isLess(storm::utility::zero<ValueType>(), bestValue - currentValue)) ||
                                    (!min && cc.isLess(storm::utility::zero<ValueType>(), currentValue - bestValue))) {
                                    improvement = true;
                                    bestValue = currentValue;
                                    chosenActionIndex = action;
                                }
                                // TODO tie breaker?
                            }
                            result[currentBelief.id] = bestValue;
                        }
                    }
                    finished = !improvement;
                    // back up
                    for (auto iter = result.begin(); iter != result.end(); ++iter) {
                        result_backup[iter->first] = result[iter->first];
                    }
                    ++iteration;
                }

                // maybe change this so the initial Belief always has ID 0
                Belief<ValueType> initialBelief = getInitialBelief(pomdp, nextId);
                ++nextId;

                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>> temp = computeSubSimplexAndLambdas(
                        initialBelief.probabilities, gridResolution);
                std::vector<std::vector<ValueType>> subSimplex = temp.first;
                std::vector<ValueType> lambdas = temp.second;

                ValueType overApprox = storm::utility::zero<ValueType>();
                for (size_t j = 0; j < lambdas.size(); ++j) {
                    if (lambdas[j] != storm::utility::zero<ValueType>()) {
                        overApprox += lambdas[j] *
                                      result_backup[getBeliefIdInGrid(beliefGrid, initialBelief.observation,
                                                                      subSimplex[j])];
                    }
                }

                STORM_LOG_DEBUG("Over-Approximation Result: " << overApprox);
            }

            template<typename ValueType, typename RewardModelType>
            uint64_t ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefIdInGrid(
                    std::vector<Belief<ValueType>> &grid, uint32_t observation, std::vector<ValueType> probabilities) {
                for (auto const &belief : grid) {
                    if (belief.observation == observation && probabilities.size() == belief.probabilities.size()) {
                        if (belief.probabilities == probabilities) {
                            STORM_LOG_DEBUG("Found belief with id " << std::to_string(belief.id));
                            return belief.id;
                        }
                    }
                }
                STORM_LOG_DEBUG("Did not find the belief in the grid");
                return -1;
            }

            template<typename ValueType, typename RewardModelType>
            Belief<ValueType> ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getInitialBelief(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, uint64_t id) {
                STORM_LOG_ASSERT(pomdp.getInitialStates().getNumberOfSetBits() > 1,
                                 "POMDP contains more than one initial state");
                std::vector<ValueType> distribution(pomdp.getNumberOfStates(), storm::utility::zero<ValueType>());
                uint32_t observation = 0;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (pomdp.getInitialStates()[state] == 1) {
                        distribution[state] = storm::utility::one<ValueType>();
                        observation = pomdp.getObservation(state);
                    }
                }
                return Belief<ValueType>{id, observation, distribution};
            }

            template<typename ValueType, typename RewardModelType>
            void ApproximatePOMDPModelchecker<ValueType, RewardModelType>::constructBeliefGrid(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                    std::set<uint32_t> target_observations, uint64_t gridResolution,
                    std::vector<Belief<ValueType>> &grid, std::vector<bool> &beliefIsKnown) {
                bool isTarget;
                uint64_t newId = 0;

                for (uint32_t observation = 0; observation < pomdp.getNrObservations(); ++observation) {
                    std::vector<uint64_t> statesWithObservation = pomdp.getStatesWithObservation(observation);
                    isTarget = target_observations.find(observation) != target_observations.end();

                    // TODO this can probably be condensed
                    if (statesWithObservation.size() == 1) {
                        // If there is only one state with the observation, we can directly add the corresponding belief
                        std::vector<ValueType> distribution(pomdp.getNumberOfStates(),
                                                            storm::utility::zero<ValueType>());
                        distribution[statesWithObservation.front()] = storm::utility::one<ValueType>();
                        Belief<ValueType> belief = {newId, observation, distribution};
                        STORM_LOG_TRACE(
                                "Add Belief " << std::to_string(newId) << " [(" << std::to_string(observation) << "),"
                                              << distribution << "]");
                        grid.push_back(belief);
                        beliefIsKnown.push_back(isTarget);
                        ++newId;
                    } else {
                        // Otherwise we have to enumerate all possible distributions with regards to the grid
                        // helper is used to derive the distribution of the belief
                        std::vector<uint64_t> helper(statesWithObservation.size(), 0);
                        helper[0] = gridResolution;
                        bool done = false;
                        uint64_t index = 0;

                        while (!done) {
                            std::vector<ValueType> distribution(pomdp.getNumberOfStates(),
                                                                storm::utility::zero<ValueType>());
                            for (size_t i = 0; i < statesWithObservation.size() - 1; ++i) {
                                distribution[statesWithObservation[i]] = ValueType(
                                        double(helper[i] - helper[i + 1]) / gridResolution);
                            }
                            distribution[statesWithObservation.back()] = ValueType(
                                    double(helper[statesWithObservation.size() - 1]) / gridResolution);

                            Belief<ValueType> belief = {newId, observation, distribution};
                            STORM_LOG_TRACE(
                                    "Add Belief " << std::to_string(newId) << " [(" << std::to_string(observation)
                                                  << ")," << distribution << "]");
                            grid.push_back(belief);
                            beliefIsKnown.push_back(isTarget);
                            if (helper[statesWithObservation.size() - 1] == gridResolution) {
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

                std::vector<ValueType> x(probabilities.size(), storm::utility::zero<ValueType>());
                std::vector<ValueType> v(probabilities.size(), storm::utility::zero<ValueType>());
                std::vector<ValueType> d(probabilities.size(), storm::utility::zero<ValueType>());

                for (size_t i = 0; i < probabilities.size(); ++i) {
                    for (size_t j = i; j < probabilities.size(); ++j) {
                        x[i] += storm::utility::convertNumber<ValueType>(resolution) * probabilities[j];
                    }
                    v[i] = storm::utility::floor(x[i]);
                    d[i] = x[i] - v[i];
                }

                auto p = storm::utility::vector::getSortedIndices(d);

                std::vector<std::vector<ValueType>> qs;
                for (size_t i = 0; i < probabilities.size(); ++i) {
                    std::vector<ValueType> q;
                    if (i == 0) {
                        for (size_t j = 0; j < probabilities.size(); ++j) {
                            q[i] = v[i];
                        }
                        qs.push_back(q);
                    } else {
                        for (size_t j = 0; j < probabilities.size(); ++j) {
                            if (j == p[i - 1]) {
                                q[j] = qs[i - 1][j] + storm::utility::one<ValueType>();
                            } else {
                                q[j] = qs[i - 1][j];
                            }
                        }
                        qs.push_back(q);
                    }
                }

                std::vector<std::vector<ValueType>> subSimplex;
                for (auto q : qs) {
                    std::vector<ValueType> node;
                    for (size_t i = 0; i < probabilities.size(); ++i) {
                        if (i != probabilities.size() - 1) {
                            node.push_back((q[i] - q[i + 1]) / storm::utility::convertNumber<ValueType>(resolution));
                        } else {
                            node.push_back(q[i] / storm::utility::convertNumber<ValueType>(resolution));
                        }
                    }
                    subSimplex.push_back(node);
                }

                std::vector<ValueType> lambdas(probabilities.size(), storm::utility::zero<ValueType>());
                auto sum = storm::utility::zero<ValueType>();
                for (size_t i = 1; i < probabilities.size(); ++i) {
                    lambdas[i] = d[p[i - 1]] - d[p[i]];
                    sum += d[p[i - 1]] - d[p[i]];
                }
                lambdas[0] = storm::utility::one<ValueType>() - sum;

                //TODO add assertion that we are close enough
                return std::make_pair(subSimplex, lambdas);
            }


            template<typename ValueType, typename RewardModelType>
            std::map<uint32_t, ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::computeObservationProbabilitiesAfterAction(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, Belief<ValueType> belief,
                    uint64_t actionIndex) {
                std::map<uint32_t, ValueType> res;
                // the id is not important here as we immediately discard the belief (very hacky, I don't like it either)
                std::vector<ValueType> postProbabilities = getBeliefAfterAction(pomdp, belief, actionIndex,
                                                                                0).probabilities;
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
            Belief<ValueType> ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefAfterAction(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, Belief<ValueType> belief,
                    uint64_t actionIndex, uint64_t id) {
                std::vector<ValueType> distributionAfter(pomdp.getNumberOfStates(), storm::utility::zero<ValueType>());
                uint32_t observation = 0;
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (belief.probabilities[state] != storm::utility::zero<ValueType>()) {
                        auto row = pomdp.getTransitionMatrix().getRow(
                                pomdp.getChoiceIndex(storm::storage::StateActionPair(state, actionIndex)));
                        for (auto const &entry : row) {
                            observation = pomdp.getObservation(entry.getColumn());
                            distributionAfter[entry.getColumn()] += belief.probabilities[state] * entry.getValue();
                        }
                    }
                }
                /* Should not be necessary
                // We have to normalize the distribution
                auto sum = storm::utility::zero<ValueType>();
                for(ValueType const& entry : distributionAfter){
                    sum += entry;
                }
                for(size_t i = 0; i  < pomdp.getNumberOfStates(); ++i){
                    distributionAfter[i] /= sum;
                }*/
                return Belief<ValueType>{id, observation, distributionAfter};
            }

            template<typename ValueType, typename RewardModelType>
            Belief<ValueType>
            ApproximatePOMDPModelchecker<ValueType, RewardModelType>::getBeliefAfterActionAndObservation(
                    storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, Belief<ValueType> belief,
                    uint64_t actionIndex, uint32_t observation, uint64_t id) {
                std::vector<ValueType> distributionAfter(pomdp.getNumberOfStates(), storm::utility::zero<ValueType>());
                for (uint64_t state = 0; state < pomdp.getNumberOfStates(); ++state) {
                    if (belief.probabilities[state] != storm::utility::zero<ValueType>()) {
                        auto row = pomdp.getTransitionMatrix().getRow(
                                pomdp.getChoiceIndex(storm::storage::StateActionPair(state, actionIndex)));
                        for (auto const &entry : row) {
                            if (pomdp.getObservation(entry.getColumn()) == observation) {
                                distributionAfter[entry.getColumn()] += belief.probabilities[state] * entry.getValue();
                            }
                        }
                    }
                }
                // We have to normalize the distribution
                auto sum = storm::utility::zero<ValueType>();
                for (ValueType const &entry : distributionAfter) {
                    sum += entry;
                }
                for (size_t i = 0; i < pomdp.getNumberOfStates(); ++i) {
                    distributionAfter[i] /= sum;
                }
                return Belief<ValueType>{id, observation, distributionAfter};
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