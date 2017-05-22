#include "storm/models/sparse/DeterministicModel.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/constants.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace sparse {
            template <typename ValueType, typename RewardModelType>
            DeterministicModel<ValueType, RewardModelType>::DeterministicModel(storm::models::ModelType const& modelType,
                                                              storm::storage::SparseMatrix<ValueType> const& transitionMatrix,
                                                              storm::models::sparse::StateLabeling const& stateLabeling,
                                                              std::unordered_map<std::string, RewardModelType> const& rewardModels,
                                                              boost::optional<storm::models::sparse::ChoiceLabeling> const& optionalChoiceLabeling)
            : Model<ValueType, RewardModelType>(modelType, transitionMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, typename RewardModelType>
            DeterministicModel<ValueType, RewardModelType>::DeterministicModel(storm::models::ModelType const& modelType,
                                                              storm::storage::SparseMatrix<ValueType>&& transitionMatrix,
                                                              storm::models::sparse::StateLabeling&& stateLabeling,
                                                              std::unordered_map<std::string, RewardModelType>&& rewardModels,
                                                              boost::optional<storm::models::sparse::ChoiceLabeling>&& optionalChoiceLabeling)
            : Model<ValueType, RewardModelType>(modelType, std::move(transitionMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, typename RewardModelType>
            void DeterministicModel<ValueType, RewardModelType>::writeDotToStream(std::ostream& outStream, bool includeLabeling, storm::storage::BitVector const* subsystem, std::vector<ValueType> const* firstValue, std::vector<ValueType> const* secondValue, std::vector<uint_fast64_t> const* stateColoring, std::vector<std::string> const* colors, std::vector<uint_fast64_t>* scheduler, bool finalizeOutput) const {
                Model<ValueType, RewardModelType>::writeDotToStream(outStream, includeLabeling, subsystem, firstValue, secondValue, stateColoring, colors, scheduler, false);
                
                // Simply iterate over all transitions and draw the arrows with probability information attached.
                auto rowIt = this->getTransitionMatrix().begin();
                for (uint_fast64_t i = 0; i < this->getTransitionMatrix().getRowCount(); ++i, ++rowIt) {
                    typename storm::storage::SparseMatrix<ValueType>::const_rows row = this->getTransitionMatrix().getRow(i);
                    for (auto const& transition : row) {
                        if (transition.getValue() != storm::utility::zero<ValueType>()) {
                            if (subsystem == nullptr || subsystem->get(transition.getColumn())) {
                                outStream << "\t" << i << " -> " << transition.getColumn() << " [ label= \"" << transition.getValue() << "\" ];" << std::endl;
                            }
                        }
                    }
                }
                
                if (finalizeOutput) {
                    outStream << "}" << std::endl;
                }
            }
            
            template<typename ValueType, typename RewardModelType>
            void DeterministicModel<ValueType, RewardModelType>::reduceToStateBasedRewards() {
                for (auto& rewardModel : this->getRewardModels()) {
                    rewardModel.second.reduceToStateBasedRewards(this->getTransitionMatrix(), true);
                }
            }
            
            template class DeterministicModel<double>;
#ifdef STORM_HAVE_CARL
            template class DeterministicModel<storm::RationalNumber>;
            
            template class DeterministicModel<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class DeterministicModel<storm::RationalFunction>;
#endif
        } // namespace sparse
    } // namespace models
} // namespace storm
