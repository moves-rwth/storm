#include "storm/solver/LpMinMaxLinearEquationSolver.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory) : StandardMinMaxLinearEquationSolver<ValueType>(A, std::move(linearEquationSolverFactory)), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A), std::move(linearEquationSolverFactory)), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool LpMinMaxLinearEquationSolver<ValueType>::solveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            
            // Set up the LP solver
            std::unique_ptr<storm::solver::LpSolver<ValueType>> solver = lpSolverFactory->create("");
            solver->setOptimizationDirection(invert(dir));
            
            // Create a variable for each row group
            std::vector<storm::expressions::Variable> variables;
            variables.reserve(this->A.getRowGroupCount());
            for (uint64_t rowGroup = 0; rowGroup < this->A.getRowGroupCount(); ++rowGroup) {
                if (this->lowerBound) {
                    if (this->upperBound) {
                        variables.push_back(solver->addBoundedContinuousVariable("x" + std::to_string(rowGroup), this->lowerBound.get(), this->upperBound.get(), storm::utility::one<ValueType>()));
                    } else {
                        variables.push_back(solver->addLowerBoundedContinuousVariable("x" + std::to_string(rowGroup), this->lowerBound.get(), storm::utility::one<ValueType>()));
                    }
                } else {
                    if (this->upperBound) {
                        variables.push_back(solver->addUpperBoundedContinuousVariable("x" + std::to_string(rowGroup), this->upperBound.get(), storm::utility::one<ValueType>()));
                    } else {
                        variables.push_back(solver->addUnboundedContinuousVariable("x" + std::to_string(rowGroup), storm::utility::one<ValueType>()));
                    }
                }
            }
            solver->update();
            
            // Add a constraint for each row
            for (uint64_t rowGroup = 0; rowGroup < this->A.getRowGroupCount(); ++rowGroup) {
                for (uint64_t row = this->A.getRowGroupIndices()[rowGroup]; row < this->A.getRowGroupIndices()[rowGroup + 1]; ++row) {
                    storm::expressions::Expression rowConstraint = solver->getConstant(b[row]);
                    for (auto const& entry : this->A.getRow(row)) {
                        rowConstraint = rowConstraint + (solver->getConstant(entry.getValue()) * variables[entry.getColumn()].getExpression());
                    }
                    if (minimize(dir)) {
                        rowConstraint = variables[rowGroup].getExpression() <= rowConstraint;
                    } else {
                        rowConstraint = variables[rowGroup].getExpression() >= rowConstraint;
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
            STORM_LOG_ASSERT(x.size() == variables.size(), "Dimension of x-vector does not match number of varibales.");
            auto xIt = x.begin();
            auto vIt = variables.begin();
            for (; xIt != x.end(); ++xIt, ++vIt) {
                *xIt = solver->getContinuousValue(*vIt);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::vector<uint_fast64_t>(this->A.getRowGroupCount());
                for (uint64_t rowGroup = 0; rowGroup < this->A.getRowGroupCount(); ++rowGroup) {
                    uint64_t row = this->A.getRowGroupIndices()[rowGroup];
                    uint64_t optimalChoiceIndex = 0;
                    uint64_t currChoice = 0;
                    ValueType optimalGroupValue = this->A.multiplyRowWithVector(row, x) + b[row];
                    for (++row, ++currChoice; row < this->A.getRowGroupIndices()[rowGroup + 1]; ++row, ++currChoice) {
                        ValueType rowValue = this->A.multiplyRowWithVector(row, x) + b[row];
                        if ((minimize(dir) && rowValue < optimalGroupValue) || (maximize(dir) && rowValue > optimalGroupValue)) {
                            optimalGroupValue = rowValue;
                            optimalChoiceIndex = currChoice;
                        }
                    }
                    this->schedulerChoices.get()[rowGroup] = optimalChoiceIndex;
                }
            }

            return true;
        }
       
        template<typename ValueType>
        void LpMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            StandardMinMaxLinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolverFactory<ValueType>::LpMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(MinMaxMethodSelection::LinearProgramming, trackScheduler), lpSolverFactory(std::make_unique<storm::utility::solver::LpSolverFactory<ValueType>>()) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolverFactory<ValueType>::LpMinMaxLinearEquationSolverFactory(std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(MinMaxMethodSelection::LinearProgramming, trackScheduler), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty
        }
       
        template<typename ValueType>
        LpMinMaxLinearEquationSolverFactory<ValueType>::LpMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(std::move(linearEquationSolverFactory), MinMaxMethodSelection::LinearProgramming, trackScheduler), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> LpMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            STORM_LOG_ASSERT(this->linearEquationSolverFactory, "Linear equation solver factory not initialized.");
            STORM_LOG_ASSERT(this->lpSolverFactory, "Lp solver factory not initialized.");
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result = std::make_unique<LpMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), this->linearEquationSolverFactory->clone(), this->lpSolverFactory->clone());
            result->setTrackScheduler(this->isTrackSchedulerSet());
            return result;
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> LpMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            STORM_LOG_ASSERT(this->linearEquationSolverFactory, "Linear equation solver factory not initialized.");
            STORM_LOG_ASSERT(this->lpSolverFactory, "Lp solver factory not initialized.");
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result = std::make_unique<LpMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), this->linearEquationSolverFactory->clone(), this->lpSolverFactory->clone());
            result->setTrackScheduler(this->isTrackSchedulerSet());
            return result;
        }
        
        template<typename ValueType>
        void LpMinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethodSelection const& newMethod) {
            STORM_LOG_THROW(newMethod == MinMaxMethodSelection::LinearProgramming, storm::exceptions::InvalidOperationException, "The factory can only create linear programming based MinMax solvers.");
            MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(newMethod);
        }
        
        template<typename ValueType>
        void LpMinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethod const& newMethod) {
            STORM_LOG_THROW(newMethod == MinMaxMethod::LinearProgramming, storm::exceptions::InvalidOperationException, "The factory can only create linear programming based MinMax solvers.");
            MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(newMethod);
        }
        
        template class LpMinMaxLinearEquationSolver<double>;
        template class LpMinMaxLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class LpMinMaxLinearEquationSolver<storm::RationalNumber>;
        template class LpMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
