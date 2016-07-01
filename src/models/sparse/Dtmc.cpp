#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/adapters/CarlAdapter.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/constants.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix,
                 storm::models::sparse::StateLabeling const& stateLabeling,
                 std::unordered_map<std::string, RewardModelType> const& rewardModels,
                 boost::optional<std::vector<LabelSet>> const& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Dtmc, probabilityMatrix, stateLabeling, rewardModels, optionalChoiceLabeling) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::Dtmc(storm::storage::SparseMatrix<ValueType>&& probabilityMatrix, storm::models::sparse::StateLabeling&& stateLabeling,
                std::unordered_map<std::string, RewardModelType>&& rewardModels, boost::optional<std::vector<LabelSet>>&& optionalChoiceLabeling)
            : DeterministicModel<ValueType, RewardModelType>(storm::models::ModelType::Dtmc, std::move(probabilityMatrix), std::move(stateLabeling), std::move(rewardModels), std::move(optionalChoiceLabeling)) {
                STORM_LOG_THROW(probabilityMatrix.isProbabilistic(), storm::exceptions::InvalidArgumentException, "The probability matrix is invalid.");
            }
            
#ifdef STORM_HAVE_CARL
            template <typename ValueType, typename RewardModelType>
            Dtmc<ValueType, RewardModelType>::ConstraintCollector::ConstraintCollector(Dtmc<ValueType> const& dtmc) {
                process(dtmc);
            }
            
            template <typename ValueType, typename RewardModelType>
            std::unordered_set<storm::ArithConstraint<ValueType>> const& Dtmc<ValueType, RewardModelType>::ConstraintCollector::getWellformedConstraints() const {
                return this->wellformedConstraintSet;
            }
            
            template <typename ValueType, typename RewardModelType>
            std::unordered_set<storm::ArithConstraint<ValueType>> const& Dtmc<ValueType, RewardModelType>::ConstraintCollector::getGraphPreservingConstraints() const {
                return this->graphPreservingConstraintSet;
            }
            
            template <typename ValueType, typename RewardModelType>
            void Dtmc<ValueType, RewardModelType>::ConstraintCollector::process(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
                for(uint_fast64_t state = 0; state < dtmc.getNumberOfStates(); ++state) {
                    ValueType sum = storm::utility::zero<ValueType>();
                    for (auto const& transition : dtmc.getRows(state)) {
                        sum += transition.getValue();
                        if (!storm::utility::isConstant(transition.getValue())) {
                            wellformedConstraintSet.emplace(transition.getValue() - 1, storm::CompareRelation::LEQ);
                            wellformedConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GEQ);
                            graphPreservingConstraintSet.emplace(transition.getValue(), storm::CompareRelation::GREATER);
                        }
                    }
                    STORM_LOG_ASSERT(!storm::utility::isConstant(sum) || storm::utility::isOne(sum), "If the sum is a constant, it must be equal to 1.");
                    if(!storm::utility::isConstant(sum)) {
                        wellformedConstraintSet.emplace(sum - 1, storm::CompareRelation::EQ);
                    }
                    
                }
            }
            
            template <typename ValueType, typename RewardModelType>
            void Dtmc<ValueType, RewardModelType>::ConstraintCollector::operator()(storm::models::sparse::Dtmc<ValueType> const& dtmc) {
                process(dtmc);
            }
#endif
            
            template class Dtmc<double>;
            template class Dtmc<float>;
            template class Dtmc<storm::RationalNumber>;

            template class Dtmc<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
            template class Dtmc<storm::RationalFunction>;

        } // namespace sparse
    } // namespace models
} // namespace storm
