#include "src/models/sparse/Ctmc.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template<typename ValueType>
            Ctmc<ValueType>::Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                  boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix,
                                  boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Ctmc, rateMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
                // Intentionally left empty.
            }
            
            template<typename ValueType>
            Ctmc<ValueType>::Ctmc(storm::storage::SparseMatrix<ValueType>&& rateMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                  boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix,
                                  boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Ctmc, std::move(rateMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix), std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template class Ctmc<double>;

#ifdef STORM_HAVE_CARL
            template class Ctmc<storm::RationalFunction>;
#endif
            
        } // namespace sparse
    } // namespace models
} // namespace storm