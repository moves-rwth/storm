#pragma once

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace transformer {

class BinaryDtmcTransformer {
   public:
    BinaryDtmcTransformer() = default;

    /*!
     * Transforms a pDTMC that has linear transitions (e.g. 0.5*p,
     * 0.2*p, 0.3*p, 1-p) into a pDTMC that has only one p and 1-p
     * transition per state (a simple pDTMC). It's "binary" because
     * there are two successors if there is a parametric transition.
     * @param dtmc The pMC to transform
     * @param keepStateValuations Blow up state valuations of DTMC into new state valuations and keep these in the result
     */
    std::shared_ptr<storm::models::sparse::Dtmc<RationalFunction>> transform(storm::models::sparse::Dtmc<RationalFunction> const& dtmc,
                                                                             bool keepStateValuations = false) const;

   private:
    struct TransformationData {
        storm::storage::SparseMatrix<RationalFunction> simpleMatrix;
        std::vector<uint64_t> simpleStateToOriginalState;
    };

    TransformationData transformTransitions(storm::models::sparse::Dtmc<RationalFunction> const& dtmc) const;
    storm::models::sparse::StateLabeling transformStateLabeling(storm::models::sparse::Dtmc<RationalFunction> const& dtmc,
                                                                TransformationData const& data) const;
    storm::models::sparse::StandardRewardModel<RationalFunction> transformRewardModel(
        storm::models::sparse::Dtmc<RationalFunction> const& dtmc, storm::models::sparse::StandardRewardModel<RationalFunction> const& rewardModel,
        TransformationData const& data) const;
    storm::models::sparse::ChoiceLabeling transformChoiceLabeling(storm::models::sparse::Dtmc<RationalFunction> const& dtmc,
                                                                  TransformationData const& data) const;
};
}  // namespace transformer
}  // namespace storm
