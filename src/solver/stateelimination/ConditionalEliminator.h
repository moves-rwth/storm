#ifndef STORM_SOLVER_STATEELIMINATION_CONDITIONALELIMINATOR_H_
#define STORM_SOLVER_STATEELIMINATION_CONDITIONALELIMINATOR_H_

#include "src/solver/stateelimination/StateEliminator.h"

#include "src/storage/BitVector.h"

namespace storm {
    namespace solver {
        namespace stateelimination {
            
            template<typename SparseModelType>
            class ConditionalEliminator : public StateEliminator<SparseModelType> {

                typedef typename SparseModelType::ValueType ValueType;

                enum SpecificState { NONE, PHI, PSI};
                
            public:
                ConditionalEliminator(storm::storage::FlexibleSparseMatrix<ValueType>& transitionMatrix, storm::storage::FlexibleSparseMatrix<ValueType>& backwardTransitions, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector& phiStates, storm::storage::BitVector& psiStates);
                
                // Instantiaton of Virtual methods
                void updateValue(storm::storage::sparse::state_type const& state, ValueType const& loopProbability) override;
                void updatePredecessor(storm::storage::sparse::state_type const& predecessor, ValueType const& probability, storm::storage::sparse::state_type const& state) override;
                void updatePriority(storm::storage::sparse::state_type const& state) override;
                bool filterPredecessor(storm::storage::sparse::state_type const& state) override;
                bool isFilterPredecessor() const override;
                
                void setStatePsi() {
                    specificState = PSI;
                }
                
                void setStatePhi() {
                    specificState = PHI;
                }
                
                void clearState() {
                    specificState = NONE;
                }
                
            private:
                std::vector<ValueType>& oneStepProbabilities;
                storm::storage::BitVector& phiStates;
                storm::storage::BitVector& psiStates;
                SpecificState specificState;
                
            };
            
        } // namespace stateelimination
    } // namespace storage
} // namespace storm

#endif // STORM_SOLVER_STATEELIMINATION_CONDITIONALELIMINATOR_H_
