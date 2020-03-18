#include <cstdlib>
#include "storm/api/storm.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"
#include "storm-pomdp/storage/Belief.h"
#include <boost/bimap.hpp>

#include "storm/storage/jani/Property.h"

namespace storm {
    namespace logic {
        class Formula;
    }
    
    namespace pomdp {
        namespace modelchecker {
            typedef boost::bimap<uint64_t, uint64_t> bsmap_type;

            template<class ValueType>
            struct POMDPCheckResult {
                ValueType overApproxValue;
                ValueType underApproxValue;
            };

            /**
             *  Struct containing information which is supposed to be persistent over multiple refinement steps
             *
             */
            template<class ValueType, typename RewardModelType = models::sparse::StandardRewardModel<ValueType>>
            struct RefinementComponents {
                std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> overApproxModelPtr;
                ValueType overApproxValue;
                ValueType underApproxValue;
                std::map<uint64_t, ValueType> overApproxMap;
                std::map<uint64_t, ValueType> underApproxMap;
                std::vector<storm::pomdp::Belief<ValueType>> beliefList;
                std::vector<storm::pomdp::Belief<ValueType>> beliefGrid;
                std::vector<bool> beliefIsTarget;
                bsmap_type overApproxBeliefStateMap;
                bsmap_type underApproxBeliefStateMap;
                uint64_t initialBeliefId;
            };

            template<class ValueType, typename RewardModelType = models::sparse::StandardRewardModel<ValueType>>
            struct UnderApproxComponents {
                ValueType underApproxValue;
                std::map<uint64_t, ValueType> underApproxMap;
                bsmap_type underApproxBeliefStateMap;
            };

            template<class ValueType, typename RewardModelType = models::sparse::StandardRewardModel<ValueType>>
            class ApproximatePOMDPModelchecker {
            public:
                
                struct Options {
                    Options();
                    uint64_t  initialGridResolution; /// Decides how precise the bounds are
                    ValueType explorationThreshold; /// the threshold for exploration stopping. If the difference between over- and underapproximation for a state is smaller than the threshold, stop exploration of the state
                    bool doRefinement; /// Sets whether the bounds should be refined automatically until the refinement precision is reached
                    ValueType refinementPrecision; /// Used to decide when the refinement should terminate
                    ValueType numericPrecision; /// Used to decide whether two values are equal
                };
                
                ApproximatePOMDPModelchecker(storm::models::sparse::Pomdp<ValueType, RewardModelType> const& pomdp, Options options = Options());
                
                std::unique_ptr<POMDPCheckResult<ValueType>> check(storm::logic::Formula const& formula);

            private:
                /**
                 * Compute the reachability probability of given target observations on a POMDP using the automatic refinement loop
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum probability is to be computed
                 * @return A struct containing the final overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                refineReachabilityProbability(std::set<uint32_t> const &targetObservations, bool min);

                /**
                 * Compute the reachability probability of given target observations on a POMDP for the given resolution only.
                 * On-the-fly state space generation is used for the overapproximation
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum probability is to be computed
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityProbabilityOTF(std::set<uint32_t> const &targetObservations, bool min);

                /**
                 * Compute the reachability rewards for given target observations on a POMDP for the given resolution only.
                 * On-the-fly state space generation is used for the overapproximation
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum rewards are to be computed
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityRewardOTF(std::set<uint32_t> const &targetObservations, bool min);

                // TODO: Check if this is obsolete
                /**
                 * Compute the reachability probability for given target observations on a POMDP for the given resolution only.
                 * Static state space generation is used for the overapproximation, i.e. the whole grid is generated
                 *
                 * @param pomdp the POMDP to be checked
                 * @param targetObservations the set of observations to be reached
                 * @param min true if the minimum probability is to be computed
                 * @param gridResolution the initial grid resolution
                 * @return A struct containing the final overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityProbability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                               std::set<uint32_t> const &targetObservations, bool min,
                                               uint64_t gridResolution);

                // TODO: Check if this is obsolete
                /**
                 * Compute the reachability rewards for given target observations on a POMDP for the given resolution only.
                 * Static state space generation is used for the overapproximation, i.e. the whole grid is generated
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if the minimum rewards are to be computed
                 * @param gridResolution the initial grid resolution
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityReward(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                          std::set<uint32_t> const &targetObservations, bool min,
                                          uint64_t gridResolution);

            private:
                /**
                 * Helper method to compute the inital step of the refinement loop
                 *
                 * @param targetObservations set of target observations
                 * @param min true if minimum value is to be computed
                 * @param observationResolutionVector vector containing the resolution to be used for each observation
                 * @param computeRewards true if rewards are to be computed, false if probability is computed
                 * @param overApproximationMap optional mapping of original POMDP states to a naive overapproximation value
                 * @param underApproximationMap optional mapping of original POMDP states to a naive underapproximation value
                 * @param maxUaModelSize the maximum size of the underapproximation model to be generated
                 * @return struct containing components generated during the computation to be used in later refinement iterations
                 */
                std::shared_ptr<RefinementComponents<ValueType>>
                computeFirstRefinementStep(std::set<uint32_t> const &targetObservations, bool min, std::vector<uint64_t> &observationResolutionVector,
                                           bool computeRewards, boost::optional<std::map<uint64_t, ValueType>> overApproximationMap = boost::none,
                                           boost::optional<std::map<uint64_t, ValueType>> underApproximationMap = boost::none, uint64_t maxUaModelSize = 200);

                std::shared_ptr<RefinementComponents<ValueType>>
                computeRefinementStep(std::set<uint32_t> const &targetObservations, bool min, std::vector<uint64_t> &observationResolutionVector,
                                      bool computeRewards, std::shared_ptr<RefinementComponents<ValueType>> refinementComponents,
                                      std::set<uint32_t> changedObservations,
                                      boost::optional<std::map<uint64_t, ValueType>> overApproximationMap = boost::none,
                                      boost::optional<std::map<uint64_t, ValueType>> underApproximationMap = boost::none, uint64_t maxUaModelSize = 200);

                /**
                 * Helper method that handles the computation of reachability probabilities and rewards using the on-the-fly state space generation for a fixed grid size
                 *
                 * @param targetObservations set of target observations
                 * @param min true if minimum value is to be computed
                 * @param observationResolutionVector vector containing the resolution to be used for each observation
                 * @param computeRewards true if rewards are to be computed, false if probability is computed
                 * @param overApproximationMap optional mapping of original POMDP states to a naive overapproximation value
                 * @param underApproximationMap optional mapping of original POMDP states to a naive underapproximation value
                 * @param maxUaModelSize the maximum size of the underapproximation model to be generated
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachabilityOTF(std::set<uint32_t> const &targetObservations, bool min,
                                       std::vector<uint64_t> &observationResolutionVector, bool computeRewards,
                                       boost::optional<std::map<uint64_t, ValueType>> overApproximationMap = boost::none,
                                       boost::optional<std::map<uint64_t, ValueType>> underApproximationMap = boost::none, uint64_t maxUaModelSize = 200);

                // TODO: Check if this is obsolete
                /**
                 * Helper method to compute reachability properties using static state space generation
                 *
                 * @param targetObservations set of target observations
                 * @param min true if minimum value is to be computed
                 * @param gridResolution the resolution of the grid to be used
                 * @param computeRewards true if rewards are to be computed, false if probability is computed
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                std::unique_ptr<POMDPCheckResult<ValueType>>
                computeReachability(storm::models::sparse::Pomdp<ValueType, RewardModelType> const &pomdp,
                                    std::set<uint32_t> const &targetObservations, bool min,
                                    uint64_t gridResolution, bool computeRewards);

                /**
                 * Helper to compute an underapproximation of the reachability property.
                 * The implemented method unrolls the belief support of the given POMDP up to a given number of belief states.
                 *
                 * @param beliefList vector containing already generated beliefs
                 * @param beliefIsTarget vector containinf for each belief in beliefList true if the belief is a target
                 * @param targetObservations set of target observations
                 * @param initialBeliefId Id of the belief corresponding to the POMDP's initial state
                 * @param min true if minimum value is to be computed
                 * @param computeReward true if rewards are to be computed
                 * @param maxModelSize number of states up until which the belief support should be unrolled
                 * @return struct containing the components generated during the under approximation
                 */
                std::unique_ptr<UnderApproxComponents<ValueType, RewardModelType>> computeUnderapproximation(std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                                                                             std::vector<bool> &beliefIsTarget,
                                                                                                             std::set<uint32_t> const &targetObservations,
                                                                                                             uint64_t initialBeliefId, bool min, bool computeReward,
                                                                                                             uint64_t maxModelSize);

                /**
                 * Constructs the initial belief for the given POMDP
                 *
                 * @param pomdp the POMDP
                 * @param id the id the initial belief is given
                 * @return a belief representing the initial belief
                 */
                storm::pomdp::Belief<ValueType>
                getInitialBelief(uint64_t id);


                /**
                 * Subroutine to compute the subsimplex a given belief is contained in and the corresponding lambda values necessary for the Freudenthal triangulation
                 *
                 * @param probabilities the probability distribution of the belief
                 * @param gridResolution the resolution used for the belief
                 * @param nrStates number of states in the POMDP
                 * @return a pair containing: 1) the subsimplices 2) the lambda values
                 */
                std::pair<std::vector<std::map<uint64_t, ValueType>>, std::vector<ValueType>>
                computeSubSimplexAndLambdas(std::map<uint64_t, ValueType> &probabilities, uint64_t gridResolution, uint64_t nrStates);


                /**
                 * Helper method to construct the static belief grid for the POMDP overapproximation
                 *
                 * @param target_observations set of target observations
                 * @param gridResolution the resolution of the grid to be constructed
                 * @param beliefList data structure to store all generated beliefs
                 * @param grid data structure to store references to the grid beliefs specifically
                 * @param beliefIsTarget vector containing true if the corresponding belief in the beleif list is a target belief
                 * @param nextId the ID to be used for the next generated belief
                 */
                void constructBeliefGrid(std::set<uint32_t> const &target_observations, uint64_t gridResolution,
                                         std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                         std::vector<storm::pomdp::Belief<ValueType>> &grid,
                                         std::vector<bool> &beliefIsTarget, uint64_t nextId);


                /**
                 * Helper method to get the probabilities to be in a state with each observation after performing an action
                 *
                 * @param belief the belief in which the action is performed
                 * @param actionIndex the index of the action to be performed
                 * @return mapping from each observation to the probability to be in a state with that observation after performing the action
                 */
                std::map<uint32_t, ValueType> computeObservationProbabilitiesAfterAction(storm::pomdp::Belief<ValueType> &belief,
                        uint64_t actionIndex);

                /**
                 * Helper method to get the id of the next belief that results from a belief by performing an action and observing an observation.
                 * If the belief does not exist yet, it is created and added to the list of all beliefs
                 *
                 * @param beliefList data structure to store all generated beliefs
                 * @param beliefIsTarget vector containing true if the corresponding belief in the beleif list is a target belief
                 * @param targetObservations set of target observations
                 * @param belief the starting belief
                 * @param actionIndex the index of the action to be performed
                 * @param observation the observation after the action was performed
                 * @return the resulting belief (observation and distribution)
                 */
                uint64_t getBeliefAfterActionAndObservation(std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                        std::vector<bool> &beliefIsTarget,
                        std::set<uint32_t> const &targetObservations,
                        storm::pomdp::Belief<ValueType> &belief,
                        uint64_t actionIndex, uint32_t observation, uint64_t id);

                /**
                 * Helper method to generate the next belief that results from a belief by performing an action
                 *
                 * @param belief the starting belief
                 * @param actionIndex the index of the action to be performed
                 * @param id the ID for the generated belief
                 * @return a belief object representing the belief after performing the action in the starting belief
                 */
                storm::pomdp::Belief<ValueType>
                getBeliefAfterAction(storm::pomdp::Belief<ValueType> &belief, uint64_t actionIndex, uint64_t id);

                /**
                 * Helper to get the id of a Belief stored in a given vector structure
                 *
                 * @param grid the vector on which the lookup is performed
                 * @param observation the observation of the belief
                 * @param probabilities the probability distribution over the POMDP states of the Belief
                 * @return if the belief was found in the vector, the belief's ID, otherwise -1
                 */
                uint64_t getBeliefIdInVector(std::vector<storm::pomdp::Belief<ValueType>> const &grid, uint32_t observation,
                                             std::map<uint64_t, ValueType> &probabilities);

                /**
                 * Helper method to build the transition matrix from a data structure containing transations
                 *
                 * @param transitions  data structure that contains the transition information of the form: origin-state -> action -> (successor-state -> probability)
                 * @return sparseMatrix representing the transitions
                 */
                storm::storage::SparseMatrix<ValueType> buildTransitionMatrix(std::vector<std::vector<std::map<uint64_t, ValueType>>> &transitions);

                /**
                 * Get the reward for performing an action in a given belief
                 *
                 * @param action the index of the action to be performed
                 * @param belief the belief in which the action is performed
                 * @return the reward earned by performing the action in the belief
                 */
                ValueType getRewardAfterAction(uint64_t action, storm::pomdp::Belief<ValueType> &belief);


                /**
                 * Helper method for value iteration on data structures representing the belief grid
                 * This is very close to the method implemented in PRISM POMDP
                 *
                 * @param pomdp The POMDP
                 * @param beliefList data structure to store all generated beliefs
                 * @param beliefGrid data structure to store references to the grid beliefs specifically
                 * @param beliefIsTarget vector containing true if the corresponding belief in the beleif list is a target belief
                 * @param observationProbabilities data structure containing for each belief and possible action the probability to go to a state with a given observation
                 * @param nextBelieves data structure containing for each belief the successor belief after performing an action and observing a given observation
                 * @param beliefActionRewards data structure containing for each belief and possible action the reward for performing the action
                 * @param subSimplexCache caching data structure to store already computed subsimplices
                 * @param lambdaCache caching data structure to store already computed lambda values
                 * @param result data structure to store result values for each grid state
                 * @param chosenActions data structure to store the action(s) that lead to the computed result value
                 * @param gridResolution the resolution of the grid
                 * @param min true if minimal values are to be computed
                 * @param computeRewards true if rewards are to be computed
                 * @return the resulting probability/reward in the initial state
                 */
                ValueType
                overApproximationValueIteration(std::vector<storm::pomdp::Belief<ValueType>> &beliefList,
                                                std::vector<storm::pomdp::Belief<ValueType>> &beliefGrid, std::vector<bool> &beliefIsTarget,
                                                std::map<uint64_t, std::vector<std::map<uint32_t, ValueType>>> &observationProbabilities,
                                                std::map<uint64_t, std::vector<std::map<uint32_t, uint64_t>>> &nextBelieves,
                                                std::map<uint64_t, std::vector<ValueType>> &beliefActionRewards,
                                                std::map<uint64_t, std::vector<std::map<uint64_t, ValueType>>> &subSimplexCache,
                                                std::map<uint64_t, std::vector<ValueType>> &lambdaCache, std::map<uint64_t, ValueType> &result,
                                                std::map<uint64_t, std::vector<uint64_t>> &chosenActions,
                                                uint64_t gridResolution, bool min, bool computeRewards);

                storm::models::sparse::Pomdp<ValueType, RewardModelType> const& pomdp;
                Options options;
                storm::utility::ConstantsComparator<ValueType> cc;
                // TODO: these should be obsolete, right?
                bool useMdp;
                bool cacheSubsimplices;
                uint64_t maxIterations;
            };

        }
    }
}