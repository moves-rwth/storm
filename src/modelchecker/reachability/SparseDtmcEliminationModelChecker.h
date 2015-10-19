#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/storage/FlexibleSparseMatrix.h"
#include "src/models/sparse/Dtmc.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/utility/constants.h"

//forward declaration of friend class
namespace storm {
	namespace modelchecker {
            namespace region {
                template<typename ParametricModelType, typename ConstantType>
                class SparseDtmcRegionModelChecker;
            }
	}
}
            
namespace storm {
    namespace modelchecker {
        
        template<typename SparseDtmcModelType>
        class SparseDtmcEliminationModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
            template<typename ParametricModelType, typename ConstantType> friend class storm::modelchecker::region::SparseDtmcRegionModelChecker;
        public:
            typedef typename SparseDtmcModelType::ValueType ValueType;
            typedef typename SparseDtmcModelType::RewardModelType RewardModelType;
            
            explicit SparseDtmcEliminationModelChecker(storm::models::sparse::Dtmc<ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, boost::optional<std::string> const& rewardModelName = boost::optional<std::string>(), bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, bool qualitative = false, boost::optional<OptimizationDirection> const& optimalityType = boost::optional<OptimizationDirection>()) override;
            
        private:
            
            ValueType computeReachabilityValue(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            uint_fast64_t treatScc(storm::storage::FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            std::vector<std::size_t> getStatePriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities);
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_ */
