#include "src/solver/NativeLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/utility/vector.h"
#include "src/exceptions/InvalidStateException.h"
#include "src/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>::NativeLinearEquationSolverSettings() {
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>();
            
            storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod methodAsSetting = settings.getLinearEquationSystemMethod();
            if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::GaussSeidel) {
                method = SolutionMethod::GaussSeidel;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::Jacobi) {
                method = SolutionMethod::Jacobi;
            } else if (methodAsSetting == storm::settings::modules::NativeEquationSolverSettings::LinearEquationMethod::SOR) {
                method = SolutionMethod::SOR;
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "The selected solution technique is invalid for this solver.");
            }
            
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
            omega = storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getOmega();
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setSolutionMethod(SolutionMethod const& method) {
            this->method = method;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setPrecision(ValueType precision) {
            this->precision = precision;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setMaximalNumberOfIterations(uint64_t maximalNumberOfIterations) {
            this->maximalNumberOfIterations = maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setRelativeTerminationCriterion(bool value) {
            this->relative = value;
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolverSettings<ValueType>::setOmega(ValueType omega) {
            this->omega = omega;
        }
        
        template<typename ValueType>
        typename NativeLinearEquationSolverSettings<ValueType>::SolutionMethod NativeLinearEquationSolverSettings<ValueType>::getSolutionMethod() const {
            return method;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getPrecision() const {
            return precision;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getMaximalNumberOfIterations() const {
            return maximalNumberOfIterations;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolverSettings<ValueType>::getRelativeTerminationCriterion() const {
            return relative;
        }
        
        template<typename ValueType>
        ValueType NativeLinearEquationSolverSettings<ValueType>::getOmega() const {
            return omega;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings), auxiliarySolvingMemory(nullptr) {
            this->setMatrix(A);
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(nullptr), settings(settings), auxiliarySolvingMemory(nullptr) {
            this->setMatrix(std::move(A));
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::setMatrix(storm::storage::SparseMatrix<ValueType> const& A) {
            localA.reset();
            this->A = &A;
        }
        

        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            bool allocatedAuxStorage = !this->hasAuxMemory(LinearEquationSolverOperation::SolveEquations);
            if (allocatedAuxStorage) {
                this->allocateAuxMemory(LinearEquationSolverOperation::SolveEquations);
            }
            
            if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR || this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::GaussSeidel) {
                // Define the omega used for SOR.
                ValueType omega = this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR ? this->getSettings().getOmega() : storm::utility::one<ValueType>();
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations()) {
                    A->performSuccessiveOverRelaxationStep(omega, x, b);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(*auxiliarySolvingMemory, x, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion()) || (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x));
                    
                    // If we did not yet converge, we need to backup the contents of x.
                    if (!converged) {
                        *auxiliarySolvingMemory = x;
                    }
                    
                    // Increase iteration count so we can abort if convergence is too slow.
                    ++iterationCount;
                }
                // If we allocated auxiliary memory, we need to dispose of it now.
                if (allocatedAuxStorage) {
                    this->deallocateAuxMemory(LinearEquationSolverOperation::SolveEquations);
                }

                return converged;
                
            } else {
                // Get a Jacobi decomposition of the matrix A.
                std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> jacobiDecomposition = A->getJacobiDecomposition();
                
                std::vector<ValueType>* currentX = &x;
                std::vector<ValueType>* nextX = auxiliarySolvingMemory.get();
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                    // Compute D^-1 * (b - LU * x) and store result in nextX.
                    jacobiDecomposition.first.multiplyWithVector(*currentX, *nextX);
                    storm::utility::vector::subtractVectors(b, *nextX, *nextX);
                    storm::utility::vector::multiplyVectorsPointwise(jacobiDecomposition.second, *nextX, *nextX);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());

                    // Swap the two pointers as a preparation for the next iteration.
                    std::swap(nextX, currentX);
                    
                    // Increase iteration count so we can abort if convergence is too slow.
                    ++iterationCount;
                }
                                
                // If the last iteration did not write to the original x we have to swap the contents, because the
                // output has to be written to the input parameter x.
                if (currentX == auxiliarySolvingMemory.get()) {
                    std::swap(x, *currentX);
                }

                // If we allocated auxiliary memory, we need to dispose of it now.
                if (allocatedAuxStorage) {
                    this->deallocateAuxMemory(LinearEquationSolverOperation::SolveEquations);
                }
                return converged;
            }


        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType> const* b, std::vector<ValueType>& result) const {
            if (&x != &result) {
                A->multiplyWithVector(x, result);
                if (b != nullptr) {
                    storm::utility::vector::addVectors(result, *b, result);
                }
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                std::vector<ValueType> tmp(result.size());
                A->multiplyWithVector(x, tmp);
                if (b != nullptr) {
                    storm::utility::vector::addVectors(tmp, *b, result);
                }
            }
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>& NativeLinearEquationSolver<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolver<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::allocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (!auxiliarySolvingMemory) {
                    auxiliarySolvingMemory = std::make_unique<std::vector<ValueType>>(this->getMatrixRowCount());
                    result = true;
                }
            }
            result |= LinearEquationSolver<ValueType>::allocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::deallocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (auxiliarySolvingMemory) {
                    result = true;
                    auxiliarySolvingMemory.reset();
                }
            }
            result |= LinearEquationSolver<ValueType>::deallocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::reallocateAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                if (auxiliarySolvingMemory) {
                    result = auxiliarySolvingMemory->size() != this->getMatrixColumnCount();
                    auxiliarySolvingMemory->resize(this->getMatrixRowCount());
                }
            }
            result |= LinearEquationSolver<ValueType>::reallocateAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::hasAuxMemory(LinearEquationSolverOperation operation) const {
            bool result = false;
            if (operation == LinearEquationSolverOperation::SolveEquations) {
                result |= static_cast<bool>(auxiliarySolvingMemory);
            }
            result |= LinearEquationSolver<ValueType>::hasAuxMemory(operation);
            return result;
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixRowCount() const {
            return this->A->getRowCount();
        }
        
        template<typename ValueType>
        uint64_t NativeLinearEquationSolver<ValueType>::getMatrixColumnCount() const {
            return this->A->getColumnCount();
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType> const& matrix) const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(matrix, settings);
        }
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> NativeLinearEquationSolverFactory<ValueType>::create(storm::storage::SparseMatrix<ValueType>&& matrix) const {
            return std::make_unique<storm::solver::NativeLinearEquationSolver<ValueType>>(std::move(matrix), settings);
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType>& NativeLinearEquationSolverFactory<ValueType>::getSettings() {
            return settings;
        }
        
        template<typename ValueType>
        NativeLinearEquationSolverSettings<ValueType> const& NativeLinearEquationSolverFactory<ValueType>::getSettings() const {
            return settings;
        }
        
        template<typename ValueType>
        std::unique_ptr<LinearEquationSolverFactory<ValueType>> NativeLinearEquationSolverFactory<ValueType>::clone() const {
            return std::make_unique<NativeLinearEquationSolverFactory<ValueType>>(*this);
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolverSettings<double>;
        template class NativeLinearEquationSolver<double>;
        template class NativeLinearEquationSolverFactory<double>;
    }
}