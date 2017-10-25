#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"

#include "storm/utility/ConstantsComparator.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/utility/KwekMehlhorn.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/UnmetRequirementException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/PrecisionExceededException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverSettings<ValueType>::IterativeMinMaxLinearEquationSolverSettings() {
            // Get the settings object to customize solving.
            storm::settings::modules::MinMaxEquationSolverSettings const& minMaxSettings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
            
            maximalNumberOfIterations = minMaxSettings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(minMaxSettings.getPrecision());
            relative = minMaxSettings.getConvergenceCriterion() == storm::settings::modules::MinMaxEquationSolverSettings::ConvergenceCriterion::Relative;
            valueIterationMultiplicationStyle = minMaxSettings.getValueIterationMultiplicationStyle();
            
            setSolutionMethod(minMaxSettings.getMinMaxEquationSolvingMethod());
            
            // Finally force soundness and potentially overwrite some other settings.
            this->setForceSoundness(storm::settings::getModule<storm::settings::modules::GeneralSettings>().isSoundSet());
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setSolutionMethod(MinMaxMethod const& solutionMethod) {
            switch (solutionMethod) {
                case MinMaxMethod::ValueIteration: this->solutionMethod = SolutionMethod::ValueIteration; break;
                case MinMaxMethod::PolicyIteration: this->solutionMethod = SolutionMethod::PolicyIteration; break;
                case MinMaxMethod::RationalSearch: this->solutionMethod = SolutionMethod::RationalSearch; break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique for iterative MinMax linear equation solver.");
            }
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setValueIterationMultiplicationStyle(MultiplicationStyle value) {
            this->valueIterationMultiplicationStyle = value;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverSettings<ValueType>::setForceSoundness(bool value) {
            this->forceSoundness = value;
        }
        
        template<typename ValueType>
        typename IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod const& IterativeMinMaxLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return solutionMethod;
        }
        
        template<typename ValueType>
        uint64_t IterativeMinMaxLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        ValueType IterativeMinMaxLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        MultiplicationStyle IterativeMinMaxLinearEquationSolverSettings<ValueType>::getValueIterationMultiplicationStyle() const {
            return valueIterationMultiplicationStyle;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolverSettings<ValueType>::getForceSoundness() const {
            return forceSoundness;
        }
    
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolver<ValueType>::IterativeMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(linearEquationSolverFactory)), settings(settings) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolver<ValueType>::IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings) : StandardMinMaxLinearEquationSolver<ValueType>(A, std::move(linearEquationSolverFactory)), settings(settings) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolver<ValueType>::IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, IterativeMinMaxLinearEquationSolverSettings<ValueType> const& settings) : StandardMinMaxLinearEquationSolver<ValueType>(std::move(A), std::move(linearEquationSolverFactory)), settings(settings) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::internalSolveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            bool result = false;
            switch (this->getSettings().getSolutionMethod()) {
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::ValueIteration:
                    if (this->getSettings().getForceSoundness()) {
                        result = solveEquationsSoundValueIteration(dir, x, b);
                    } else {
                        result = solveEquationsValueIteration(dir, x, b);
                    }
                    break;
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration:
                    result = solveEquationsPolicyIteration(dir, x, b);
                    break;
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch:
                    result = solveEquationsRationalSearch(dir, x, b);
                    break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "This solver does not implement the selected solution method");
            }
            
            return result;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Create the initial scheduler.
            std::vector<storm::storage::sparse::state_type> scheduler = this->hasInitialScheduler() ? this->getInitialScheduler() : std::vector<storm::storage::sparse::state_type>(this->A->getRowGroupCount());
            
            // Get a vector for storing the right-hand side of the inner equation system.
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            std::vector<ValueType>& subB = *auxiliaryRowGroupVector;

            // Resolve the nondeterminism according to the current scheduler.
            bool convertToEquationSystem = this->linearEquationSolverFactory->getEquationProblemFormat() == LinearEquationSolverProblemFormat::EquationSystem;
            storm::storage::SparseMatrix<ValueType> submatrix = this->A->selectRowsFromRowGroups(scheduler, convertToEquationSystem);
            if (convertToEquationSystem) {
                submatrix.convertToEquationSystem();
            }
            storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A->getRowGroupIndices(), b);

            // Create a solver that we will use throughout the procedure. We will modify the matrix in each iteration.
            auto solver = this->linearEquationSolverFactory->create(std::move(submatrix));
            if (this->lowerBound) {
                solver->setLowerBound(this->lowerBound.get());
            }
            if (this->upperBound) {
                solver->setUpperBound(this->upperBound.get());
            }
            solver->setCachingEnabled(true);
            
            SolverStatus status = SolverStatus::InProgress;
            uint64_t iterations = 0;
            this->startMeasureProgress();
            do {
                // Solve the equation system for the 'DTMC'.
                solver->solveEquations(x, subB);
                
                // Go through the multiplication result and see whether we can improve any of the choices.
                bool schedulerImproved = false;
                for (uint_fast64_t group = 0; group < this->A->getRowGroupCount(); ++group) {
                    uint_fast64_t currentChoice = scheduler[group];
                    for (uint_fast64_t choice = this->A->getRowGroupIndices()[group]; choice < this->A->getRowGroupIndices()[group + 1]; ++choice) {
                        // If the choice is the currently selected one, we can skip it.
                        if (choice - this->A->getRowGroupIndices()[group] == currentChoice) {
                            continue;
                        }
                        
                        // Create the value of the choice.
                        ValueType choiceValue = storm::utility::zero<ValueType>();
                        for (auto const& entry : this->A->getRow(choice)) {
                            choiceValue += entry.getValue() * x[entry.getColumn()];
                        }
                        choiceValue += b[choice];
                        
                        // If the value is strictly better than the solution of the inner system, we need to improve the scheduler.
                        // TODO: If the underlying solver is not precise, this might run forever (i.e. when a state has two choices where the (exact) values are equal).
                        // only changing the scheduler if the values are not equal (modulo precision) would make this unsound.
                        if (valueImproved(dir, x[group], choiceValue)) {
                            schedulerImproved = true;
                            scheduler[group] = choice - this->A->getRowGroupIndices()[group];
                            x[group] = std::move(choiceValue);
                        }
                    }
                }
                
                // If the scheduler did not improve, we are done.
                if (!schedulerImproved) {
                    status = SolverStatus::Converged;
                } else {
                    // Update the scheduler and the solver.
                    submatrix = this->A->selectRowsFromRowGroups(scheduler, true);
                    if (convertToEquationSystem) {
                        submatrix.convertToEquationSystem();
                    }
                    storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A->getRowGroupIndices(), b);
                    solver->setMatrix(std::move(submatrix));
                }
                
                // Update environment variables.
                ++iterations;
                status = updateStatusIfNotConverged(status, x, iterations, dir == storm::OptimizationDirection::Minimize ? SolverGuarantee::GreaterOrEqual : SolverGuarantee::LessOrEqual);

                // Potentially show progress.
                this->showProgressIterative(iterations);
            } while (status == SolverStatus::InProgress);
            
            reportStatus(status, iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::move(scheduler);
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }

            return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const {
            if (dir == OptimizationDirection::Minimize) {
                return value2 < value1;
            } else {
                return value2 > value1;
            }
        }

        template<typename ValueType>
        ValueType IterativeMinMaxLinearEquationSolver<ValueType>::getPrecision() const {
            return this->getSettings().getPrecision();
        }

        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::getRelative() const {
            return this->getSettings().getRelativeTerminationCriterion();
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements IterativeMinMaxLinearEquationSolver<ValueType>::getRequirements(boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            // Start by copying the requirements of the linear equation solver.
            MinMaxLinearEquationSolverRequirements requirements(this->linearEquationSolverFactory->getRequirements());

            if (this->getSettings().getSolutionMethod() == IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::ValueIteration) {
                if (this->getSettings().getForceSoundness()) {
                    // Interval iteration requires a unique solution and lower+upper bounds
                    if (!this->hasUniqueSolution()) {
                        requirements.requireNoEndComponents();
                    }
                    requirements.requireBounds();
                } else if (!this->hasUniqueSolution()) { // Traditional value iteration has no requirements if the solution is unique.
                    // Computing a scheduler is only possible if the solution is unique
                    if (this->isTrackSchedulerSet()) {
                        requirements.requireNoEndComponents();
                    } else {
                        // As we want the smallest (largest) solution for maximizing (minimizing) equation systems, we have to approach the solution from below (above).
                        if (!direction || direction.get() == OptimizationDirection::Maximize) {
                            requirements.requireLowerBounds();
                        }
                        if (!direction || direction.get() == OptimizationDirection::Minimize) {
                            requirements.requireUpperBounds();
                        }
                    }
                }
            } else if (this->getSettings().getSolutionMethod() == IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::RationalSearch) {
                // Rational search needs to approach the solution from below.
                requirements.requireLowerBounds();
                // The solution needs to be unique in case of minimizing or in cases where we want a scheduler.
                if (!this->hasUniqueSolution() && (!direction || direction.get() == OptimizationDirection::Minimize || this->isTrackSchedulerSet())) {
                    requirements.requireNoEndComponents();
                }
            } else if (this->getSettings().getSolutionMethod() == IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration) {
                if (!this->hasUniqueSolution()) {
                    requirements.requireValidInitialScheduler();
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique for iterative MinMax linear equation solver.");
            }
        
            return requirements;
        }

        template<typename ValueType>
        typename IterativeMinMaxLinearEquationSolver<ValueType>::ValueIterationResult IterativeMinMaxLinearEquationSolver<ValueType>::performValueIteration(OptimizationDirection dir, std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations) const {
            
            STORM_LOG_ASSERT(currentX != newX, "Vectors must not be aliased.");
            
            // Get handle to linear equation solver.
            storm::solver::LinearEquationSolver<ValueType> const& linearEquationSolver = *this->linEqSolverA;
            
            // Allow aliased multiplications.
            bool useGaussSeidelMultiplication = linearEquationSolver.supportsGaussSeidelMultiplication() && settings.getValueIterationMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = currentIterations;
            
            std::vector<ValueType>* originalX = currentX;
            
            SolverStatus status = SolverStatus::InProgress;
            while (status == SolverStatus::InProgress) {
                // Compute x' = min/max(A*x + b).
                if (useGaussSeidelMultiplication) {
                    // Copy over the current vector so we can modify it in-place.
                    *newX = *currentX;
                    linearEquationSolver.multiplyAndReduceGaussSeidel(dir, this->A->getRowGroupIndices(), *newX, &b);
                } else {
                    linearEquationSolver.multiplyAndReduce(dir, this->A->getRowGroupIndices(), *currentX, &b, *newX);
                }
                
                // Determine whether the method converged.
                if (storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, precision, relative)) {
                    status = SolverStatus::Converged;
                }
                
                // Update environment variables.
                std::swap(currentX, newX);
                ++iterations;
                status = updateStatusIfNotConverged(status, *currentX, iterations, guarantee);

                // Potentially show progress.
                this->showProgressIterative(iterations);
            }
            
            // Swap the pointers so that the output is always in currentX.
            if (originalX == newX) {
                std::swap(currentX, newX);
            }
            
            return ValueIterationResult(iterations - currentIterations, status);
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            // By default, the guarantee that we can provide is that our solution is always less-or-equal than the
            // actual solution.
            SolverGuarantee guarantee = SolverGuarantee::LessOrEqual;
            
            if (this->hasInitialScheduler()) {
                // Resolve the nondeterminism according to the initial scheduler.
                bool convertToEquationSystem = this->linearEquationSolverFactory->getEquationProblemFormat() == LinearEquationSolverProblemFormat::EquationSystem;
                storm::storage::SparseMatrix<ValueType> submatrix = this->A->selectRowsFromRowGroups(this->getInitialScheduler(), convertToEquationSystem);
                if (convertToEquationSystem) {
                    submatrix.convertToEquationSystem();
                }
                storm::utility::vector::selectVectorValues<ValueType>(*auxiliaryRowGroupVector, this->getInitialScheduler(), this->A->getRowGroupIndices(), b);

                // Solve the resulting equation system.
                auto submatrixSolver = this->linearEquationSolverFactory->create(std::move(submatrix));
                submatrixSolver->setCachingEnabled(true);
                if (this->lowerBound) {
                    submatrixSolver->setLowerBound(this->lowerBound.get());
                }
                if (this->upperBound) {
                    submatrixSolver->setUpperBound(this->upperBound.get());
                }
                submatrixSolver->solveEquations(x, *auxiliaryRowGroupVector);
                
                // If we were given an initial scheduler and are in fact minimizing, our current solution becomes
                // always greater-or-equal than the actual solution.
                if (dir == storm::OptimizationDirection::Minimize) {
                    guarantee = SolverGuarantee::GreaterOrEqual;
                }
            } else {
                // If no initial scheduler is given, we start from the lower bound.
                this->createLowerBoundsVector(x);
            }

            std::vector<ValueType>* newX = auxiliaryRowGroupVector.get();
            std::vector<ValueType>* currentX = &x;
            
            this->startMeasureProgress();
            ValueIterationResult result = performValueIteration(dir, currentX, newX, b, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion(), guarantee, 0);

            // Swap the result into the output x.
            if (currentX == auxiliaryRowGroupVector.get()) {
                std::swap(x, *currentX);
            }
            
            reportStatus(result.status, result.iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), x, &b, *auxiliaryRowGroupVector.get(), &this->schedulerChoices.get());
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            return result.status == SolverStatus::Converged || result.status == SolverStatus::TerminatedEarly;
        }
        
        template<typename ValueType>
        void preserveOldRelevantValues(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType>& oldValues) {
            storm::utility::vector::selectVectorValues(oldValues, relevantValues, allValues);
        }
        
        template<typename ValueType>
        ValueType computeMaxAbsDiff(std::vector<ValueType> const& allValues, storm::storage::BitVector const& relevantValues, std::vector<ValueType> const& oldValues) {
            ValueType result = storm::utility::zero<ValueType>();
            auto oldValueIt = oldValues.begin();
            for (auto value : relevantValues) {
                result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allValues[value] - *oldValueIt));
            }
            return result;
        }
        
        template<typename ValueType>
        ValueType computeMaxAbsDiff(std::vector<ValueType> const& allOldValues, std::vector<ValueType> const& allNewValues, storm::storage::BitVector const& relevantValues) {
            ValueType result = storm::utility::zero<ValueType>();
            for (auto value : relevantValues) {
                result = storm::utility::max<ValueType>(result, storm::utility::abs<ValueType>(allNewValues[value] - allOldValues[value]));
            }
            return result;
        }
        
        /*!
         * This version of value iteration is sound, because it approaches the solution from below and above. This
         * technique is due to Haddad and Monmege (Interval iteration algorithm for MDPs and IMDPs, TCS 2017) and was
         * extended to rewards by Baier, Klein, Leuschner, Parker and Wunderlich (Ensuring the Reliability of Your
         * Model Checker: Interval Iteration for Markov Decision Processes, CAV 2017).
         */
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsSoundValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(this->hasUpperBound(), storm::exceptions::UnmetRequirementException, "Solver requires upper bound, but none was given.");

            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            // Allow aliased multiplications.
            bool useGaussSeidelMultiplication = this->linEqSolverA->supportsGaussSeidelMultiplication() && settings.getValueIterationMultiplicationStyle() == storm::solver::MultiplicationStyle::GaussSeidel;
            
            std::vector<ValueType>* lowerX = &x;
            this->createLowerBoundsVector(*lowerX);
            this->createUpperBoundsVector(this->auxiliaryRowGroupVector, this->A->getRowGroupCount());
            std::vector<ValueType>* upperX = this->auxiliaryRowGroupVector.get();
            
            std::vector<ValueType>* tmp = nullptr;
            if (!useGaussSeidelMultiplication) {
                auxiliaryRowGroupVector2 = std::make_unique<std::vector<ValueType>>(lowerX->size());
                tmp = auxiliaryRowGroupVector2.get();
            }
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = 0;
            
            SolverStatus status = SolverStatus::InProgress;
            bool doConvergenceCheck = true;
            bool useDiffs = this->hasRelevantValues();
            std::vector<ValueType> oldValues;
            if (useGaussSeidelMultiplication && useDiffs) {
                oldValues.resize(this->getRelevantValues().getNumberOfSetBits());
            }
            ValueType maxLowerDiff = storm::utility::zero<ValueType>();
            ValueType maxUpperDiff = storm::utility::zero<ValueType>();
            ValueType precision = static_cast<ValueType>(this->getSettings().getPrecision());
            if (!this->getSettings().getRelativeTerminationCriterion()) {
                precision *= storm::utility::convertNumber<ValueType>(2.0);
            }
            this->startMeasureProgress();
            while (status == SolverStatus::InProgress && iterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Remember in which directions we took steps in this iteration.
                bool lowerStep = false;
                bool upperStep = false;

                // In every thousandth iteration, we improve both bounds.
                if (iterations % 1000 == 0 || maxLowerDiff == maxUpperDiff) {
                    lowerStep = true;
                    upperStep = true;
                    if (useGaussSeidelMultiplication) {
                        if (useDiffs) {
                            preserveOldRelevantValues(*lowerX, this->getRelevantValues(), oldValues);
                        }
                        this->linEqSolverA->multiplyAndReduceGaussSeidel(dir, this->A->getRowGroupIndices(), *lowerX, &b);
                        if (useDiffs) {
                            maxLowerDiff = computeMaxAbsDiff(*lowerX, this->getRelevantValues(), oldValues);
                            preserveOldRelevantValues(*upperX, this->getRelevantValues(), oldValues);
                        }
                        this->linEqSolverA->multiplyAndReduceGaussSeidel(dir, this->A->getRowGroupIndices(), *upperX, &b);
                        if (useDiffs) {
                            maxUpperDiff = computeMaxAbsDiff(*upperX, this->getRelevantValues(), oldValues);
                        }
                    } else {
                        this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), *lowerX, &b, *tmp);
                        if (useDiffs) {
                            maxLowerDiff = computeMaxAbsDiff(*lowerX, *tmp, this->getRelevantValues());
                        }
                        std::swap(lowerX, tmp);
                        this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), *upperX, &b, *tmp);
                        if (useDiffs) {
                            maxUpperDiff = computeMaxAbsDiff(*upperX, *tmp, this->getRelevantValues());
                        }
                        std::swap(upperX, tmp);
                    }
                } else {
                    // In the following iterations, we improve the bound with the greatest difference.
                    if (useGaussSeidelMultiplication) {
                        if (maxLowerDiff >= maxUpperDiff) {
                            if (useDiffs) {
                                preserveOldRelevantValues(*lowerX, this->getRelevantValues(), oldValues);
                            }
                            this->linEqSolverA->multiplyAndReduceGaussSeidel(dir, this->A->getRowGroupIndices(), *lowerX, &b);
                            if (useDiffs) {
                                maxLowerDiff = computeMaxAbsDiff(*lowerX, this->getRelevantValues(), oldValues);
                            }
                            lowerStep = true;
                        } else {
                            if (useDiffs) {
                                preserveOldRelevantValues(*upperX, this->getRelevantValues(), oldValues);
                            }
                            this->linEqSolverA->multiplyAndReduceGaussSeidel(dir, this->A->getRowGroupIndices(), *upperX, &b);
                            if (useDiffs) {
                                maxUpperDiff = computeMaxAbsDiff(*upperX, this->getRelevantValues(), oldValues);
                            }
                            upperStep = true;
                        }
                    } else {
                        if (maxLowerDiff >= maxUpperDiff) {
                            this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), *lowerX, &b, *tmp);
                            if (useDiffs) {
                                maxLowerDiff = computeMaxAbsDiff(*lowerX, *tmp, this->getRelevantValues());
                            }
                            std::swap(tmp, lowerX);
                            lowerStep = true;
                        } else {
                            this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), *upperX, &b, *tmp);
                            if (useDiffs) {
                                maxUpperDiff = computeMaxAbsDiff(*upperX, *tmp, this->getRelevantValues());
                            }
                            std::swap(tmp, upperX);
                            upperStep = true;
                        }
                    }
                }
                STORM_LOG_ASSERT(maxLowerDiff >= storm::utility::zero<ValueType>(), "Expected non-negative lower diff.");
                STORM_LOG_ASSERT(maxUpperDiff >= storm::utility::zero<ValueType>(), "Expected non-negative upper diff.");
                if (iterations % 1000 == 0) {
                    STORM_LOG_TRACE("Iteration " << iterations << ": lower difference: " << maxLowerDiff << ", upper difference: " << maxUpperDiff << ".");
                }

                if (doConvergenceCheck) {
                    // Determine whether the method converged.
                    if (this->hasRelevantValues()) {
                        status = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, this->getRelevantValues(), precision, this->getSettings().getRelativeTerminationCriterion()) ? SolverStatus::Converged : status;
                    } else {
                        status = storm::utility::vector::equalModuloPrecision<ValueType>(*lowerX, *upperX, precision, this->getSettings().getRelativeTerminationCriterion()) ? SolverStatus::Converged : status;
                    }
                }
                
                // Update environment variables.
                ++iterations;
                doConvergenceCheck = !doConvergenceCheck;
                if (lowerStep) {
                    status = updateStatusIfNotConverged(status, *lowerX, iterations, SolverGuarantee::LessOrEqual);
                }
                if (upperStep) {
                    status = updateStatusIfNotConverged(status, *upperX, iterations, SolverGuarantee::GreaterOrEqual);
                }

                // Potentially show progress.
                this->showProgressIterative(iterations);
            }
            
            reportStatus(status, iterations);
            
            // We take the means of the lower and upper bound so we guarantee the desired precision.
            ValueType two = storm::utility::convertNumber<ValueType>(2.0);
            storm::utility::vector::applyPointwise<ValueType, ValueType, ValueType>(*lowerX, *upperX, *lowerX, [&two] (ValueType const& a, ValueType const& b) -> ValueType { return (a + b) / two; });
            
            // Since we shuffled the pointer around, we need to write the actual results to the input/output vector x.
            if (&x == tmp) {
                std::swap(x, *tmp);
            } else if (&x == this->auxiliaryRowGroupVector.get()) {
                std::swap(x, *this->auxiliaryRowGroupVector);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                this->linEqSolverA->multiplyAndReduce(dir, this->A->getRowGroupIndices(), x, &b, *this->auxiliaryRowGroupVector, &this->schedulerChoices.get());
            }
            
            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            return status == SolverStatus::Converged;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::isSolution(storm::OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b) {
            storm::utility::ConstantsComparator<ValueType> comparator;
            
            auto valueIt = values.begin();
            auto bIt = b.begin();
            for (uint64_t group = 0; group < matrix.getRowGroupCount(); ++group, ++valueIt) {
                ValueType groupValue = *bIt;
                uint64_t row = matrix.getRowGroupIndices()[group];
                groupValue += matrix.multiplyRowWithVector(row, values);

                ++row;
                ++bIt;

                for (auto endRow = matrix.getRowGroupIndices()[group + 1]; row < endRow; ++row, ++bIt) {
                    ValueType newValue = *bIt;
                    newValue += matrix.multiplyRowWithVector(row, values);
                    
                    if ((dir == storm::OptimizationDirection::Minimize && newValue < groupValue) || (dir == storm::OptimizationDirection::Maximize && newValue > groupValue)) {
                        groupValue = newValue;
                    }
                }
                
                // If the value does not match the one in the values vector, the given vector is not a solution.
                if (!comparator.isEqual(groupValue, *valueIt)) {
                    return false;
                }
            }
            
            // Checked all values at this point.
            return true;
        }

        template<typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::sharpen(storm::OptimizationDirection dir, uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x, std::vector<RationalType> const& b, std::vector<RationalType>& tmp) {
            
            for (uint64_t p = 0; p <= precision; ++p) {
                storm::utility::kwek_mehlhorn::sharpen(p, x, tmp);

                if (IterativeMinMaxLinearEquationSolver<RationalType>::isSolution(dir, A, tmp, b)) {
                    return true;
                }
            }
            return false;
        }

        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::createLinearEquationSolver() const {
            this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
        }

        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is imprecise.

            // Create a rational representation of the input so we can check for a proper solution later.
            storm::storage::SparseMatrix<storm::RationalNumber> rationalA = this->A->template toValueType<storm::RationalNumber>();
            std::vector<storm::RationalNumber> rationalX(x.size());
            std::vector<storm::RationalNumber> rationalB = storm::utility::vector::convertNumericVector<storm::RationalNumber>(b);
            
            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<storm::RationalNumber, ImpreciseType>(dir, *this, rationalA, rationalX, rationalB, *this->A, x, b, *auxiliaryRowGroupVector);
            
            // Translate back rational result to imprecise result.
            auto targetIt = x.begin();
            for (auto it = rationalX.begin(), ite = rationalX.end(); it != ite; ++it, ++targetIt) {
                *targetIt = storm::utility::convertNumber<ValueType>(*it);
            }

            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }
        
        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is exact and the same type is to be used for the imprecise part.
            
            if (!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            // Forward the call to the core rational search routine.
            bool converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(dir, *this, *this->A, x, b, *this->A, *auxiliaryRowGroupVector, b, x);

            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }

        template<typename ValueType>
        template<typename ImpreciseType>
        typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Version for when the overall value type is exact and the imprecise one is not. We first try to solve the
            // problem using the imprecise data type and fall back to the exact type as needed.
            
            // Translate A to its imprecise version.
            storm::storage::SparseMatrix<ImpreciseType> impreciseA = this->A->template toValueType<ImpreciseType>();
            
            // Translate x to its imprecise version.
            std::vector<ImpreciseType> impreciseX(x.size());
            {
                std::vector<ValueType> tmp(x.size());
                this->createLowerBoundsVector(tmp);
                auto targetIt = impreciseX.begin();
                for (auto sourceIt = tmp.begin(); targetIt != impreciseX.end(); ++targetIt, ++sourceIt) {
                    *targetIt = storm::utility::convertNumber<ImpreciseType, ValueType>(*sourceIt);
                }
            }
            
            // Create temporary storage for an imprecise x.
            std::vector<ImpreciseType> impreciseTmpX(x.size());
            
            // Translate b to its imprecise version.
            std::vector<ImpreciseType> impreciseB(b.size());
            auto targetIt = impreciseB.begin();
            for (auto sourceIt = b.begin(); targetIt != impreciseB.end(); ++targetIt, ++sourceIt) {
                *targetIt = storm::utility::convertNumber<ImpreciseType, ValueType>(*sourceIt);
            }
            
            // Create imprecise solver from the imprecise data.
            IterativeMinMaxLinearEquationSolver<ImpreciseType> impreciseSolver(std::make_unique<storm::solver::GeneralLinearEquationSolverFactory<ImpreciseType>>());
            impreciseSolver.setMatrix(impreciseA);
            impreciseSolver.createLinearEquationSolver();
            impreciseSolver.setCachingEnabled(true);
            
            bool converged = false;
            try {
                // Forward the call to the core rational search routine.
                converged = solveEquationsRationalSearchHelper<ValueType, ImpreciseType>(dir, impreciseSolver, *this->A, x, b, impreciseA, impreciseX, impreciseB, impreciseTmpX);
            } catch (storm::exceptions::PrecisionExceededException const& e) {
                STORM_LOG_WARN("Precision of value type was exceeded, trying to recover by switching to rational arithmetic.");
                
                if (!auxiliaryRowGroupVector) {
                    auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
                }

                // Translate the imprecise value iteration result to the one we are going to use from now on.
                auto targetIt = auxiliaryRowGroupVector->begin();
                for (auto it = impreciseX.begin(), ite = impreciseX.end(); it != ite; ++it, ++targetIt) {
                    *targetIt = storm::utility::convertNumber<ValueType>(*it);
                }
                
                // Get rid of the superfluous data structures.
                impreciseX = std::vector<ImpreciseType>();
                impreciseTmpX = std::vector<ImpreciseType>();
                impreciseB = std::vector<ImpreciseType>();
                impreciseA = storm::storage::SparseMatrix<ImpreciseType>();

                if (!this->linEqSolverA) {
                    this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                    this->linEqSolverA->setCachingEnabled(true);
                }
                
                // Forward the call to the core rational search routine, but now with our value type as the imprecise value type.
                converged = solveEquationsRationalSearchHelper<ValueType, ValueType>(dir, *this, *this->A, x, b, *this->A, *auxiliaryRowGroupVector, b, x);
            }
            
            if (!this->isCachingEnabled()) {
                this->clearCache();
            }
            
            return converged;
        }

        template<typename RationalType, typename ImpreciseType>
        struct TemporaryHelper {
            static std::vector<RationalType>* getTemporary(std::vector<RationalType>& rationalX, std::vector<ImpreciseType>*& currentX, std::vector<ImpreciseType>*& newX) {
                return &rationalX;
            }
            
            static void swapSolutions(std::vector<RationalType>& rationalX, std::vector<RationalType>*& rationalSolution, std::vector<ImpreciseType>& x, std::vector<ImpreciseType>*& currentX, std::vector<ImpreciseType>*& newX) {
                // Nothing to do.
            }
        };
        
        template<typename RationalType>
        struct TemporaryHelper<RationalType, RationalType> {
            static std::vector<RationalType>* getTemporary(std::vector<RationalType>& rationalX, std::vector<RationalType>*& currentX, std::vector<RationalType>*& newX) {
                return newX;
            }

            static void swapSolutions(std::vector<RationalType>& rationalX, std::vector<RationalType>*& rationalSolution, std::vector<RationalType>& x, std::vector<RationalType>*& currentX, std::vector<RationalType>*& newX) {
                if (&rationalX == rationalSolution) {
                    // In this case, the rational solution is in place.
                    
                    // However, since the rational solution is no alias to current x, the imprecise solution is stored
                    // in current x and and rational x is not an alias to x, we can swap the contents of currentX to x.
                    std::swap(x, *currentX);
                } else {
                    // Still, we may assume that the rational solution is not current x and is therefore new x.
                    std::swap(rationalX, *rationalSolution);
                    std::swap(x, *currentX);
                }
            }
        };
        
        template<typename ValueType>
        template<typename RationalType, typename ImpreciseType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsRationalSearchHelper(OptimizationDirection dir, IterativeMinMaxLinearEquationSolver<ImpreciseType> const& impreciseSolver, storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX, std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A, std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const {
            
            std::vector<ImpreciseType>* currentX = &x;
            std::vector<ImpreciseType>* newX = &tmpX;

            SolverStatus status = SolverStatus::InProgress;
            uint64_t overallIterations = 0;
            uint64_t valueIterationInvocations = 0;
            ValueType precision = this->getSettings().getPrecision();
            impreciseSolver.startMeasureProgress();
            while (status == SolverStatus::InProgress && overallIterations < this->getSettings().getMaximalNumberOfIterations()) {
                // Perform value iteration with the current precision.
                typename IterativeMinMaxLinearEquationSolver<ImpreciseType>::ValueIterationResult result = impreciseSolver.performValueIteration(dir, currentX, newX, b, storm::utility::convertNumber<ImpreciseType, ValueType>(precision), this->getSettings().getRelativeTerminationCriterion(), SolverGuarantee::LessOrEqual, overallIterations);
                
                // At this point, the result of the imprecise value iteration is stored in the (imprecise) current x.
                
                ++valueIterationInvocations;
                STORM_LOG_TRACE("Completed " << valueIterationInvocations << " value iteration invocations, the last one with precision " << precision << " completed in " << result.iterations << " iterations.");
                
                // Count the iterations.
                overallIterations += result.iterations;
                
                // Compute maximal precision until which to sharpen.
                uint64_t p = storm::utility::convertNumber<uint64_t>(storm::utility::ceil(storm::utility::log10<ValueType>(storm::utility::one<ValueType>() / precision)));
                
                // Make sure that currentX and rationalX are not aliased.
                std::vector<RationalType>* temporaryRational = TemporaryHelper<RationalType, ImpreciseType>::getTemporary(rationalX, currentX, newX);
                
                // Sharpen solution and place it in the temporary rational.
                bool foundSolution = sharpen(dir, p, rationalA, *currentX, rationalB, *temporaryRational);
                
                // After sharpen, if a solution was found, it is contained in the free rational.
                
                if (foundSolution) {
                    status = SolverStatus::Converged;
                    
                    TemporaryHelper<RationalType, ImpreciseType>::swapSolutions(rationalX, temporaryRational, x, currentX, newX);
                } else {
                    // Increase the precision.
                    precision /= 10;
                }
            }
            
            if (status == SolverStatus::InProgress && overallIterations == this->getSettings().getMaximalNumberOfIterations()) {
                status = SolverStatus::MaximalIterationsExceeded;
            }
            
            reportStatus(status, overallIterations);
            
            return status == SolverStatus::Converged || status == SolverStatus::TerminatedEarly;
        }

        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsRationalSearch(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            return solveEquationsRationalSearchHelper<double>(dir, x, b);
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::computeOptimalValueForRowGroup(uint_fast64_t group, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, uint_fast64_t* choice) const {
            uint64_t row = this->A->getRowGroupIndices()[group];
            uint64_t groupEnd = this->A->getRowGroupIndices()[group + 1];
            assert(row != groupEnd);
            
            auto bIt = b.begin() + row;
            ValueType& xi = x[group];
            xi = this->A->multiplyRowWithVector(row, x) + *bIt;
            uint64_t optimalRow = row;
            
            for (++row, ++bIt; row < groupEnd; ++row, ++bIt) {
                ValueType choiceVal = this->A->multiplyRowWithVector(row, x) + *bIt;
                if (minimize(dir)) {
                    if (choiceVal < xi) {
                        xi = choiceVal;
                        optimalRow = row;
                    }
                } else {
                    if (choiceVal > xi) {
                        xi = choiceVal;
                        optimalRow = row;
                    }
                }
            }
            if (choice != nullptr) {
                *choice = optimalRow - this->A->getRowGroupIndices()[group];
            }
        }

        template<typename ValueType>
        SolverStatus IterativeMinMaxLinearEquationSolver<ValueType>::updateStatusIfNotConverged(SolverStatus status, std::vector<ValueType> const& x, uint64_t iterations, SolverGuarantee const& guarantee) const {
            if (status != SolverStatus::Converged) {
                if (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x, guarantee)) {
                    status = SolverStatus::TerminatedEarly;
                } else if (iterations >= this->getSettings().getMaximalNumberOfIterations()) {
                    status = SolverStatus::MaximalIterationsExceeded;
                }
            }
            return status;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::reportStatus(SolverStatus status, uint64_t iterations) {
            switch (status) {
                case SolverStatus::Converged: STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations."); break;
                case SolverStatus::TerminatedEarly: STORM_LOG_INFO("Iterative solver terminated early after " << iterations << " iterations."); break;
                case SolverStatus::MaximalIterationsExceeded: STORM_LOG_WARN("Iterative solver did not converge after " << iterations << " iterations."); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Iterative solver terminated unexpectedly.");
            }
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverSettings<ValueType> const& IterativeMinMaxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::setSettings(IterativeMinMaxLinearEquationSolverSettings<ValueType> const& newSettings) {
            settings = newSettings;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::clearCache() const {
            auxiliaryRowGroupVector.reset();
            auxiliaryRowGroupVector2.reset();
            rowGroupOrdering.reset();
            StandardMinMaxLinearEquationSolver<ValueType>::clearCache();
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverFactory<ValueType>::IterativeMinMaxLinearEquationSolverFactory(MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(method, trackScheduler) {
            settings.setSolutionMethod(this->getMinMaxMethod());
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverFactory<ValueType>::IterativeMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(std::move(linearEquationSolverFactory), method, trackScheduler) {
            settings.setSolutionMethod(this->getMinMaxMethod());
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverFactory<ValueType>::IterativeMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, MinMaxMethodSelection const& method, bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(solverType, method, trackScheduler) {
            settings.setSolutionMethod(this->getMinMaxMethod());
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> IterativeMinMaxLinearEquationSolverFactory<ValueType>::create() const {
            STORM_LOG_ASSERT(this->linearEquationSolverFactory, "Linear equation solver factory not initialized.");
            
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(this->linearEquationSolverFactory->clone(), settings);
            result->setTrackScheduler(this->isTrackSchedulerSet());
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverSettings<ValueType>& IterativeMinMaxLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverSettings<ValueType> const& IterativeMinMaxLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
 
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethodSelection const& newMethod) {
            MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(newMethod);
            settings.setSolutionMethod(this->getMinMaxMethod());
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(MinMaxMethod const& newMethod) {
            MinMaxLinearEquationSolverFactory<ValueType>::setMinMaxMethod(newMethod);
            settings.setSolutionMethod(this->getMinMaxMethod());
        }

        template class IterativeMinMaxLinearEquationSolverSettings<double>;
        template class IterativeMinMaxLinearEquationSolver<double>;
        template class IterativeMinMaxLinearEquationSolverFactory<double>;
        
#ifdef STORM_HAVE_CARL
        template class IterativeMinMaxLinearEquationSolverSettings<storm::RationalNumber>;
        template class IterativeMinMaxLinearEquationSolver<storm::RationalNumber>;
        template class IterativeMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
