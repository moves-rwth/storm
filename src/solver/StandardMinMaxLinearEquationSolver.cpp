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

namespace storm {
    namespace solver {
        
        StandardMinMaxLinearEquationSolverSettings::StandardMinMaxLinearEquationSolverSettings() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::MinMaxEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::MinMaxEquationSolverSettings>();

            maximalNumberOfIterations = settings.getMaximalIterationCount();
            
            auto method = settings.getMinMaxEquationSolvingMethod();
            switch (method) {
                case MinMaxMethod::ValueIteration: this->solutionMethod = SolutionMethod::ValueIteration; break;
                case MinMaxMethod::PolicyIteration: this->solutionMethod = SolutionMethod::PolicyIteration; break;
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
        }
        
        void StandardMinMaxLinearEquationSolverSettings::setSolutionMethod(SolutionMethod const& solutionMethod) {
            this->solutionMethod = solutionMethod;
        }
        
        void StandardMinMaxLinearEquationSolverSettings::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        void StandardMinMaxLinearEquationSolverSettings::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        void StandardMinMaxLinearEquationSolverSettings::setPrecision(double precision) {
            this->precision = precision;
        }
        
        StandardMinMaxLinearEquationSolverSettings::SolutionMethod const& StandardMinMaxLinearEquationSolverSettings::getSolutionMethod() const {
            return solutionMethod;
        }
        
        uint64_t StandardMinMaxLinearEquationSolverSettings::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        double StandardMinMaxLinearEquationSolverSettings::getPrecision() const {
            return precision;
        }
        
        bool StandardMinMaxLinearEquationSolverSettings::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(nullptr), A(A) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        StandardMinMaxLinearEquationSolver<ValueType>::StandardMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory, StandardMinMaxLinearEquationSolverSettings const& settings) : settings(settings), linearEquationSolverFactory(std::move(linearEquationSolverFactory)), localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(*localA) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationSystem(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            switch (this->getSettings().getSolutionMethod()) {
                case StandardMinMaxLinearEquationSolverSettings::SolutionMethod::ValueIteration: solveEquationSystemValueIteration(dir, x, b, multiplyResult, newX); break;
                case StandardMinMaxLinearEquationSolverSettings::SolutionMethod::PolicyIteration: solveEquationSystemPolicyIteration(dir, x, b, multiplyResult, newX); break;
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationSystemPolicyIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
            // FIXME.
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::solveEquationSystemValueIteration(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult, std::vector<ValueType>* newX) const {
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
            
            uint64_t iterations = 0;
            bool converged = false;
            
            // Proceed with the iterations as long as the method did not converge or reach the maximum number of iterations.
            while (!converged && iterations < this->getSettings().getMaximalNumberOfIterations() && (!this->hasCustomTerminationCondition() || this->getTerminationCondition().terminateNow(*currentX))) {
                // Compute x' = A*x + b.
                this->A.multiplyWithVector(*currentX, *multiplyResult);
                storm::utility::vector::addVectors(*multiplyResult, b, *multiplyResult);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                // element of the min/max operator stack.
                storm::utility::vector::reduceVectorMinOrMax(dir, *multiplyResult, *newX, this->A.getRowGroupIndices());
                
                // Determine whether the method converged.
                converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *newX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                
                // Update environment variables.
                std::swap(currentX, newX);
                ++iterations;
            }
            
            // Check if the solver converged and issue a warning otherwise.
            if (converged) {
                STORM_LOG_INFO("Iterative solver converged after " << iterations << " iterations.");
            } else {
                STORM_LOG_WARN("Iterative solver did not converge after " << iterations << " iterations.");
            }
            
            // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
            // is currently stored in currentX, but x is the output vector.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            if (!xMemoryProvided) {
                delete copyX;
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        template<typename ValueType>
        void StandardMinMaxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver = linearEquationSolverFactory->create(A);
            
            // If scratch memory was not provided, we create it.
            bool multiplyResultMemoryProvided = multiplyResult != nullptr;
            if (!multiplyResult) {
                multiplyResult = new std::vector<ValueType>(this->A.getRowCount());
            }
            
            for (uint64_t i = 0; i < n; ++i) {
                solver->performMatrixVectorMultiplication(x, *multiplyResult, b);
                
                // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                // element of the min/max operator stack.
                storm::utility::vector::reduceVectorMinOrMax(dir, *multiplyResult, x, this->A.getRowGroupIndices());
            }
            
            if (!multiplyResultMemoryProvided) {
                delete multiplyResult;
            }
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings const& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }

        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings& StandardMinMaxLinearEquationSolver<ValueType>::getSettings() {
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
                case  EquationSolverType::Gmmxx: linearEquationSolverFactory = std::make_unique<GmmxxLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Eigen: linearEquationSolverFactory = std::make_unique<EigenLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Native: linearEquationSolverFactory = std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(); break;
                case  EquationSolverType::Elimination: linearEquationSolverFactory = std::make_unique<EliminationLinearEquationSolverFactory<ValueType>>(); break;
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
        StandardMinMaxLinearEquationSolverSettings& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        StandardMinMaxLinearEquationSolverSettings const& StandardMinMaxLinearEquationSolverFactory<ValueType>::getSettings() const {
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
        
        template class StandardMinMaxLinearEquationSolver<double>;
        
        template class StandardMinMaxLinearEquationSolverFactory<double>;
        template class GmmxxMinMaxLinearEquationSolverFactory<double>;
        template class EigenMinMaxLinearEquationSolverFactory<double>;
        template class NativeMinMaxLinearEquationSolverFactory<double>;
        template class EliminationMinMaxLinearEquationSolverFactory<double>;

    }
}