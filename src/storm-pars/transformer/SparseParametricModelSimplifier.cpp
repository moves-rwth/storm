#include "storm-pars/transformer/SparseParametricModelSimplifier.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/stateelimination/NondeterministicModelStateEliminator.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidPropertyException.h"

namespace storm {
    namespace transformer {

        template<typename SparseModelType>
        SparseParametricModelSimplifier<SparseModelType>::SparseParametricModelSimplifier(SparseModelType const& model) : originalModel(model) {
            // intentionally left empty
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplify(storm::logic::Formula const& formula, bool keepRewardsAsConstantAsPossible) {
            // Make sure that there is no old result from a previous call
            simplifiedModel = nullptr;
            simplifiedFormula = nullptr;
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probOpForm = formula.asProbabilityOperatorFormula();
                if (probOpForm.getSubformula().isUntilFormula()) {
                    return simplifyForUntilProbabilities(probOpForm);
                } else if (probOpForm.getSubformula().isReachabilityProbabilityFormula()) {
                    return simplifyForReachabilityProbabilities(probOpForm);
                } else if (probOpForm.getSubformula().isBoundedUntilFormula()) {
                    return simplifyForBoundedUntilProbabilities(probOpForm);
                }
            } else if (formula.isRewardOperatorFormula()) {
                storm::logic::RewardOperatorFormula rewOpForm = formula.asRewardOperatorFormula();
                STORM_LOG_THROW((rewOpForm.hasRewardModelName() && originalModel.hasRewardModel(rewOpForm.getRewardModelName())) || (!rewOpForm.hasRewardModelName() && originalModel.hasUniqueRewardModel()), storm::exceptions::InvalidPropertyException, "The reward model specified by formula " << formula << " is not available in the given model.");
                if (rewOpForm.getSubformula().isReachabilityRewardFormula()) {
                    return simplifyForReachabilityRewards(rewOpForm, keepRewardsAsConstantAsPossible);
                } else if (rewOpForm.getSubformula().isCumulativeRewardFormula()) {
                    return simplifyForCumulativeRewards(rewOpForm, keepRewardsAsConstantAsPossible);
                }
            }
            // reaching this point means that the provided formula is not supported. Thus, no simplification is possible.
            STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::getSimplifiedModel() const {
            STORM_LOG_THROW(simplifiedModel, storm::exceptions::InvalidStateException, "Tried to get the simplified model but simplification was not invoked before.");
            return simplifiedModel;
        }
        
        template<typename SparseModelType>
        std::shared_ptr<storm::logic::Formula const> SparseParametricModelSimplifier<SparseModelType>::getSimplifiedFormula() const {
            STORM_LOG_THROW(simplifiedFormula, storm::exceptions::InvalidStateException, "Tried to get the simplified formula but simplification was not invoked before.");
            return simplifiedFormula;
        }

        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // If this method was not overridden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForReachabilityProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // transform to until formula
            auto untilFormula = std::make_shared<storm::logic::UntilFormula const>(storm::logic::Formula::getTrueFormula(), formula.getSubformula().asEventuallyFormula().getSubformula().asSharedPointer());
            return simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula(untilFormula, formula.getOperatorInformation()));
        }

        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForBoundedUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
            // If this method was not overridden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula, bool keepRewardsAsConstantAsPossible) {
            // If this method was not overridden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
        bool SparseParametricModelSimplifier<SparseModelType>::simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula, bool keepRewardsAsConstantAsPossible) {
            // If this method was not overridden by any subclass, simplification is not possible
            STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
            return false;
        }
        
        template<typename SparseModelType>
         std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::eliminateConstantDeterministicStates(SparseModelType const& model, storm::storage::BitVector const& consideredStates, boost::optional<std::string> const& rewardModelName, bool keepRewardsAsConstantAsPossible) {
            storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& sparseMatrix = model.getTransitionMatrix();
            
            // get the action-based reward values
            std::vector<typename SparseModelType::ValueType> actionRewards;
            if(rewardModelName) {
                actionRewards = model.getRewardModel(*rewardModelName).getTotalRewardVector(sparseMatrix);
            } else {
                actionRewards = std::vector<typename SparseModelType::ValueType>(model.getTransitionMatrix().getRowCount(), storm::utility::zero<typename SparseModelType::ValueType>());
            }

            boost::optional<storage::SparseMatrix<typename SparseModelType::ValueType>> transposeMatrix;
            // Find the states that are to be eliminated
            storm::storage::BitVector selectedStatesRound1 = consideredStates;

            if (keepRewardsAsConstantAsPossible) {
                // We need two rounds
                // 1) eliminate all states with constant outgoing transitions and constant ingoing transitions
                // 2) eliminate all states with constant outgoing transitions and reward of 0
                STORM_LOG_ASSERT(rewardModelName.is_initialized(), "Keeping rewards constant while not having rewards makes no sense");
                // Round 1
                for (auto state : consideredStates) {
                    if (sparseMatrix.getRowGroupSize(state) > 1) {
                        selectedStatesRound1.set(state, false);
                        continue;
                    }
                    if (!transposeMatrix.is_initialized()) {
                        // we join groups, so if there is non-determinism, all transitions are now in the same row
                        transposeMatrix = sparseMatrix.transpose(true);
                    }
                    // Check ingoing transitions
                    for (auto const& entry : transposeMatrix->getRow(state)) {
                        if (!entry.getValue().isConstant()) {
                            selectedStatesRound1.set(state, false);
                            break;
                        }
                    }
                    // Check outgoing transitions
                    for (auto const& entry : sparseMatrix.getRowGroup(state)) {
                        if(!storm::utility::isConstant(entry.getValue())) {
                            selectedStatesRound1.set(state, false);
                            break;
                        }
                    }
                }
                // Now eliminate the states for round 1
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrixRound1(sparseMatrix);
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitionsRound1(sparseMatrix.transpose(), true);
                storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminatorRound1(flexibleMatrixRound1, flexibleBackwardTransitionsRound1, actionRewards);
                // We eliminate the states in selectedStatesRound1
                for(auto state : selectedStatesRound1) {
                    stateEliminatorRound1.eliminateState(state, true);
                }
                selectedStatesRound1.complement();
                // The rows we want to keep, that are the rows of the states that are not selected.
                auto keptRowsRound1 = sparseMatrix.getRowFilter(selectedStatesRound1);
                storm::storage::SparseMatrix<typename  SparseModelType::ValueType> newTransitionMatrixRound1 = flexibleMatrixRound1.createSparseMatrix(keptRowsRound1, selectedStatesRound1);
                // obtain the reward model for the resulting system
                if(rewardModelName) {
                    storm::utility::vector::filterVectorInPlace(actionRewards, keptRowsRound1);
                }
                auto labeling = model.getStateLabeling().getSubLabeling(selectedStatesRound1);


                // Round 2 (eliminate all states with constant outgoing transitions and reward of 0)
                storm::storage::BitVector selectedStatesRound2(newTransitionMatrixRound1.getRowGroupCount(), false);
                // Now eliminate the states for round 2 eliminate all states with constant outgoing transitions and reward of 0
                auto nrEliminated = 0;
                // State refers to the original state nr in considered states
                // state-nrEliminated is the stateNumber after elimination
                for (auto state = 0; state < consideredStates.size(); ++state) {
                    // Skip non-deterministic states and states we don't want to consider
                    if (sparseMatrix.getRowGroupSize(state) > 1 || !consideredStates[state]) {
                        continue;
                    }
                    // We took the complement, so selectedStatesRound1 is true for the states we keep
                    if (!selectedStatesRound1[state]) {
                        nrEliminated++;
                    }  else {
                        auto newStateNumber = state - nrEliminated;
                        // If the state is kept and we should consider it
                        // we check if the outgoing transitions in the new transition matrix are constant, and the reward is 0
                        if (newTransitionMatrixRound1.getRowGroupSize(newStateNumber) == 1 && storm::utility::isZero(actionRewards[sparseMatrix.getRowGroupIndices()[state-nrEliminated]])) {
//                            selectedStatesRound2.set(newStateNumber, true);
                            for (auto const& entry : newTransitionMatrixRound1.getRowGroup(newStateNumber)) {
                                if (!storm::utility::isConstant(entry.getValue())) {
//                                    selectedStatesRound2.set(newStateNumber, false);
                                    break;
                                }
                            }
                        }
                    }
                }

                // Get resulting model
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrixRound2(newTransitionMatrixRound1);
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitionsRound2(newTransitionMatrixRound1.transpose(), true);
                storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminatorRound2(
                    flexibleMatrixRound2, flexibleBackwardTransitionsRound2, actionRewards);
                for (auto state : selectedStatesRound2) {
                    stateEliminatorRound2.eliminateState(state, true);
                }

                selectedStatesRound2.complement();
                auto keptRowsRound2 = newTransitionMatrixRound1.getRowFilter(selectedStatesRound2);
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> newTransitionMatrixRound2 =
                    flexibleMatrixRound2.createSparseMatrix(keptRowsRound2, selectedStatesRound2);

                // obtain the reward model for the resulting system
                std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModelsRound2;
                if (rewardModelName) {
                    storm::utility::vector::filterVectorInPlace(actionRewards, keptRowsRound2);
                    rewardModelsRound2.insert(std::make_pair(*rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(actionRewards))));
                }

                return std::make_shared<SparseModelType>(std::move(newTransitionMatrixRound2), model.getStateLabeling().getSubLabeling(selectedStatesRound1).getSubLabeling(selectedStatesRound2),
                                                         std::move(rewardModelsRound2));

            } else {
                // We don't care about rewards possibly getting non-constant
                for (auto state : consideredStates) {
                    if (sparseMatrix.getRowGroupSize(state) == 1 &&
                        (!rewardModelName.is_initialized() || storm::utility::isConstant(actionRewards[sparseMatrix.getRowGroupIndices()[state]]))) {
                        for (auto const& entry : sparseMatrix.getRowGroup(state)) {
                            if (!storm::utility::isConstant(entry.getValue())) {
                                selectedStatesRound1.set(state, false);
                                break;
                            }
                        }
                    } else {
                        selectedStatesRound1.set(state, false);
                    }
                }
                // invoke elimination and obtain resulting transition matrix
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrix(sparseMatrix);
                storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitions(sparseMatrix.transpose(), true);
                storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminator(
                    flexibleMatrix, flexibleBackwardTransitions, actionRewards);

                for (auto state : selectedStatesRound1) {
//                    if (sparseMatrix.getRowGroupSize(state) == 1) {
                        stateEliminator.eliminateState(state, true);
//                    } else {
//                        selectedStates.set(state, false);
//                        STORM_LOG_WARN("Elimination of state " << state << " not possible, > 1 action is enabled for this state.");
//                    }
                }
                selectedStatesRound1.complement();
                auto keptRows = sparseMatrix.getRowFilter(selectedStatesRound1);
                storm::storage::SparseMatrix<typename SparseModelType::ValueType> newTransitionMatrix =
                    flexibleMatrix.createSparseMatrix(keptRows, selectedStatesRound1);

                // obtain the reward model for the resulting system
                std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
                if (rewardModelName) {
                    storm::utility::vector::filterVectorInPlace(actionRewards, keptRows);
                    rewardModels.insert(std::make_pair(*rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(actionRewards))));
                }

                return std::make_shared<SparseModelType>(std::move(newTransitionMatrix), model.getStateLabeling().getSubLabeling(selectedStatesRound1),
                                                         std::move(rewardModels));
            }
        }


        template class SparseParametricModelSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>;
        template class SparseParametricModelSimplifier<storm::models::sparse::Mdp<storm::RationalFunction>>;
    }
}
