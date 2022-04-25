#include "storm/solver/LpMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/storage/expressions/RationalLiteralExpression.h"
#include "storm/storage/expressions/VariableExpression.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

namespace storm {
namespace solver {

template<typename ValueType>
LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory)
    : lpSolverFactory(std::move(lpSolverFactory)) {
    // Intentionally left empty.
}

template<typename ValueType>
LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A,
                                                                      std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory)
    : StandardMinMaxLinearEquationSolver<ValueType>(A), lpSolverFactory(std::move(lpSolverFactory)) {
    // Intentionally left empty.
}

template<typename ValueType>
LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A,
                                                                      std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory)
    : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A)), lpSolverFactory(std::move(lpSolverFactory)) {
    // Intentionally left empty.
}

template<typename ValueType>
bool LpMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x,
                                                                     std::vector<ValueType> const& b) const {
    STORM_LOG_THROW(env.solver().minMax().getMethod() == MinMaxMethod::LinearProgramming, storm::exceptions::InvalidEnvironmentException,
                    "This min max solver does not support the selected technique.");

    // Set up the LP solver
    std::unique_ptr<storm::solver::LpSolver<ValueType>> solver = lpSolverFactory->create("");
    solver->setOptimizationDirection(invert(dir));
    // Create a variable for each row group
    std::vector<storm::expressions::Expression> variableExpressions;
    variableExpressions.reserve(this->A->getRowGroupCount());
    for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
        if (this->hasLowerBound()) {
            ValueType lowerBound = this->getLowerBound(rowGroup);
            if (this->hasUpperBound()) {
                ValueType upperBound = this->getUpperBound(rowGroup);
                if (lowerBound == upperBound) {
                    // Some solvers (like glpk) don't support variables with bounds [x,x]. We therefore just use a constant instead. This should be more
                    // efficient anyways.
                    variableExpressions.push_back(solver->getConstant(lowerBound));
                } else {
                    STORM_LOG_ASSERT(lowerBound <= upperBound,
                                     "Lower Bound at row group " << rowGroup << " is " << lowerBound << " which exceeds the upper bound " << upperBound << ".");
                    variableExpressions.emplace_back(
                        solver->addBoundedContinuousVariable("x" + std::to_string(rowGroup), lowerBound, upperBound, storm::utility::one<ValueType>()));
                }
            } else {
                variableExpressions.emplace_back(
                    solver->addLowerBoundedContinuousVariable("x" + std::to_string(rowGroup), lowerBound, storm::utility::one<ValueType>()));
            }
        } else {
            if (this->upperBound) {
                variableExpressions.emplace_back(
                    solver->addUpperBoundedContinuousVariable("x" + std::to_string(rowGroup), this->getUpperBound(rowGroup), storm::utility::one<ValueType>()));
            } else {
                variableExpressions.emplace_back(solver->addUnboundedContinuousVariable("x" + std::to_string(rowGroup), storm::utility::one<ValueType>()));
            }
        }
    }
    solver->update();

    // Add a constraint for each row
    for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
        // The rowgroup refers to the state number
        uint64_t rowIndex, rowGroupEnd;
        if (this->choiceFixedForRowGroup && this->choiceFixedForRowGroup.get()[rowGroup]) {
            rowIndex = this->A->getRowGroupIndices()[rowGroup] + this->getInitialScheduler()[rowGroup];
            rowGroupEnd = rowIndex + 1;
        } else {
            rowIndex = this->A->getRowGroupIndices()[rowGroup];
            rowGroupEnd = this->A->getRowGroupIndices()[rowGroup + 1];
        }
        for (; rowIndex < rowGroupEnd; ++rowIndex) {
            auto row = this->A->getRow(rowIndex);
            std::vector<storm::expressions::Expression> summands;
            summands.reserve(1 + row.getNumberOfEntries());
            summands.push_back(solver->getConstant(b[rowIndex]));
            for (auto const& entry : row) {
                summands.push_back(solver->getConstant(entry.getValue()) * variableExpressions[entry.getColumn()]);
            }
            storm::expressions::Expression rowConstraint = storm::expressions::sum(summands);
            if (minimize(dir)) {
                rowConstraint = variableExpressions[rowGroup] <= rowConstraint;
            } else {
                rowConstraint = variableExpressions[rowGroup] >= rowConstraint;
            }
            solver->addConstraint("", rowConstraint);
        }
    }

    // Invoke optimization
    solver->optimize();
    STORM_LOG_THROW(!solver->isInfeasible(), storm::exceptions::UnexpectedException, "The MinMax equation system is infeasible.");
    STORM_LOG_THROW(!solver->isUnbounded(), storm::exceptions::UnexpectedException, "The MinMax equation system is unbounded.");
    STORM_LOG_THROW(solver->isOptimal(), storm::exceptions::UnexpectedException, "Unable to find optimal solution for MinMax equation system.");

    // write the solution into the solution vector
    STORM_LOG_ASSERT(x.size() == variableExpressions.size(), "Dimension of x-vector does not match number of varibales.");
    auto xIt = x.begin();
    auto vIt = variableExpressions.begin();
    for (; xIt != x.end(); ++xIt, ++vIt) {
        auto const& vBaseExpr = vIt->getBaseExpression();
        if (vBaseExpr.isVariable()) {
            *xIt = solver->getContinuousValue(vBaseExpr.asVariableExpression().getVariable());
        } else {
            STORM_LOG_ASSERT(vBaseExpr.isRationalLiteralExpression(), "Variable expression has unexpected type.");
            *xIt = storm::utility::convertNumber<ValueType>(vBaseExpr.asRationalLiteralExpression().getValue());
        }
    }

    // If requested, we store the scheduler for retrieval.
    if (this->isTrackSchedulerSet()) {
        this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
        for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
            if (!this->choiceFixedForRowGroup || !this->choiceFixedForRowGroup.get()[rowGroup]) {
                // Only update scheduler choice for the states that don't have a fixed choice
                uint64_t row = this->A->getRowGroupIndices()[rowGroup];
                uint64_t optimalChoiceIndex = 0;
                uint64_t currChoice = 0;
                ValueType optimalGroupValue = this->A->multiplyRowWithVector(row, x) + b[row];
                for (++row, ++currChoice; row < this->A->getRowGroupIndices()[rowGroup + 1]; ++row, ++currChoice) {
                    ValueType rowValue = this->A->multiplyRowWithVector(row, x) + b[row];
                    if ((minimize(dir) && rowValue < optimalGroupValue) || (maximize(dir) && rowValue > optimalGroupValue)) {
                        optimalGroupValue = rowValue;
                        optimalChoiceIndex = currChoice;
                    }
                }
                this->schedulerChoices.get()[rowGroup] = optimalChoiceIndex;
            }
        }
    }
    return true;
}

template<typename ValueType>
void LpMinMaxLinearEquationSolver<ValueType>::clearCache() const {
    StandardMinMaxLinearEquationSolver<ValueType>::clearCache();
}

template<typename ValueType>
MinMaxLinearEquationSolverRequirements LpMinMaxLinearEquationSolver<ValueType>::getRequirements(
    Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
    MinMaxLinearEquationSolverRequirements requirements;

    // In case we need to retrieve a scheduler, the solution has to be unique
    if (!this->hasUniqueSolution() && this->isTrackSchedulerSet()) {
        requirements.requireUniqueSolution();
    }

    requirements.requireBounds(false);

    return requirements;
}

template class LpMinMaxLinearEquationSolver<double>;

#ifdef STORM_HAVE_CARL
template class LpMinMaxLinearEquationSolver<storm::RationalNumber>;
#endif
}  // namespace solver
}  // namespace storm
