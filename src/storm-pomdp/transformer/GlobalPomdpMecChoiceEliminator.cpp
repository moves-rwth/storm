#include "storm-pomdp/transformer/GlobalPomdpMecChoiceEliminator.h"
#include <vector>

#include "storm/transformer/ChoiceSelector.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/graph.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace transformer {


        template<typename ValueType>
        GlobalPomdpMecChoiceEliminator<ValueType>::GlobalPomdpMecChoiceEliminator(storm::models::sparse::Pomdp<ValueType> const& pomdp) : pomdp(pomdp) {
        
        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> GlobalPomdpMecChoiceEliminator<ValueType>::transform(storm::logic::Formula const& formula) const {
            // check whether the property is minimizing or maximizing
            STORM_LOG_THROW(formula.isOperatorFormula(), storm::exceptions::InvalidPropertyException, "Expected an operator formula");
            STORM_LOG_THROW(formula.asOperatorFormula().hasOptimalityType() || formula.asOperatorFormula().hasBound(), storm::exceptions::InvalidPropertyException, "The formula " << formula << " does not specify whether to minimize or maximize.");
            bool minimizes = (formula.asOperatorFormula().hasOptimalityType() && storm::solver::minimize(formula.asOperatorFormula().getOptimalityType())) || (formula.asOperatorFormula().hasBound() && storm::logic::isLowerBound(formula.asOperatorFormula().getBound().comparisonType));
            
            std::shared_ptr<storm::logic::Formula const> subformula = formula.asOperatorFormula().getSubformula().asSharedPointer();
            // If necessary, convert the subformula to a more general case
            if (subformula->isEventuallyFormula() && subformula->asEventuallyFormula().isProbabilityPathFormula()) {
                subformula = std::make_shared<storm::logic::UntilFormula>(storm::logic::Formula::getTrueFormula(), subformula->asEventuallyFormula().getSubformula().asSharedPointer());
            }
            
            if (subformula->isUntilFormula()) {
                if (!minimizes) {
                    return transformMax(subformula->asUntilFormula());
                }
            }
            STORM_LOG_THROW(false, storm::exceptions::InvalidPropertyException, "Mec elimination is not supported for the property " << formula);

        }
        
        template<typename ValueType>
        std::shared_ptr<storm::models::sparse::Pomdp<ValueType>> GlobalPomdpMecChoiceEliminator<ValueType>::transformMax(storm::logic::UntilFormula const& formula) const {
            auto backwardTransitions = pomdp.getBackwardTransitions();
            auto prob01States = storm::utility::graph::performProb01Max(pomdp.getTransitionMatrix(), pomdp.getTransitionMatrix().getRowGroupIndices(), backwardTransitions, checkPropositionalFormula(formula.getLeftSubformula()), checkPropositionalFormula(formula.getRightSubformula()));
            auto mecs = decomposeEndComponents(~(prob01States.first | prob01States.second), prob01States.first);
            
            std::vector<storm::storage::BitVector> mecChoicesPerObservation(pomdp.getNrObservations());
            storm::storage::BitVector uniqueOutStates(pomdp.getNumberOfStates(), false);
            // Find the MECs that have only one 'out' state
            for (auto const& mec : mecs) {
                boost::optional<uint64_t> uniqueOutState = boost::none;
                for (auto const& stateActionsPair : mec) {
                    // Check whether this is an 'out' state, i.e., not all actions stay inside the MEC
                    if (stateActionsPair.second.size() != pomdp.getNumberOfChoices(stateActionsPair.first)) {
                        if (uniqueOutState) {
                            // we already found one out state, so this mec is invalid
                            uniqueOutState = boost::none;
                            break;
                        } else {
                            uniqueOutState = stateActionsPair.first;
                        }
                    }
                }
                if (uniqueOutState) {
                    uniqueOutStates.set(uniqueOutState.get(), true);
                    
                    storm::storage::BitVector localChoiceIndices(pomdp.getNumberOfChoices(uniqueOutState.get()), false);
                    uint64_t offset = pomdp.getTransitionMatrix().getRowGroupIndices()[uniqueOutState.get()];
                    for (auto const& choice : mec.getChoicesForState(uniqueOutState.get())) {
                        assert(choice >= offset);
                        localChoiceIndices.set(choice - offset, true);
                    }
                    
                    auto& mecChoices = mecChoicesPerObservation[pomdp.getObservation(uniqueOutState.get())];
                    if (mecChoices.size() == 0) {
                        mecChoices = localChoiceIndices;
                    } else {
                        STORM_LOG_ASSERT(mecChoices.size() == localChoiceIndices.size(), "Observation action count does not match for two states with the same observation");
                        mecChoices &= localChoiceIndices;
                    }
                }
            }
            
            // Filter the observations that have a state that is neither an out state, nor a prob0A or prob1A state
            storm::storage::BitVector stateFilter = ~(uniqueOutStates | prob01States.first | prob01States.second);
            for (auto const& state : stateFilter) {
                mecChoicesPerObservation[pomdp.getObservation(state)].clear();
            }
            
            // It should not be possible to clear all choices for an observation since we only consider states that lead outside of its MEC.
            for (auto& clearedChoices : mecChoicesPerObservation) {
                STORM_LOG_ASSERT(clearedChoices.size() == 0 || !clearedChoices.full(), "Tried to clear all choices for an observation.");
            }
            
            // transform the set of selected choices to global choice indices
            storm::storage::BitVector choiceFilter(pomdp.getNumberOfChoices(), true);
            stateFilter.complement();
            for (auto const& state : stateFilter) {
                uint64_t offset = pomdp.getTransitionMatrix().getRowGroupIndices()[state];
                for (auto const& choice : mecChoicesPerObservation[pomdp.getObservation(state)]) {
                    choiceFilter.set(offset + choice, false);
                }
            }
            
            ChoiceSelector<ValueType> cs(pomdp);
            return cs.transform(choiceFilter)->template as<storm::models::sparse::Pomdp<ValueType>>();
        }
        
        template<typename ValueType>
        storm::storage::MaximalEndComponentDecomposition<ValueType> GlobalPomdpMecChoiceEliminator<ValueType>::decomposeEndComponents(storm::storage::BitVector const& subsystem, storm::storage::BitVector const& redirectingStates) const {
            if (redirectingStates.empty()) {
                return storm::storage::MaximalEndComponentDecomposition<ValueType>(pomdp.getTransitionMatrix(), pomdp.getBackwardTransitions(), subsystem);
            } else {
                // Redirect all incoming transitions of a redirictingState back to the origin of the transition.
                storm::storage::SparseMatrixBuilder<ValueType> builder(pomdp.getTransitionMatrix().getRowCount(), pomdp.getTransitionMatrix().getColumnCount(), 0, true, true, pomdp.getTransitionMatrix().getRowGroupCount());
                uint64_t row = 0;
                for (uint64_t rowGroup = 0; rowGroup < pomdp.getTransitionMatrix().getRowGroupCount(); ++rowGroup) {
                    assert(row == pomdp.getTransitionMatrix().getRowGroupIndices()[rowGroup]);
                    builder.newRowGroup(row);
                    for (; row < pomdp.getTransitionMatrix().getRowGroupIndices()[rowGroup + 1]; ++row) {
                        ValueType redirectedProbabilityMass = pomdp.getTransitionMatrix().getConstrainedRowSum(row, redirectingStates);
                        bool insertSelfloop = !storm::utility::isZero(redirectedProbabilityMass);
                        for (auto const& entry : pomdp.getTransitionMatrix().getRow(row)) {
                            if (!redirectingStates.get(entry.getColumn())) {
                                if (insertSelfloop && entry.getColumn() >= rowGroup) {
                                    // insert selfloop now
                                    insertSelfloop = false;
                                    if (entry.getColumn() == rowGroup) {
                                        builder.addNextValue(row, rowGroup, entry.getValue() + redirectedProbabilityMass);
                                        continue;
                                    } else {
                                        builder.addNextValue(row, rowGroup, redirectedProbabilityMass);
                                    }
                                }
                                builder.addNextValue(row, entry.getColumn(), entry.getValue());
                            }
                        }
                        if (insertSelfloop) {
                            builder.addNextValue(row, rowGroup, redirectedProbabilityMass);
                        }
                    }
                }
                storm::storage::SparseMatrix<ValueType> transitionMatrix = builder.build();
                return storm::storage::MaximalEndComponentDecomposition<ValueType>(transitionMatrix, transitionMatrix.transpose(true), subsystem);
            }
        }
        
        template<typename ValueType>
        storm::storage::BitVector GlobalPomdpMecChoiceEliminator<ValueType>::checkPropositionalFormula(storm::logic::Formula const& propositionalFormula) const {
            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<ValueType>> mc(pomdp);
            STORM_LOG_THROW(mc.canHandle(propositionalFormula), storm::exceptions::InvalidPropertyException, "Propositional model checker can not handle formula " << propositionalFormula);
            return mc.check(propositionalFormula)->asExplicitQualitativeCheckResult().getTruthValuesVector();
        }

        
        template class GlobalPomdpMecChoiceEliminator<storm::RationalNumber>;
    }
}