#include "storm/api/storm.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"
#include "storm-pomdp/storage/Belief.h"
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
            struct TrivialPomdpValueBounds;
            
            template<typename PomdpModelType, typename BeliefValueType = typename PomdpModelType::ValueType>
            class BeliefExplorationPomdpModelChecker {
            public:
                typedef typename PomdpModelType::ValueType ValueType;
                typedef typename PomdpModelType::RewardModelType RewardModelType;
                typedef storm::storage::BeliefManager<PomdpModelType, BeliefValueType> BeliefManagerType;
                typedef storm::builder::BeliefMdpExplorer<PomdpModelType, BeliefValueType> ExplorerType;
                typedef BeliefExplorationPomdpModelCheckerOptions<ValueType> Options;
                
                struct Result {
                    Result(ValueType lower, ValueType upper);
                    ValueType lowerBound;
                    ValueType upperBound;
                    ValueType diff (bool relative = false) const;
                    bool updateLowerBound(ValueType const& value);
                    bool updateUpperBound(ValueType const& value);
                };
                
                BeliefExplorationPomdpModelChecker(std::shared_ptr<PomdpModelType> pomdp, Options options = Options());
                
                Result check(storm::logic::Formula const& formula);

                void printStatisticsToStream(std::ostream& stream) const;
                
            private:
                
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
                void computeReachabilityOTF(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::TrivialPomdpValueBounds<ValueType> const& pomdpValueBounds, Result& result);
                
                
                /**
                 * Compute the reachability probability of given target observations on a POMDP using the automatic refinement loop
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum probability is to be computed
                 * @return A struct containing the final overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                void refineReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, storm::pomdp::modelchecker::TrivialPomdpValueBounds<ValueType> const& pomdpValueBounds, Result& result);
                
                struct HeuristicParameters {
                    ValueType gapThreshold;
                    ValueType observationThreshold;
                    uint64_t sizeThreshold;
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
                bool buildUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, HeuristicParameters const& heuristicParameters, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& underApproximation);

                BeliefValueType rateObservation(typename ExplorerType::SuccessorObservationInformation const& info, BeliefValueType const& observationResolution, BeliefValueType const& maxResolution);
                
                std::vector<BeliefValueType> getObservationRatings(std::shared_ptr<ExplorerType> const& overApproximation, std::vector<BeliefValueType> const& observationResolutionVector);
                
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
                    
                    bool aborted;
                };
                Statistics statistics;
                
                std::shared_ptr<PomdpModelType> inputPomdp;
                std::shared_ptr<PomdpModelType> preprocessedPomdp;
                
                Options options;
                storm::utility::ConstantsComparator<ValueType> cc;
            };

        }
    }
}