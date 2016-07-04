#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_

#include <vector>
#include <type_traits>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"
#include "src/utility/NumberTraits.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*!
             * Helper Class that takes preprocessed multi objective data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMaModelType>
            class SparseMaMultiObjectiveWeightVectorChecker : public SparseMultiObjectiveWeightVectorChecker<SparseMaModelType> {
            public:
                typedef typename SparseMaModelType::ValueType ValueType;
                typedef SparseMultiObjectivePreprocessorData<SparseMaModelType> PreprocessorData;
            
                SparseMaMultiObjectiveWeightVectorChecker(PreprocessorData const& data);
                
            private:
                
                /*!
                 *
                 * @param weightVector the weight vector of the current check
                 * @param weightedRewardVector the weighted rewards (initially only considering the unbounded objectives, will be extended to all objectives)
                 */
                virtual void boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) override;
                
                /*!
                 *
                 * Retrieves the delta used for the digitization
                 */
                template <typename VT = ValueType, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                VT getDigitizationConstant() const;
                template <typename VT = ValueType, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                VT getDigitizationConstant() const;
  
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTEDVECTORCHECKER_H_ */
