#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MinMaxEquationSolverSettings.h"

#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidStateException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        IterativeMinMaxLinearEquationSolverSettings<ValueType>::IterativeMinMaxLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::MinMaxEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();
            
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = storm::utility::convertNumber<ValueType>(settings.getPrecision());
            relative = settings.getConvergenceCriterion() == storm::settings::modules::MinMaxEquationSolverSettings::ConvergenceCriterion::Relative;
            
            setSolutionMethod(settings.getMinMaxEquationSolvingMethod());
            
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
                case MinMaxMethod::Acyclic: this->solutionMethod = SolutionMethod::Acyclic; break;
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
            switch (this->getSettings().getSolutionMethod()) {
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::ValueIteration:
                    return solveEquationsValueIteration(dir, x, b);
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration:
                    return solveEquationsPolicyIteration(dir, x, b);
                case IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::Acyclic:
                    return solveEquationsAcyclic(dir, x, b);
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "This solver does not implement the selected solution method");
            }
            return false;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            // Create the initial scheduler.
            std::vector<storm::storage::sparse::state_type> scheduler = this->hasSchedulerHint() ? this->choicesHint.get() : std::vector<storm::storage::sparse::state_type>(this->A->getRowGroupCount());
            
            // Get a vector for storing the right-hand side of the inner equation system.
            if(!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            std::vector<ValueType>& subB = *auxiliaryRowGroupVector;

            // Resolve the nondeterminism according to the current scheduler.
            storm::storage::SparseMatrix<ValueType> submatrix = this->A->selectRowsFromRowGroups(scheduler, true);
            submatrix.convertToEquationSystem();
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
            
            Status status = Status::InProgress;
            uint64_t iterations = 0;
            do {
                // Solve the equation system for the 'DTMC'.
                // FIXME: we need to remove the 0- and 1- states to make the solution unique.
                // HOWEVER: if we start with a valid scheduler, then we will never get an illegal one, because staying
                // within illegal MECs will never strictly improve the value. Is this true?
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
                    status = Status::Converged;
                } else {
                    // Update the scheduler and the solver.
                    submatrix = this->A->selectRowsFromRowGroups(scheduler, true);
                    submatrix.convertToEquationSystem();
                    storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A->getRowGroupIndices(), b);
                    solver->setMatrix(std::move(submatrix));
                }
                
                // Update environment variables.
                ++iterations;
                status = updateStatusIfNotConverged(status, x, iterations);
            } while (status == Status::InProgress);
            
            reportStatus(status, iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->schedulerChoices = std::move(scheduler);
            }
            
            if(!this->isCachingEnabled()) {
                clearCache();
            }
            
            return status == Status::Converged || status == Status::TerminatedEarly;
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
        std::vector<MinMaxLinearEquationSolverRequirement> IterativeMinMaxLinearEquationSolver<ValueType>::getRequirements(MinMaxLinearEquationSolverSystemType const& equationSystemType, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            std::vector<MinMaxLinearEquationSolverRequirement> requirements;
            if (equationSystemType == MinMaxLinearEquationSolverSystemType::UntilProbabilities) {
                if (this->getSettings().getSolutionMethod() == IterativeMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration) {
                    if (!direction || direction.get() == OptimizationDirection::Maximize) {
                        requirements.push_back(MinMaxLinearEquationSolverRequirement::ValidSchedulerHint);
                    }
                }
            }
            return requirements;
        }

        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            if(!this->linEqSolverA) {
                this->linEqSolverA = this->linearEquationSolverFactory->create(*this->A);
                this->linEqSolverA->setCachingEnabled(true);
            }
            
            if (!this->auxiliaryRowVector) {
                this->auxiliaryRowVector = std::make_unique<std::vector<ValueType>>(this->A->getRowCount());
            }
            std::vector<ValueType>& multiplyResult = *this->auxiliaryRowVector;
            
            if (!auxiliaryRowGroupVector) {
                auxiliaryRowGroupVector = std::make_unique<std::vector<ValueType>>(this->A->getRowGroupCount());
            }
            
            if (this->hasSchedulerHint()) {
                // Resolve the nondeterminism according to the scheduler hint
                storm::storage::SparseMatrix<ValueType> submatrix = this->A->selectRowsFromRowGroups(this->choicesHint.get(), true);
                submatrix.convertToEquationSystem();
                storm::utility::vector::selectVectorValues<ValueType>(*auxiliaryRowGroupVector, this->choicesHint.get(), this->A->getRowGroupIndices(), b);

                // Solve the resulting equation system.
                // Note that the linEqSolver might consider a slightly different interpretation of "equalModuloPrecision". Hence, we iteratively increase its precision.
                auto submatrixSolver = this->linearEquationSolverFactory->create(std::move(submatrix));
                submatrixSolver->setCachingEnabled(true);
                if (this->lowerBound) { submatrixSolver->setLowerBound(this->lowerBound.get()); }
                if (this->upperBound) { submatrixSolver->setUpperBound(this->upperBound.get()); }
                submatrixSolver->solveEquations(x, *auxiliaryRowGroupVector);
            }
            
            std::vector<ValueType>* newX = auxiliaryRowGroupVector.get();
            
            std::vector<ValueType>* currentX = &x;
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = 0;
            
            Status status = Status::InProgress;
            while (status == Status::InProgress) {
                // Compute x' = A*x + b.
                this->linEqSolverA->multiply(*currentX, &b, multiplyResult);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices.
                storm::utility::vector::reduceVectorMinOrMax(dir, multiplyResult, *newX, this->A->getRowGroupIndices());
                
                // Determine whether the method converged.
                if (storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, this->getSettings().getPrecision(), this->getSettings().getRelativeTerminationCriterion())) {
                    status = Status::Converged;
                }
                
                // Update environment variables.
                std::swap(currentX, newX);
                ++iterations;
                status = updateStatusIfNotConverged(status, *currentX, iterations);
            }
            
            reportStatus(status, iterations);
            
            // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
            // is currently stored in currentX, but x is the output vector.
            if (currentX == auxiliaryRowGroupVector.get()) {
                std::swap(x, *currentX);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                // Due to a custom termination condition, it may be the case that no iterations are performed. In this
                // case we need to compute x'= A*x+b once.
                if (iterations==0) {
                    this->linEqSolverA->multiply(x, &b, multiplyResult);
                }
                this->schedulerChoices = std::vector<uint_fast64_t>(this->A->getRowGroupCount());
                // Reduce the multiplyResult and keep track of the choices made
                storm::utility::vector::reduceVectorMinOrMax(dir, multiplyResult, x, this->A->getRowGroupIndices(), &this->schedulerChoices.get());
            }

            if (!this->isCachingEnabled()) {
                clearCache();
            }
            
            return status == Status::Converged || status == Status::TerminatedEarly;
        }
        
        template<typename ValueType>
        bool IterativeMinMaxLinearEquationSolver<ValueType>::solveEquationsAcyclic(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            uint64_t numGroups = this->A->getRowGroupCount();

            // Allocate memory for the scheduler (if required)
            if (this->isTrackSchedulerSet()) {
                if (this->schedulerChoices) {
                    this->schedulerChoices->resize(numGroups);
                } else {
                    this->schedulerChoices = std::vector<uint_fast64_t>(numGroups);
                }
            }
            
            // We now compute a topological sort of the row groups.
            // If caching is enabled, it might be possible to obtain this sort from the cache.
            if (this->isCachingEnabled()) {
                if (rowGroupOrdering) {
                    for (auto const& group : *rowGroupOrdering) {
                        computeOptimalValueForRowGroup(group, dir, x, b, this->isTrackSchedulerSet() ? &this->schedulerChoices.get()[group] : nullptr);
                    }
                    return true;
                } else {
                    rowGroupOrdering = std::make_unique<std::vector<uint64_t>>();
                    rowGroupOrdering->reserve(numGroups);
                }
            }
            
            auto transposedMatrix = this->A->transpose(true);
            
            // We store the groups that have already been processed, i.e., the groups for which x[group] was already set to the correct value.
            storm::storage::BitVector processedGroups(numGroups, false);
            // Furthermore, we keep track of all candidate groups for which we still need to check whether this group can be processed now.
            // A group can be processed if all successors have already been processed.
            // Notice that the BitVector candidates is considered in a reversed way, i.e., group i is a candidate iff candidates.get(numGroups - i - 1) is true.
            // This is due to the observation that groups with higher indices usually need to be processed earlier.
            storm::storage::BitVector candidates(numGroups, true);
            uint64_t candidate = numGroups - 1;
            for (uint64_t numCandidates = candidates.size(); numCandidates > 0; --numCandidates) {
                candidates.set(numGroups - candidate - 1, false);
                
                // Check if the candidate row group has an unprocessed successor
                bool hasUnprocessedSuccessor = false;
                for (auto const& entry : this->A->getRowGroup(candidate)) {
                    if (!processedGroups.get(entry.getColumn())) {
                        hasUnprocessedSuccessor = true;
                        break;
                    }
                }
                
                uint64_t nextCandidate = numGroups - candidates.getNextSetIndex(numGroups - candidate - 1 + 1) - 1;
                
                if (!hasUnprocessedSuccessor) {
                    // This candidate can be processed.
                    processedGroups.set(candidate);
                    computeOptimalValueForRowGroup(candidate, dir, x, b, this->isTrackSchedulerSet() ? &this->schedulerChoices.get()[candidate] : nullptr);
                    if (this->isCachingEnabled()) {
                        rowGroupOrdering->push_back(candidate);
                    }
                    
                    // Add new candidates
                    for (auto const& predecessorEntry : transposedMatrix.getRow(candidate)) {
                        uint64_t predecessor = predecessorEntry.getColumn();
                        if (!candidates.get(numGroups - predecessor - 1)) {
                            candidates.set(numGroups - predecessor - 1, true);
                            nextCandidate = std::max(nextCandidate, predecessor);
                            ++numCandidates;
                        }
                    }
                }
                candidate = nextCandidate;
            }
            
            assert(candidates.empty());
            STORM_LOG_THROW(processedGroups.full(), storm::exceptions::InvalidOperationException, "The MinMax equation system is not acyclic.");
            return true;
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
        typename IterativeMinMaxLinearEquationSolver<ValueType>::Status IterativeMinMaxLinearEquationSolver<ValueType>::updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const {
            if (status != Status::Converged) {
                if (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x)) {
                    status = Status::TerminatedEarly;
                } else if (iterations >= this->getSettings().getMaximalNumberOfIterations()) {
                    status = Status::MaximalIterationsExceeded;
                }
            }
            return status;
        }
        
        template<typename ValueType>
        void IterativeMinMaxLinearEquationSolver<ValueType>::reportStatus(Status status, uint64_t iterations) const {
            switch (status) {
                case Status::Converged: STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations."); break;
                case Status::TerminatedEarly: STORM_LOG_INFO("Iterative solver terminated early after " << iterations << " iterations."); break;
                case Status::MaximalIterationsExceeded: STORM_LOG_WARN("Iterative solver did not converge after " << iterations << " iterations."); break;
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
