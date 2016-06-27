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
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(nullptr), A(A), settings(settings) {
            // Intentionally left empty.
        }

        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, NativeLinearEquationSolverSettings<ValueType> const& settings) : localA(std::make_unique<storm::storage::SparseMatrix<ValueType>>(std::move(A))), A(*localA), settings(settings) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::solveEquations(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            if (this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR || this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::GaussSeidel) {
                // Define the omega used for SOR.
                ValueType omega = this->getSettings().getSolutionMethod() == NativeLinearEquationSolverSettings<ValueType>::SolutionMethod::SOR ? this->getSettings().getOmega() : storm::utility::one<ValueType>();
                
                // To avoid copying the contents of the vector in the loop, we create a temporary x to swap with.
                bool tmpXProvided = true;
                std::vector<ValueType>* tmpX = multiplyResult;
                if (multiplyResult == nullptr) {
                    tmpX = new std::vector<ValueType>(x);
                    tmpXProvided = false;
                } else {
                    *tmpX = x;
                }
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations()) {
                    A.performSuccessiveOverRelaxationStep(omega, x, b);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(x, *tmpX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion()) || (this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(x));
                    
                    // If we did not yet converge, we need to copy the contents of x to *tmpX.
                    if (!converged) {
                        *tmpX = x;
                    }
                    
                    // Increase iteration count so we can abort if convergence is too slow.
                    ++iterationCount;
                }
                
                // If the vector for the temporary multiplication result was not provided, we need to delete it.
                if (!tmpXProvided) {
                    delete tmpX;
                }
            } else {
                // Get a Jacobi decomposition of the matrix A.
                std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
                
                // To avoid copying the contents of the vector in the loop, we create a temporary x to swap with.
                bool multiplyResultProvided = true;
                std::vector<ValueType>* nextX = multiplyResult;
                if (nextX == nullptr) {
                    nextX = new std::vector<ValueType>(x.size());
                    multiplyResultProvided = false;
                }
                std::vector<ValueType> const* copyX = nextX;
                std::vector<ValueType>* currentX = &x;
                
                // Target vector for multiplication.
                std::vector<ValueType> tmpX(x.size());
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < this->getSettings().getMaximalNumberOfIterations() && !(this->hasCustomTerminationCondition() && this->getTerminationCondition().terminateNow(*currentX))) {
                    // Compute D^-1 * (b - LU * x) and store result in nextX.
                    jacobiDecomposition.first.multiplyWithVector(*currentX, tmpX);
                    storm::utility::vector::subtractVectors(b, tmpX, tmpX);
                    storm::utility::vector::multiplyVectorsPointwise(jacobiDecomposition.second, tmpX, *nextX);
                    
                    // Swap the two pointers as a preparation for the next iteration.
                    std::swap(nextX, currentX);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(this->getSettings().getPrecision()), this->getSettings().getRelativeTerminationCriterion());
                    
                    // Increase iteration count so we can abort if convergence is too slow.
                    ++iterationCount;
                }
                                
                // If the last iteration did not write to the original x we have to swap the contents, because the
                // output has to be written to the input parameter x.
                if (currentX == copyX) {
                    std::swap(x, *currentX);
                }
                
                // If the vector for the temporary multiplication result was not provided, we need to delete it.
                if (!multiplyResultProvided) {
                    delete copyX;
                }
            }
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::multiply(std::vector<ValueType>& x, std::vector<ValueType>& result, std::vector<ValueType> const* b) const {
            if (&x != &result) {
                A.multiplyWithVector(x, result);
                if (b != nullptr) {
                    storm::utility::vector::addVectors(result, *b, result);
                }
            } else {
                // If the two vectors are aliases, we need to create a temporary.
                std::vector<ValueType> tmp(result.size());
                A.multiplyWithVector(x, tmp);
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