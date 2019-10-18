#include <cstdlib>
#include "storm/api/storm.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"
#include "storm-pomdp/storage/Belief.h"

#include "storm/storage/jani/Property.h"

namespace storm {
    namespace pomdp {
        namespace modelchecker {
            template<class ValueType>
            struct POMDPCheckResult {
                ValueType OverapproximationValue;
                ValueType UnderapproximationValue;
            };

            template<class ValueType, typename RewardModelType = models::sparse::StandardRewardModel<ValueType>>
            class ApproximatePOMDPModelchecker {
            public:
                explicit ApproximatePOMDPModelchecker();

                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityProbability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                               std::set<uint32_t> targetObservations, bool min,
                                               uint64_t gridResolution);

                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityReward(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                          std::set<uint32_t> targetObservations, bool min,
                                          uint64_t gridResolution);

            private:
                /**
                 * TODO
                 * @param pomdp
                 * @param beliefList
                 * @param observationProbabilities
                 * @param nextBelieves
                 * @param result
                 * @param gridResolution
                 * @param currentBeliefId
                 * @param nextId
                 * @param min
                 * @return
                 */
                uint64_t extractBestAction(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                           std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                           std::vector<bool> &beliefIsTarget,
                                           std::set<uint32_t> &target_observations,
                                           std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                           std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                           std::map<uint64_t, ValueType> &result,
                                           uint64_t gridResolution, uint64_t currentBeliefId, uint64_t nextId,
                                           bool min);

                /**
                 * TODO
                 * @param pomdp
                 * @param beliefList
                 * @param beliefIsTarget
                 * @param targetObservations
                 * @param observationProbabilities
                 * @param nextBelieves
                 * @param result
                 * @param chosenActions
                 * @param gridResolution
                 * @param initialBeliefId
                 * @param min
                 * @return
                 */
                ValueType computeUnderapproximationWithDTMC(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                            std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                            std::vector<bool> &beliefIsTarget,
                                                            std::set<uint32_t> &targetObservations,
                                                            std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                                            std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                                            std::map<uint64_t, ValueType> &result,
                                                            std::map<uint64_t, uint64_t> chosenActions,
                                                            uint64_t gridResolution, uint64_t initialBeliefId, bool min);

                ValueType computeUnderapproximationWithMDP(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                                           std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                           std::vector<bool> &beliefIsTarget,
                                                           std::set<uint32_t> &targetObservations,
                                                           std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                                           std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                                           std::map<uint64_t, ValueType> &result,
                                                           std::map<uint64_t, uint64_t> chosenActions,
                                                           uint64_t gridResolution, uint64_t initialBeliefId, bool min);

                /**
                 *
                 * @param pomdp
                 * @param id
                 * @return
                 */
                storm::pomdp::Belief<ValueType>
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
                                         std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                         std::vector<storm::pomdp::Belief<ValueType>> &grid,
                                         std::vector<bool> &beliefIsKnown, uint64_t nextId);


                /**
                 * Helper method to get the probabilities of each observation after performing an action
                 *
                 * @param pomdp
                 * @param belief
                 * @param actionIndex
                 * @return
                 */
                std::map<uint32_t, ValueType> computeObservationProbabilitiesAfterAction(
                        storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                        storm::pomdp::Belief<ValueType> belief,
                        uint64_t actionIndex);

                /**
                 * Helper method to get the id of the next belief that results from a belief by performing an action and observing an observation.
                 * If the belief does not exist yet, it is created and added to the list of all beliefs
                 *
                 * @param pomdp the POMDP on which the evaluation should be performed
                 * @param belief the starting belief
                 * @param actionIndex the index of the action to be performed
                 * @param observation the observation after the action was performed
                 * @return the resulting belief (observation and distribution)
                 */
                uint64_t
                getBeliefAfterActionAndObservation(
                        storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                        std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                        std::vector<bool> &beliefIsTarget,
                        std::set<uint32_t> &targetObservations,
                        storm::pomdp::Belief<ValueType> belief,
                        uint64_t actionIndex, uint32_t observation, uint64_t id);

                /**
                 * Helper method to get the next belief that results from a belief by performing an action
                 *
                 * @param pomdp
                 * @param belief
                 * @param actionIndex
                 * @return
                 */
                storm::pomdp::Belief<ValueType>
                getBeliefAfterAction(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                     storm::pomdp::Belief<ValueType> belief, uint64_t actionIndex, uint64_t id);

                /**
                 * Helper to get the id of a Belief stored in a given vector structure
                 *
                 * @param observation
                 * @param probabilities
                 * @return
                 */
                uint64_t getBeliefIdInVector(std::vector<storm::pomdp::Belief<ValueType>> const &grid, uint32_t observation,
                                             std::vector<ValueType> probabilities);

                storm::storage::SparseMatrix<ValueType>
                buildTransitionMatrix(std::vector<std::map<uint64_t, ValueType>> transitions);

                storm::storage::SparseMatrix<ValueType>
                buildTransitionMatrix(std::vector<std::vector<std::map<uint64_t, ValueType>>> transitions);

                ValueType getRewardAfterAction(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp, uint64_t action, storm::pomdp::Belief<ValueType> belief);

                storm::utility::ConstantsComparator<ValueType> cc;
            };

        }
    }
}