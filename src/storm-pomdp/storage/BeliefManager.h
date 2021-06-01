#pragma once

#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "storm/utility/ConstantsComparator.h"

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

            BeliefId noId() const;

            bool isEqual(BeliefId const &first, BeliefId const &second) const;

            std::string toString(BeliefId const &beliefId) const;


            std::string toString(Triangulation const &t) const;

            ValueType getWeightedSum(BeliefId const &beliefId, std::vector<ValueType> const &summands);

            BeliefId const &getInitialBelief() const;

            ValueType getBeliefActionReward(BeliefId const &beliefId, uint64_t const &localActionIndex) const;

            uint32_t getBeliefObservation(BeliefId beliefId);

            uint64_t getBeliefNumberOfChoices(BeliefId beliefId);

            Triangulation triangulateBelief(BeliefId beliefId, BeliefValueType resolution);

            template<typename DistributionType>
            void addToDistribution(DistributionType &distr, StateType const &state, BeliefValueType const &value);

            void joinSupport(BeliefId const &beliefId, BeliefSupportType &support);

            BeliefId getNumberOfBeliefIds() const;

            std::vector<std::pair<BeliefId, ValueType>>
            expandAndTriangulate(BeliefId const &beliefId, uint64_t actionIndex, std::vector<BeliefValueType> const &observationResolutions);

            std::vector<std::pair<BeliefId, ValueType>> expand(BeliefId const &beliefId, uint64_t actionIndex);

        private:

            struct BeliefHash {
                std::size_t operator()(const BeliefType &belief) const;
            };

            struct FreudenthalDiff {
                FreudenthalDiff(StateType const &dimension, BeliefValueType &&diff);

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
            expandInternal(BeliefId const &beliefId, uint64_t actionIndex, boost::optional<std::vector<BeliefValueType>> const &observationTriangulationResolutions = boost::none);

            BeliefId computeInitialBelief();

            BeliefId getOrAddBeliefId(BeliefType const &belief);

            PomdpType const& pomdp;
            std::vector<ValueType> pomdpActionRewardVector;
            
            std::vector<BeliefType> beliefs;
            std::vector<std::unordered_map<BeliefType, BeliefId, BeliefHash>> beliefToIdMap;
            BeliefId initialBeliefId;
            
            storm::utility::ConstantsComparator<ValueType> cc;
            
            TriangulationMode triangulationMode;
            
        };
    }
}