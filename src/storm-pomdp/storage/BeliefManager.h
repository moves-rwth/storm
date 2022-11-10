#pragma once

#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/solver.h"
#include "storm/solver/LpSolver.h"
#include "storm/utility/constants.h"
#include "storm/storage/BitVector.h"

namespace storm {
    namespace storage {
        
        template <typename PomdpType, typename BeliefValueType = typename PomdpType::ValueType, typename StateType = uint64_t>
        class BeliefManager {
        public:
            typedef typename PomdpType::ValueType ValueType;
            typedef boost::container::flat_map<StateType, BeliefValueType> BeliefType; // iterating over this shall be ordered (for correct hash computation)
            typedef boost::container::flat_set<StateType> BeliefSupportType;
            typedef uint64_t BeliefId;

            enum class TriangulationMode {
                Static,
                Dynamic
            };

            BeliefManager(PomdpType const &pomdp, BeliefValueType const &precision, TriangulationMode const &triangulationMode);

            void setRewardModel(boost::optional<std::string> rewardModelName = boost::none);

            void unsetRewardModel();

            struct Triangulation {
                std::vector<BeliefId> gridPoints;
                std::vector<BeliefValueType> weights;
                uint64_t size() const;
            };

            struct BeliefClipping {
                bool isClippable;
                BeliefId startingBelief;
                BeliefId targetBelief;
                BeliefValueType delta;
                BeliefType deltaValues;
                bool onGrid = false;
            };

            BeliefId noId() const;

            bool isEqual(BeliefId const &first, BeliefId const &second) const;

            std::string toString(BeliefId const &beliefId) const;


            std::string toString(Triangulation const &t) const;

            ValueType getWeightedSum(BeliefId const &beliefId, std::vector<ValueType> const &summands);

            std::pair<bool, ValueType> getWeightedSum(BeliefId const &beliefId, std::unordered_map<StateType, ValueType> const &summands);

            BeliefId const &getInitialBelief() const;

            ValueType getBeliefActionReward(BeliefId const &beliefId, uint64_t const &localActionIndex) const;

            uint32_t getBeliefObservation(BeliefId beliefId);

            uint64_t getBeliefNumberOfChoices(BeliefId beliefId);

            /**
             * Returns the first state in the belief as a representative
             * @param beliefId
             * @return
             */
            uint64_t getRepresentativeState(BeliefId const beliefId);

            Triangulation triangulateBelief(BeliefId beliefId, BeliefValueType resolution);

            template<typename DistributionType>
            void addToDistribution(DistributionType &distr, StateType const &state, BeliefValueType const &value);

            void joinSupport(BeliefId const &beliefId, BeliefSupportType &support);

            BeliefId getNumberOfBeliefIds() const;

            std::vector<std::pair<BeliefId, ValueType>>
            expandAndTriangulate(BeliefId const &beliefId, uint64_t actionIndex, std::vector<BeliefValueType> const &observationResolutions);

            std::vector<std::pair<BeliefId, ValueType>>
            expandAndClip(BeliefId const &beliefId, uint64_t actionIndex, std::vector<uint64_t> const &observationResolutions);

            std::vector<std::pair<BeliefId, ValueType>> expand(BeliefId const &beliefId, uint64_t actionIndex);

            BeliefClipping clipBelief(BeliefId const &beliefId, BeliefValueType threshold = storm::utility::one<BeliefValueType>(), boost::optional<std::vector<BeliefId>> const &targets = boost::none);

            BeliefValueType computeDifference1norm(BeliefId const &belief1, BeliefId const &belief2);

            BeliefClipping clipBeliefToGrid(BeliefId const &beliefId, uint64_t resolution, storm::storage::BitVector isInfinite);

            std::string getObservationLabel(BeliefId const & beliefId);

        private:

            BeliefClipping clipBeliefToGrid(BeliefType const &belief, uint64_t resolution, const storm::storage::BitVector& isInfinite);

            template<typename DistributionType>
            void adjustDistribution(DistributionType &distr);

            BeliefValueType computeDifference1normInternal(BeliefType const &belief1, BeliefType const &belief2);

            struct BeliefHash {
                std::size_t operator()(const BeliefType &belief) const;
            };

            struct Belief_equal_to {
                bool operator()(const BeliefType &lhBelief, const BeliefType &rhBelief) const;
            };

            struct FreudenthalDiff {
                FreudenthalDiff(StateType const &dimension, BeliefValueType diff);

                StateType dimension; // i
                BeliefValueType diff; // d[i]
                bool operator>(FreudenthalDiff const &other) const;
            };

            BeliefType const &getBelief(BeliefId const &id) const;

            BeliefId getId(BeliefType const &belief) const;

            std::string toString(BeliefType const &belief) const;

            bool isEqual(BeliefType const &first, BeliefType const &second) const;

            bool assertBelief(BeliefType const &belief) const;

            bool assertTriangulation(BeliefType const &belief, Triangulation const &triangulation) const;

            uint32_t getBeliefObservation(BeliefType belief) const;

            void triangulateBeliefFreudenthal(BeliefType const &belief, BeliefValueType const &resolution, Triangulation &result);

            void triangulateBeliefDynamic(BeliefType const &belief, BeliefValueType const &resolution, Triangulation &result);

            Triangulation triangulateBelief(BeliefType const &belief, BeliefValueType const &resolution);

            std::vector<std::pair<BeliefId, ValueType>>
            expandInternal(BeliefId const &beliefId, uint64_t actionIndex, boost::optional<std::vector<BeliefValueType>> const &observationTriangulationResolutions = boost::none,
                           boost::optional<std::vector<uint64_t>> const &observationGridClippingResolutions = boost::none);

            BeliefId computeInitialBelief();

            BeliefId getOrAddBeliefId(BeliefType const &belief);

            PomdpType const& pomdp;
            std::vector<ValueType> pomdpActionRewardVector;
            
            std::vector<BeliefType> beliefs;
            std::vector<std::unordered_map<BeliefType, BeliefId, BeliefHash, Belief_equal_to>> beliefToIdMap;
            BeliefId initialBeliefId;
            
            storm::utility::ConstantsComparator<BeliefValueType> cc;

            std::shared_ptr<storm::solver::LpSolver<BeliefValueType>> lpSolver;
            
            TriangulationMode triangulationMode;
            
        };
    }
}
