#include <cstdlib>
#include "storm/api/storm.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/logging.h"
#include "storm-pomdp/storage/Belief.h"
#include "storm-pomdp/storage/BeliefManager.h"
#include "storm-pomdp/builder/BeliefMdpExplorer.h"
#include <boost/bimap.hpp>

#include "storm/storage/jani/Property.h"

namespace storm {
    namespace logic {
        class Formula;
    }
    
    namespace pomdp {
        namespace modelchecker {
            typedef boost::bimap<uint64_t, uint64_t> bsmap_type;

            template<typename PomdpModelType, typename BeliefValueType = typename PomdpModelType::ValueType>
            class ApproximatePOMDPModelchecker {
            public:
                typedef typename PomdpModelType::ValueType ValueType;
                typedef typename PomdpModelType::RewardModelType RewardModelType;
                typedef storm::storage::BeliefManager<PomdpModelType, BeliefValueType> BeliefManagerType;
                typedef storm::builder::BeliefMdpExplorer<PomdpModelType, BeliefValueType> ExplorerType;
                
                struct Options {
                    Options();
                    uint64_t  initialGridResolution; /// Decides how precise the bounds are
                    ValueType explorationThreshold; /// the threshold for exploration stopping. If the difference between over- and underapproximation for a state is smaller than the threshold, stop exploration of the state
                    bool doRefinement; /// Sets whether the bounds should be refined automatically until the refinement precision is reached
                    ValueType refinementPrecision; /// Used to decide when the refinement should terminate
                    ValueType numericPrecision; /// Used to decide whether two values are equal
                    bool cacheSubsimplices; /// Enables caching of subsimplices
                };
                
                struct Result {
                    Result(ValueType lower, ValueType upper);
                    ValueType lowerBound;
                    ValueType upperBound;
                    ValueType diff (bool relative = false) const;
                };
                
                ApproximatePOMDPModelchecker(PomdpModelType const& pomdp, Options options = Options());
                
                Result check(storm::logic::Formula const& formula);

                void printStatisticsToStream(std::ostream& stream) const;
                
            private:
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
                void computeReachabilityOTF(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, Result& result);
                
                
                /**
                 * Compute the reachability probability of given target observations on a POMDP using the automatic refinement loop
                 *
                 * @param targetObservations the set of observations to be reached
                 * @param min true if minimum probability is to be computed
                 * @return A struct containing the final overapproximation (overApproxValue) and underapproximation (underApproxValue) values
                 */
                void refineReachability(std::set<uint32_t> const &targetObservations, bool min, boost::optional<std::string> rewardModelName, std::vector<ValueType> const& lowerPomdpValueBounds, std::vector<ValueType> const& upperPomdpValueBounds, Result& result);

                /**
                 * Builds and checks an MDP that over-approximates the POMDP behavior, i.e. provides an upper bound for maximizing and a lower bound for minimizing properties
                 */
                void buildOverApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, bool refine, ValueType* refinementAggressiveness, std::vector<uint64_t>& observationResolutionVector, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& overApproximation);

                /**
                 * Builds and checks an MDP that under-approximates the POMDP behavior, i.e. provides a lower bound for maximizing and an upper bound for minimizing properties
                 */
                void buildUnderApproximation(std::set<uint32_t> const &targetObservations, bool min, bool computeRewards, uint64_t maxStateCount, std::shared_ptr<BeliefManagerType>& beliefManager, std::shared_ptr<ExplorerType>& underApproximation);

                ValueType rateObservation(typename ExplorerType::SuccessorObservationInformation const& info, uint64_t const& observationResolution, uint64_t const& maxResolution);
                
                std::vector<ValueType> getObservationRatings(std::shared_ptr<ExplorerType> const& overApproximation, std::vector<uint64_t> const& observationResolutionVector, uint64_t const& maxResolution);
                
                struct Statistics {
                    Statistics();
                    boost::optional<uint64_t> refinementSteps;
                    
                    boost::optional<uint64_t> overApproximationStates;
                    bool overApproximationBuildAborted;
                    storm::utility::Stopwatch overApproximationBuildTime;
                    storm::utility::Stopwatch overApproximationCheckTime;
                    
                    boost::optional<uint64_t> underApproximationStates;
                    bool underApproximationBuildAborted;
                    storm::utility::Stopwatch underApproximationBuildTime;
                    storm::utility::Stopwatch underApproximationCheckTime;
                    
                    bool aborted;
                };
                Statistics statistics;
                
                PomdpModelType const& pomdp;
                Options options;
                storm::utility::ConstantsComparator<ValueType> cc;
            };

        }
    }
}