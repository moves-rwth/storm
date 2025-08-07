#pragma once

#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace transformer {

template<typename ValueType>
struct PomdpTransformationResult {
    std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> transformedPomdp;
    std::vector<uint64_t> transformedStateToOriginalStateMap;
};

template<typename ValueType>
class BinaryPomdpTransformer {
   public:
    BinaryPomdpTransformer();

    PomdpTransformationResult<ValueType> transform(storm::models::sparse::Pomdp<ValueType> const& pomdp, bool transformSimple,
                                                   bool keepStateValuations = false) const;

   private:
    struct TransformationData {
        storm::storage::SparseMatrix<ValueType> simpleMatrix;
        std::vector<uint32_t> simpleObservations;
        std::vector<uint64_t> originalToSimpleChoiceMap;
        std::vector<uint64_t> simpleStateToOriginalState;
    };

    TransformationData transformTransitions(storm::models::sparse::Pomdp<ValueType> const& pomdp, bool transformSimple) const;
    storm::models::sparse::StateLabeling transformStateLabeling(storm::models::sparse::Pomdp<ValueType> const& pomdp, TransformationData const& data) const;
    storm::models::sparse::StandardRewardModel<ValueType> transformRewardModel(storm::models::sparse::Pomdp<ValueType> const& pomdp,
                                                                               storm::models::sparse::StandardRewardModel<ValueType> const& rewardModel,
                                                                               TransformationData const& data) const;
    storm::models::sparse::ChoiceLabeling transformChoiceLabeling(storm::models::sparse::Pomdp<ValueType> const& pomdp, TransformationData const& data) const;
};
}  // namespace transformer
}  // namespace storm