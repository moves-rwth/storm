#ifndef STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "AbstractLinearEquationSolver.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/storage/SparseMatrix.h"

#include "gmm/gmm_matrix.h"

#include <vector>

namespace storm {
    namespace solver {
        
        template<class Type>
        class GmmxxNondeterministicLinearEquationSolver : public AbstractNondeterministicLinearEquationSolver<Type> {
        public:
            
            virtual AbstractNondeterministicLinearEquationSolver<Type>* clone() const {
                return new GmmxxNondeterministicLinearEquationSolver<Type>();
            }
            
            virtual void performMatrixVectorMultiplication(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const override {
                // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                std::unique_ptr<gmm::csr_matrix<Type>> gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                
                // Create vector for result of multiplication, which is reduced to the result vector after
                // each multiplication.
                std::vector<Type> multiplyResult(A.getRowCount());
                
                // Now perform matrix-vector multiplication as long as we meet the bound of the formula.
                for (uint_fast64_t i = 0; i < n; ++i) {
                    gmm::mult(*gmmxxMatrix, x, multiplyResult);
                    
                    if (b != nullptr) {
                        gmm::add(*b, multiplyResult);
                    }
                    
                    if (minimize) {
                        storm::utility::vector::reduceVectorMin(multiplyResult, x, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(multiplyResult, x, nondeterministicChoiceIndices);
                    }
                }
            }
            
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* multiplyResult = nullptr, std::vector<Type>* newX = nullptr) const override {
                // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                std::unique_ptr<gmm::csr_matrix<Type>> gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                
                // Set up the environment for the power method.
                bool multiplyResultMemoryProvided = true;
                if (multiplyResult == nullptr) {
                    multiplyResult = new std::vector<Type>(A.getRowCount());
                    multiplyResultMemoryProvided = false;
                }
        
                std::vector<Type>* currentX = &x;
                bool xMemoryProvided = true;
                if (newX == nullptr) {
                    newX = new std::vector<Type>(x.size());
                    xMemoryProvided = false;
                }
                std::vector<Type>* swap = nullptr;
                uint_fast64_t iterations = 0;
                bool converged = false;
                
                // Proceed with the iterations as long as the method did not converge or reach the user-specified maximum number
                // of iterations.
                while (!converged && iterations < this->maxIterations) {
                    // Compute x' = A*x + b.
                    gmm::mult(*gmmxxMatrix, *currentX, *multiplyResult);
                    gmm::add(b, *multiplyResult);
                    
                    // Reduce the vector x' by applying min/max for all non-deterministic choices.
                    if (minimize) {
                        storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, nondeterministicChoiceIndices);
                    }
                    
                    // Determine whether the method converged.
                    converged = storm::utility::vector::equalModuloPrecision(*currentX, *newX, this->precision, this->relative);
                    
                    // Update environment variables.
                    swap = currentX;
                    currentX = newX;
                    newX = swap;
                    ++iterations;
                }
                
                // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
                // is currently stored in currentX, but x is the output vector.
                if (iterations % 2 == 1) {
                    std::swap(x, *currentX);
                    if (!xMemoryProvided) {
                        delete currentX;
                    }
                } else if (!xMemoryProvided) {
                    delete newX;
                }
        
                if (!multiplyResultMemoryProvided) {
                    delete multiplyResult;
                }
        
                // Check if the solver converged and issue a warning otherwise.
                if (converged) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge after " << iterations << " iterations.");
                }
            }
            
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXNONDETERMINISTICLINEAREQUATIONSOLVER_H_ */