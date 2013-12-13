#ifndef STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_

#include "AbstractLinearEquationSolver.h"

namespace storm {
    namespace solver {

        /*!
         * A class that uses the gmm++ library to implement the AbstractLinearEquationSolver interface.
         */
        template<typename ValueType>
        class GmmxxLinearEquationSolver : public AbstractLinearEquationSolver<ValueType> {
        public:
            // An enumeration specifying the available preconditioners.
            enum Preconditioner {
                ILU, DIAGONAL, NONE
            };
            
            // An enumeration specifying the available solution methods.
            enum SolutionMethod {
                BICGSTAB, QMR, GMRES, JACOBI
            };
            
            /*!
             * Constructs a linear equation solver with the given parameters.
             *
             * @param method The method to use for linear equation solving.
             * @param precision The precision to use for convergence detection.
             * @param relative If set, the relative error rather than the absolute error is considered for convergence
             * detection.
             * @param maximalNumberOfIterations The maximal number of iterations do perform before iteration is aborted.
             * @param preconditioner The preconditioner to use when solving linear equation systems.
             * @param restart An optional argument that specifies after how many iterations restarted methods are
             * supposed to actually to a restart.
             */
            GmmxxLinearEquationSolver(SolutionMethod method, double precision, bool relative, uint_fast64_t maximalNumberOfIterations, Preconditioner preconditioner, uint_fast64_t restart = 0);
            
            /*!
             * Constructs a linear equation solver with parameters being set according to the settings object.
             */
            GmmxxLinearEquationSolver();
            
            virtual AbstractLinearEquationSolver<ValueType>* clone() const override;
            
            virtual void solveEquationSystem(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
            virtual void performMatrixVectorMultiplication(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n = 1, std::vector<ValueType>* multiplyResult = nullptr) const override;
            
        private:
            /*!
             * Solves the linear equation system A*x = b given by the parameters using the Jacobi method.
             *
             * @param A The matrix specifying the coefficients of the linear equations.
             * @param x The solution vector x. The initial values of x represent a guess of the real values to the
             * solver, but may be set to zero.
             * @param b The right-hand side of the equation system.
             * @return The number of iterations needed until convergence if the solver converged and
             * maximalNumberOfIteration otherwise.
             * @param multiplyResult If non-null, this memory is used as a scratch memory. If given, the length of this
             * vector must be equal to the number of rows of A.
             */
            uint_fast64_t solveLinearEquationSystemWithJacobi(storm::storage::SparseMatrix<ValueType> const& A, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<ValueType>* multiplyResult = nullptr) const;

            /*!
             * Retrieves the string representation of the solution method associated with this solver.
             *
             * @return The string representation of the solution method associated with this solver.
             */
            std::string methodToString() const;
            
            /*!
             * Retrieves the string representation of the preconditioner associated with this solver.
             *
             * @return The string representation of the preconditioner associated with this solver.
             */
            std::string preconditionerToString() const;
            
            // The method to use for solving linear equation systems.
            SolutionMethod method;

            // The required precision for the iterative methods.
            double precision;

            // Sets whether the relative or absolute error is to be considered for convergence detection. Not that this
            // only applies to the Jacobi method for this solver.
            bool relative;

            // The maximal number of iterations to do before iteration is aborted.
            uint_fast64_t maximalNumberOfIterations;

            // The preconditioner to use when solving the linear equation system.
            Preconditioner preconditioner;

            // A restart value that determines when restarted methods shall do so.
            uint_fast64_t restart;
        };

    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_GMMXXLINEAREQUATIONSOLVER_H_ */
