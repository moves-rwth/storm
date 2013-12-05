#ifndef STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_

#include "AbstractLinearEquationSolver.h"
#include "src/adapters/GmmxxAdapter.h"
#include "src/utility/constants.h"
#include "src/settings/Settings.h"
#include "src/utility/vector.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

#include <cmath>

namespace storm {
    namespace solver {

        template<class Type>
        class GmmxxLinearEquationSolver : public AbstractLinearEquationSolver<Type> {
        public:
            
            virtual AbstractLinearEquationSolver<Type>* clone() const {
                return new GmmxxLinearEquationSolver<Type>();
            }
            
            virtual void solveEquationSystem(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
                // Get the settings object to customize linear solving.
                storm::settings::Settings* s = storm::settings::Settings::getInstance();
                
                // Prepare an iteration object that determines the accuracy, maximum number of iterations
                // and the like.
				uint_fast64_t maxIterations = s->getOptionByLongName("maxIterations").getArgument(0).getValueAsUnsignedInteger();
                gmm::iteration iter(s->getOptionByLongName("precision").getArgument(0).getValueAsDouble(), 0, maxIterations);
                
                // Print some information about the used preconditioner.
                std::string const precond = s->getOptionByLongName("preconditioner").getArgument(0).getValueAsString();
                LOG4CPLUS_INFO(logger, "Starting iterative solver.");

				// ALL available solvers must be declared in the cpp File, where the options are registered!
				// Dito for the Preconditioners

				std::string const chosenLeMethod = s->getOptionByLongName("leMethod").getArgument(0).getValueAsString();
                if (chosenLeMethod == "jacobi") {
                    if (precond != "none") {
                        LOG4CPLUS_WARN(logger, "Requested preconditioner '" << precond << "', which is unavailable for the Jacobi method. Dropping preconditioner.");
                    }
                } else {
                    if (precond == "ilu") {
                        LOG4CPLUS_INFO(logger, "Using ILU preconditioner.");
                    } else if (precond == "diagonal") {
                        LOG4CPLUS_INFO(logger, "Using diagonal preconditioner.");
                    } else if (precond == "ildlt") {
                        LOG4CPLUS_INFO(logger, "Using ILDLT preconditioner.");
                    } else if (precond == "none") {
                        LOG4CPLUS_INFO(logger, "Using no preconditioner.");
                    }
                }
                
                // Now do the actual solving.
                if (chosenLeMethod == "bicgstab") {
                    LOG4CPLUS_INFO(logger, "Using BiCGStab method.");
                    // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                    gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                    if (precond == "ilu") {
                        gmm::bicgstab(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "diagonal") {
                        gmm::bicgstab(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "ildlt") {
                        gmm::bicgstab(*gmmA, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "none") {
                        gmm::bicgstab(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                    
                    // Check if the solver converged and issue a warning otherwise.
                    if (iter.converged()) {
                        LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    } else {
                        LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                    }
                    delete gmmA;
                } else if (chosenLeMethod == "qmr") {
                    LOG4CPLUS_INFO(logger, "Using QMR method.");
                    // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                    gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                    if (precond == "ilu") {
                        gmm::qmr(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "diagonal") {
                        gmm::qmr(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "ildlt") {
                        gmm::qmr(*gmmA, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmA), iter);
                    } else if (precond == "none") {
                        gmm::qmr(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                    
                    // Check if the solver converged and issue a warning otherwise.
                    if (iter.converged()) {
                        LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    } else {
                        LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                    }
                    delete gmmA;
                } else if (chosenLeMethod == "lscg") {
                    LOG4CPLUS_INFO(logger, "Using LSCG method.");
                    // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                    gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                    
                    if (precond != "none") {
                        LOG4CPLUS_WARN(logger, "Requested preconditioner '" << precond << "', which is unavailable for the LSCG method. Dropping preconditioner.");
                    }
                    gmm::least_squares_cg(*gmmA, x, b, iter);
                    
                    // Check if the solver converged and issue a warning otherwise.
                    if (iter.converged()) {
                        LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    } else {
                        LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                    }
                    delete gmmA;
                } else if (chosenLeMethod == "gmres") {
                    LOG4CPLUS_INFO(logger, "Using GMRES method.");
                    // Transform the transition probability matrix to the gmm++ format to use its arithmetic.
                    gmm::csr_matrix<Type>* gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                    if (precond == "ilu") {
                        gmm::gmres(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<Type>>(*gmmA), 50, iter);
                    } else if (precond == "diagonal") {
                        gmm::gmres(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<Type>>(*gmmA), 50, iter);
                    } else if (precond == "ildlt") {
                        gmm::gmres(*gmmA, x, b, gmm::ildlt_precond<gmm::csr_matrix<Type>>(*gmmA), 50, iter);
                    } else if (precond == "none") {
                        gmm::gmres(*gmmA, x, b, gmm::identity_matrix(), 50, iter);
                    }
                    
                    // Check if the solver converged and issue a warning otherwise.
                    if (iter.converged()) {
                        LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                    } else {
                        LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                    }
                    delete gmmA;
                } else if (chosenLeMethod == "jacobi") {
                    LOG4CPLUS_INFO(logger, "Using Jacobi method.");
                    uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(A, x, b);
                    uint_fast64_t maxIterations = s->getOptionByLongName("maxIterations").getArgument(0).getValueAsUnsignedInteger();
                    // Check if the solver converged and issue a warning otherwise.
					if (iterations < maxIterations) {
                        LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
                    } else {
                        LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                    }
                }
            }
            
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type>* b, uint_fast64_t n = 1) const {
                // Transform the transition probability A to the gmm++ format to use its arithmetic.
                gmm::csr_matrix<Type>* gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(A);
                
                // Set up some temporary variables so that we can just swap pointers instead of copying the result after each
                // iteration.
                std::vector<Type>* swap = nullptr;
                std::vector<Type>* currentVector = &x;
                std::vector<Type>* tmpVector = new std::vector<Type>(A.getRowCount());
                
                // Now perform matrix-vector multiplication as long as we meet the bound.
                for (uint_fast64_t i = 0; i < n; ++i) {
                    gmm::mult(*gmmxxMatrix, *currentVector, *tmpVector);
                    swap = tmpVector;
                    tmpVector = currentVector;
                    currentVector = swap;
                    
                    // If requested, add an offset to the current result vector.
                    if (b != nullptr) {
                        gmm::add(*b, *currentVector);
                    }
                }
                
                // If we performed an odd number of repetitions, we need to swap the contents of currentVector and x, because
                // the output is supposed to be stored in x.
                if (n % 2 == 1) {
                    std::swap(x, *currentVector);
                    delete currentVector;
                } else {
                    delete tmpVector;
                }
                
                delete gmmxxMatrix;
            }
            
        private:
            /*!
             * Solves the linear equation system A*x = b given by the parameters using the Jacobi method.
             *
             * @param A The matrix specifying the coefficients of the linear equations.
             * @param x The solution vector x. The initial values of x represent a guess of the real values to the solver, but
             * may be ignored.
             * @param b The right-hand side of the equation system.
             * @returns The solution vector x of the system of linear equations as the content of the parameter x.
             * @returns The number of iterations needed until convergence.
             */
            uint_fast64_t solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<Type> const& A, std::vector<Type>& x, std::vector<Type> const& b) const {
                // Get the settings object to customize linear solving.
                storm::settings::Settings* s = storm::settings::Settings::getInstance();
                
                double precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
				uint_fast64_t maxIterations = s->getOptionByLongName("maxIterations").getArgument(0).getValueAsUnsignedInteger();
				bool relative = s->getOptionByLongName("relative").getArgument(0).getValueAsBoolean();
                
                // Get a Jacobi decomposition of the matrix A.
                typename storm::storage::SparseMatrix<Type>::SparseJacobiDecomposition_t jacobiDecomposition = A.getJacobiDecomposition();
                
                // Convert the (inverted) diagonal matrix to gmm++'s format.
                gmm::csr_matrix<Type>* gmmxxDiagonalInverted = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(std::move(jacobiDecomposition.second));
                // Convert the LU matrix to gmm++'s format.
                gmm::csr_matrix<Type>* gmmxxLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<Type>(std::move(jacobiDecomposition.first));
                
                LOG4CPLUS_INFO(logger, "Starting iterative Jacobi Solver.");
                
                // x_(k + 1) = D^-1 * (b  - R * x_k)
                // In x we keep a copy of the result for swapping in the loop (e.g. less copy-back).
                std::vector<Type>* xNext = new std::vector<Type>(x.size());
                const std::vector<Type>* xCopy = xNext;
                std::vector<Type>* xCurrent = &x;
                
                // Target vector for precision calculation.
                std::vector<Type> tmpX(x.size());
                
                // Set up additional environment variables.
                uint_fast64_t iterationCount = 0;
                bool converged = false;
                
                while (!converged && iterationCount < maxIterations) {
                    // R * x_k (xCurrent is x_k) -> xNext
                    gmm::mult(*gmmxxLU, *xCurrent, tmpX);
                    // b - R * x_k (xNext contains R * x_k) -> xNext
                    gmm::add(b, gmm::scaled(tmpX, -1.0), tmpX);
                    // D^-1 * (b - R * x_k) -> xNext
                    gmm::mult(*gmmxxDiagonalInverted, tmpX, *xNext);
                    
                    // Swap xNext with xCurrent so that the next iteration can use xCurrent again without having to copy the
                    // vector.
                    std::vector<Type> *const swap = xNext;
                    xNext = xCurrent;
                    xCurrent = swap;
                    
                    // Now check if the process already converged within our precision.
                    converged = storm::utility::vector::equalModuloPrecision(*xCurrent, *xNext, precision, relative);
                    
                    // Increase iteration count so we can abort if convergence is too slow.
                    ++iterationCount;
                }
                
                // If the last iteration did not write to the original x we have to swap the contents, because the output has to
                // be written to the parameter x.
                if (xCurrent == xCopy) {
                    std::swap(x, *xCurrent);
                }
                
                // As xCopy always points to the copy of x used for swapping, we can safely delete it.
                delete xCopy;
                
                // Also delete the other dynamically allocated variables.
                delete gmmxxDiagonalInverted;
                delete gmmxxLU;
                
                return iterationCount;
            }
            
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_ */
