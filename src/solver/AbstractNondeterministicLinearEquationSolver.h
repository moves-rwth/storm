#ifndef STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_

#include "src/storage/SparseMatrix.h"

#include <vector>

namespace storm {
    namespace solver {
        
        template<class Type>
        class AbstractNondeterministicLinearEquationSolver {
        public:
            
            virtual AbstractNondeterministicLinearEquationSolver<Type>* clone() const {
                return new AbstractNondeterministicLinearEquationSolver<Type>();
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
            virtual void solveEquationSystem(bool minimize, storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices) const {
                LOG4CPLUS_INFO(logger, "Starting iterative solver.");
                
                // Get the settings object to customize solving.
                storm::settings::Settings* s = storm::settings::instance();
                
                // Get relevant user-defined settings for solving the equations.
                double precision = s->get<double>("precision");
                unsigned maxIterations = s->get<unsigned>("maxiter");
                bool relative = s->get<bool>("relative");
                
                // Set up the environment for the power method.
                std::vector<Type> multiplyResult(A.getRowCount());
                std::vector<Type>* currentX = &x;
                std::vector<Type>* newX = new std::vector<Type>(x.size());
                std::vector<Type>* swap = nullptr;
                uint_fast64_t iterations = 0;
                bool converged = false;
                
                // Proceed with the iterations as long as the method did not converge or reach the
                // user-specified maximum number of iterations.
                while (!converged && iterations < maxIterations) {
                    // Compute x' = A*x + b.
                    A.multiplyWithVector(*currentX, multiplyResult);
                    storm::utility::vector::addVectorsInPlace(multiplyResult, b);
                    
                    // Reduce the vector x' by applying min/max for all non-deterministic choices as given by the topmost
                    // element of the min/max operator stack.
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
                
                // If we performed an odd number of iterations, we need to swap the x and currentX, because the newest result
                // is currently stored in currentX, but x is the output vector.
                if (iterations % 2 == 1) {
                    std::swap(x, *currentX);
                    delete currentX;
                } else {
                    delete newX;
                }
                
                // Check if the solver converged and issue a warning otherwise.
                if (converged) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                }
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
             * Returns the name of this solver.
             *
             * @returns The name of this solver.
             */
            static std::string getName() {
                return "native";
            }
            
            /*!
             * Registers all options associated with the gmm++ matrix library.
             */
            static void putOptions(boost::program_options::options_description* desc) {
                desc->add_options()("lemethod", boost::program_options::value<std::string>()->default_value("bicgstab")->notifier(&validateLeMethod), "Sets the method used for linear equation solving. Must be in {bicgstab, qmr, jacobi}.");
                desc->add_options()("maxiter", boost::program_options::value<unsigned>()->default_value(10000), "Sets the maximal number of iterations for iterative equation solving.");
                desc->add_options()("precision", boost::program_options::value<double>()->default_value(1e-6), "Sets the precision for iterative equation solving.");
                desc->add_options()("precond", boost::program_options::value<std::string>()->default_value("ilu")->notifier(&validatePreconditioner), "Sets the preconditioning technique for linear equation solving. Must be in {ilu, diagonal, ildlt, none}.");
                desc->add_options()("relative", boost::program_options::value<bool>()->default_value(true), "Sets whether the relative or absolute error is considered for detecting convergence.");
            }
            
            /*!
             * Validates whether the given lemethod matches one of the available ones.
             * Throws an exception of type InvalidSettings in case the selected method is illegal.
             */
            static void validateLeMethod(const std::string& lemethod) {
                if ((lemethod != "bicgstab") && (lemethod != "qmr") && (lemethod != "jacobi")) {
                    throw exceptions::InvalidSettingsException() << "Argument " << lemethod << " for option 'lemethod' is invalid.";
                }
            }
            
            /*!
             * Validates whether the given preconditioner matches one of the available ones.
             * Throws an exception of type InvalidSettings in case the selected preconditioner is illegal.
             */
            static void validatePreconditioner(const std::string& preconditioner) {
                if ((preconditioner != "ilu") && (preconditioner != "diagonal") && (preconditioner != "ildlt") && (preconditioner != "none")) {
                    throw exceptions::InvalidSettingsException() << "Argument " << preconditioner << " for option 'precond' is invalid.";
                }
            }
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_ABSTRACTNONDETERMINISTICLINEAREQUATIONSOLVER_H_ */
