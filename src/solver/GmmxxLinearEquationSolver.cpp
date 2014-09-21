#include "GmmxxLinearEquationSolver.h"

#include <cmath>
#include <utility>

#include "src/adapters/GmmxxAdapter.h"
#include "src/settings/SettingsManager.h"
#include "src/utility/vector.h"
#include "src/utility/constants.h"
#include "src/exceptions/InvalidStateException.h"

#include "gmm/gmm_matrix.h"
#include "gmm/gmm_iter_solvers.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver(SolutionMethod method, double precision, uint_fast64_t maximalNumberOfIterations, Preconditioner preconditioner, bool relative, uint_fast64_t restart) : method(method), precision(precision), maximalNumberOfIterations(maximalNumberOfIterations), preconditioner(preconditioner), restart(restart) {
            // Intentionally left empty.
        }
        

        template<typename ValueType>
        GmmxxLinearEquationSolver<ValueType>::GmmxxLinearEquationSolver() {
            // Get the settings object to customize linear solving.
            storm::settings::modules::GmmxxEquationSolverSettings const& settings =storm::settings::gmmxxEquationSolverSettings();
            
            // Get appropriate settings.
            maximalNumberOfIterations = settings.getMaximalIterationCount();
            precision = settings.getPrecision();;
            relative = settings.getConvergenceCriterion() == storm::settings::modules::GmmxxEquationSolverSettings::ConvergenceCriterion::Relative;
            restart = settings.getRestartIterationCount();
            
            // Determine the method to be used.
            storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationTechnique methodAsSetting = settings.getLinearEquationSystemTechnique();
            if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationTechnique::Bicgstab) {
                method = SolutionMethod::Bicgstab;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationTechnique::Qmr) {
                method = SolutionMethod::Qmr;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationTechnique::Gmres) {
                method = SolutionMethod::Gmres;
            } else if (methodAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::LinearEquationTechnique::Jacobi) {
                method = SolutionMethod::Jacobi;
            }
            
            // Check which preconditioner to use.
            storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningTechnique preconditionAsSetting = settings.getPreconditioningTechnique();
            if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningTechnique::Ilu) {
                preconditioner = Preconditioner::Ilu;
            } else if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningTechnique::Diagonal) {
                preconditioner = Preconditioner::Diagonal;
            } else if (preconditionAsSetting == storm::settings::modules::GmmxxEquationSolverSettings::PreconditioningTechnique::None) {
                preconditioner = Preconditioner::None;
            }
        }
        
        template<typename ValueType>
        LinearEquationSolver<ValueType>* GmmxxLinearEquationSolver<ValueType>::clone() const {
            return new GmmxxLinearEquationSolver<ValueType>(*this);
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::solveEquationSystem(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            LOG4CPLUS_INFO(logger, "Using method '" << methodToString() << "' with preconditioner " << preconditionerToString() << "'.");
            if (method == SolutionMethod::Jacobi && preconditioner != Preconditioner::None) {
                LOG4CPLUS_WARN(logger, "Jacobi method currently does not support preconditioners. The requested preconditioner will be ignored.");
            }
            
            if (method == SolutionMethod::Bicgstab || method == SolutionMethod::Qmr || method == SolutionMethod::Gmres) {
                std::unique_ptr<gmm::csr_matrix<ValueType>> gmmA = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A);
                
                // Prepare an iteration object that determines the accuracy and the maximum number of iterations.
                gmm::iteration iter(precision, 0, maximalNumberOfIterations);
                
                if (method == SolutionMethod::Bicgstab) {
                    if (preconditioner == Preconditioner::Ilu) {
                        gmm::bicgstab(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == Preconditioner::Diagonal) {
                        gmm::bicgstab(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == Preconditioner::None) {
                        gmm::bicgstab(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == SolutionMethod::Qmr) {
                    if (preconditioner == Preconditioner::Ilu) {
                        gmm::qmr(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == Preconditioner::Diagonal) {
                        gmm::qmr(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), iter);
                    } else if (preconditioner == Preconditioner::None) {
                        gmm::qmr(*gmmA, x, b, gmm::identity_matrix(), iter);
                    }
                } else if (method == SolutionMethod::Qmr) {
                    if (preconditioner == Preconditioner::Ilu) {
                        gmm::gmres(*gmmA, x, b, gmm::ilu_precond<gmm::csr_matrix<ValueType>>(*gmmA), restart, iter);
                    } else if (preconditioner == Preconditioner::Diagonal) {
                        gmm::gmres(*gmmA, x, b, gmm::diagonal_precond<gmm::csr_matrix<ValueType>>(*gmmA), restart, iter);
                    } else if (preconditioner == Preconditioner::None) {
                        gmm::gmres(*gmmA, x, b, gmm::identity_matrix(), restart, iter);
                    }
                }
                
                // Check if the solver converged and issue a warning otherwise.
                if (iter.converged()) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iter.get_iteration() << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                }
            } else if (method == SolutionMethod::Jacobi) {
                uint_fast64_t iterations = solveLinearEquationSystemWithJacobi(A, x, b, multiplyResult);
                
                // Check if the solver converged and issue a warning otherwise.
                if (iterations < maximalNumberOfIterations) {
                    LOG4CPLUS_INFO(logger, "Iterative solver converged after " << iterations << " iterations.");
                } else {
                    LOG4CPLUS_WARN(logger, "Iterative solver did not converge.");
                }
            }
        }
        
        template<typename ValueType>
        void GmmxxLinearEquationSolver<ValueType>::performMatrixVectorMultiplication(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n, std::vector<ValueType>* multiplyResult) const {
            // Transform the transition probability A to the gmm++ format to use its arithmetic.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmxxMatrix = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(A);

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
                gmm::mult(*gmmxxMatrix, *currentX, *nextX);
                std::swap(nextX, currentX);
                
                // If requested, add an offset to the current result vector.
                if (b != nullptr) {
                    gmm::add(*b, *currentX);
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
        uint_fast64_t GmmxxLinearEquationSolver<ValueType>::solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult) const {
            // Get a Jacobi decomposition of the matrix A.
            std::pair<storm::storage::SparseMatrix<ValueType>, storm::storage::SparseMatrix<ValueType>> jacobiDecomposition = A.getJacobiDecomposition();
            
            // Convert the (inverted) diagonal matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmDinv = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.second));
            // Convert the LU matrix to gmm++'s format.
            std::unique_ptr<gmm::csr_matrix<ValueType>> gmmLU = storm::adapters::GmmxxAdapter::toGmmxxSparseMatrix<ValueType>(std::move(jacobiDecomposition.first));
        
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
                gmm::mult(*gmmLU, *currentX, tmpX);
                gmm::add(b, gmm::scaled(tmpX, -storm::utility::constantOne<ValueType>()), tmpX);
                gmm::mult(*gmmDinv, tmpX, *nextX);
                
                // Now check if the process already converged within our precision.
                converged = storm::utility::vector::equalModuloPrecision(*currentX, *nextX, precision, relative);

                // Swap the two pointers as a preparation for the next iteration.
                std::swap(nextX, currentX);

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
            
            return iterationCount;
        }
        
        template<typename ValueType>
        std::string GmmxxLinearEquationSolver<ValueType>::methodToString() const {
            switch (method) {
                case SolutionMethod::Bicgstab: return "bicgstab";
                case SolutionMethod::Qmr: return "qmr";
                case SolutionMethod::Gmres: return "gmres";
                case SolutionMethod::Jacobi: return "jacobi";
            }
        }
        
        template<typename ValueType>
        std::string GmmxxLinearEquationSolver<ValueType>::preconditionerToString() const {
            switch (preconditioner) {
                case Preconditioner::Ilu: return "ilu";
                case Preconditioner::Diagonal: return "diagonal";
                case Preconditioner::None: return "none";
            }
        }
        
        // Explicitly instantiate the solver.
        template class GmmxxLinearEquationSolver<double>;
    }
}