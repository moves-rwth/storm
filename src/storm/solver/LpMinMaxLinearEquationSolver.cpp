#include "storm/solver/LpMinMaxLinearEquationSolver.h"

#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidEnvironmentException.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory) : lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory) : StandardMinMaxLinearEquationSolver<ValueType>(A), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        LpMinMaxLinearEquationSolver<ValueType>::LpMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<storm::utility::solver::LpSolverFactory<ValueType>>&& lpSolverFactory) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A)), lpSolverFactory(std::move(lpSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool LpMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            
            STORM_LOG_THROW(env.solver().minMax().getMethod() == MinMaxMethod::LinearProgramming, storm::exceptions::InvalidEnvironmentException, "This min max solver does not support the selected technique.");
            
            // Set up the LP solver
            std::unique_ptr<storm::solver::LpSolver<ValueType>> solver = lpSolverFactory->create("");
            solver->setOptimizationDirection(invert(dir));
            
            // Create a variable for each row group
            std::vector<storm::expressions::Variable> variables;
            variables.reserve(this->A->getRowGroupCount());
            for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
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
            for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
                for (uint64_t row = this->A->getRowGroupIndices()[rowGroup]; row < this->A->getRowGroupIndices()[rowGroup + 1]; ++row) {
                    storm::expressions::Expression rowConstraint = solver->getConstant(b[row]);
                    for (auto const& entry : this->A->getRow(row)) {
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
                this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                for (uint64_t rowGroup = 0; rowGroup < this->A->getRowGroupCount(); ++rowGroup) {
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

            return true;
        }
       
        template<typename ValueType>
        void LpMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            StandardMinMaxLinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements LpMinMaxLinearEquationSolver<ValueType>::getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction, bool const& hasInitialScheduler) const {
            
            MinMaxLinearEquationSolverRequirements requirements;
            
            // In case we need to retrieve a scheduler, the solution has to be unique
            if (!this->hasUniqueSolution() && this->isTrackSchedulerSet()) {
                requirements.requireUniqueSolution();
            }
            
            requirements.requireBounds(false);
            
            return  requirements;
        }
        
        template class LpMinMaxLinearEquationSolver<double>;
        
#ifdef STORM_HAVE_CARL
        template class LpMinMaxLinearEquationSolver<storm::RationalNumber>;
#endif
    }
}
