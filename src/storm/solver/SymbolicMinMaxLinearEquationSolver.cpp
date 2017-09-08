#include "storm/solver/SymbolicMinMaxLinearEquationSolver.h"



#include "storm/storage/dd/DdManager.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/utility/constants.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/utility/dd.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        SymbolicMinMaxLinearEquationSolverSettings<ValueType>::SymbolicMinMaxLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::MinMaxEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
            
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(settings.getPrecision());
            relative = settings.getConvergenceCriterion() == storm::settings::modules::MinMaxEquationSolverSettings::ConvergenceCriterion::Relative;
            
            auto method = settings.getMinMaxEquationSolvingMethod();
            switch (method) {
                case MinMaxMethod::ValueIteration: this->solutionMethod = SolutionMethod::ValueIteration; break;
                case MinMaxMethod::PolicyIteration: this->solutionMethod = SolutionMethod::PolicyIteration; break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
        }
        
        template<typename ValueType>
        void SymbolicMinMaxLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        template<typename ValueType>
        void SymbolicMinMaxLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void SymbolicMinMaxLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void SymbolicMinMaxLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        typename SymbolicMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod const& SymbolicMinMaxLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return solutionMethod;
        }
        
        template<typename ValueType>
        uint64_t SymbolicMinMaxLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        ValueType SymbolicMinMaxLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        bool SymbolicMinMaxLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::SymbolicMinMaxLinearEquationSolver(SymbolicMinMaxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings), requirementsChecked(false) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::SymbolicMinMaxLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, std::unique_ptr<SymbolicLinearEquationSolverFactory<DdType, ValueType>>&& linearEquationSolverFactory, SymbolicMinMaxLinearEquationSolverSettings<ValueType> const& settings) : A(A), allRows(allRows), illegalMaskAdd(illegalMask.ite(A.getDdManager().getConstant(storm::utility::infinity<ValueType>()), A.getDdManager().template getAddZero<ValueType>())), rowMetaVariables(rowMetaVariables), columnMetaVariables(columnMetaVariables), choiceVariables(choiceVariables), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), settings(settings), requirementsChecked(false) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquations(storm::solver::OptimizationDirection const& dir, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            STORM_LOG_WARN_COND_DEBUG(this->isRequirementsCheckedSet(), "The requirements of the solver have not been marked as checked. Please provide the appropriate check or mark the requirements as checked (if applicable).");
            switch (this->getSettings().getSolutionMethod()) {
                case SymbolicMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::ValueIteration:
                    return solveEquationsValueIteration(dir, x, b);
                    break;
                case SymbolicMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration:
                    return solveEquationsPolicyIteration(dir, x, b);
                    break;
            }
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquationsValueIteration(storm::solver::OptimizationDirection const& dir, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            // Set up the environment.
            storm::dd::Add<DdType, ValueType> xCopy = x;
            uint_fast64_t iterations = 0;
            bool converged = false;
            
            // If we were given an initial scheduler, we take its solution as the starting point.
            if (this->hasInitialScheduler()) {
                xCopy = solveEquationsWithScheduler(this->getInitialScheduler(), xCopy, b);
            }
            
            while (!converged && iterations < this->settings.getMaximalNumberOfIterations()) {
                // Compute tmp = A * x + b
                storm::dd::Add<DdType, ValueType> xCopyAsColumn = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                storm::dd::Add<DdType, ValueType> tmp = this->A.multiplyMatrix(xCopyAsColumn, this->columnMetaVariables);
                tmp += b;
                
                if (dir == storm::solver::OptimizationDirection::Minimize) {
                    tmp += illegalMaskAdd;
                    tmp = tmp.minAbstract(this->choiceVariables);
                } else {
                    tmp = tmp.maxAbstract(this->choiceVariables);
                }
                
                // Now check if the process already converged within our precision.
                converged = xCopy.equalModuloPrecision(tmp, this->settings.getPrecision(), this->settings.getRelativeTerminationCriterion());
                
                xCopy = tmp;
                
                ++iterations;
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver (value iteration) converged in " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver (value iteration) did not converge in " << iterations << " iterations.");
            }
            
            return xCopy;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquationsWithScheduler(storm::dd::Bdd<DdType> const& scheduler, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            
            std::unique_ptr<SymbolicLinearEquationSolver<DdType, ValueType>> solver = linearEquationSolverFactory->create(this->allRows, this->rowMetaVariables, this->columnMetaVariables, this->rowColumnMetaVariablePairs);
            storm::dd::Add<DdType, ValueType> diagonal = (storm::utility::dd::getRowColumnDiagonal<DdType>(x.getDdManager(), this->rowColumnMetaVariablePairs) && this->allRows).template toAdd<ValueType>();
            return solveEquationsWithScheduler(*solver, scheduler, x, b, diagonal);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquationsWithScheduler(SymbolicLinearEquationSolver<DdType, ValueType>& solver, storm::dd::Bdd<DdType> const& scheduler, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b, storm::dd::Add<DdType, ValueType> const& diagonal) const {
            
            // Apply scheduler to the matrix and vector.
            storm::dd::Add<DdType, ValueType> schedulerA = diagonal - scheduler.ite(this->A, scheduler.getDdManager().template getAddZero<ValueType>()).sumAbstract(this->choiceVariables);
            storm::dd::Add<DdType, ValueType> schedulerB = scheduler.ite(b, scheduler.getDdManager().template getAddZero<ValueType>()).sumAbstract(this->choiceVariables);

            // Set the matrix for the solver.
            solver.setMatrix(schedulerA);
            
            // Solve for the value of the scheduler.
            storm::dd::Add<DdType, ValueType> schedulerX = solver.solveEquations(x, schedulerB);

            return schedulerX;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType>  SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::solveEquationsPolicyIteration(storm::solver::OptimizationDirection const& dir, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            // Set up the environment.
            storm::dd::Add<DdType, ValueType> currentSolution = x;
            storm::dd::Add<DdType, ValueType> diagonal = (storm::utility::dd::getRowColumnDiagonal<DdType>(x.getDdManager(), this->rowColumnMetaVariablePairs) && this->allRows).template toAdd<ValueType>();
            uint_fast64_t iterations = 0;
            bool converged = false;
        
            // Choose initial scheduler.
            storm::dd::Bdd<DdType> scheduler = this->hasInitialScheduler() ? this->getInitialScheduler() : this->A.sumAbstract(this->columnMetaVariables).maxAbstractRepresentative(this->choiceVariables);
            
            // Initialize linear equation solver.
            std::unique_ptr<SymbolicLinearEquationSolver<DdType, ValueType>> linearEquationSolver = linearEquationSolverFactory->create(this->allRows, this->rowMetaVariables, this->columnMetaVariables, this->rowColumnMetaVariablePairs);
            
            // Iteratively solve and improve the scheduler.
            while (!converged && iterations < this->settings.getMaximalNumberOfIterations()) {
                storm::dd::Add<DdType, ValueType> schedulerX = solveEquationsWithScheduler(*linearEquationSolver, scheduler, currentSolution, b, diagonal);
                
                // Policy improvement step.
                storm::dd::Add<DdType, ValueType> choiceValues = this->A.multiplyMatrix(schedulerX.swapVariables(this->rowColumnMetaVariablePairs), this->columnMetaVariables) + b;
                
                storm::dd::Bdd<DdType> nextScheduler;
                if (dir == storm::solver::OptimizationDirection::Minimize) {
                    choiceValues += illegalMaskAdd;
                    nextScheduler = choiceValues.minAbstractRepresentative(this->choiceVariables);
                } else {
                    nextScheduler = choiceValues.maxAbstractRepresentative(this->choiceVariables);
                }
                
                // Check for convergence.
                converged = nextScheduler == scheduler;
                
                // Set up next iteration.
                if (!converged) {
                    scheduler = nextScheduler;
                }
                
                currentSolution = schedulerX;
                ++iterations;
            }
            
            if (converged) {
                STORM_LOG_INFO("Iterative solver (policy iteration) converged in " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver (policy iteration) did not converge in " << iterations << " iterations.");
            }

            return currentSolution;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::multiply(storm::solver::OptimizationDirection const& dir, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b, uint_fast64_t n) const {
            storm::dd::Add<DdType, ValueType> xCopy = x;
            
            // Perform matrix-vector multiplication while the bound is met.
            for (uint_fast64_t i = 0; i < n; ++i) {
                xCopy = xCopy.swapVariables(this->rowColumnMetaVariablePairs);
                xCopy = this->A.multiplyMatrix(xCopy, this->columnMetaVariables);
                if (b != nullptr) {
                    xCopy += *b;
                }
                
                if (dir == storm::solver::OptimizationDirection::Minimize) {
                    // This is a hack and only here because of the lack of a suitable minAbstract/maxAbstract function
                    // that can properly deal with a restriction of the choices.
                    xCopy += illegalMaskAdd;
                    xCopy = xCopy.minAbstract(this->choiceVariables);
                } else {
                    xCopy = xCopy.maxAbstract(this->choiceVariables);
                }
            }
            
            return xCopy;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::setInitialScheduler(storm::dd::Bdd<DdType> const& scheduler) {
            this->initialScheduler = scheduler;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Bdd<DdType> const& SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::getInitialScheduler() const {
            return initialScheduler.get();
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::hasInitialScheduler() const {
            return static_cast<bool>(initialScheduler);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        MinMaxLinearEquationSolverRequirements SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::getRequirements(EquationSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            MinMaxLinearEquationSolverRequirements requirements;
            
            if (equationSystemType == EquationSystemType::UntilProbabilities) {
                if (this->getSettings().getSolutionMethod() == SymbolicMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration) {
                    if (!direction || direction.get() == OptimizationDirection::Maximize) {
                        requirements.set(MinMaxLinearEquationSolverRequirements::Element::ValidInitialScheduler);
                    }
                }
            } else if (equationSystemType == EquationSystemType::ReachabilityRewards) {
                if (!direction || direction.get() == OptimizationDirection::Minimize) {
                    requirements.set(MinMaxLinearEquationSolverRequirements::Element::ValidInitialScheduler);
                }
            }
            
            return requirements;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        void SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::setRequirementsChecked(bool value) {
            this->requirementsChecked = value;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        bool SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::isRequirementsCheckedSet() const {
            return this->requirementsChecked;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolverSettings<ValueType> const& SymbolicMinMaxLinearEquationSolver<DdType, ValueType>::getSettings() const {
            return settings;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        MinMaxLinearEquationSolverRequirements SymbolicMinMaxLinearEquationSolverFactory<DdType, ValueType>::getRequirements(EquationSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> solver = this->create();
            return solver->getRequirements(equationSystemType, direction);
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType>::create(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) const {
            return std::make_unique<SymbolicMinMaxLinearEquationSolver<DdType, ValueType>>(A, allRows, illegalMask, rowMetaVariables, columnMetaVariables, choiceVariables, rowColumnMetaVariablePairs, std::make_unique<GeneralSymbolicLinearEquationSolverFactory<DdType, ValueType>>(), settings);
        }
                
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolverSettings<ValueType>& SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType>::getSettings() {
            return settings;
        }

        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicMinMaxLinearEquationSolverSettings<ValueType> const& SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType>::getSettings() const {
            return settings;
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        std::unique_ptr<storm::solver::SymbolicMinMaxLinearEquationSolver<DdType, ValueType>> SymbolicGeneralMinMaxLinearEquationSolverFactory<DdType, ValueType>::create() const {
            return std::make_unique<SymbolicMinMaxLinearEquationSolver<DdType, ValueType>>(settings);
        }
        
        template class SymbolicMinMaxLinearEquationSolverSettings<double>;
        template class SymbolicMinMaxLinearEquationSolverSettings<storm::RationalNumber>;
        
        template class SymbolicMinMaxLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicMinMaxLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        template class SymbolicMinMaxLinearEquationSolver<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        
        template class SymbolicGeneralMinMaxLinearEquationSolverFactory<storm::dd::DdType::CUDD, double>;
        template class SymbolicGeneralMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, double>;
        template class SymbolicGeneralMinMaxLinearEquationSolverFactory<storm::dd::DdType::Sylvan, storm::RationalNumber>;
        
    }
}
