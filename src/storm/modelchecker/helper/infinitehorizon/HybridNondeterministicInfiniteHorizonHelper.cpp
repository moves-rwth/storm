#include "HybridNondeterministicInfiniteHorizonHelper.h"

#include "storm/modelchecker/helper/infinitehorizon/SparseNondeterministicInfiniteHorizonHelper.h"
#include "storm/modelchecker/helper/utility/SetInformationFromOtherHelper.h"

#include "storm/storage/SparseMatrix.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType, storm::dd::DdType DdType>
            HybridNondeterministicInfiniteHorizonHelper<ValueType, DdType>::HybridNondeterministicInfiniteHorizonHelper(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix) : _model(model), _transitionMatrix(transitionMatrix), _markovianStates(nullptr), _exitRates(nullptr) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            HybridNondeterministicInfiniteHorizonHelper<ValueType, DdType>::HybridNondeterministicInfiniteHorizonHelper(storm::models::symbolic::NondeterministicModel<DdType, ValueType> const& model, storm::dd::Add<DdType, ValueType> const& transitionMatrix, storm::dd::Bdd<DdType> const& markovianStates, storm::dd::Add<DdType, ValueType> const& exitRateVector) : _model(model), _transitionMatrix(transitionMatrix), _markovianStates(&markovianStates), _exitRates(&exitRateVector) {
                // Intentionally left empty.
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            std::unique_ptr<HybridQuantitativeCheckResult<DdType, ValueType>> HybridNondeterministicInfiniteHorizonHelper<ValueType, DdType>::computeLongRunAverageProbabilities(Environment const& env, storm::dd::Bdd<DdType> const& psiStates) {
                // Convert this query to an instance for the sparse engine.
                // Create ODD for the translation.
                storm::dd::Odd odd = _model.getReachableStates().createOdd();
                storm::storage::SparseMatrix<ValueType> explicitTransitionMatrix = _transitionMatrix.toMatrix(_model.getNondeterminismVariables(), odd, odd);
                std::unique_ptr<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>> sparseHelper;
                std::vector<ValueType> explicitExitRateVector;
                storm::storage::BitVector explicitMarkovianStates;
                if (isContinuousTime()) {
                    explicitExitRateVector = _exitRates->toVector(odd);
                    explicitMarkovianStates = _markovianStates->toVector(odd);
                    sparseHelper = std::make_unique<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>>(explicitTransitionMatrix, explicitMarkovianStates, explicitExitRateVector);
                } else {
                    sparseHelper = std::make_unique<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>>(explicitTransitionMatrix);
                }
                storm::modelchecker::helper::setInformationFromOtherHelperNondeterministic(*sparseHelper, *this, [&odd](storm::dd::Bdd<DdType> const& s){ return s.toVector(odd); });
                STORM_LOG_WARN_COND(!this->isProduceSchedulerSet(), "Scheduler extraction not supported in Hybrid engine.");
                auto explicitResult = sparseHelper->computeLongRunAverageProbabilities(env, psiStates.toVector(odd));
                return std::make_unique<HybridQuantitativeCheckResult<DdType, ValueType>>(_model.getReachableStates(), _model.getManager().getBddZero(), _model.getManager().template getAddZero<ValueType>(), _model.getReachableStates(), std::move(odd), std::move(explicitResult));
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            std::unique_ptr<HybridQuantitativeCheckResult<DdType, ValueType>> HybridNondeterministicInfiniteHorizonHelper<ValueType, DdType>::computeLongRunAverageRewards(Environment const& env, storm::models::symbolic::StandardRewardModel<DdType, ValueType> const& rewardModel) {
                // Convert this query to an instance for the sparse engine.
                // Create ODD for the translation.
                storm::dd::Odd odd = _model.getReachableStates().createOdd();
                
                // Create matrix and reward vectors
                storm::storage::SparseMatrix<ValueType> explicitTransitionMatrix;
                std::vector<ValueType> explicitStateRewards, explicitActionRewards;
                if (rewardModel.hasStateRewards()) {
                    explicitStateRewards = rewardModel.getStateRewardVector().toVector(odd);
                }
                if (rewardModel.hasStateActionRewards()) {
                    // Matrix and action-based vector have to be produced at the same time to guarantee the correct order
                    auto matrixRewards = _transitionMatrix.toMatrixVector(rewardModel.getStateActionRewardVector(), _model.getNondeterminismVariables(), odd, odd);
                    explicitTransitionMatrix = std::move(matrixRewards.first);
                    explicitActionRewards = std::move(matrixRewards.second);
                } else {
                    // Translate matrix only
                    explicitTransitionMatrix = _transitionMatrix.toMatrix(_model.getNondeterminismVariables(), odd, odd);
                }
                STORM_LOG_THROW(!rewardModel.hasTransitionRewards(), storm::exceptions::NotSupportedException, "Transition rewards are not supported in this engine.");
                
                // Create remaining components and helper
                std::vector<ValueType> explicitExitRateVector;
                storm::storage::BitVector explicitMarkovianStates;
                std::unique_ptr<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>> sparseHelper;
                if (isContinuousTime()) {
                    explicitExitRateVector = _exitRates->toVector(odd);
                    explicitMarkovianStates = _markovianStates->toVector(odd);
                    sparseHelper = std::make_unique<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>>(explicitTransitionMatrix, explicitMarkovianStates, explicitExitRateVector);
                } else {
                    sparseHelper = std::make_unique<storm::modelchecker::helper::SparseNondeterministicInfiniteHorizonHelper<ValueType>>(explicitTransitionMatrix);
                }
                storm::modelchecker::helper::setInformationFromOtherHelperNondeterministic(*sparseHelper, *this, [&odd](storm::dd::Bdd<DdType> const& s){ return s.toVector(odd); });

                STORM_LOG_WARN_COND(!this->isProduceSchedulerSet(), "Scheduler extraction not supported in Hybrid engine.");
                auto explicitResult = sparseHelper->computeLongRunAverageValues(env, rewardModel.hasStateRewards() ? &explicitStateRewards : nullptr, rewardModel.hasStateActionRewards() ? &explicitActionRewards : nullptr);
                return std::make_unique<HybridQuantitativeCheckResult<DdType, ValueType>>(_model.getReachableStates(), _model.getManager().getBddZero(), _model.getManager().template getAddZero<ValueType>(), _model.getReachableStates(), std::move(odd), std::move(explicitResult));
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool HybridNondeterministicInfiniteHorizonHelper<ValueType, DdType>::isContinuousTime() const {
                STORM_LOG_ASSERT((_markovianStates == nullptr) == (_exitRates == nullptr), "Inconsistent information given: Have Markovian states but no exit rates (or vice versa)." );
                return _markovianStates != nullptr;
            }
            
            template class HybridNondeterministicInfiniteHorizonHelper<double, storm::dd::DdType::CUDD>;
            template class HybridNondeterministicInfiniteHorizonHelper<double, storm::dd::DdType::Sylvan>;
            template class HybridNondeterministicInfiniteHorizonHelper<storm::RationalNumber, storm::dd::DdType::Sylvan>;
            
        }
    }
}