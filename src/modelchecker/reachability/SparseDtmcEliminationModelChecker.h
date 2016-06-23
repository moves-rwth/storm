#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/storage/FlexibleSparseMatrix.h"
#include "src/models/sparse/Dtmc.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        uint_fast64_t estimateComplexity(ValueType const& value);

        template<typename ValueType>
        class StatePriorityQueue {
        public:
            virtual bool hasNextState() const = 0;
            virtual storm::storage::sparse::state_type popNextState() = 0;
            virtual void update(storm::storage::sparse::state_type state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            virtual std::size_t size() const = 0;
        };
        
        template<typename SparseDtmcModelType>
        class SparseDtmcEliminationModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
        public:
            typedef typename SparseDtmcModelType::ValueType ValueType;
            typedef typename SparseDtmcModelType::RewardModelType RewardModelType;
            typedef typename storm::storage::FlexibleSparseMatrix<ValueType>::row_type FlexibleRowType;
            typedef typename FlexibleRowType::iterator FlexibleRowIterator;

            /*!
             * Creates an elimination-based model checker for the given model.
             *
             * @param model The model to analyze.
             */
            explicit SparseDtmcEliminationModelChecker(storm::models::sparse::Dtmc<ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeBoundedUntilProbabilities(CheckTask<storm::logic::BoundedUntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(storm::logic::RewardMeasureType rewardMeasureType, CheckTask<storm::logic::LongRunAverageRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) override;
           
            // Static helper methods 
            static std::unique_ptr<CheckResult> computeUntilProbabilities(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool computeForInitialStatesOnly);

            static std::unique_ptr<CheckResult> computeReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& targetStates, std::vector<ValueType>& stateRewardValues, bool computeForInitialStatesOnly);

        private:
            
            class StaticStatePriorityQueue : public StatePriorityQueue<ValueType> {
            public:
                StaticStatePriorityQueue(std::vector<storm::storage::sparse::state_type> const& sortedStates);
                
                virtual bool hasNextState() const override;
                virtual storm::storage::sparse::state_type popNextState() override;
                virtual std::size_t size() const override;
                
            private:
                std::vector<uint_fast64_t> sortedStates;
                uint_fast64_t currentPosition;
            };
            
            struct PriorityComparator {
                bool operator()(std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& first, std::pair<storm::storage::sparse::state_type, uint_fast64_t> const& second) {
                    return (first.second < second.second) || (first.second == second.second && first.first < second.first) ;
                }
            };
            
            typedef std::function<uint_fast64_t (storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities)> PenaltyFunctionType;
            
            class DynamicPenaltyStatePriorityQueue : public StatePriorityQueue<ValueType> {
            public:
                DynamicPenaltyStatePriorityQueue(std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> const& sortedStatePenaltyPairs, PenaltyFunctionType const& penaltyFunction);
                
                virtual bool hasNextState() const override;
                virtual storm::storage::sparse::state_type popNextState() override;
                virtual void update(storm::storage::sparse::state_type state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) override;
                virtual std::size_t size() const override;

            private:
                std::set<std::pair<storm::storage::sparse::state_type, uint_fast64_t>, PriorityComparator> priorityQueue;
                std::unordered_map<storm::storage::sparse::state_type, uint_fast64_t> stateToPriorityMapping;
                PenaltyFunctionType penaltyFunction;
            };
            
            static std::vector<ValueType> computeLongRunValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& maybeStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& stateValues);
            
            static std::unique_ptr<CheckResult> computeReachabilityRewards(storm::storage::SparseMatrix<ValueType> const& probabilityMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& targetStates, std::function<std::vector<ValueType>(uint_fast64_t, storm::storage::SparseMatrix<ValueType> const&, storm::storage::BitVector const&)> const& totalStateRewardVectorGetter, bool computeForInitialStatesOnly);

            static std::vector<ValueType> computeReachabilityValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& values, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& oneStepProbabilitiesToTarget);
            
            static std::shared_ptr<StatePriorityQueue<ValueType>> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& stateDistances, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& states);

            static std::shared_ptr<StatePriorityQueue<ValueType>> createNaivePriorityQueue(storm::storage::BitVector const& states);
            
            static void performPrioritizedStateElimination(std::shared_ptr<StatePriorityQueue<ValueType>>& priorityQueue, storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, std::vector<ValueType>& values, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly);
            
            static void performOrdinaryStateElimination(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<ValueType>>& additionalStateValues, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);

            static void performOrdinaryStateElimination(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);
            
            static uint_fast64_t performHybridStateElimination(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);
            
            static uint_fast64_t treatScc(storm::storage::FlexibleSparseMatrix<ValueType>& matrix, std::vector<ValueType>& values, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::BitVector const& initialStates, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, bool computeResultsForInitialStatesOnly, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities = boost::none);

            static std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward, bool reverse);
            
            static std::vector<std::size_t> getStateDistances(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward);
            
            static uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            static uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state, storm::storage::FlexibleSparseMatrix<ValueType> const& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType> const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            static bool checkConsistent(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions);
            
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_ */
