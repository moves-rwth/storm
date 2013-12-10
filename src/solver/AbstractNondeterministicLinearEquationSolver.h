#ifndef STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "src/storage/SparseMatrix.h"
#include "src/utility/vector.h"
#include "src/settings/Settings.h"

#include <vector>

namespace storm {
    namespace solver {
        
        template<class Type>
        class AbstractNondeterministicLinearEquationSolver {
        public:
            AbstractNondeterministicLinearEquationSolver() {
                storm::settings::Settings* s = storm::settings::Settings::getInstance();
                precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
                maxIterations = s->getOptionByLongName("maxIterations").getArgument(0).getValueAsUnsignedInteger();
                relative = s->getOptionByLongName("relative").getArgument(0).getValueAsBoolean();
            }
            
            AbstractNondeterministicLinearEquationSolver(double precision, uint_fast64_t maxIterations, bool relative) : precision(precision), maxIterations(maxIterations), relative(relative) {
                // Intentionally left empty.
            }
            
            virtual AbstractNondeterministicLinearEquationSolver<Type>* clone() const {
                return new AbstractNondeterministicLinearEquationSolver<Type>(this->precision, this->maxIterations, this->relative);
            }
            
            /*!
             * Performs (repeated) matrix-vector multiplication with the given parameters, i.e. computes x[i+1] = A*x[i] + b
             * until x[n], where x[0] = x.
             *
             * @param minimize If set, all choices are resolved such that the solution value becomes minimal and maximal otherwise.
             * @param A The matrix that is to be multiplied against the vector.
             * @param x The initial vector that is to be multiplied against the matrix. This is also the output parameter,
             * i.e. after the method returns, this vector will contain the computed values.
             * @param nondeterministicChoiceIndices The assignment of states to their rows in the matrix.
             * @param b If not null, this vector is being added to the result after each matrix-vector multiplication.
             * @param n Specifies the number of iterations the matrix-vector multiplication is performed.
             * @returns The result of the repeated matrix-vector multiplication as the content of the parameter vector.
             */
            virtual void performMatrixVectorMultiplication(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* b = nullptr, uint_fast64_t n = 1) const {
                // Create vector for result of multiplication, which is reduced to the result vector after
                // each multiplication.
                std::vector<Type> multiplyResult(A.getRowCount());
                
                // Now perform matrix-vector multiplication as long as we meet the bound of the formula.
                for (uint_fast64_t i = 0; i < n; ++i) {
                    A.multiplyWithVector(x, multiplyResult);
                    
                    // Add b if it is non-null.
                    if (b != nullptr) {
                        storm::utility::vector::addVectorsInPlace(multiplyResult, *b);
                    }
                    
                    // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                    // element of the min/max operator stack.
                    if (minimize) {
                        storm::utility::vector::reduceVectorMin(multiplyResult, x, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(multiplyResult, x, nondeterministicChoiceIndices);
                    }
                }
            }
            
            /*!
             * Solves the equation system A*x = b given by the parameters.
             *
             * @param minimize If set, all choices are resolved such that the solution value becomes minimal and maximal otherwise.
             * @param A The matrix specifying the coefficients of the linear equations.
             * @param x The solution vector x. The initial values of x represent a guess of the real values to the solver, but
             * may be ignored.
             * @param b The right-hand side of the equation system.
             * @param nondeterministicChoiceIndices The assignment of states to their rows in the matrix.
             * @param takenChoices If not null, this vector will be filled with the nondeterministic choices taken by the states
             * to achieve the solution of the equation system. This assumes that the given vector has at least as many elements
             * as there are states in the MDP.
             * @returns The solution vector x of the system of linear equations as the content of the parameter x.
             */
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, std::vector<Type>* multiplyResult = nullptr, std::vector<Type>* newX = nullptr) const {
                
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

                // Proceed with the iterations as long as the method did not converge or reach the
                // user-specified maximum number of iterations.
                std::chrono::nanoseconds multTime(0);
                std::chrono::nanoseconds addTime(0);
                std::chrono::nanoseconds reduceTime(0);
                std::chrono::nanoseconds convergedTime(0);
                auto clock = std::chrono::high_resolution_clock::now();
                while (!converged && iterations < maxIterations) {
                    // Compute x' = A*x + b.
                    clock = std::chrono::high_resolution_clock::now();
                    A.multiplyWithVector(*currentX, *multiplyResult);
                    multTime += std::chrono::high_resolution_clock::now() - clock;
                    clock = std::chrono::high_resolution_clock::now();
                    storm::utility::vector::addVectorsInPlace(*multiplyResult, b);
                    addTime += std::chrono::high_resolution_clock::now() - clock;
                    
                    // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                    // element of the min/max operator stack.
                    clock = std::chrono::high_resolution_clock::now();
                    if (minimize) {
                        storm::utility::vector::reduceVectorMin(*multiplyResult, *newX, nondeterministicChoiceIndices);
                    } else {
                        storm::utility::vector::reduceVectorMax(*multiplyResult, *newX, nondeterministicChoiceIndices);
                    }
                    reduceTime += std::chrono::high_resolution_clock::now() - clock;
                    
                    // Determine whether the method converged.
                    clock = std::chrono::high_resolution_clock::now();
                    converged = storm::utility::vector::equalModuloPrecision(*currentX, *newX, precision, relative);
                    convergedTime += std::chrono::high_resolution_clock::now() - clock;
                    
                    // Update environment variables.
                    swap = currentX;
                    currentX = newX;
                    newX = swap;
                    ++iterations;
                }
                
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(multTime).count() << "ms" << std::endl;
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(addTime).count() << "ms" << std::endl;
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(reduceTime).count() << "ms" << std::endl;
                std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(convergedTime).count() << "ms" << std::endl;

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
            }
            
        protected:
            double precision;
            uint_fast64_t maxIterations;
            bool relative;

        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_ */
