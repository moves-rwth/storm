#include "storm-pars/transformer/SparseParametricModelSimplifier.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/solver/Z3SmtSolver.h"
#include "storm/solver/stateelimination/NondeterministicModelStateEliminator.h"
#include "storm/storage/FlexibleSparseMatrix.h"
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/RationalFunctionToExpression.h"
#include "storm/utility/solver.h"
#include "storm/utility/vector.h"

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
        STORM_LOG_THROW((rewOpForm.hasRewardModelName() && originalModel.hasRewardModel(rewOpForm.getRewardModelName())) ||
                            (!rewOpForm.hasRewardModelName() && originalModel.hasUniqueRewardModel()),
                        storm::exceptions::InvalidPropertyException,
                        "The reward model specified by formula " << formula << " is not available in the given model.");
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
    STORM_LOG_THROW(simplifiedFormula, storm::exceptions::InvalidStateException,
                    "Tried to get the simplified formula but simplification was not invoked before.");
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
    auto untilFormula = std::make_shared<storm::logic::UntilFormula const>(storm::logic::Formula::getTrueFormula(),
                                                                           formula.getSubformula().asEventuallyFormula().getSubformula().asSharedPointer());
    return simplifyForUntilProbabilities(storm::logic::ProbabilityOperatorFormula(untilFormula, formula.getOperatorInformation()));
}

template<typename SparseModelType>
bool SparseParametricModelSimplifier<SparseModelType>::simplifyForBoundedUntilProbabilities(storm::logic::ProbabilityOperatorFormula const& formula) {
    // If this method was not overridden by any subclass, simplification is not possible
    STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
    return false;
}

template<typename SparseModelType>
bool SparseParametricModelSimplifier<SparseModelType>::simplifyForReachabilityRewards(storm::logic::RewardOperatorFormula const& formula,
                                                                                      bool keepRewardsAsConstantAsPossible) {
    // If this method was not overridden by any subclass, simplification is not possible
    STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
    return false;
}

template<typename SparseModelType>
bool SparseParametricModelSimplifier<SparseModelType>::simplifyForCumulativeRewards(storm::logic::RewardOperatorFormula const& formula,
                                                                                    bool keepRewardsAsConstantAsPossible) {
    // If this method was not overridden by any subclass, simplification is not possible
    STORM_LOG_DEBUG("Simplification not possible because the formula is not supported. Formula: " << formula);
    return false;
}

template<typename SparseModelType>
std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::eliminateConstantDeterministicStates(
    SparseModelType const& model, storm::storage::BitVector const& consideredStates, boost::optional<std::string> const& rewardModelName,
    bool keepRewardsAsConstantAsPossible) {
    storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& sparseMatrix = model.getTransitionMatrix();

    // get the action-based reward values
    std::vector<typename SparseModelType::ValueType> actionRewards;
    if (rewardModelName) {
        actionRewards = model.getRewardModel(*rewardModelName).getTotalRewardVector(sparseMatrix);
    } else {
        actionRewards = std::vector<typename SparseModelType::ValueType>(model.getTransitionMatrix().getRowCount(),
                                                                         storm::utility::zero<typename SparseModelType::ValueType>());
    }

    boost::optional<storage::SparseMatrix<typename SparseModelType::ValueType>> transposeMatrix;
    // Find the states that are to be eliminated

    if (keepRewardsAsConstantAsPossible) {
        storm::storage::BitVector selectedStatesRound1 = consideredStates;

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
                if (!storm::utility::isConstant(entry.getValue())) {
                    selectedStatesRound1.set(state, false);
                    break;
                }
            }
        }
        // Now eliminate the states for round 1
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrixRound1(sparseMatrix);
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitionsRound1(sparseMatrix.transpose(), true);
        storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminatorRound1(
            flexibleMatrixRound1, flexibleBackwardTransitionsRound1, actionRewards);
        // We eliminate the states in selectedStatesRound1
        for (auto state : selectedStatesRound1) {
            stateEliminatorRound1.eliminateState(state, true);
        }
        selectedStatesRound1.complement();
        // The rows we want to keep, that are the rows of the states that are not selected.
        auto keptRowsRound1 = sparseMatrix.getRowFilter(selectedStatesRound1);
        storm::storage::SparseMatrix<typename SparseModelType::ValueType> newTransitionMatrixRound1 =
            flexibleMatrixRound1.createSparseMatrix(keptRowsRound1, selectedStatesRound1);
        // obtain the reward model for the resulting system
        if (rewardModelName) {
            storm::utility::vector::filterVectorInPlace(actionRewards, keptRowsRound1);
        }
        auto labeling = model.getStateLabeling().getSubLabeling(selectedStatesRound1);

        // Round 2 (eliminate all states with constant outgoing transitions and reward of 0)
        storm::storage::BitVector selectedStatesRound2(newTransitionMatrixRound1.getRowGroupCount(), false);
        // Now eliminate the states for round 2 eliminate all states with constant outgoing transitions and reward of 0
        auto nrEliminated = 0;
        // State refers to the original state nr in considered states
        // state-nrEliminated is the stateNumber after elimination
        for (auto state : consideredStates) {
            // Skip non-deterministic states
            if (sparseMatrix.getRowGroupSize(state) > 1) {
                continue;
            }
            // We took the complement, so selectedStatesRound1 is true for the states we keep
            if (!selectedStatesRound1[state]) {
                nrEliminated++;
            } else {
                auto newStateNumber = state - nrEliminated;
                // If the state is kept and we should consider it
                // we check if the outgoing transitions in the new transition matrix are constant, and the reward is 0
                if (newTransitionMatrixRound1.getRowGroupSize(newStateNumber) == 1 &&
                    storm::utility::isZero(actionRewards[sparseMatrix.getRowGroupIndices()[state - nrEliminated]])) {
                    for (auto const& entry : newTransitionMatrixRound1.getRowGroup(newStateNumber)) {
                        if (!storm::utility::isConstant(entry.getValue())) {
                            break;
                        }
                    }
                }
            }
        }

        // Get resulting model
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrixRound2(newTransitionMatrixRound1);
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitionsRound2(newTransitionMatrixRound1.transpose(),
                                                                                                                    true);
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

        return std::make_shared<SparseModelType>(std::move(newTransitionMatrixRound2),
                                                 model.getStateLabeling().getSubLabeling(selectedStatesRound1).getSubLabeling(selectedStatesRound2),
                                                 std::move(rewardModelsRound2));

    } else {
        storm::storage::BitVector selectedStates = consideredStates;

        // We don't care about rewards possibly getting non-constant
        for (auto state : consideredStates) {
            if (sparseMatrix.getRowGroupSize(state) == 1 &&
                (!rewardModelName.is_initialized() || storm::utility::isConstant(actionRewards[sparseMatrix.getRowGroupIndices()[state]]))) {
                for (auto const& entry : sparseMatrix.getRowGroup(state)) {
                    if (!storm::utility::isConstant(entry.getValue())) {
                        selectedStates.set(state, false);
                        break;
                    }
                }
            } else {
                selectedStates.set(state, false);
            }
        }
        // invoke elimination and obtain resulting transition matrix
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrix(sparseMatrix);
        storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitions(sparseMatrix.transpose(), true);
        storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminator(
            flexibleMatrix, flexibleBackwardTransitions, actionRewards);

        for (auto state : selectedStates) {
            stateEliminator.eliminateState(state, true);
        }
        selectedStates.complement();
        auto keptRows = sparseMatrix.getRowFilter(selectedStates);
        storm::storage::SparseMatrix<typename SparseModelType::ValueType> newTransitionMatrix = flexibleMatrix.createSparseMatrix(keptRows, selectedStates);

        // obtain the reward model for the resulting system
        std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
        if (rewardModelName) {
            storm::utility::vector::filterVectorInPlace(actionRewards, keptRows);
            rewardModels.insert(std::make_pair(*rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(actionRewards))));
        }

        return std::make_shared<SparseModelType>(std::move(newTransitionMatrix), model.getStateLabeling().getSubLabeling(selectedStates),
                                                 std::move(rewardModels));
    }
}

// TODO move to MDPSimplifier
template<typename SparseModelType>
std::shared_ptr<SparseModelType> SparseParametricModelSimplifier<SparseModelType>::removeDontCareNonDeterminism(
    SparseModelType const& model, boost::optional<std::string> const& rewardModelName) {
    storm::storage::SparseMatrix<typename SparseModelType::ValueType> const& sparseMatrix = model.getTransitionMatrix();

    // get the action-based reward values
    std::vector<typename SparseModelType::ValueType> actionRewards;
    if (rewardModelName) {
        actionRewards = model.getRewardModel(*rewardModelName).getTotalRewardVector(sparseMatrix);
    } else {
        actionRewards = std::vector<typename SparseModelType::ValueType>(model.getTransitionMatrix().getRowCount(),
                                                                         storm::utility::zero<typename SparseModelType::ValueType>());
    }

    boost::optional<storage::SparseMatrix<typename SparseModelType::ValueType>> transposeMatrix;
    storm::storage::BitVector consideredStates(sparseMatrix.getRowGroupCount(), false);
    storm::storage::BitVector selectedStates(sparseMatrix.getRowGroupCount(), false);
    storm::storage::BitVector allStates(sparseMatrix.getRowGroupCount(), true);

    // Find non-deterministic states with only constant transitions
    for (auto state = 0; state < sparseMatrix.getRowGroupCount(); state++) {
        if (sparseMatrix.getRowGroupSize(state) > 1) {
            bool allConstant = true;
            for (auto& entry : sparseMatrix.getRowGroup(state)) {
                if (!storm::utility::isConstant(entry.getValue())) {
                    allConstant = false;
                    break;
                }
            }
            consideredStates.set(state, allConstant);
        }
    }

    std::shared_ptr<expressions::ExpressionManager> manager =
        std::make_shared<expressions::ExpressionManager>(expressions::ExpressionManager(expressions::ExpressionManager()));
    auto valueTypeToExpression = expressions::RationalFunctionToExpression<typename SparseModelType::ValueType>(manager);
    auto rowStarts = sparseMatrix.getRowGroupIndices();
    for (auto state : consideredStates) {
        bool actionDontCare = true;
        for (auto act = 0; act < sparseMatrix.getRowGroupSize(state) - 1; act++) {
            // Check if act and act+1 yield same
            auto row1 = sparseMatrix.getRow(state, act);
            auto row2 = sparseMatrix.getRow(state, act + 1);

            // we want to check if result for row1 equals result for row2
            // We will check the negation of the thing above
            auto exprLHS = valueTypeToExpression.toExpression(actionRewards[rowStarts[state] + act]);
            storm::storage::BitVector successors(sparseMatrix.getRowGroupCount(), false);

            // quickly check if states in rows are equal
            if (row1.getNumberOfEntries() != row2.getNumberOfEntries()) {
                actionDontCare = false;
                break;
            } else {
                for (auto i = 0; i < row1.getNumberOfEntries(); i++) {
                    if (*(row1.begin() + i) != *(row2.begin() + i)) {
                        actionDontCare = false;
                        break;
                    }
                }
            }
            if (!actionDontCare) {
                break;
            }
            //
            for (auto& entry : row1) {
                auto varname = "s" + std::to_string(entry.getColumn());
                successors.set(entry.getColumn());
                if (!manager->hasVariable(varname)) {
                    manager->declareRationalVariable(varname);
                }
                exprLHS = exprLHS + (manager->rational(storm::utility::convertNumber<RationalNumber>(entry.getValue())) * manager->getVariable(varname));
            }
            auto exprRHS = valueTypeToExpression.toExpression(actionRewards[rowStarts[state] + act + 1]);
            for (auto& entry : row2) {
                auto varname = "s" + std::to_string(entry.getColumn());
                successors.set(entry.getColumn());
                if (!manager->hasVariable(varname)) {
                    manager->declareRationalVariable(varname);
                }
                exprRHS = exprRHS + (manager->rational(storm::utility::convertNumber<RationalNumber>(entry.getValue())) * manager->getVariable(varname));
            }

            auto assumption = exprLHS != exprRHS;
            solver::Z3SmtSolver s(*manager);

            // --------------------------------------------------------------------------------
            // Expressions for the states in the assumption
            // --------------------------------------------------------------------------------

            for (auto state : successors) {
                expressions::Expression exprState = manager->boolean(false);
                for (auto act = 0; act < sparseMatrix.getRowGroupSize(state); ++act) {
                    expressions::Expression expr = valueTypeToExpression.toExpression(actionRewards[rowStarts[state + act]]);
                    for (auto& entry : sparseMatrix.getRow(state, act)) {
                        auto varname = "s" + std::to_string(entry.getColumn());
                        if (!manager->hasVariable(varname)) {
                            manager->declareRationalVariable(varname);
                        }
                        expr = expr + (valueTypeToExpression.toExpression(entry.getValue()) * manager->getVariable(varname));
                    }
                    exprState = exprState || (manager->getVariable("s" + std::to_string(state)) == expr);
                }
                s.add(exprState);
            }

            // Bounds for the states and parameters, could be improved with a region
            expressions::Expression exprBounds = manager->boolean(true);
            for (auto& var : manager->getVariables()) {
                if (rewardModelName) {
                    exprBounds = exprBounds && manager->rational(0) <= var;
                } else {
                    exprBounds = exprBounds && manager->rational(0) <= var && var <= manager->rational(1);
                }
            }

            s.add(exprBounds);
            s.setTimeout(1000);
            STORM_LOG_ASSERT(s.check() == solver::SmtSolver::CheckResult::Sat, "Cannot satisfy bounds + expressions");
            s.add(assumption);
            if (s.check() == solver::SmtSolver::CheckResult::Sat) {
                actionDontCare = false;
                break;
            }
        }

        if (actionDontCare) {
            selectedStates.set(state);
        }
    }

    // First we create the matrix in which we only have one action for the state, then we eliminate the states
    selectedStates.complement();
    auto keptRows = sparseMatrix.getRowFilter(selectedStates);
    selectedStates.complement();

    for (auto state : selectedStates) {
        // Keep one row for the current state
        keptRows.set(rowStarts[state]);
    }
    storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrix(sparseMatrix);
    storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitions(sparseMatrix.transpose(), true);
    storm::storage::SparseMatrix<typename SparseModelType::ValueType> newSparseMatrix = flexibleMatrix.createSparseMatrix(keptRows, allStates);
    // obtain the reward model for the resulting system
    storm::utility::vector::filterVectorInPlace(actionRewards, keptRows);

    // invoke elimination and obtain resulting transition matrix
    storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleMatrix2(newSparseMatrix);
    storm::storage::FlexibleSparseMatrix<typename SparseModelType::ValueType> flexibleBackwardTransitions2(newSparseMatrix.transpose(), true);
    storm::solver::stateelimination::NondeterministicModelStateEliminator<typename SparseModelType::ValueType> stateEliminator(
        flexibleMatrix2, flexibleBackwardTransitions2, actionRewards);

    for (auto state : selectedStates) {
        stateEliminator.eliminateState(state, true);
    }
    selectedStates.complement();
    auto keptRows2 = newSparseMatrix.getRowFilter(selectedStates);
    storm::storage::SparseMatrix<typename SparseModelType::ValueType> newTransitionMatrix = flexibleMatrix2.createSparseMatrix(keptRows2, selectedStates);

    // obtain the reward model for the resulting system
    std::unordered_map<std::string, typename SparseModelType::RewardModelType> rewardModels;
    if (rewardModelName) {
        storm::utility::vector::filterVectorInPlace(actionRewards, keptRows2);
        rewardModels.insert(std::make_pair(*rewardModelName, typename SparseModelType::RewardModelType(boost::none, std::move(actionRewards))));
    }

    return std::make_shared<SparseModelType>(std::move(newTransitionMatrix), model.getStateLabeling().getSubLabeling(selectedStates), std::move(rewardModels));
}

template class SparseParametricModelSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>>;
template class SparseParametricModelSimplifier<storm::models::sparse::Mdp<storm::RationalFunction>>;
}  // namespace transformer
}  // namespace storm
