#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {
        
        template<typename SparseDtmcModelType>
        class SparseDtmcEliminationModelChecker : public SparsePropositionalModelChecker<SparseDtmcModelType> {
        public:
            typedef typename SparseDtmcModelType::ValueType ValueType;
            typedef typename SparseDtmcModelType::RewardModelType RewardModelType;
            
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
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageRewards(CheckTask<storm::logic::LongRunAverageRewardFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(CheckTask<storm::logic::ConditionalFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeLongRunAverageProbabilities(CheckTask<storm::logic::StateFormula> const& checkTask) override;

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
                
                bool empty() const;
                
                void filter(storm::storage::BitVector const& rowFilter, storm::storage::BitVector const& columnFilter);
                
                /*!
                 * Checks whether the given state has a self-loop with an arbitrary probability in the probability matrix.
                 *
                 * @param state The state for which to check whether it possesses a self-loop.
                 * @return True iff the given state has a self-loop with an arbitrary probability in the probability matrix.
                 */
                bool hasSelfLoop(storm::storage::sparse::state_type state);
                
            private:
                std::vector<row_type> data;
            };
            
            class StatePriorityQueue {
            public:
                virtual bool hasNextState() const = 0;
                virtual storm::storage::sparse::state_type popNextState() = 0;
                virtual void update(storm::storage::sparse::state_type state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
                virtual std::size_t size() const = 0;
            };
            
            class StaticStatePriorityQueue : public StatePriorityQueue {
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
            
            typedef std::function<uint_fast64_t (storm::storage::sparse::state_type const& state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities)> PenaltyFunctionType;
            
            class DynamicPenaltyStatePriorityQueue : public StatePriorityQueue {
            public:
                DynamicPenaltyStatePriorityQueue(std::vector<std::pair<storm::storage::sparse::state_type, uint_fast64_t>> const& sortedStatePenaltyPairs, PenaltyFunctionType const& penaltyFunction);
                
                virtual bool hasNextState() const override;
                virtual storm::storage::sparse::state_type popNextState() override;
                virtual void update(storm::storage::sparse::state_type state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities) override;
                virtual std::size_t size() const override;

            private:
                std::set<std::pair<storm::storage::sparse::state_type, uint_fast64_t>, PriorityComparator> priorityQueue;
                std::unordered_map<storm::storage::sparse::state_type, uint_fast64_t> stateToPriorityMapping;
                PenaltyFunctionType penaltyFunction;
            };
            
            static std::vector<ValueType> computeLongRunValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& maybeStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& stateValues);
            
            static std::vector<ValueType> computeReachabilityValues(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& values, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, std::vector<ValueType> const& oneStepProbabilitiesToTarget);
            
            static std::unique_ptr<StatePriorityQueue> createStatePriorityQueue(boost::optional<std::vector<uint_fast64_t>> const& stateDistances, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& states);

            static std::unique_ptr<StatePriorityQueue> createNaivePriorityQueue(storm::storage::BitVector const& states);
            
            static void performPrioritizedStateElimination(std::unique_ptr<StatePriorityQueue>& priorityQueue, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, std::vector<ValueType>& values, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly);
            
            static void performOrdinaryStateElimination(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<ValueType>>& additionalStateValues, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);

            static void performOrdinaryStateElimination(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);
            
            static uint_fast64_t performHybridStateElimination(storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions, storm::storage::BitVector const& subsystem, storm::storage::BitVector const& initialStates, bool computeResultsForInitialStatesOnly, std::vector<ValueType>& values, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities);
            
            static uint_fast64_t treatScc(FlexibleSparseMatrix& matrix, std::vector<ValueType>& values, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::BitVector const& initialStates, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, bool computeResultsForInitialStatesOnly, boost::optional<std::vector<uint_fast64_t>> const& distanceBasedPriorities = boost::none);
            
            static FlexibleSparseMatrix getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);
                        
            typedef std::function<void (storm::storage::sparse::state_type const& state, ValueType const& loopProbability)> ValueUpdateCallback;
            typedef std::function<void (storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state)> PredecessorUpdateCallback;
            typedef std::function<void (storm::storage::sparse::state_type const& state)> PriorityUpdateCallback;
            typedef std::function<bool (storm::storage::sparse::state_type const& state)> PredecessorFilterCallback;
            
            static void eliminateState(storm::storage::sparse::state_type state, FlexibleSparseMatrix& matrix, FlexibleSparseMatrix& backwardTransitions, ValueUpdateCallback const& valueUpdateCallback, PredecessorUpdateCallback const& predecessorCallback, boost::optional<PriorityUpdateCallback> const& priorityUpdateCallback = boost::none, boost::optional<PredecessorFilterCallback> const& predecessorFilterCallback = boost::none, bool removeForwardTransitions = true);
            
            static std::vector<uint_fast64_t> getDistanceBasedPriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward, bool reverse);
            
            static std::vector<std::size_t> getStateDistances(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities, bool forward);
            
            static uint_fast64_t computeStatePenalty(storm::storage::sparse::state_type const& state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            static uint_fast64_t computeStatePenaltyRegularExpression(storm::storage::sparse::state_type const& state, FlexibleSparseMatrix const& transitionMatrix, FlexibleSparseMatrix const& backwardTransitions, std::vector<ValueType> const& oneStepProbabilities);
            
            static bool checkConsistent(FlexibleSparseMatrix& transitionMatrix, FlexibleSparseMatrix& backwardTransitions);
            
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_ */
