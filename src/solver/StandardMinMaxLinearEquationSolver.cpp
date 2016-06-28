#include "src/solver/StandardMinMaxLinearEquationSolver.h"

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/MinMaxEquationSolverSettings.h"

#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/solver/EigenLinearEquationSolver.h"
#include "src/solver/NativeLinearEquationSolver.h"
#include "src/solver/EliminationLinearEquationSolver.h"

#include "src/utility/vector.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidSettingsException.h"
#include "src/exceptions/InvalidStateException.h"
namespace storm {
    namespace solver {
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings<ValueType>::StandardMinMaxLinearEquationSolverSettings() {
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
        void StandardMinMaxLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        typename StandardMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod const& StandardMinMaxLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return solutionMethod;
        }
        
        template<typename ValueType>
        uint64_t StandardMinMaxLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        ValueType StandardMinMaxLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        bool StandardMinMaxLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(nullptr), A(A) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings<ValueType> const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(*localA) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquations(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            switch (this->getSettings().getSolutionMethod()) {
                case StandardMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::ValueIteration: solveEquationsValueIteration(dir, x, b, multiplyResult, newX); break;
                case StandardMinMaxLinearEquationSolverSettings<ValueType>::SolutionMethod::PolicyIteration: solveEquationsPolicyIteration(dir, x, b, multiplyResult, newX); break;
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationsPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            
            // Create scratch memory if none was provided.
            bool multiplyResultMemoryProvided = multiplyResult != nullptr;
            if (multiplyResult == nullptr) {
                multiplyResult = new std::vector<ValueType>(this->A.getRowCount());
            }
            
            // Create the initial scheduler.
            std::vector<storm::storage::sparse::state_type> scheduler(this->A.getRowGroupCount());
            
            // Create a vector for storing the right-hand side of the inner equation system.
            std::vector<ValueType> subB(this->A.getRowGroupCount());
            
            // Create a vector that the inner equation solver can use as scratch memory.
            std::vector<ValueType> deterministicMultiplyResult(this->A.getRowGroupCount());
            
            Status status = Status::InProgress;
            uint64_t iterations = 0;
            do {
                // Resolve the nondeterminism according to the current scheduler.
                storm::storage::SparseMatrix<ValueType> submatrix = this->A.selectRowsFromRowGroups(scheduler, true);
                submatrix.convertToEquationSystem();
                storm::utility::vector::selectVectorValues<ValueType>(subB, scheduler, this->A.getRowGroupIndices(), b);
                
                // Solve the equation system for the 'DTMC'.
                // FIXME: we need to remove the 0- and 1- states to make the solution unique.
                // HOWEVER: if we start with a valid scheduler, then we will never get an illegal one, because staying
                // within illegal MECs will never strictly improve the value. Is this true?
                auto solver = linearEquationSolverFactory->create(std::move(submatrix));
                solver->solveEquations(x, subB, &deterministicMultiplyResult);
                
                // Go through the multiplication result and see whether we can improve any of the choices.
                bool schedulerImproved = false;
                for (uint_fast64_t group = 0; group < this->A.getRowGroupCount(); ++group) {
                    for (uint_fast64_t choice = this->A.getRowGroupIndices()[group]; choice < this->A.getRowGroupIndices()[group + 1]; ++choice) {
                        // If the choice is the currently selected one, we can skip it.
                        if (choice - this->A.getRowGroupIndices()[group] == scheduler[group]) {
                            continue;
                        }
                        
                        // Create the value of the choice.
                        ValueType choiceValue = storm::utility::zero<ValueType>();
                        for (auto const& entry : this->A.getRow(choice)) {
                            choiceValue += entry.getValue() * x[entry.getColumn()];
                        }
                        choiceValue += b[choice];
                        
                        // If the value is strictly better than the solution of the inner system, we need to improve the scheduler.
                        if (valueImproved(dir, x[group], choiceValue)) {
                            schedulerImproved = true;
                            scheduler[group] = choice - this->A.getRowGroupIndices()[group];
                        }
                    }
                }
                
                // If the scheduler did not improve, we are done.
                if (!schedulerImproved) {
                    status = Status::Converged;
                }
                
                // Update environment variables.
                ++iterations;
                status = updateStatusIfNotConverged(status, x, iterations);
            } while (status == Status::InProgress);
            
            reportStatus(status, iterations);
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                this->scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(scheduler));
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        template<typename ValueType>
        bool StandardMinMaxLinearEquationSolver<ValueType>::valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const {
            if (dir == OptimizationDirection::Minimize) {
                if (value1 > value2) {
                    return true;
                }
                return false;
            } else {
                if (value1 < value2) {
                    return true;
                }
                return false;
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationsValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(A);
            
            // Create scratch memory if none was provided.
            bool multiplyResultMemoryProvided = multiplyResult != nullptr;
            if (multiplyResult == nullptr) {
                multiplyResult = new std::vector<ValueType>(this->A.getRowCount());
            }
            std::vector<ValueType>* currentX = &x;
            bool xMemoryProvided = newX != nullptr;
            if (newX == nullptr) {
                newX = new std::vector<ValueType>(x.size());
            }
            
            // Keep track of which of the vectors for x is the auxiliary copy.
            std::vector<ValueType>* copyX = newX;
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            uint64_t iterations = 0;
            
            Status status = Status::InProgress;
            while (status == Status::InProgress) {
                // Compute x' = A*x + b.
                solver->multiply(*currentX, *multiplyResult, &b);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices.
                storm::utility::vector::reduceVectorMinOrMax(dir, *multiplyResult, *newX, this->A.getRowGroupIndices());
                
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
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If requested, we store the scheduler for retrieval.
            if (this->isTrackSchedulerSet()) {
                if(iterations==0){ //may happen due to custom termination condition. Then we need to compute x'= A*x+b
                    solver->multiply(x, *multiplyResult, &b);
                }
                std::vector<storm::storage::sparse::state_type> choices(this->A.getRowGroupCount());
                // Reduce the multiplyResult and keep track of the choices made
                storm::utility::vector::reduceVectorMinOrMax(dir, *multiplyResult, x, this->A.getRowGroupIndices(), &choices);
                this->scheduler = std::make_unique<storm::storage::TotalScheduler>(std::move(choices));
            }
            
            // Dispose of allocated scratch memory.
            if (!xMemoryProvided) {
                delete copyX;
            }
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::multiply(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(A);
            
            // If scratch memory was not provided, we create it.
            bool multiplyResultMemoryProvided = multiplyResult != nullptr;
            if (!multiplyResult) {
                multiplyResult = new std::vector<ValueType>(this->A.getRowCount());
            }
            
            for (uint64_t i = 0; i < n; ++i) {
                solver->multiply(x, *multiplyResult, b);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                // element of the min/max operator stack.
                storm::utility::vector::reduceVectorMinOrMax(dir, *multiplyResult, x, this->A.getRowGroupIndices());
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        template<typename ValueType>
        typename StandardMinMaxLinearEquationSolver<ValueType>::Status StandardMinMaxLinearEquationSolver<ValueType>::updateStatusIfNotConverged(Status status, std::vector<ValueType> const& x, uint64_t iterations) const {
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
        void StandardMinMaxLinearEquationSolver<ValueType>::reportStatus(Status status, uint64_t iterations) const {
            switch (status) {
                case Status::Converged: STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations."); break;
                case Status::TerminatedEarly: STORM_LOG_INFO("Iterative solver terminated early after " << iterations << " iterations."); break;
                case Status::MaximalIterationsExceeded: STORM_LOG_WARN("Iterative solver did not converge after " << iterations << " iterations."); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidStateException, "Iterative solver terminated unexpectedly.");
            }
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings<ValueType> const& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings<ValueType>& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler), linearEquationSolverFactory(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler), linearEquationSolverFactory(std::move(linearEquationSolverFactory)) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverFactory<ValueType>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, bool trackScheduler) : MinMaxLinearEquationSolverFactory<ValueType>(trackScheduler) {
            switch (solverType) {
                case EquationSolverType::Gmmxx: linearEquationSolverFactory = std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Native: linearEquationSolverFactory = std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(); break;
                case EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<ValueType>>(); break;
            }
        }
        
        template<>
        StandardMinMaxLinearEquationSolverFactory<storm::RationalNumber>::StandardMinMaxLinearEquationSolverFactory(EquationSolverType const& solverType, bool trackScheduler) : MinMaxLinearEquationSolverFactory<storm::RationalNumber>(trackScheduler) {
            switch (solverType) {
                case  EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                case  EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<storm::RationalNumber>>(); break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Cannot create the requested solver for this data type.");
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            if (linearEquationSolverFactory) {
                return std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(matrix, linearEquationSolverFactory->clone(), settings);
            } else {
                return std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(matrix, std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), settings);
            }
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> StandardMinMaxLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            if (linearEquationSolverFactory) {
                result = std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), linearEquationSolverFactory->clone(), settings);
            } else {
                result = std::make_unique<StandardMinMaxLinearEquationSolver<ValueType>>(std::move(matrix), std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), settings);
            }
            if (this->isTrackSchedulerSet()) {
                result->setTrackScheduler(true);
            }
            return result;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings<ValueType>& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings<ValueType> const& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        GmmxxMinMaxLinearEquationSolverFactory<ValueType>::GmmxxMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Gmmxx, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EigenMinMaxLinearEquationSolverFactory<ValueType>::EigenMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Eigen, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        NativeMinMaxLinearEquationSolverFactory<ValueType>::NativeMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Native, trackScheduler) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        EliminationMinMaxLinearEquationSolverFactory<ValueType>::EliminationMinMaxLinearEquationSolverFactory(bool trackScheduler) : StandardMinMaxLinearEquationSolverFactory<ValueType>(EquationSolverType::Elimination, trackScheduler) {
            // Intentionally left empty.
        }
        
        template class StandardMinMaxLinearEquationSolverSettings<double>;
        template class StandardMinMaxLinearEquationSolver<double>;
        template class StandardMinMaxLinearEquationSolverFactory<double>;
        template class GmmxxMinMaxLinearEquationSolverFactory<double>;
        template class EigenMinMaxLinearEquationSolverFactory<double>;
        template class NativeMinMaxLinearEquationSolverFactory<double>;
        template class EliminationMinMaxLinearEquationSolverFactory<double>;
        
        template class StandardMinMaxLinearEquationSolverSettings<storm::RationalNumber>;
        template class StandardMinMaxLinearEquationSolver<storm::RationalNumber>;
        template class StandardMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class EigenMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class EliminationMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        
    }
}
