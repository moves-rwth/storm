#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEMDPPCAAWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEMDPPCAAWEIGHTVECTORCHECKER_H_

#include <vector>

#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/rewardbounded/MultiDimensionalRewardUnfolding.h"

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
                typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
                /*
                 * Creates a weight vextor checker.
                 *
                 * @param model The (preprocessed) model
                 * @param objectives The (preprocessed) objectives
                 * @param possibleECActions Overapproximation of the actions that are part of an EC
                 * @param possibleBottomStates The states for which it is posible to not collect further reward with prob. 1
                 *
                 */
                
                SparseMdpPcaaWeightVectorChecker(SparseMultiObjectivePreprocessorResult<SparseMdpModelType> const& preprocessorResult);

                virtual ~SparseMdpPcaaWeightVectorChecker() = default;

            protected:
                virtual void initializeModelTypeSpecificData(SparseMdpModelType const& model) override;

            private:
                
                /*!
                 * Computes the maximizing scheduler for the weighted sum of the objectives, including also step bounded objectives.
                 * Moreover, the values of the individual objectives are computed w.r.t. this scheduler.
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
