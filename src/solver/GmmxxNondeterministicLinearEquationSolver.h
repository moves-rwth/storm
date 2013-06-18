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
            
            virtual AbstractLinearEquationSolver<Type>* clone() const {
                return new GmmxxNondeterministicLinearEquationSolver<Type>();
            }
            
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const {
                // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                
                // Create vector for result of multiplication, which is reduced to the result vector after
                // each multiplication.
                std::vector<Type> multiplyResult(A.getRowCount());
                
                // Now perform matrix-vector multiplication as long as we meet the bound of the formula.
                for (uint_fast64_t i = 0; i < n; ++i) {
                    gmm::mult(*gmmxxMatrix, x, multiplyResult);
                    
                    if (b != nullptr) {
                        gmm::add(*b, multiplyResult);
                    }
                    
                    if (this->minimumOperatorStack.top()) {
                        storm::utility::vector::reduceVectorMin(multiplyResult, x, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(multiplyResult, x, nondeterministicChoiceIndices);
                    }
                }
                
                // Delete intermediate results and return result.
                delete gmmxxMatrix;
            }
            
            void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<uint_fast64_t>* takenChoices = nullptr) const override {
                // Get the settings object to customize solving.
                storm::settings::Settings* s = storm::settings::instance();
                
                // Get relevant user-defined settings for solving the equations.
                double precision = s->get<double>("precision");
                unsigned maxIterations = s->get<unsigned>("maxiter");
                bool relative = s->get<bool>("relative");
                
                // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                
                // Set up the environment for the power method.
                std::vector<Type> multiplyResult(A.getRowCount());
                std::vector<Type>* currentX = &x;
                std::vector<Type>* newX = new std::vector<Type>(x.size());
                std::vector<Type>* swap = nullptr;
                uint_fast64_t iterations = 0;
                bool converged = false;
                
                // Proceed with the iterations as long as the method did not converge or reach the user-specified maximum number
                // of iterations.
                while (!converged && iterations < maxIterations) {
                    // Compute x' = A*x + b.
                    gmm::mult(*gmmxxMatrix, *currentX, multiplyResult);
                    gmm::add(b, multiplyResult);
                    
                    // Reduce the vector x' by applying min/max for all non-deterministic choices.
                    if (minimize) {
                        storm::utility::vector::reduceVectorMin(multiplyResult, *newX, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(multiplyResult, *newX, nondeterministicChoiceIndices);
                    }
                    
                    // Determine whether the method converged.
                    converged = storm::utility::vector::equalModuloPrecision(*currentX, *newX, precision, relative);
                    
                    // Update environment variables.
                    swap = currentX;
                    currentX = newX;
                    newX = swap;
                    ++iterations;
                }
                
                // If we were requested to record the taken choices, we have to construct the vector now.
                if (takenChoices != nullptr) {
                    this->computeTakenChoices(minimize, multiplyResult, *takenChoices, nondeterministicChoiceIndices);
                }
                
                // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
                // is currently stored in currentX, but x is the output vector.
                if (iterations % 2 == 1) {
                    std::swap(x, *currentX);
                    delete currentX;
                } else {
                    delete newX;
                }
                delete gmmxxMatrix;
                
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