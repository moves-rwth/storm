#include "storm/api/storm.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"
#include "storm-pomdp/storage/BeliefManager.h"
#include "storm-pomdp/modelchecker/BeliefExplorationPomdpModelCheckerOptions.h"
#include "storm-pomdp/builder/BeliefMdpExplorer.h"

#include "storm/storage/jani/Property.h"

namespace storm {
    namespace logic {
        class Formula;
    }
    
    namespace pomdp {
        namespace modelchecker {
            
            template<typename ValueType>
            struct PreprocessingPomdpValueBounds;

            template<typename ValueType>
            struct POMDPValueBounds{
                storm::pomdp::modelchecker::PreprocessingPomdpValueBounds<ValueType> trivialPomdpValueBounds;
                storm::pomdp::modelchecker::ExtremePOMDPValueBound<ValueType> extremePomdpValueBound;
                std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>> fmSchedulerValueList;
            };
            
            template<typename PomdpModelType, typename BeliefValueType = typename PomdpModelType::ValueType, typename BeliefMDPType = typename PomdpModelType::ValueType>
            class BeliefExplorationPomdpModelChecker {
            public:
                typedef BeliefMDPType ValueType;
                typedef typename PomdpModelType::RewardModelType RewardModelType;
                typedef storm::storage::BeliefManager<PomdpModelType, BeliefValueType> BeliefManagerType;
                typedef storm::builder::BeliefMdpExplorer<PomdpModelType, BeliefValueType> ExplorerType;
                typedef BeliefExplorationPomdpModelCheckerOptions<ValueType> Options;

                enum class Status {
                    Uninitialized,
                    Exploring,
                    ModelExplorationFinished,
                    ResultAvailable,
                    Terminated,
                };
                
                struct Result {
                    Result(ValueType lower, ValueType upper);
                    ValueType lowerBound;
                    ValueType upperBound;
                    ValueType diff (bool relative = false) const;
                    bool updateLowerBound(ValueType const& value);
                    bool updateUpperBound(ValueType const& value);
                    std::shared_ptr<storm::models::sparse::Model<ValueType>> schedulerAsMarkovChain;
                    std::vector<storm::storage::Scheduler<ValueType>> cutoffSchedulers;
                };
                
                BeliefExplorationPomdpModelChecker(std::shared_ptr<PomdpModelType> pomdp, Options options = Options());
                
                Result check(storm::logic::Formula const& formula, std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>> const& additionalUnderApproximationBounds = std::vector<std::vector<std::unordered_map<uint64_t,ValueType>>>());

                void printStatisticsToStream(std::ostream& stream) const;

                void precomputeValueBounds(const logic::Formula& formula, storm::solver::MinMaxMethod minMaxMethod = storm::solver::MinMaxMethod::SoundValueIteration);

                void unfoldInteractively(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::POMDPValueBounds<ValueType> const & valueBounds, Result &result);

                void pauseUnfolding();

                void continueUnfolding();

                void terminateUnfolding();

                bool isResultReady();

                bool isExploring();

                Result getInteractiveResult();

                std::shared_ptr<ExplorerType> getInteractiveBeliefExplorer();

            private:
                 enum class UnfoldingControl {
                     Run,
                     Pause,
                     Terminate
                 };
                
                /**
                 * Returns the pomdp that is to be analyzed
                 */
                PomdpModelType const& pomdp() const;
                
                /**
                 * Helper method that handles the computation of reachability probabilities and rewards using the on-the-fly state space generation for a fixed grid size
                 *
                 * @param targetObservations set of target observations
                 * @param min true if minimum value is to be computed
                 * @param computeRewards true if rewards are to be computed, false if probability is computed
                 * @param overApproximationMap optional mapping of original POMDP states to a naive overapproximation value
                 * @param underApproximationMap optional mapping of original POMDP states to a naive underapproximation value
                 * @param maxUaModelSize the maximum size of the underapproximation model to be generated
                 * @return A struct containing the overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                void computeReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::POMDPValueBounds<ValueType> const& valueBounds, Result& result);
                
                
                /**
                 * Compute the reachability probability of given target observations on a POMDP using the automatic refinement loop
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum probability is to be computed
                 * @return A struct containing the final overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                void refineReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::POMDPValueBounds<ValueType> const& valueBounds, Result& result);
                
                struct HeuristicParameters {
                    ValueType gapThreshold;
                    ValueType observationThreshold;
                    uint64_t sizeThreshold;
                    ValueType clippingThreshold;
                    ValueType optimalChoiceValueEpsilon;
                };
                
                /**
                 * Builds and checks an MDP that over-approximates the POMDP behavior, i.e. provides an upper bound for maximizing and a lower bound for minimizing properties
                 * Returns true if a fixpoint for the refinement has been detected (i.e. if further refinement steps would not change the mdp)
                 */
                bool buildOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, HeuristicParameters const& heuristicParameters, std::vector<BeliefValueType>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& overApproximation);

                /**
                 * Builds and checks an MDP that under-approximates the POMDP behavior, i.e. provides a lower bound for maximizing and an upper bound for minimizing properties
                 * Returns true if a fixpoint for the refinement has been detected (i.e. if further refinement steps would not change the mdp)
                 */
                bool buildUnderApproximation(std::set<uint32_t> const& targetObservations, bool min, bool computeRewards, bool refine,
                                             HeuristicParameters const& heuristicParameters, std::shared_ptr<BeliefManagerType>& beliefManager,
                                             std::shared_ptr<ExplorerType>& underApproximation, bool interactive);

                /**
                 * Clips the belief with the given state ID to a belief grid by clipping its direct successor ("grid clipping")
                 * Transitions to explored successors and successors on the grid are added, otherwise successors are not generated
                 * @param clippingStateId the state ID of the clipping belief
                 * @param computeRewards true, if rewards are computed
                 * @param min true, if objective is to minimise
                 * @param beliefManager the belief manager used
                 * @param beliefExplorer the belief MDP explorer used
                 */
                void clipToGrid(uint64_t clippingStateId, bool computeRewards, bool min, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer);

                /**
                 * Clips the belief with the given state ID to a belief grid.
                 * If a new candidate is added to the belief space, it is expanded. If necessary, its direct successors are added to the exploration queue to be handled by the main exploration routine.
                 * @param clippingStateId the state ID of the clipping belief
                 * @param computeRewards true, if rewards are computed
                 * @param min true, if objective is to minimise
                 * @param beliefManager the belief manager used
                 * @param beliefExplorer the belief MDP explorer used
                 */
                bool clipToGridExplicitly(uint64_t clippingStateId, bool computeRewards, bool min, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer, uint64_t localActionIndex);

                /**
                 * Clips the belief with the given state ID using already explored beliefs as candidates ("classic clipping")
                 * A clipping threshold can be given and a reduction of the candidate set to a given size using the belief difference 1-norm is applied if it is not disabled
                 * @param clippingStateId the state ID of the clipping belief
                 * @param threshold clipping threshold
                 * @param computeRewards true, if rewards are computed
                 * @param reducedCandidateSetSize target candidate set size of the differnce 1-norm reduction
                 * @param beliefManager the belief manager used
                 * @param beliefExplorer the belief MDP explorer used
                 * @return true, if clipping using an adequate candidate is succcessful. Otherwise, false.
                 */
                bool clipToExploredBeliefs(uint64_t clippingStateId, BeliefValueType threshold, bool computeRewards, uint64_t reducedCandidateSetSize, std::shared_ptr<BeliefManagerType> &beliefManager, std::shared_ptr<ExplorerType> &beliefExplorer);

                /**
                 * Heuristically rates the quality of the approximation described by the given successor observation info.
                 * Here, 0 means a bad approximation and 1 means a good approximation.
                 */
                BeliefValueType rateObservation(typename ExplorerType::SuccessorObservationInformation const& info, BeliefValueType const& observationResolution, BeliefValueType const& maxResolution);
                
                std::vector<BeliefValueType> getObservationRatings(std::shared_ptr<ExplorerType> const& overApproximation, std::vector<BeliefValueType> const& observationResolutionVector);

                void setUnfoldingControl(UnfoldingControl newUnfoldingControl);

                struct Statistics {
                    Statistics();
                    boost::optional<uint64_t> refinementSteps;
                    storm::utility::Stopwatch totalTime;
                    
                    bool beliefMdpDetectedToBeFinite;
                    bool refinementFixpointDetected;
                    
                    boost::optional<uint64_t> overApproximationStates;
                    bool overApproximationBuildAborted;
                    storm::utility::Stopwatch overApproximationBuildTime;
                    storm::utility::Stopwatch overApproximationCheckTime;
                    boost::optional<BeliefValueType> overApproximationMaxResolution;
                    
                    boost::optional<uint64_t> underApproximationStates;
                    bool underApproximationBuildAborted;
                    storm::utility::Stopwatch underApproximationBuildTime;
                    storm::utility::Stopwatch underApproximationCheckTime;
                    boost::optional<uint64_t> underApproximationStateLimit;
                    boost::optional<uint64_t> nrClippingAttempts;
                    boost::optional<uint64_t> nrClippedStates;
                    boost::optional<uint64_t> nrTruncatedStates;
                    storm::utility::Stopwatch clipWatch;
                    storm::utility::Stopwatch clippingPreTime;
                    
                    bool aborted;
                };
                Statistics statistics;
                
                std::shared_ptr<PomdpModelType> inputPomdp;
                std::shared_ptr<PomdpModelType> preprocessedPomdp;
                
                Options options;
                storm::utility::ConstantsComparator<BeliefValueType> beliefTypeCC;
                storm::utility::ConstantsComparator<ValueType> valueTypeCC;

                storm::pomdp::modelchecker::POMDPValueBounds<ValueType> pomdpValueBounds;
                std::shared_ptr<ExplorerType> interactiveUnderApproximationExplorer;
                Status unfoldingStatus;
                UnfoldingControl unfoldingControl;
                Result interactiveResult = Result(-storm::utility::infinity<ValueType>(), storm::utility::infinity<ValueType>());
            };

        }
    }
}