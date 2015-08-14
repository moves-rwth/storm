#include "src/models/sparse/Ctmc.h"

#include "src/adapters/CarlAdapter.h"
#include "src/utility/macros.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template<typename ValueType>
            Ctmc<ValueType>::Ctmc(storm::storage::SparseMatrix<ValueType> const& rateMatrix, storm::models::sparse::StateLabeling const& stateLabeling,
                                  boost::optional<std::vector<ValueType>> const& optionalStateRewardVector,
                                  boost::optional<storm::storage::SparseMatrix<ValueType>> const& optionalTransitionRewardMatrix,
                                  boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Ctmc, rateMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
                exitRates = createExitRateVector(this->getTransitionMatrix());
            }
            
            template<typename ValueType>
            Ctmc<ValueType>::Ctmc(storm::storage::SparseMatrix<ValueType>&& rateMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                                  boost::optional<std::vector<ValueType>>&& optionalStateRewardVector,
                                  boost::optional<storm::storage::SparseMatrix<ValueType>>&& optionalTransitionRewardMatrix,
                                  boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType>(storm::models::ModelType::Ctmc, std::move(rateMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix), std::move(optionalChoiceLabeling)) {
                // It is important to refer to the transition matrix here, because the given rate matrix has been move elsewhere.
                exitRates = createExitRateVector(this->getTransitionMatrix());
            }
            
            template<typename ValueType>
            std::vector<ValueType> const& Ctmc<ValueType>::getExitRateVector() const {
                return exitRates;
            }
            
            template<typename ValueType>
            std::vector<ValueType> Ctmc<ValueType>::createExitRateVector(storm::storage::SparseMatrix<ValueType> const& rateMatrix) {
                std::vector<ValueType> exitRates(rateMatrix.getRowCount());
                for (uint_fast64_t row = 0; row < rateMatrix.getRowCount(); ++row) {
                    exitRates[row] = rateMatrix.getRowSum(row);
                }
                return exitRates;
            }
            
            template class Ctmc<double>;

#ifdef STORM_HAVE_CARL
            template class Ctmc<storm::RationalFunction>;
#endif
            
        } // namespace sparse
    } // namespace models
} // namespace storm