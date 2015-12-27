#include "src/solver/NativeLinearEquationSolver.h"

#include <utility>

#include "src/settings/SettingsManager.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"

#include "src/utility/vector.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSolutionMethod method, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : A(A), method(method), precision(precision), relative(relative), maximalNumberOfIterations(maximalNumberOfIterations) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        NativeLinearEquationSolver<ValueType>::NativeLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, NativeLinearEquationSolverSolutionMethod method) : A(A), method(method) {
            // Get the settings object to customize linear solving.
            storm::settings::modules::NativeEquationSolverSettings const& settings = storm::settings::nativeEquationSolverSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();
            relative = settings.getConvergenceCriterion() == storm::settings::modules::NativeEquationSolverSettings::ConvergenceCriterion::Relative;
        }
                
        template<typename ValueType>
        bool NativeLinearEquationSolver<ValueType>::solveEquationSystem(std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            if (method == NativeLinearEquationSolverSolutionMethod::SOR || method == NativeLinearEquationSolverSolutionMethod::GaussSeidel) {
                // Define the omega used for SOR.
                ValueType omega = method == NativeLinearEquationSolverSolutionMethod::SOR ? storm::settings::nativeEquationSolverSettings().getOmega() : storm::utility::one<ValueType>();
                
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
                
                while (!converged && iterationCount < maximalNumberOfIterations) {
                    A.performSuccessiveOverRelaxationStep(omega, x, b);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(x, *tmpX, static_cast<ValueType>(precision), relative);
                    
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
                
                return converged;
                
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
                
                // Target vector for precision calculation.
                std::vector<ValueType> tmpX(x.size());
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < maximalNumberOfIterations) {
                    // Compute D^-1 * (b - LU * x) and store result in nextX.
                    jacobiDecomposition.first.multiplyWithVector(*currentX, tmpX);
                    storm::utility::vector::subtractVectors(b, tmpX, tmpX);
                    storm::utility::vector::multiplyVectorsPointwise(jacobiDecomposition.second, tmpX, *nextX);
                    
                    // Swap the two pointers as a preparation for the next iteration.
                    std::swap(nextX, currentX);
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision<ValueType>(*currentX, *nextX, static_cast<ValueType>(precision), relative);
                    
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
                
                return converged;
            }
        }
        
        template<typename ValueType>
        void NativeLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Set up some temporary variables so that we can just swap pointers instead of copying the result after
            // each iteration.
            std::vector<ValueType>* currentX = &x;
            
            bool multiplyResultProvided = true;
            std::vector<ValueType>* nextX = multiplyResult;
            if (nextX == nullptr) {
                nextX = new std::vector<ValueType>(x.size());
                multiplyResultProvided = false;
            }
            std::vector<ValueType> const* copyX = nextX;
            
            // Now perform matrix-vector multiplication as long as we meet the bound.
            for (uint_fast64_t i = 0; i < n; ++i) {
                A.multiplyWithVector(*currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    storm::utility::vector::addVectors(*currentX, *b, *currentX);
                }
            }
            
            // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x,
            // because the output is supposed to be stored in the input vector x.
            if (currentX == copyX) {
                std::swap(x, *currentX);
            }
            
            // If the vector for the temporary multiplication result was not provided, we need to delete it.
            if (!multiplyResultProvided) {
                delete copyX;
            }
        }


        template<typename ValueType>
        std::string NativeLinearEquationSolver<ValueType>::methodToString() const {
            switch (method) {
                case NativeLinearEquationSolverSolutionMethod::Jacobi: return "jacobi";
                case NativeLinearEquationSolverSolutionMethod::GaussSeidel: return "gauss-seidel";
                case NativeLinearEquationSolverSolutionMethod::SOR: return "sor";
                default: return "invalid";
            }
        }
        
        // Explicitly instantiate the linear equation solver.
        template class NativeLinearEquationSolver<double>;
		template class NativeLinearEquationSolver<float>;
    }
}