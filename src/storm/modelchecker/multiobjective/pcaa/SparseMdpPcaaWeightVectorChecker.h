#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEMDPPCAAWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEMDPPCAAWEIGHTVECTORCHECKER_H_

#include <vector>

#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaWeightVectorChecker.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Helper Class that takes preprocessed Pcaa data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMdpModelType>
            class SparseMdpPcaaWeightVectorChecker : public SparsePcaaWeightVectorChecker<SparseMdpModelType> {
            public:
                typedef typename SparseMdpModelType::ValueType ValueType;
            
                SparseMdpPcaaWeightVectorChecker(SparseMdpModelType const& model,
                                                 std::vector<PcaaObjective<ValueType>> const& objectives,
                                                 storm::storage::BitVector const& actionsWithNegativeReward,
                                                 storm::storage::BitVector const& ecActions,
                                                 storm::storage::BitVector const& possiblyRecurrentStates);

                virtual ~SparseMdpPcaaWeightVectorChecker() = default;

            private:
                
                /*!
                 * For each time epoch (starting with the maximal stepBound occurring in the objectives), this method
                 * - determines the objectives that are relevant in the current time epoch
                 * - determines the maximizing scheduler for the weighted reward vector of these objectives
                 * - computes the values of these objectives w.r.t. this scheduler
                 *
                 * @param weightVector the weight vector of the current check
                 * @param weightedRewardVector the weighted rewards considering the unbounded objectives. Will be invalidated after calling this.
                 */
                virtual void boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) override;
  
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEMDPPCAAWEIGHTVECTORCHECKER_H_ */
