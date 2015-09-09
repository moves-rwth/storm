#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
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
            class FlexibleSparseMatrix {
            public:
                typedef uint_fast64_t index_type;
                typedef ValueType value_type;
                typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
                typedef typename row_type::iterator iterator;
                typedef typename row_type::const_iterator const_iterator;
                
                FlexibleSparseMatrix() = default;
                FlexibleSparseMatrix(index_type rows);
                
                void reserveInRow(index_type row, index_type numberOfElements);
                
                row_type& getRow(index_type);
                row_type const& getRow(index_type) const;
                
                index_type getNumberOfRows() const;
                
                void print() const;
                
                /*!
                 * Checks whether the given state has a self-loop with an arbitrary probability in the given probability matrix.
                 *
                 * @param state The state for which to check whether it possesses a self-loop.
                 * @param matrix The matrix in which to look for the loop.
                 * @return True iff the given state has a self-loop with an arbitrary probability in the given probability matrix.
                 */
                bool hasSelfLoop(storm::storage::sparse::state_type state);
                
            private:
                std::vector<row_type> data;
            };
            
            ValueType computeReachabilityValue(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            uint_fast64_t treatScc(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            static FlexibleSparseMatrix getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);
            
            void eliminateState(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix& backwardTransitions, boost::optional<std::vector<ValueType>>& stateRewards, bool removeForwardTransitions = true, bool constrained = false, storm::storage::BitVector const& predecessorConstraint = storm::storage::BitVector());
            
            std::vector<std::size_t> getStatePriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities);
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_ */
