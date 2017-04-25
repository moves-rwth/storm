#include "storm/storage/memorystructure/MemoryStructure.h"

#include "storm/logic/Formulas.h"
#include "storm/utility/macros.h"
#include "storm/storage/memorystructure/SparseModelMemoryProduct.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace storage {
        
        MemoryStructure::MemoryStructure(TransitionMatrix const& transitionMatrix, storm::models::sparse::StateLabeling const& memoryStateLabeling) : transitions(transitionMatrix), stateLabeling(memoryStateLabeling) {
            // intentionally left empty
        }
        
        MemoryStructure::MemoryStructure(TransitionMatrix&& transitionMatrix, storm::models::sparse::StateLabeling&& memoryStateLabeling) : transitions(std::move(transitionMatrix)), stateLabeling(std::move(memoryStateLabeling)) {
            // intentionally left empty
        }
            
        MemoryStructure::TransitionMatrix const& MemoryStructure::getTransitionMatrix() const {
            return this->transitions;
        }
        
        storm::models::sparse::StateLabeling const& MemoryStructure::getStateLabeling() const {
            return stateLabeling;
        }
            
        MemoryStructure MemoryStructure::product(MemoryStructure const& rhs) const {
            uint_fast64_t lhsNumStates = this->getTransitionMatrix().size();
            uint_fast64_t rhsNumStates = rhs.getTransitionMatrix().size();
            uint_fast64_t resNumStates = lhsNumStates * rhsNumStates;
                
            // Transition matrix
            TransitionMatrix resultTransitions(resNumStates, std::vector<std::shared_ptr<storm::logic::Formula const>>(resNumStates));
            uint_fast64_t resState = 0;
            for (uint_fast64_t lhsState = 0; lhsState < lhsNumStates; ++lhsState) {
                for (uint_fast64_t rhsState = 0; rhsState < rhsNumStates; ++rhsState) {
                    assert (resState == (lhsState * rhsNumStates) + rhsState);
                    auto resStateTransitions = resultTransitions[resState];
                    for (uint_fast64_t lhsTransitionTarget = 0; lhsTransitionTarget < lhsNumStates; ++lhsTransitionTarget) {
                        auto& lhsTransition = this->getTransitionMatrix()[lhsState][lhsTransitionTarget];
                        if (lhsTransition) {
                            for (uint_fast64_t rhsTransitionTarget = 0; rhsTransitionTarget < rhsNumStates; ++rhsTransitionTarget) {
                                auto& rhsTransition = this->getTransitionMatrix()[rhsState][rhsTransitionTarget];
                                if (rhsTransition) {
                                    uint_fast64_t resTransitionTarget = (lhsTransitionTarget * rhsNumStates) + rhsTransitionTarget;
                                    resStateTransitions[resTransitionTarget] = std::make_shared<storm::logic::BinaryBooleanStateFormula const>(storm::logic::BinaryBooleanStateFormula::OperatorType::And, lhsTransition,rhsTransition);
                                }
                            }
                        }
                    }
                    ++resState;
                }
            }
                
            // State Labels
            storm::models::sparse::StateLabeling resultLabeling(resNumStates);
            for (std::string lhsLabel : this->getStateLabeling().getLabels()) {
                storm::storage::BitVector const& lhsLabeledStates = this->getStateLabeling().getStates(lhsLabel);
                storm::storage::BitVector resLabeledStates(resNumStates, false);
                for (auto const& lhsState : lhsLabeledStates) {
                    for (uint_fast64_t rhsState = 0; rhsState < rhsNumStates; ++rhsState) {
                        resState = (lhsState * rhsNumStates) + rhsState;
                        resLabeledStates.set(resState, true);
                    }
                }
                resultLabeling.addLabel(lhsLabel, std::move(resLabeledStates));
            }
            for (std::string rhsLabel : rhs.getStateLabeling().getLabels()) {
                STORM_LOG_THROW(!resultLabeling.containsLabel(rhsLabel), storm::exceptions::InvalidOperationException, "Failed to build the product of two memory structures: State labelings are not disjoint as both structures contain the label " << rhsLabel << ".");
                storm::storage::BitVector const& rhsLabeledStates = rhs.getStateLabeling().getStates(rhsLabel);
                storm::storage::BitVector resLabeledStates(resNumStates, false);
                for (auto const& rhsState : rhsLabeledStates) {
                    for (uint_fast64_t lhsState = 0; lhsState < lhsNumStates; ++lhsState) {
                        resState = (lhsState * rhsNumStates) + rhsState;
                        resLabeledStates.set(resState, true);
                    }
                }
                resultLabeling.addLabel(rhsLabel, std::move(resLabeledStates));
            }
            return MemoryStructure(std::move(resultTransitions), std::move(resultLabeling));
        }
            
        template <typename ValueType>
        SparseModelMemoryProduct<ValueType> MemoryStructure::product(storm::models::sparse::Model<ValueType> const& sparseModel) const {
            return SparseModelMemoryProduct<ValueType>(sparseModel, *this);
        }
        
        template SparseModelMemoryProduct<double> MemoryStructure::product(storm::models::sparse::Model<double> const& sparseModel) const;
        template SparseModelMemoryProduct<storm::RationalNumber> MemoryStructure::product(storm::models::sparse::Model<storm::RationalNumber> const& sparseModel) const;


    }
}


