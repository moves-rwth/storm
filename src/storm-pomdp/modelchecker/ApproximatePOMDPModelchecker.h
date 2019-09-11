#include <cstdlib>
#include "storm/modelchecker/CheckTask.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            class POMDPCheckResult;

            // Structure used to represent a belief
            template<typename ValueType>
            struct Belief {
                uint64_t id;
                uint32_t observation;
                //TODO make this sparse?
                std::vector<ValueType> probabilities;
            };

            template<class ValueType, typename RewardModelType = models::sparse::StandardRewardModel <ValueType>>
            class ApproximatePOMDPModelchecker {
            public:
                explicit ApproximatePOMDPModelchecker();

                /*std::unique_ptr<POMDPCheckResult>*/ void
                computeReachabilityProbability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                               std::set<uint32_t> target_observations, bool min,
                                               uint64_t gridResolution);

                std::unique_ptr<POMDPCheckResult>
                computeReachabilityReward(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                          std::set<uint32_t> target_observations, uint64_t gridResolution);

            private:
                /**
                 *
                 * @param pomdp
                 * @param id
                 * @return
                 */
                Belief<ValueType>
                getInitialBelief(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, uint64_t id);


                /**
                 *
                 * @param probabilities
                 * @param gridResolution
                 * @return
                 */
                std::pair<std::vector<std::vector<ValueType>>, std::vector<ValueType>>
                computeSubSimplexAndLambdas(std::vector<ValueType> probabilities, uint64_t gridResolution);


                /**
                 *  Helper method to construct the grid of Belief states to approximate the POMDP
                 *
                 * @param pomdp
                 * @param gridResolution
                 *
                 */
                void constructBeliefGrid(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                         std::set<uint32_t> target_observations, uint64_t gridResolution,
                                         std::vector<Belief<ValueType>> &grid, std::vector<bool> &beliefIsKnown);


                /**
                 * Helper method to get the probabilities of each observation after performing an action
                 *
                 * @param pomdp
                 * @param belief
                 * @param actionIndex
                 * @return
                 */
                std::map<uint32_t, ValueType> computeObservationProbabilitiesAfterAction(
                        storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, Belief<ValueType> belief,
                        uint64_t actionIndex);

                /**
                 * Helper method to get the next belief that results from a belief by performing an action and observing an observation
                 *
                 * @param pomdp the POMDP on which the evaluation should be performed
                 * @param belief the starting belief
                 * @param actionIndex the index of the action to be performed
                 * @param observation the observation after the action was performed
                 * @return the resulting belief (observation and distribution)
                 */
                Belief<ValueType>
                getBeliefAfterActionAndObservation(const models::sparse::Pomdp <ValueType, RewardModelType> &pomdp,
                                                   Belief<ValueType> belief,
                                                   uint64_t actionIndex, uint32_t observation, uint64_t id);

                /**
                 * Helper method to get the next belief that results from a belief by performing an action
                 *
                 * @param pomdp
                 * @param belief
                 * @param actionIndex
                 * @return
                 */
                Belief<ValueType>
                getBeliefAfterAction(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                     Belief<ValueType> belief, uint64_t actionIndex, uint64_t id);

                /**
                 * Helper to get the id of a Belief in the grid
                 *
                 * @param observation
                 * @param probabilities
                 * @return
                 */
                uint64_t getBeliefIdInGrid(std::vector<Belief<ValueType>> &grid, uint32_t observation,
                                           std::vector<ValueType> probabilities);
            };

        }
    }
}