#pragma once

#include <memory>

#include <boost/optional.hpp>

#include "storm/logic/ComparisonType.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/BitVector.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"

namespace storm {
namespace storage {
template<typename ValueType>
class SparseMatrix;
}

namespace solver {
template<typename ValueType>
class MinMaxLinearEquationSolverFactory;
template<typename ValueType>
class LinearEquationSolverFactory;
}  // namespace solver

namespace modelchecker {
template<typename FormulaType, typename ValueType>
class CheckTask;
}
namespace models {
namespace sparse {
template<typename ValueType, typename RewardModelType>
class Model;
}
}  // namespace models

namespace solver {
template<typename ValueType>
class MinMaxLinearEquationSolver;
template<typename ValueType>
class LinearEquationSolver;

template<typename ValueType>
class SolveGoal {
   public:
    SolveGoal();

    template<typename RewardModelType, typename FormulaType>
    SolveGoal(storm::models::sparse::Model<ValueType, RewardModelType> const& model, storm::modelchecker::CheckTask<FormulaType, ValueType> const& checkTask) {
        if (checkTask.isOptimizationDirectionSet()) {
            optimizationDirection = checkTask.getOptimizationDirection();
        }
        if (checkTask.isOnlyInitialStatesRelevantSet()) {
            relevantValueVector = model.getInitialStates();
        }
        if (checkTask.isBoundSet()) {
            comparisonType = checkTask.getBoundComparisonType();
            threshold = checkTask.getBoundThreshold();
        }
    }

    SolveGoal(bool minimize);
    SolveGoal(OptimizationDirection d);
    SolveGoal(OptimizationDirection d, storm::logic::ComparisonType boundComparisonType, ValueType const& boundThreshold,
              storm::storage::BitVector const& relevantValues);
    SolveGoal(OptimizationDirection d, storm::storage::BitVector const& relevantValues);

    /*!
     * Flips the comparison type, the direction, and computes the new threshold as 1 - old threshold.
     */
    void oneMinus();

    bool hasDirection() const;

    bool minimize() const;

    OptimizationDirection direction() const;

    bool isBounded() const;

    bool boundIsALowerBound() const;

    bool boundIsStrict() const;

    ValueType const& thresholdValue() const;

    bool hasRelevantValues() const;

    storm::storage::BitVector& relevantValues();
    storm::storage::BitVector const& relevantValues() const;

    void restrictRelevantValues(storm::storage::BitVector const& filter);
    void setRelevantValues(storm::storage::BitVector&& values);

   private:
    boost::optional<OptimizationDirection> optimizationDirection;

    boost::optional<storm::logic::ComparisonType> comparisonType;
    boost::optional<ValueType> threshold;
    boost::optional<storm::storage::BitVector> relevantValueVector;
};

template<typename ValueType, typename MatrixType>
std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> configureMinMaxLinearEquationSolver(
    Environment const& env, SolveGoal<ValueType>&& goal, storm::solver::MinMaxLinearEquationSolverFactory<ValueType> const& factory, MatrixType&& matrix) {
    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver = factory.create(env, std::forward<MatrixType>(matrix));
    solver->setOptimizationDirection(goal.direction());
    if (goal.isBounded()) {
        if (goal.boundIsALowerBound()) {
            solver->setTerminationCondition(std::make_unique<TerminateIfFilteredExtremumExceedsThreshold<ValueType>>(
                goal.relevantValues(), goal.boundIsStrict(), goal.thresholdValue(), true));
        } else {
            solver->setTerminationCondition(std::make_unique<TerminateIfFilteredExtremumBelowThreshold<ValueType>>(goal.relevantValues(), goal.boundIsStrict(),
                                                                                                                   goal.thresholdValue(), false));
        }
    }
    if (goal.hasRelevantValues()) {
        solver->setRelevantValues(std::move(goal.relevantValues()));
    }
    return solver;
}

template<typename ValueType, typename MatrixType>
std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> configureLinearEquationSolver(
    Environment const& env, SolveGoal<ValueType>&& goal, storm::solver::LinearEquationSolverFactory<ValueType> const& factory, MatrixType&& matrix) {
    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = factory.create(env, std::forward<MatrixType>(matrix));
    if (goal.isBounded()) {
        solver->setTerminationCondition(std::make_unique<TerminateIfFilteredExtremumExceedsThreshold<ValueType>>(goal.relevantValues(), goal.boundIsStrict(),
                                                                                                                 goal.thresholdValue(), goal.minimize()));
    }
    return solver;
}

template<typename MatrixType>
std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> configureLinearEquationSolver(
    Environment const& env, SolveGoal<storm::RationalFunction>&& goal, storm::solver::LinearEquationSolverFactory<storm::RationalFunction> const& factory,
    MatrixType&& matrix) {
    std::unique_ptr<storm::solver::LinearEquationSolver<storm::RationalFunction>> solver = factory.create(env, std::forward<MatrixType>(matrix));
    return solver;
}

}  // namespace solver
}  // namespace storm
